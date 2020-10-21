#include "topk_conuter.h"


std::vector<std::shared_ptr<TopkCounter::StrCount>> TopkCounter::get_topk() {
    for (size_t i=0; i<numFiles; ++i) { 
        process_subfile(prefix+"_"+std::to_string(i));
    }
    return min_heap;
}

void TopkCounter::process_subfile(const std::string& filename, int level) {
    std::unordered_map<std::string, size_t> counter;
    std::ifstream fin(filename);
    std::string str;
    bool need_to_split = false;
    while (!fin.eof()) {
        getline(fin, str);
        ++counter[str];
        // If counter size > `maxHashmap`, memory may run out soon.
        if (counter.size() > maxHashmap) {
            need_to_split = true;
            break;
        }
    }
    fin.close();
    if (need_to_split) {
        // Clear hashmap.
        std::unordered_map<std::string, size_t>().swap(counter);
        // Release free heap memory.
        malloc_trim(0);
        split_and_process(filename, level+1);
        return;
    }
    for (auto& str_cnt : counter) {
        if (min_heap.size() < K) {
            min_heap.push_back(std::make_shared<StrCount>(str_cnt));
            if (min_heap.size() == K) {
                std::make_heap(min_heap.begin(), min_heap.end(), StrCountGreater());
            }
        } else if (min_heap.front()->second < str_cnt.second) {
            std::pop_heap(min_heap.begin(), min_heap.end(), StrCountGreater());
            min_heap.pop_back();
            min_heap.push_back(std::make_shared<StrCount>(str_cnt));
            std::push_heap(min_heap.begin(), min_heap.end(), StrCountGreater());
        }
    }
}

void TopkCounter::split_and_process(const std::string& filename, int level) {
    size_t n;
    struct stat statbuf;
    stat(filename.c_str(), &statbuf);
    n = (statbuf.st_size+fileSize-1) / fileSize;
    n = n > 1 ? n : 2;  // Split into at least 2 parts.

    Splitter splitter(n, filename, filename, level);
    splitter.split();
    for (size_t i=0; i<n; ++i) { 
        process_subfile(filename+"_"+std::to_string(i), level);
    }
}