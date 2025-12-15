#ifndef _PLOT_H_
#define _PLOT_H_

#include <stdarg.h>

struct Node_t;
struct Tree_t;

void PrintPlot(Tree_t* tree, const char* tex_name, double x0, double x_delta, double y_delta);
void AddPlot(const char* tex_name, const char* plot_name);
void PrintData(Node_t* node, const char* file_name, double x0, double x_delta, double y_delta);
char* GnuPlot(double x0, double x_delta, double y0, double y_delta, int plot_cnt, ...);
void PrintPoint(double x, double y);
void PrintPlotParams(double x0, double x_delta, double y0, double y_delta, const char* plot_name, const char* plot_param);
char* GetColor(int ind, long long ptr);
void PrintPdf(const char* tex_file_name);

#endif // _PLOT_H_