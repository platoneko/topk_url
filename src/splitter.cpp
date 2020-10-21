#include "splitter.h"


int Splitter::split() {
    std::ifstream fin(inputFile);
    if (!fin) {
        std::cerr << "open `" << inputFile << "` failed!" << std::endl;
        return -1;
    }
    std::string str;
    std::vector<std::ofstream> split_fout;
    for (size_t i=0; i<numFiles; ++i) {
        split_fout.push_back(std::ofstream(prefix+"_"+std::to_string(i)));
    }
    std::hash<std::string> hash;
    std::hash<int> i_hash;
    size_t h_val;
    while (!fin.eof()) {
        getline(fin, str);
        h_val = hash(str);
        // Hash on the last hash result.
        for (int i=0; i<level; ++i) {
            h_val = i_hash(h_val);
        }
        auto& fout = split_fout[h_val%numFiles];
        fout << str << '\n';
    }
    fin.close();
    for (auto& fout : split_fout) {
        fout.close();
    }
    return 0;
}