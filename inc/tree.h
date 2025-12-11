#ifndef _LIST_H_
#define _LIST_H_

#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <math.h>
#include "dump.h"
#include "tex_utils.h"
#include "parsing.h"
#include "errors.h"
#include "vars.h"
#include "calc.h"
#include "differ.h"
#include "plot.h"

#define DUMP(forest, ind, error_code, add_info) Dump(forest, ind, VarInfo{#forest, __FILE__, __FUNCTION__, __LINE__, error_code, add_info})
#define NUM_NODE(x) NodeCtor(NUM, ValueNumCtor(x), NULL, NULL)
#define VAR_NODE(name) NodeCtor(VAR, ValueVarCtor(name), NULL, NULL)
#define OP_NODE(OPNAME, left, right) NodeCtor(OPER, ValueOperCtor(OPNAME), left, right)

#define PRINT_TO_TEX(forest, message) PrintToTex(forest->tex_file_name, message)

#define c(node) CopyNode(node)

const int MAX_VAR_SIZE = 100;
const int MAX_NODES_CNT = 10000;
const int MAX_DIFFER = 1000;
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
    OP_POW     =   6,
    OP_LN      =   7,
    OP_LOG     =   8,
    OP_EXP     =   9,
    OP_TAN     =  10,
    OP_COT     =  11,
    OP_ASIN    =  12,
    OP_ACOS    =  13,
    OP_ATAN    =  14,
    OP_ACOT    =  15,

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

    const char* tex_view;

    const char* dump_view;

    double (*func) (Node_t*, Node_t*);
};

const Operation_t opers[] = {
    { .type = OP_ADD,   .tex_view = " + ",            .dump_view = "+",        .func = Sum   },
   
    { .type = OP_SUB,   .tex_view = " - ",            .dump_view = "-",        .func = Sub   },
   
    { .type = OP_MUL,   .tex_view = " \\cdot ",       .dump_view = "*",        .func = Mul   },
   
    { .type = OP_DIV,   .tex_view = "",               .dump_view = "/",        .func = Div   },
   
    { .type = OP_COS,   .tex_view = "\\cos ",         .dump_view = "cos",      .func = Cos   },
   
    { .type = OP_SIN,   .tex_view = "\\sin ",         .dump_view = "sin",      .func = Sin   },
   
    { .type = OP_POW,   .tex_view = " ^ ",            .dump_view = "pow",      .func = Pow   },
   
    { .type = OP_LN,    .tex_view = "\\ln",           .dump_view = "ln",       .func = Ln    },
   
    { .type = OP_LOG,   .tex_view = "",               .dump_view = "log",      .func = Log   },
   
    { .type = OP_EXP,   .tex_view = "e ^ ",           .dump_view = "exp",      .func = Exp   },
   
    { .type = OP_TAN,   .tex_view = "\\tan ",         .dump_view = "tan",      .func = Tan   },
       
    { .type = OP_COT,   .tex_view = "\\cot ",         .dump_view = "cot",      .func = Cot   },
    
    { .type = OP_ASIN,   .tex_view = "\\arcsin ",     .dump_view = "arcsin",   .func = Asin  },
    
    { .type = OP_ACOS,   .tex_view = "\\arccos ",     .dump_view = "arccos",   .func = Acos  },
    
    { .type = OP_ATAN,   .tex_view = "\\arctan ",     .dump_view = "arctan",   .func = Atan  },
    
    { .type = OP_ACOT,   .tex_view = "\\operatorname{arccot} ", .dump_view = "arccot",   .func = Acot  },

};

Forest_t* ForestCtor();
Tree_t* TreeCtor();
Node_t* NodeCtor(TYPES type, ValueType* val_type, Node_t* left, Node_t* right);

CodeError_t ForestDtor(Forest_t* forest);
CodeError_t TreeDtor(Node_t* root);
CodeError_t TreeVerify(Tree_t* tree);
int GetSize(Node_t* root);

CodeError_t SetParams(const char* param_file, Forest_t** forest, double* x0, int* der_cnt, char** var_name, double* x_delta, double* y_delta);

CodeError_t ReadBase(Forest_t* forest, const char* file_name);
int get_file_size(const char* file_name);
CodeError_t ParseForest(Forest_t* forest);

CodeError_t SelectForestVars(Forest_t* forest);
void SelectTreeVars(Node_t* node, Forest_t* forest);
Var_t* VarCtor(char* name);
bool CheckVars(Forest_t* forest, char* var_name);

Node_t* CopyNode(Node_t* node);

ValueType* ValueOperCtor(int type);
ValueType* ValueVarCtor(char* name);
ValueType* ValueNumCtor(double value);

double CalculatingTree(Forest_t* forest, int ind);
double CalculatingNode(Node_t* node, Forest_t* forest);
double CalcConstNode(Node_t* node, double value);

double GetValue(Node_t* node);
void ConvolConst(Node_t** node);
void RemovingNeutral(Node_t** node);

CodeError_t Optimization(Node_t** node_ptr);

#endif