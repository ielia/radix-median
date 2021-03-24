#define TRUE 1
#define FALSE 0

#include "logging.h"
#include "median.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: See if the code can be less bulky.
long double median(int64_t min, int64_t max, uint64_t availMem, BOOL(*atEnd)(void), uint64_t(*readNextBatch)(uint64_t, int64_t *), void(*rewindStream)(void)) {
    BOOL even = FALSE;
    unsigned int addressSpaceSize, currShift = 0, shift = 0, totalBits, typeSize = sizeof(int64_t);
    int64_t count, middle, nSmaller = 0;
    uint64_t adjustmentMask, batchSize, currBatchSize, element, i, leftMask, mask, nElems, number, rightMask, tableSize, totalSpace, window = 0;
    int64_t *batch;
    uint64_t *hashtable;

    totalSpace = max - min;
    totalBits = ilogb(totalSpace) + 1;
    if (totalSpace > (1 << totalBits)) {
        ++totalBits;
    }
    availMem /= typeSize;
    availMem -= 24;
    if (availMem <= 8) {
        logError("ERROR: Not enough available memory.");
        return 0;
    }
    addressSpaceSize = ilogb(availMem);
    shift = totalBits <= addressSpaceSize - 1 ? totalBits : addressSpaceSize - 1;
    batchSize = 1 << (addressSpaceSize - 1);
    tableSize = 1 << shift;
    mask = batchSize - 1;
    logDebug("BATCH SIZE: %lu\n", batchSize);
    logDebug("TABLE SIZE: %lu\n", tableSize);
    batch = (int64_t *) malloc(batchSize * typeSize);
    hashtable = (uint64_t *) malloc(tableSize * typeSize);
    currShift = totalBits - shift;
    mask <<= currShift;
    leftMask = (batchSize << currShift) - 1;
    rightMask = leftMask;
    logDebug("typeSize=%u | availMem=%lu | min=%ld | max=%ld | full="B64P" | totalBits=%u | shift=%u | currShift=%u\n\n", typeSize, availMem, min, max, I64TOB(INTMAX_MAX | INTMAX_MIN), totalBits, shift, currShift);

    while (mask && leftMask == rightMask) {
        memset(hashtable, 0, tableSize * typeSize);
        nElems = 0;
        rewindStream();
        while(!atEnd()) {
            currBatchSize = readNextBatch(batchSize, batch);
            nElems += currBatchSize;
            for (i = 0; i < currBatchSize; ++i) {
                element = batch[i] - min;
                logMinutia("element[%lu]: %ld\n", i, element);
                if ((element & window) == (leftMask & window)) {
                    logMinutia("<<<<< element[%lu]: %ld >>>>>\n", i, element);
                    ++hashtable[(element & mask) >> currShift];
                }
            }
        }
        even = !(nElems & 1);
        middle = nElems >> 1;
        count = nSmaller;
        logTrace("nElems=%lu | middle=%lu | even=%d | nSmaller=%lu | currShift=%u | mask="B64P" | leftMask="B64P" | rightMask="B64P" | window="B64P"\n", nElems, middle, even, nSmaller, currShift, I64TOB(mask), I64TOB(leftMask), I64TOB(rightMask), I64TOB(window));
        logTrace("hashtable ="); for (i = 0; i < tableSize; ++i) { logDebug(" %ld", hashtable[i]); } logDebug("\n");
        // TODO: Check if it can be done inline instead of on a 2nd pass.
        for (i = 0; i < tableSize; ++i) {
            number = hashtable[i];
            if (number) {
                count += number;
                if (count > middle) {
                    adjustmentMask = ((INTMAX_MIN | INTMAX_MAX) ^ mask) | (i << currShift);
                    logTrace("[adjustmentMask = "B64P"]\n", I64TOB(adjustmentMask));
                    rightMask &= adjustmentMask;
                    logTrace("<><><> At %lu, setting rightMask = %ld\n", i, rightMask);
                    if (count > middle + 1 || !even) {
                        leftMask &= adjustmentMask;
                        logTrace("<><><> At %lu, setting leftMask = %ld\n", i, leftMask);
                        nSmaller = count - hashtable[i];
                    }
                    break;
                } else if (count == middle && even) {
                    adjustmentMask = ((INTMAX_MIN | INTMAX_MAX) ^ mask) | (i << currShift);
                    logTrace("[adjustmentMask = "B64P"]\n", I64TOB(adjustmentMask));
                    leftMask &= adjustmentMask;
                    logTrace("<><><> At %lu, setting leftMask = %ld\n", i, leftMask);
                    nSmaller = count - hashtable[i];
                }
            }
        }
        currShift = currShift >= shift ? currShift - shift : 0;
        window |= mask;
        mask >>= shift;
    }

    if (leftMask != rightMask) {
        logTrace("<><><><><> (left) %lu != %lu (right) <><><><><>", leftMask, rightMask);
        leftMask &= window;
        rewindStream();
        while(!atEnd()) {
            currBatchSize = readNextBatch(batchSize, batch);
            nElems += currBatchSize;
            for (i = 0; i < currBatchSize; ++i) {
                element = batch[i] - min;
                if ((element & window) == (leftMask & window) && element > leftMask) {
                    leftMask = element;
                } else if ((element & window) == (rightMask & window) && element < rightMask) {
                    rightMask = element;
                }
            }
        }
    }

    free(batch);
    free(hashtable);

    logDebug("\nMOVED LEFT: %lu\nMOVED RIGHT: %lu\n", leftMask, rightMask);
    logDebug("\nLEFT: %ld\nRIGHT: %ld\n", leftMask + min, rightMask + min);
    return leftMask / 2.0L + rightMask / 2.0L + min;
}

