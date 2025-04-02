#include "DumpWorker.h"
#include "HashTable.h"
#include "common.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int StoreDump(const char * filepath, const StatData * data, int size) {
    FILE * file = fopen(filepath, "wb");
    if(!file) {
        return -1;
    }

    int written_size = fwrite(data, sizeof(StatData), size, file);
    if(written_size != size) {
        fclose(file);
        return -2;
    }

    fclose(file);
    return 0;
}

int LoadDump(const char * filepath, StatData ** out_data, int * out_size) {
    FILE * file = fopen(filepath, "rb");
    if (!file) {
        *out_data = NULL;
        *out_size = 0;
        printf("Failed to open file: %s\n", filepath);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size == 0) {
        *out_data = NULL;
        *out_size = 0;
        fclose(file);
        printf("File is empty\n");
        return 0;
    }

    *out_size = file_size / sizeof(StatData);
    *out_data = (StatData*)malloc(file_size);
    if (!*out_data) {
        *out_size = 0;
        fclose(file);
        printf("Failed to allocate memory\n");
        return -1;
    }

    int read_count = fread(*out_data, sizeof(StatData), *out_size, file);
    if (read_count != *out_size) {
        free(*out_data);
        *out_data = NULL; // Установить в NULL после освобождения
        printf("Failed to read data, expected rows = %d, read = %d\n", *out_size, read_count);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

StatData * JoinDump(const StatData * lhs, const int lhs_size, const StatData * rhs, const int rhs_size, int * out_data_size) {
    if (!lhs && !rhs) {
        *out_data_size = 0;
        return NULL;
    }

    HashTable * ht = HTcreate();
    if (!ht) {
        printf("Failed to create hash table\n");
        *out_data_size = -1;
        return NULL;
    }

    int keys_idx = 0;
    int max_keys = lhs_size + rhs_size;
    int * keys = (int*)malloc(sizeof(int) * max_keys);
    if (!keys) {
        printf("Failed to allocate memory for keys\n");
        HTfree(ht);
        *out_data_size = -1;
        return NULL;
    }

    int general_count = (lhs_size < rhs_size) ? lhs_size : rhs_size;
    for (int i = 0; i < general_count; ++i) {
        int key = HTinsert(ht, lhs[i].id, lhs[i]);
        if (key >= 0) {
            keys[keys_idx++] = key;
        }
        key = HTinsert(ht, rhs[i].id, rhs[i]);
        if (key >= 0) {
            keys[keys_idx++] = key;
        }
    }

    const StatData * rest_data = (lhs_size < rhs_size) ? rhs : lhs;
    int rest_size = (lhs_size < rhs_size) ? rhs_size : lhs_size;

    for (int i = general_count; i < rest_size; ++i) {
        int key = HTinsert(ht, rest_data[i].id, rest_data[i]);
        if (key >= 0) {
            keys[keys_idx++] = key;
        }
    }

    StatData * result = (StatData *)malloc(sizeof(StatData) * keys_idx);
    if (!result) {
        printf("Failed to allocate memory for result\n");
        free(keys);
        HTfree(ht);
        *out_data_size = -1;
        return NULL;
    }

    for (int i = 0; i < keys_idx; ++i) {
        result[i] = HTsearch(ht, keys[i]);
    }

    free(keys);
    HTfree(ht);
    *out_data_size = keys_idx;
    return result;
}

static int cmp(const void * lhs, const void * rhs) {
    const StatData *_lhs = (const StatData *)lhs;
    const StatData *_rhs = (const StatData *)rhs;

    float difference = _lhs->cost - _rhs->cost;

    if (fabsf(difference) < FLOAT_PRECISION) {
        return 0;
    } else if (difference < 0) {
        return -1;
    } else {
        return 1;
    }
}

void SortDump(StatData * data, int data_size) {
    qsort(data, data_size, sizeof(StatData), cmp);
}

static void PrintBinary(int value) {
    if (value == 0) {
        printf("0");
        return;
    }

    char binary[32];
    int index = 0;
    unsigned int num = (value < 0) ? -value : value;

    while (num > 0) {
        binary[index++] = (num % 2) + '0';
        num /= 2;
    }

    if (value < 0) {
        printf("-");
    }

    for (int i = index - 1; i >= 0; i--) {
        printf("%c", binary[i]);
    }
}

void PrintFirstElems(StatData * data, int data_size) {
    for(int i = 0; i < min(FIRST_ELEMS_IN_DATA, data_size); ++i) {
        printf("%lX     %d     %.3e     %c     ", data[i].id, data[i].count, data[i].cost, (data[i].primary == 0 ? 'n' : 'y'));
        PrintBinary(data[i].mode);
        printf("\n");
    }
    printf("\n");
}
