#include "DumpWorker.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static void print_usage() {
    printf("Specify the path to the files with array 1 and array 2, \
    the path to the file where to save the result\n\
    -l <filepath> - path to first array\n\
    -r <filepath> - path to second array\n\
    -o <filepath> - path to output file\n");
}

int main(int argc, char *argv[]) {
    int opt;
    char * l_filepath = NULL;
    char * r_filepath = NULL;
    char * out_filepath = NULL;
    while((opt = getopt(argc, argv, "l:r:o:")) != -1) {
        switch(opt) {
            case 'l':
                l_filepath = optarg;
                break;
            case 'r':
                r_filepath = optarg;
                break;
            case 'o':
                out_filepath = optarg;
                break;
            default:
                print_usage();
                break;
        }
    }
    if(!l_filepath || !r_filepath || !out_filepath) {
        print_usage();
        return ERROR_INVALID_ARGUMENTS;
    }

    StatData * l_data = NULL;
    int l_data_size;
    if(0 != LoadDump(l_filepath, &l_data, &l_data_size)) {
        return ERROR_FAILED_TO_LOAD_DUMP;
    }

    StatData * r_data = NULL;
    int r_data_size;
    if(0 != LoadDump(r_filepath, &r_data, &r_data_size)) {
        return ERROR_FAILED_TO_LOAD_DUMP;
    }

    int joined_data_size = 0;
    StatData * joined_data = JoinDump(l_data, l_data_size, r_data, r_data_size, &joined_data_size);
    if(joined_data == NULL) {
        return ERROR_INTERNAL;
    }

    SortDump(joined_data, joined_data_size);
    PrintFirstElems(joined_data, joined_data_size);
    if(0 != StoreDump(out_filepath, joined_data, joined_data_size)) {
        return ERROR_FAILED_TO_STORE_DUMP;
    }

    free(l_data);
    free(r_data);
    free(joined_data);
    return 0;
}