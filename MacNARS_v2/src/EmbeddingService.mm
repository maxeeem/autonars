#import <Foundation/Foundation.h>
#import <NaturalLanguage/NaturalLanguage.h>
#import <Accelerate/Accelerate.h>
#include "EmbeddingService.h"
#include <iostream>

std::vector<double> EmbeddingService::get_embedding(const std::string& term_string) {
    @autoreleasepool {
        NSString* nsTerm = [NSString stringWithUTF8String:term_string.c_str()];
        
        // Prioritize Sentence Embedding for multi-word support (Narsese statements)
        NLEmbedding* embedding = [NLEmbedding sentenceEmbeddingForLanguage:NLLanguageEnglish];
        
        if (!embedding) {
            // Fallback to word embedding
            embedding = [NLEmbedding wordEmbeddingForLanguage:NLLanguageEnglish];
        }

        if (embedding) {
            NSArray<NSNumber*>* vector = [embedding vectorForString:nsTerm];
            // If the sentence embedder failed (e.g. it's just a single unknown word), try word embedder explicitly
            if (!vector) {
                NLEmbedding* word_embedding = [NLEmbedding wordEmbeddingForLanguage:NLLanguageEnglish];
                if (word_embedding) {
                    vector = [word_embedding vectorForString:nsTerm];
                }
            }

            if (vector) {
                std::vector<double> cpp_vector;
                cpp_vector.reserve(vector.count);
                for (NSNumber* num in vector) {
                    cpp_vector.push_back([num doubleValue]);
                }
                return cpp_vector;
            }
        }
        return std::vector<double>();
    }
}

double EmbeddingService::cosine_similarity(const std::vector<double>& v1, const std::vector<double>& v2) {
    if (v1.empty() || v2.empty() || v1.size() != v2.size()) {
        return 0.0;
    }
    
    // Utilize Apple's Accelerate framework for fast vector math
    double dot_product = 0.0;
    vDSP_dotprD(v1.data(), 1, v2.data(), 1, &dot_product, v1.size());
    
    double norm_v1 = 0.0;
    double norm_v2 = 0.0;
    
    // Compute squares
    std::vector<double> v1_sq(v1.size());
    std::vector<double> v2_sq(v2.size());
    vDSP_vsqD(v1.data(), 1, v1_sq.data(), 1, v1.size());
    vDSP_vsqD(v2.data(), 1, v2_sq.data(), 1, v2.size());
    
    // Sum squares
    vDSP_sveD(v1_sq.data(), 1, &norm_v1, v1_sq.size());
    vDSP_sveD(v2_sq.data(), 1, &norm_v2, v2_sq.size());
    
    norm_v1 = sqrt(norm_v1);
    norm_v2 = sqrt(norm_v2);
    
    if (norm_v1 == 0.0 || norm_v2 == 0.0) return 0.0;
    
    return dot_product / (norm_v1 * norm_v2);
}
