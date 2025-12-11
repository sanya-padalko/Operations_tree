#include "plot.h"
#include "tree.h"
#include "dump.h"
#include "folders.h"

const char* colors[] = {  "#000000", "#000080", "#008000", "#800000", "#A84503", "#800080"};

const double delta = 0.001;

void PrintPlot(Node_t* node, const char* tex_name, double x0, double x_delta, double y_delta) {
    if (!node || !tex_name)
        return;
    
    PrintData(node, "data.txt", x0, x_delta, y_delta);
    char* plot_name = GnuPlot(x0, x_delta, 1, "data.txt");

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
        if (y0 - y_delta <= y && y <= y0 + y_delta)
            fprintf(data_file, "%lg %lg\n", x, y);

        x += delta;
    }

    fclose(data_file);
}

char* GnuPlot(double x0, double x_delta, int plot_cnt, ...) {
    FILE* plot_file = fopen("plot.gnu", "w");

    static int plot_ind = 0;

    char* plot_name = (char*)calloc(10, sizeof(char));
    snprintf(plot_name, 10, "plot%d.png", plot_ind++);

    va_list args;
    va_start(args, plot_cnt);

    char plot_param[300];
    int add_ptr = 0, print_symb = 0;
    double mn = 1000, mx = -1000;

    sprintf(plot_param + add_ptr, "plot %n", &print_symb);
    add_ptr += print_symb;

    for (int i = 0; i < plot_cnt; ++i) {
        const char* data_file = va_arg(args, const char*);

        FILE* data = fopen(data_file, "r");
        double x, y;
        while (fscanf(data, "%lg %lg\n", &x, &y) == 2) {
            if (y < mn || mn == NAN)
                mn = y;

            if (y > mx || mx == NAN)
                mx = y;
        }

        fclose(data);

        char* color = (char*)calloc(10, sizeof(char));
        if (i >= sizeof(colors) / sizeof(colors[0]))
            sprintf(color, "#%06X", DarkCalcHash((long long)data_file));
        else
            color = strdup(colors[i]);

        char* file_name = strdup(data_file);
        int p = 0;
        while (file_name[p] != '\0') {
            if (file_name[p] == '.') {
                file_name[p] = '\0';
                break;
            }

            ++p;
        }

        sprintf(plot_param + add_ptr, "'%s' with lines linewidth 3 lc rgb \"%s\" title '%s', %n", data_file, color, file_name, &print_symb);
        
        if (!strcmp(file_name, "x0"))
            sprintf(plot_param + add_ptr, "'x0.txt' with points ps 2 pt 7 title 'x0', %n", color, &print_symb);

        add_ptr += print_symb;
    }

    sprintf(plot_param + add_ptr, "\0");
    
    fprintf(plot_file, "set terminal png size 800,600\n");
    fprintf(plot_file, "set xrange [%lg:%lg]\n", x0 - x_delta, x0 + x_delta);
    fprintf(plot_file, "set yrange [%lg:%lg]\n", mn, mx);
    fprintf(plot_file, "set output '%s'\n", plot_name);
    fprintf(plot_file, "set grid\n");
    fprintf(plot_file, "set multiplot\n");
    fprintf(plot_file, "%s\n", plot_param);
    fprintf(plot_file, "set nomultiplot\n");
    va_end(args);
    
    fclose(plot_file);
    system("\"C:/Program Files/gnuplot/bin/gnuplot.exe\" plot.gnu");

    return plot_name;
}

double GetMin(Node_t* node, double left, double right) {
    if (!node)
        return NAN;

    double mn = CalcConstNode(node, left);
    while (left <= right) {
        double value = CalcConstNode(node, left);
        if (value < mn)
            mn = value;

        left += delta;
    }

    return mn;
}

double GetMax(Node_t* node, double left, double right) {
    if (!node)
        return NAN;

    double mx = CalcConstNode(node, left);
    while (left <= right) {
        double value = CalcConstNode(node, left);
        if (value < mx)
            mx = value;

        left += delta;
    }

    return mx;
}

void PrintPdf(const char* tex_file_name) {
    CloseTex(tex_file_name);

    char* sys_c = (char*)calloc(200, sizeof(char));
    sprintf(sys_c, "C:/Users/user/AppData/Local/Programs/MiKTeX/miktex/bin/x64/pdflatex.exe %s", tex_file_name);

    system(sys_c);
}