#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#ifndef RNGEN_H
#define RNGEN_H
class rnGen
{
 public:
    rnGen() { srandom(getpid()); }
    rnGen(unsigned s) { srandom(s); }
    const int operator()(const int range) {
        srandom(time(NULL) * random());
        return int(range * (double(random())/INT_MAX));
    }
};
#endif
