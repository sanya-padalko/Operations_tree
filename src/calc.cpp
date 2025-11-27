#include "calc.h"
#include "tree.h"

double Sum(Node_t* left, Node_t* right) {
    return GetValue(left) + GetValue(right);
}

double Sub(Node_t* left, Node_t* right) {
    return GetValue(left) - GetValue(right);
}

double Mul(Node_t* left, Node_t* right) {
    return GetValue(left) * GetValue(right);
}

double Div(Node_t* left, Node_t* right) {
    if (GetValue(right) == double(0))
        return NAN;

    return GetValue(left) / GetValue(right);
}

double Cos(Node_t* left, Node_t* right) {
    return cos(GetValue(right));
}

double Sin(Node_t* left, Node_t* right) {
    return sin(GetValue(right));
}

double Pow_c(Node_t* left, Node_t* right) {
    if (GetValue(left) < 0)
        return NAN;
    
    return pow(GetValue(left), GetValue(right));
}

double Ln(Node_t* left, Node_t* right) {
    double a = GetValue(right);

    if (a <= 0) 
        return NAN;

    return log(a);
}

double Log(Node_t* left, Node_t* right) {
    double b = GetValue(right);
    double a = GetValue(left);

    if (a <= 0 || a == 1)
        return NAN;

    return log(b) / log(a);
}