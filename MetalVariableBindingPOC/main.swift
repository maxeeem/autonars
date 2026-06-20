import Metal
import Foundation
import simd

// Same structures as Metal
struct Term {
    var type: UInt32 // 0: Atom, 1: Variable, 2: Compound
    var id: UInt32   // Name hash or operation code
    var arg1: UInt32
    var arg2: UInt32
}

struct Binding {
    var var_id: UInt32
    var term_index: UInt32
}

struct MatchResult {
    var success: UInt32
    var num_bindings: UInt32
    var b0: Binding, b1: Binding, b2: Binding, b3: Binding
    var b4: Binding, b5: Binding, b6: Binding, b7: Binding
}

func main() {
    guard let device = MTLCreateSystemDefaultDevice() else {
        fatalError("Metal is not supported on this device")
    }
    
    guard let commandQueue = device.makeCommandQueue() else {
        fatalError("Could not create command queue")
    }
    
    // Load shader
    let shaderSource = try! String(contentsOfFile: "shader.metal", encoding: .utf8)
    let compileOptions = MTLCompileOptions()
    let library = try! device.makeLibrary(source: shaderSource, options: compileOptions)
    
    guard let function = library.makeFunction(name: "match_terms") else {
        fatalError("Could not find function")
    }
    
    let pipelineState = try! device.makeComputePipelineState(function: function)
    
    let NONE: UInt32 = 0xFFFFFFFF
    
    // Operation Codes
    let OP_INHERITANCE: UInt32 = 1
    let OP_IMPLICATION: UInt32 = 2
    
    // Variable IDs
    let VAR_X: UInt32 = 901
    let VAR_Y: UInt32 = 902
    
    // Atom IDs
    let A: UInt32 = 101
    let B: UInt32 = 102
    let C: UInt32 = 103
    let D: UInt32 = 104
    
    let terms = [
        // Index 0-3: Atoms
        Term(type: 0, id: A, arg1: NONE, arg2: NONE), // 0: A
        Term(type: 0, id: B, arg1: NONE, arg2: NONE), // 1: B
        Term(type: 0, id: C, arg1: NONE, arg2: NONE), // 2: C
        Term(type: 0, id: D, arg1: NONE, arg2: NONE), // 3: D
        
        // Index 4-5: Variables
        Term(type: 1, id: VAR_X, arg1: NONE, arg2: NONE), // 4: ?x
        Term(type: 1, id: VAR_Y, arg1: NONE, arg2: NONE), // 5: ?y
        
        // Complex Term 1: <<?x --> ?y> =/> <C --> ?x>>
        Term(type: 2, id: OP_INHERITANCE, arg1: 4, arg2: 5), // 6: <?x --> ?y>
        Term(type: 2, id: OP_INHERITANCE, arg1: 2, arg2: 4), // 7: <C --> ?x>
        Term(type: 2, id: OP_IMPLICATION, arg1: 6, arg2: 7), // 8: <<?x --> ?y> =/> <C --> ?x>>
        
        // Complex Term 2: <<A --> B> =/> <C --> A>>
        Term(type: 2, id: OP_INHERITANCE, arg1: 0, arg2: 1), // 9: <A --> B>
        Term(type: 2, id: OP_INHERITANCE, arg1: 2, arg2: 0), // 10: <C --> A>
        Term(type: 2, id: OP_IMPLICATION, arg1: 9, arg2: 10),// 11: <<A --> B> =/> <C --> A>>
        
        // Complex Term 3: <<A --> B> =/> <C --> D>> (Should fail to unify with Term 1 due to ?x conflict)
        Term(type: 2, id: OP_INHERITANCE, arg1: 2, arg2: 3), // 12: <C --> D>
        Term(type: 2, id: OP_IMPLICATION, arg1: 9, arg2: 12) // 13: <<A --> B> =/> <C --> D>>
    ]
    
    // Pairs to match
    let pairs: [simd_uint2] = [
        simd_uint2(8, 11), // Match Term 8 and Term 11 (Success: ?x=A, ?y=B)
        simd_uint2(8, 13)  // Match Term 8 and Term 13 (Fail: ?x binds to A then tries to bind to D)
    ]
    
    // Buffers
    let termBuffer = device.makeBuffer(bytes: terms, length: MemoryLayout<Term>.stride * terms.count, options: .storageModeShared)!
    let pairBuffer = device.makeBuffer(bytes: pairs, length: MemoryLayout<simd_uint2>.stride * pairs.count, options: .storageModeShared)!
    let resultBuffer = device.makeBuffer(length: MemoryLayout<MatchResult>.stride * pairs.count, options: .storageModeShared)!
    
    // Dispatch
    guard let commandBuffer = commandQueue.makeCommandBuffer(),
          let computeEncoder = commandBuffer.makeComputeCommandEncoder() else {
        fatalError("Could not create command buffer")
    }
    
    computeEncoder.setComputePipelineState(pipelineState)
    computeEncoder.setBuffer(termBuffer, offset: 0, index: 0)
    computeEncoder.setBuffer(pairBuffer, offset: 0, index: 1)
    computeEncoder.setBuffer(resultBuffer, offset: 0, index: 2)
    
    let gridSize = MTLSizeMake(pairs.count, 1, 1)
    let threadGroupSize = MTLSizeMake(1, 1, 1)
    computeEncoder.dispatchThreads(gridSize, threadsPerThreadgroup: threadGroupSize)
    computeEncoder.endEncoding()
    
    commandBuffer.commit()
    commandBuffer.waitUntilCompleted()
    
    // Read results
    let results = resultBuffer.contents().bindMemory(to: MatchResult.self, capacity: pairs.count)
    
    for pairIdx in 0..<pairs.count {
        let res = results[pairIdx]
        print("Pair \(pairIdx):")
        print("  Match Success: \(res.success == 1)")
        if res.success == 1 {
            print("  Num Bindings: \(res.num_bindings)")
            let bindings = [res.b0, res.b1, res.b2, res.b3, res.b4, res.b5, res.b6, res.b7]
            for i in 0..<Int(res.num_bindings) {
                let varName = bindings[i].var_id == VAR_X ? "?x" : (bindings[i].var_id == VAR_Y ? "?y" : "Unknown")
                let termIdx = bindings[i].term_index
                let termName: String
                switch terms[Int(termIdx)].id {
                case A: termName = "A"
                case B: termName = "B"
                case C: termName = "C"
                case D: termName = "D"
                default: termName = "Term\(termIdx)"
                }
                print("  Binding \(i): \(varName) -> \(termName) (Index \(termIdx))")
            }
        } else {
            print("  Match failed (conflict or mismatch)")
        }
    }
}

main()
