#ifndef _DUMP_H_
#define _DUMP_H_

#include "errors.h"
#include "vars.h"

#define ull unsigned long long

struct Forest_t;
struct Node_t;

CodeError_t Dump(Forest_t* forest, int ind, VarInfo varinfo);
void TreeImgDump(const char* dot_file_name, Node_t* root);
void RecDump(Node_t* root, FILE* dot_file);
int CalcHash(long long p);
int DarkCalcHash(long long p);

#endif // _DUMP_H_