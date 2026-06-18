cmake_minimum_required(VERSION 3.15)
project(MacNARS_v2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_language(OBJCXX)

find_library(METAL_FRAMEWORK Metal)
find_library(FOUNDATION_FRAMEWORK Foundation)

find_library(NATURALLANGUAGE_FRAMEWORK NaturalLanguage)
find_library(ACCELERATE_FRAMEWORK Accelerate)

include_directories(include)

# Standard logic core
add_executable(MacNARS_v2 src/main.cpp)

# Quantum-inspired core
add_executable(MacNARS_Quantum src/QuantumMain.mm)
target_link_libraries(MacNARS_Quantum ${METAL_FRAMEWORK} ${FOUNDATION_FRAMEWORK})

# Hyper-Causal Drone Simulation core
add_executable(MacNARS_HyperCausal src/HyperCausalMain.mm)
target_link_libraries(MacNARS_HyperCausal ${METAL_FRAMEWORK} ${FOUNDATION_FRAMEWORK})

# Apple Intelligence Embeddings Bridge
add_executable(MacNARS_AppleEmbeddings src/AppleEmbeddingsMain.mm)
target_link_libraries(MacNARS_AppleEmbeddings ${FOUNDATION_FRAMEWORK} ${NATURALLANGUAGE_FRAMEWORK})

# Associative Manifold Memory Simulation
add_executable(MacNARS_SemanticGravity src/SemanticGravityMain.cpp src/EmbeddingService.mm)
target_link_libraries(MacNARS_SemanticGravity ${FOUNDATION_FRAMEWORK} ${NATURALLANGUAGE_FRAMEWORK} ${ACCELERATE_FRAMEWORK})


# Empirical Baseline Comparison
add_executable(MacNARS_DroneBaseline src/DroneBaseline.cpp)



