#include <iostream>
#include <thread>
#include <getopt.h>
#include <unistd.h>
#include <time.h>

#include "splitter.h"
#include "topk_conuter.h"


void monitor_routine(bool& complete) {
    size_t virt = 0, res = 0, shr = 0;
    size_t peak_res = 0;
    size_t page_size = getpagesize();
    sleep(1);
    do {
        time_t now = time(0);
        tm* tm_val = localtime(&now);
        std::ifstream buffer("/proc/self/statm");
        buffer >> virt >> res >> shr;
        buffer.close();
        peak_res = res > peak_res ? res : peak_res;
        std::cout << tm_val->tm_hour << ':' << tm_val->tm_min << ':' << tm_val->tm_sec << ' '
        << "VIRT " << ((virt * page_size) >> 10) << "KB | RES "
        << ((res * page_size) >> 10) << "KB | SHR " << ((shr * page_size) >> 10)
        << "KB | PEAK RES " << ((peak_res * page_size) >> 10) << "KB" << std::endl;
        sleep(1);
    } while (!complete);
    std::cout << "PEAK RES " << ((peak_res * page_size) >> 10) << "KB" << std::endl;
}


int main(int argc,char *argv[]) {
    char opt;
    size_t k, n, m, s;
    std::string filepath;
    bool log_on;
    while ((opt=getopt(argc, argv, "hk:f:n:m:s:l")) != -1) {
        switch (opt) {
        case 'h':
            std::cout << "-h : help\n";
            std::cout << "-k : top `k` value\n";
            std::cout << "-f : input file path\n";
            std::cout << "-n : temp file number\\n";
            std::cout << "-m : max hashmap element size (m*1024)\n";
            std::cout << "-s : expected subfile size (MB)";
            std::cout << "-l : output memory monitoring log " << std::endl;
            exit(0);
        case 'k':
            k = atoi(optarg);
            break;
        case 'f':
            filepath = optarg;
            break;
        case 'm':
            m = atoi(optarg) << 10;
            break;
        case 'n':
            n = atoi(optarg);
            break;
        case 's':
            s = atoi(optarg) << 20;
            break;
        case 'l':
            log_on = true;
            break;
        }
    }
    bool complete = false;
    std::thread t;
    if (log_on) {
        t = std::thread(monitor_routine, std::ref(complete));
    }

    Splitter splitter(n, "tmp", filepath);
    if (splitter.split() != 0) {
        exit(-1);
    }
    std::cout << "split complete!" << std::endl;
    TopkCounter topk_couter(k, n, m, "tmp", s);
    auto topk = topk_couter.get_topk();
    for (auto& str_cnt : topk) {
        std::cout << str_cnt->first << ' ' << str_cnt->second << '\n';
    }
    std::cout << "complete!" << std::endl;
    complete = true;
    if (log_on && t.joinable()) {
        t.join();
    }
}