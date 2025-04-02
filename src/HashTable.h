#pragma once

#include <stdint.h>

#include "common.h"

typedef struct HT_item {
    long key;
    StatData value;
} HTitem;

typedef struct HashTable {
    HTitem ** items;
    int count;
} HashTable;

uint32_t hash(long key);

HashTable * HTcreate();
// return key, if item is new. -1 if already exist
int HTinsert(HashTable * table, long key, StatData value);
StatData HTsearch(HashTable * table, long key);
void HTfree(HashTable * table);