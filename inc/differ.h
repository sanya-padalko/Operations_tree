#ifndef _DIFFER_H_
#define _DIFFER_H_

struct Node_t;
struct Tree_t;

Node_t* Differ(Node_t* node, char* var_name);

Tree_t* N_Differ(Tree_t* tree, int n, char* var_name);

Tree_t* Tangent(Tree_t* tree, double x0, char* var_name);

Tree_t* Taylor(Tree_t* tree, int n, double x0, char* var_name);

#endif // _DIFFER_H_