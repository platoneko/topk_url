# PingCAP Homework

## 题目

100GB url文件，使用1GB内存计算出现次数top100的url和出现的次数。

## 题目分析

统计一个文件中url出现的次数，不可避免地要使用hash map进行计数。无论多大的文件，在进行读写时，都是每次读取若干块，并不会一次性把文件加载进内存，因此题目中文件规模大并不会在读写时使内存耗尽。于是问题出在，1GB内存很可能无法容纳一个hash map来统计100GB的url的出现次数。因为文件的分布未知，相同的url可能不排布在一起，一次性统计url次数，必须要在计算时一直维护一个hash map。为了能让运行时的hash map的规模在内存可容许的范围内，则需要改变改变数据分布，我们要尽可能将相同的url分布靠近，确保某一个数据范围内只会出现url集合的一个子集，并使统计这个子集的url的hash map内存在一个阈值内。因此，我们要做的事情是，先将大文件划分成若干个只包含url子集的url的小文件，依次对每个这样的小文件用hash map去统计url出现次数，并使用一个全局的最小堆去维护出现次数top100的url。

## 解答

1. 首先需要对大文件进行划分。读取url原文件，对每个url计算出一个hash值并对N取模（这里N我设了200），将原文件中的url划分到temp_0, temp_1 ... temp_N-1这样的N个临时子文件中。

2. 依次处理N个临时文件，使用hash map统计每个小的临时文件中url的出现次数，统计完一个临时文件中的url后，遍历hash map中的url，依次将其与最小堆中的堆顶url进行比较，如果出现次数比堆顶url多，则pop出堆顶元素，将该url push到堆中。

3. 考虑到第一次划分时，可能会出现划分出的临时子文件仍然过大的情况，出现这种情况一般有两个原因：

   1. 某几个url出现次数很多，虽然这个临时子文件看起来很大，但是其url集的基数可能并不大，可以在hash map中完成统计。
   2. 不同的url很多，只是hash后取模运算不巧地将它们分到了一个临时子文件中，无法一次性在hash map中完成统计。

   针对第二种情况，我们可以将该子文件中的url进行两次hash再取模，划分到几个子文件中去。而针对第一种情况，无论hash多少次取模，由于某个url次数出现太多，其终究会产生一个很大的子文件，对该情况再次划分是多此一举。因此针对这个问题，我采取了试错机制，先尝试一次性统计该子文件，当hash map规模超过用户指定的阈值之后，回收该hash map，并开始划分该子文件（根据子文件大小决定划分成多少部分，至少划分成两个）。如果划分后的子子文件仍然在统计时超出定义的hash map规模，则继续增加hash次数划分（实际上只要N值合适这种概率非常非常小）。

为使程序能够适应多种情形，用户可以通过命令行参数指定程序运行条件：

```bash
-h : help
-k : top `k` value
-f : input file path
-n : splitted temp file number
-m : max hashmap element size (m*1024)
-s : expected subfile size (MB)
-l : output memory monitoring log
```

推荐100GB文件，1GB内存条件下，使用如下配置：

```bash
bin/topk_url -k 100 -f urls.txt -n 200 -s 512 -m 5000
```

## 运行

可使用gen_url.py生成url文件。example：

```bash
python gen_url.py 10000000 50000000 -v
```

将会生成10000000个url的集合（实际中随机产生器可能会产生重复的url），从url集合中产生50000000个url条目。`-v`会输出每个url出现的次数，以给之后验证程序的正确性。

gen_url.py需要依赖包faker。

编译程序：

```bash
sh build.sh
```

修改run.sh配置，执行run.sh脚本运行程序。

测试中由于环境限制，只生成了1GB url文件，采用如下配置运行程序，常驻内存峰值在10MB以下：

```bash
bin/topk_url -k 100 -f urls_medium.txt -n 200 -s 5 -m 50 -l
```

## FUTURE

注意到在第一次划分文件时，程序使用std::ifstream只在一个线程上读取文件，可以考虑在多个线程上使用mmap，每个线程读取文件的不同部分，从而可以加快划分的速度。同时引入的问题是，某个磁盘页的开始处并不是一个完整的url，可以考虑从处理上一个磁盘页的线程接收url的前面部分，由于数据中的url都是以"http://"开头，可以以此作为url是否完整的简单判断。另外还要注意对写入文件加互斥锁（由于划分的子文件个数很多，远比cpu内核数多，因此不会经常出现写文件时等待获取锁的情况）。

在统计url时，当需要对子文件进行进一步划分时，可以将划分操作在另一个线程中进行，而不用去阻塞统计url的线程。

