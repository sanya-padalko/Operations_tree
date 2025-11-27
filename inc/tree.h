#ifndef _LIST_H_
#define _LIST_H_

#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <math.h>
#include "errors.h"
#include "vars.h"
#include "calc.h"

#define dump(forest, ind, error_code, add_info) Dump(forest, ind, VarInfo{#forest, __FILE__, __FUNCTION__, __LINE__, error_code, add_info})

const int MAX_NODES_CNT = 10000;
const int MAX_DIFFER = 100;
const int MAX_VARS_CNT = 100;

enum TYPES {
    OPER    =   0,
    VAR     =   1,
    NUM     =   2, 
};

enum OPERATIONS {
    OP_ADD     =   0,
    OP_SUB     =   1,
    OP_MUL     =   2,
    OP_DIV     =   3,
    OP_COS     =   4,
    OP_SIN     =   5,
    OP_POW_C   =   6,
    OP_LN      =   7,
    OP_LOG     =   8,

    OPER_CNT
};

union ValueType {
    int type;
    char* name;
    double value;
};

struct Node_t {
    TYPES type;

    ValueType* value = NULL;

    Node_t* left = NULL;
    Node_t* right = NULL;
};

struct Tree_t {
    Node_t* root = NULL;

    size_t nodes_cnt = 0;
};

struct Var_t {
    char* name = NULL;
    double value = NAN;
};

struct Forest_t {
    const char* dot_file_name = NULL;
    const char* html_file_name = NULL;
    const char* tex_file_name = NULL;

    char* buf = NULL;

    int var_cnt = 0;

    Tree_t* tree[MAX_DIFFER] = {NULL};

    Var_t* vars[MAX_VARS_CNT] = {NULL};
};

struct Operation_t {
    int type;
    const char* base_view;

    const char* first_tex_view;
    const char* second_tex_view;
    const char* third_tex_view;

    const char* dump_view;

    const char* first_print_view;
    const char* second_print_view;
    const char* third_print_view;

    double (*func) (Node_t*, Node_t*);
};

const Operation_t opers[] = {
    { .type = OP_ADD, .base_view = "+", 
        .first_tex_view = "", .second_tex_view = " + ", .third_tex_view = "",
        .dump_view = "+", 
        .first_print_view = "", .second_print_view = " + ", .third_print_view = "",
        .func = Sum},

    { .type = OP_SUB, .base_view = "-", 
        .first_tex_view = "", .second_tex_view = " - ", .third_tex_view = "",
        .dump_view = "-", 
        .first_print_view = "", .second_print_view = " - ", .third_print_view = "", 
        .func = Sub},

    { .type = OP_MUL, .base_view = "*", 
        .first_tex_view = "(", .second_tex_view = ") \\times (", .third_tex_view = ")",
        .dump_view = "*", 
        .first_print_view = "(", .second_print_view = ") * (", .third_print_view = ")", 
        .func = Mul},
        
    { .type = OP_DIV, .base_view = "/", 
        .first_tex_view = "\\frac{", .second_tex_view = "}{", .third_tex_view = "}",
        .dump_view = "/", 
        .first_print_view = "(", .second_print_view = ") / (", .third_print_view = ")", 
        .func = Div},
        
    { .type = OP_COS, .base_view = "cos", 
        .first_tex_view = "", .second_tex_view = "\\cos (", .third_tex_view = ")",
        .dump_view = "cos", 
        .first_print_view = "", .second_print_view = "cos (", .third_print_view = ")", 
        .func = Cos},
        
    { .type = OP_SIN, .base_view = "sin", 
        .first_tex_view = "", .second_tex_view = "\\sin (", .third_tex_view = ")",
        .dump_view = "sin", 
        .first_print_view = "", .second_print_view = "sin (", .third_print_view = ")", 
        .func = Sin},
    
    { .type = OP_POW_C, .base_view = "pow_c", 
        .first_tex_view = "(", .second_tex_view = ") ^ ", .third_tex_view = "",
        .dump_view = "pow_c", 
        .first_print_view = "(", .second_print_view = ") pow_c ", .third_print_view = "", 
        .func = Pow_c},
        
    { .type = OP_LN, .base_view = "ln", 
        .first_tex_view = "", .second_tex_view = "ln (", .third_tex_view = ")",
        .dump_view = "ln", 
        .first_print_view = "", .second_print_view = "ln (", .third_print_view = ")", 
        .func = Ln},

    { .type = OP_LOG, .base_view = "log", 
        .first_tex_view = "log_{", .second_tex_view = "} (", .third_tex_view = ")",
        .dump_view = "log", 
        .first_print_view = "log (", .second_print_view = ") (", .third_print_view = ")", 
        .func = Log},
};

Forest_t* ForestCtor();
Tree_t* TreeCtor();
Node_t* NodeCtor(TYPES type, ValueType* val_type, Node_t* left, Node_t* right);

CodeError_t ForestDtor(Forest_t* forest);
CodeError_t TreeDtor(Node_t* root);
CodeError_t TreeVerify(Tree_t* tree);
int GetSize(Node_t* root);

CodeError_t ReadBase(Forest_t* forest, const char* file_name);
int get_file_size(const char* file_name);
Node_t* ParseBase(char** cur_pos, Forest_t* forest);
Var_t* VarCtor(char* name);
CodeError_t SeparateVars(Forest_t* forest);

CodeError_t TexDump(Forest_t* forest, int ind, const char* add_msg);
CodeError_t TexPrinting(Node_t* node, FILE* file);
CodeError_t CloseTex(Forest_t* forest);

CodeError_t Dump(Forest_t* forest, int ind, VarInfo varinfo);
void TreeImgDump(Forest_t* forest, int ind);
void RecDump(Node_t* root, FILE* dot_file);
int CalcHash(long long p);
int ParseNode(Node_t* node);

Node_t* Differ(Node_t* node, char* var_name);
Node_t* CopyNode(Node_t* node);
CodeError_t Printing(Node_t* node, FILE* file);

double CalculatingTree(Forest_t* forest, int ind);
double CalculatingNode(Node_t* node, Forest_t* forest);

double GetValue(Node_t* node);
Node_t* ConvolConst(Node_t* node);
Node_t* RemovingNeutral(Node_t* node);

#endif