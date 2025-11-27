#ifndef _CALC_H_
#define _CALC_H_

struct Node_t;

double Sum(Node_t* left, Node_t* right);
double Sub(Node_t* left, Node_t* right);
double Mul(Node_t* left, Node_t* right);
double Div(Node_t* left, Node_t* right);
double Cos(Node_t* left, Node_t* right);
double Sin(Node_t* left, Node_t* right);
double Ln(Node_t* left, Node_t* right);
double Log(Node_t* left, Node_t* right);
double Pow_c(Node_t* left, Node_t* right);

#endif