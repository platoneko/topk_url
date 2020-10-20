#ifndef TOPK_COUNTER_H
#define TOPK_COUNTER_H

#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include <malloc.h>
#include <sys/stat.h>

#include "splitter.h"


class TopkCounter {
    typedef std::pair<std::string, size_t> StrCount;
    struct StrCountGreater {
        bool operator()(std::shared_ptr<StrCount> a, std::shared_ptr<StrCount> b) {
            return a->second > b->second;
        }
    };
private:
    const size_t K;
    const size_t numFiles;
    const size_t maxHashmap;
    const std::string prefix;
    const size_t fileSize;
    std::vector<std::shared_ptr<StrCount>> min_heap;

    void process_subfile(const std::string& filename, int level=0);
    void split_and_process(const std::string& filename, int level);
public:
    TopkCounter(size_t K_, size_t numFiles_, size_t maxHashmap_, 
        const std::string& prefix_, size_t fileSize_) : 
        K(K_),
        numFiles(numFiles_),
        maxHashmap(maxHashmap_),
        prefix(prefix_),
        fileSize(fileSize_) {}
    
    std::vector<std::shared_ptr<StrCount>> get_topk();
};

#endif  // ~TOPK_COUNTER_H