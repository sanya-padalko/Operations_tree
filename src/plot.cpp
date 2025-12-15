#include "plot.h"
#include "tree.h"
#include "dump.h"
#include "folders.h"

const char* colors[] = {  "#000000", "#0808c4ff", "#008000", "#800000", "#A84503", "#800080"};
const char* point_color = "#FBB117";

const double delta = 0.00001;

void PrintPlot(Tree_t* tree, const char* tex_name, double x0, double x_delta, double y_delta) {
    if (!tree || !tex_name)
        return;
    
    char* data_file = (char*)calloc(10, sizeof(char));
    sprintf(data_file, "%s.txt", tree->name);

    PrintData(tree->root, data_file, x0, x_delta, y_delta);
    char* plot_name = GnuPlot(x0, x_delta, CalcConstNode(tree->root, x0), y_delta, 1, tree);

    AddPlot(tex_name, plot_name);
}

void AddPlot(const char* tex_name, const char* plot_name) {
    if (!tex_name || !plot_name)
        return;

    FILE* tex_file = fopen(tex_name, "a");
    fprintf(tex_file, "\\begin{center}\n");
    fprintf(tex_file, "\t\\includegraphics[width=15cm]{%s}\n", plot_name);
    fprintf(tex_file, "\\end{center}\n");
    fclose(tex_file);
}

void PrintData(Node_t* node, const char* file_name, double x0, double x_delta, double y_delta) {
    if (!node || !file_name)
        return;

    FILE* data_file = fopen(file_name, "w");
    double y0 = CalcConstNode(node, x0);
    double x = x0 - x_delta;

    while (x <= (x0 + x_delta)) {
        double y = CalcConstNode(node, x);
        fprintf(data_file, "%lg %lg\n", x, y);

        x += delta;
    }

    fclose(data_file);
}

char* GnuPlot(double x0, double x_delta, double y0, double y_delta, int plot_cnt, ...) {
    static int plot_ind = 0;
    char* plot_name = (char*)calloc(10, sizeof(char));
    snprintf(plot_name, 10, "plot%d.png", plot_ind++);

    char plot_param[300];
    int add_ptr = 0, print_symb = 0;
    sprintf(plot_param + add_ptr, "plot %n", &print_symb);
    add_ptr += print_symb;

    va_list args;
    va_start(args, plot_cnt);

    for (int i = 0; i < plot_cnt; ++i) {
        Tree_t* tree = va_arg(args, Tree_t*);
        PrintData(tree->root, tree->name, x0, x_delta, y_delta);

        char* color = GetColor(i, (long long)tree->name);
        sprintf(plot_param + add_ptr, "'%s' with lines linewidth 3 lc rgb \"%s\" title '%s', %n", tree->name, color, tree->name, &print_symb);
        add_ptr += print_symb;
    }

    va_end(args);

    PrintPoint(x0, y0);
    sprintf(plot_param + add_ptr, "'point' with points ps 2 pt 7 lc rgb \"%s\" title 'x0', \0", point_color);

    PrintPlotParams(x0, x_delta, y0, y_delta, plot_name, plot_param);
    system("\"C:/Program Files/gnuplot/bin/gnuplot.exe\" plot.gnu");

    return plot_name;
}

void PrintPoint(double x, double y) {
    PrintData(NUM_NODE(y), "point", x, 0, 0);
}

void PrintPlotParams(double x0, double x_delta, double y0, double y_delta, const char* plot_name, const char* plot_param) {
    FILE* plot_file = fopen("plot.gnu", "w");
    
    fprintf(plot_file, "set terminal png size 800,600\n");
    fprintf(plot_file, "set xrange [%lg:%lg]\n", x0 - x_delta, x0 + x_delta);
    fprintf(plot_file, "set yrange [%lg:%lg]\n", y0 - y_delta, y0 + y_delta);
    fprintf(plot_file, "set output '%s'\n", plot_name);
    fprintf(plot_file, "set grid\n");
    fprintf(plot_file, "set multiplot\n");
    fprintf(plot_file, "%s\n", plot_param);
    fprintf(plot_file, "set nomultiplot\n");
    
    fclose(plot_file);
}

char* GetColor(int ind, long long ptr) {
    char* color = (char*)calloc(10, sizeof(char));
    if (ind >= sizeof(colors) / sizeof(colors[0]))
        sprintf(color, "#%06X", DarkCalcHash(ptr));
    else
        color = strdup(colors[ind]);

    return color;
}

void PrintPdf(const char* tex_file_name) {
    CloseTex(tex_file_name);

    char* sys_c = (char*)calloc(200, sizeof(char));
    sprintf(sys_c, "C:/Users/user/AppData/Local/Programs/MiKTeX/miktex/bin/x64/pdflatex.exe %s", tex_file_name);

    system(sys_c);
}