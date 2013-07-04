#ifndef __CONFIG__H__
#define __CONFIG__H__

enum kBlock
{
    kBlockRed,
    kBlockBlue,
    kBlockYellow,
    kBlockGreen,
    kBlockGray,
    kBlockCount,
};

#define ccsf(...) CCString::createWithFormat(__VA_ARGS__)->getCString()

#endif // __CONFIG__H__