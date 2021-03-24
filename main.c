#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "median.h"
#include "logging.h"

int64_t length = 0;
int64_t cursor = 0;
int64_t *myList;

uint64_t readShit(uint64_t size, int64_t *buffer) {
    uint64_t remainingBytes = length - cursor;
    uint64_t bytes = remainingBytes < size ? remainingBytes : size;
    memcpy(buffer, &myList[cursor], bytes * sizeof(int64_t));
    cursor += bytes;
    return bytes;
}

void rewindStream(void) {
    cursor = 0;
}

BOOL atEnd(void) {
    return cursor >= length;
}


int main(char **argv) {
    int64_t typeSize = sizeof(int64_t);
    uint64_t availMem;
    // long long int value;
    int64_t value;
    int64_t min = INTMAX_MAX;
    int64_t max = INTMAX_MIN;
    long double result;
    const char *envLOGLEVEL = getenv("LOGLEVEL");
    if (envLOGLEVEL) { setLogLevelByName(envLOGLEVEL); }

    myList = (int64_t *) malloc(0);

    scanf("%lu", &availMem);
    logTest("Available memory: %lu\n", availMem);
    while(scanf("%ld", &value) != EOF) {
        if (value < min) { min = value; }
        if (value > max) { max = value; }
        logTrace("value = %ld [min=%ld, max=%ld, length=%ld]\n", value, min, max, length);
        ++length;
        myList = (int64_t *) realloc(myList, length * typeSize);
        myList[length - 1] = value;
    }

    logTest("Calculating median on");
    for (int64_t i = 0; i < length; ++i) { logDebug(" %ld", myList[i]); }
    logTest("\n");
    result = median(min, max, availMem, atEnd, readShit, rewindStream);
    logTest("Result: %Lg\n", result);

    return 0;
}

