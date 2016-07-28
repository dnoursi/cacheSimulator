# cacheSimulator
A program for simulating the hit-miss ratio of various sized caches on an arbitrary program

## Usage:

    ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>

* -s <s>: Number of set index bits (S = 2^s is the number of sets)
* -E <E>: Associativity (number of lines per set)
* -b <b>: Number of block bits (B = 2^b is the block size)
* -t <tracefile>: Name of the valgrind trace to replay (these files can be found in the traces directory)

For example:

    linux> ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace
    hits:4 misses:5 evictions:3
