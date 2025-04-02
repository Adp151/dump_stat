#ifndef COMMON_H
#define COMMON_H

static const int MAX_HASH_TABLE_SIZE =  100000;
static const int FIRST_ELEMS_IN_DATA = 10;
static const float FLOAT_PRECISION = 0.001;

static int max(int a, int b) {
    return a > b ? a : b;
}

static int min(int a, int b) {
    return a > b ? b : a;
}

enum ERROR {
    ERROR_INVALID_ARGUMENTS = 1,
    ERROR_FAILED_TO_LOAD_DUMP,
    ERROR_FAILED_TO_STORE_DUMP,
    ERROR_INTERNAL,
};

typedef struct StatData {
    long id;
    int count;
    float cost;
    unsigned int primary;
    unsigned int mode;
} StatData;

#endif
