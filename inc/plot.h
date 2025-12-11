#ifndef _PLOT_H_
#define _PLOT_H_

#include <stdarg.h>

struct Node_t;

void PrintPlot(Node_t* node, const char* tex_name, double x0, double x_delta, double y_delta);
void AddPlot(const char* tex_name, const char* plot_name);
void PrintData(Node_t* node, const char* file_name, double x0, double x_delta, double y_delta);
char* GnuPlot(double x0,  double x_delta, int plot_cnt, ...);
void PrintPdf(const char* tex_file_name);
double GetMin(Node_t* node, double left, double right);
double GetMax(Node_t* node, double left, double right);

#endif // _PLOT_H_