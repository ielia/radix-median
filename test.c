#define TRUE 1
#define FALSE 0

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "median.h"

uint64_t length;
int64_t *myList;
int64_t cursor = 0;

int64_t max(uint64_t size, int64_t* arr) {
    int64_t result = INTMAX_MIN;
    for (uint64_t i = 0; i < size; ++i) { if (result < arr[i]) { result = arr[i]; } }
    return result;
}

int64_t min(uint64_t size, int64_t* arr) {
    int64_t result = INTMAX_MAX;
    for (uint64_t i = 0; i < size; ++i) { if (result > arr[i]) { result = arr[i]; } }
    return result;
}

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

BOOL runtest(uint64_t availMem, int64_t *theList, uint64_t len, int64_t minVal, int64_t maxVal, long double expectedResult, char *description) {
    BOOL testOK;
    long double result;

    logTest("TEST(availMem=%lu, %s, expected=%Lg) :\n", availMem, description, expectedResult);

    length = len;
    myList = theList;

    if (LOGLEVEL & LOGINFO) {
        logInfo("\tmyList =");
        for (int64_t i = 0; i < length; ++i) {
            logInfo(" %ld", myList[i]);
        }
        logInfo("\n");
    }

    result = median(minVal, maxVal, availMem, atEnd, readShit, rewindStream);

    testOK = result == expectedResult;
    logTest("\tactual=%Lg (%s)\n", result, testOK ? "OK" : "Failed");
    return testOK;
}

BOOL test(uint64_t availMem, int64_t begin, int64_t step, int64_t end, int64_t repeats, long double expectedResult) {
    BOOL testOK;
    char description[100];

    length = ((end - begin) / step + 1) * repeats;
    myList = (int64_t *) malloc(length * sizeof(int64_t));
    for (int64_t i = 0; i < length / repeats; ++i) {
        int64_t value = i * step + begin;
        for (int64_t j = 0; j < repeats; ++j) {
            int64_t index = i * repeats + j;
            myList[index] = value;
        }
    }

    sprintf(description, "[%ld..(%ld)..%ld]{%lu}", begin, step, end, length);
    testOK = runtest(availMem, myList, length, begin < end ? begin : end, begin > end ? begin : end, expectedResult, description);

    free(myList);
    return testOK;
}

int main(char **argv) {
    int ok = 0;
    int failed = 0;
    const char *envLOGLEVEL = getenv("LOGLEVEL");
    if (envLOGLEVEL) { setLogLevelByName(envLOGLEVEL); }

    test(800,       1,      1,       2,   1,       1.5) ? ++ok : ++failed;
    test(800,       1,      1,       3,   1,       2.0) ? ++ok : ++failed;
    test(800,       2,     -1,       1,   1,       1.5) ? ++ok : ++failed;
    test(800,       0,      1,       2,   1,       1.0) ? ++ok : ++failed;
    test(800,       2,     -1,       0,   1,       1.0) ? ++ok : ++failed;
    test(800,       0,      2,       2,   1,       1.0) ? ++ok : ++failed;
    test(800,       2,     -2,       0,   1,       1.0) ? ++ok : ++failed;
    test(800,       0,     -2,      -2,   1,      -1.0) ? ++ok : ++failed;
    test(800,      -2,      2,       0,   1,      -1.0) ? ++ok : ++failed;
    test(800,      -1,      2,       1,   1,       0.0) ? ++ok : ++failed;
    test(800,       1,     -2,      -1,   1,       0.0) ? ++ok : ++failed;
    test(800,      -1,      1,       1,   1,       0.0) ? ++ok : ++failed;
    test(800,       1,     -1,      -1,   1,       0.0) ? ++ok : ++failed;
    test(800,       1,      1,     100,   1,      50.5) ? ++ok : ++failed;
    test(800,     100,     -1,       1,   1,      50.5) ? ++ok : ++failed;
    test(800,       1,      1, 9999999,   1, 5000000.0) ? ++ok : ++failed;
    test(800, 9999999,     -1,       1,   1, 5000000.0) ? ++ok : ++failed;
    test(800,       1, 999999, 1000000, 500,  500000.5) ? ++ok : ++failed;

    int64_t arr1[] = { 10, 10, 0, 0, 3 };
    runtest(800, arr1, 5, 0, 10, 3.0, "[10, 10, 0, 0, 3]") ? ++ok : ++failed;

    int64_t arr2[] = { 4, 10, 10, 0, 0, 3 };
    runtest(800, arr2, 6, 0, 10, 3.5, "[4, 10, 10, 0, 0, 3]") ? ++ok : ++failed;

    int64_t arr3[] = { -1000, -1 };
    runtest(800, arr3, 2, -1000, -1, -500.5, "[-1000, -1]") ? ++ok : ++failed;

    logTest("\nTotal number of tests: %d,\n    % 3d tests succeeded\n    % 3d tests failed\n", ok + failed, ok, failed);

    return 0;
}

