#ifndef _PARSING_H_
#define _PARSING_H_

#include "tree.h"

#define SyntaxError(str, c) printf("Syntax error in %s, in line %d: %s %c\n", __FUNCTION__, __LINE__, str, c);

struct Node_t;

Node_t* GetG(const char** s);
Node_t* GetE(const char** s);
Node_t* GetT(const char** s);
Node_t* GetPower(const char** s);
Node_t* GetP(const char** s);
Node_t* GetV(const char** s);
Node_t* GetN(const char** s);
void SkipSpaces(const char** s);

#endif