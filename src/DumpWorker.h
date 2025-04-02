#pragma once

#include "common.h"
 
int StoreDump(const char * filepath, const StatData * data, int size);
int LoadDump(const char * filepath, StatData ** out_data, int * out_size);
StatData * JoinDump(const StatData * lhs, const int lhs_size, const StatData * rhs, const int rhs_size, int * out_data_size);
void SortDump(StatData * data, int data_size);
void PrintFirstElems(StatData * data, int data_size);