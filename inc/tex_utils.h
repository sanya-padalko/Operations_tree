#ifndef _TEX_UTILS_H_
#define _TEX_UTILS_H_

#include "errors.h"

struct Node_t;
struct Forest_t;

CodeError_t TexDump(Forest_t* forest, int ind, const char* add_msg);
CodeError_t TexPrinting(Node_t* node, FILE* file, int prev_oper, int son_type);
void PrintToTex(const char* tex_file_name, const char* message);
CodeError_t CloseTex(const char* tex_file_name);

#endif // _TEX_UTILS_H_