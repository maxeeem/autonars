#ifndef MACNARS_EMBEDDING_SERVICE_H
#define MACNARS_EMBEDDING_SERVICE_H

#include <vector>
#include <string>

// A pure C++ interface to avoid pulling Objective-C++ headers into every file.
class EmbeddingService {
public:
    static std::vector<double> get_embedding(const std::string& term_string);
    static double cosine_similarity(const std::vector<double>& v1, const std::vector<double>& v2);
};

#endif
