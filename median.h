#define BOOL unsigned char

#include <stdint.h>

long double median(int64_t min, int64_t max, uint64_t availMem, BOOL(*atEnd)(void), uint64_t(*readNextBatch)(uint64_t, int64_t *), void(*rewindStream)(void));
