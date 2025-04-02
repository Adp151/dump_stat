#include "HashTable.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

HashTable * HTcreate() {
    HashTable * table = malloc(sizeof(HashTable));
    table->items = malloc((sizeof(HTitem *) * MAX_HASH_TABLE_SIZE));

    for (int i = 0; i < MAX_HASH_TABLE_SIZE; i++) {
        table->items[i] = NULL;
    }
    
    return table;
}

void HTfree(HashTable *table) {
    for (int i = 0; i < MAX_HASH_TABLE_SIZE; i++) {
        free(table->items[i]);
    }
    free(table->items);
    free(table);
}

uint32_t hash(long key) {
    return key % MAX_HASH_TABLE_SIZE;
}

int HTinsert(HashTable *table, long key, StatData value) {
    uint32_t index = hash(key);
    HTitem *new_item = malloc(sizeof(HTitem));
    new_item->key = key;
    new_item->value = value;

    if (table->items[index] == NULL) {
        table->items[index] = new_item;
        table->count++;
        return key;
    } else {
        HTitem *current = table->items[index];
        current->value.count += value.count;
        current->value.cost += value.cost;
        if(current->value.primary == 0 || value.primary == 0) {
            current->value.primary = 0;
        }
        current->value.mode = max(current->value.mode, value.mode);
    }
    return -1;
}

StatData HTsearch(HashTable *table, long key) {
    uint32_t index = hash(key);
    HTitem * res = table->items[index];
    return res->value;
}