#include "folders.h"
#include "tree.h"
#include "parsing.h"

int main(int argc, char* argv[]) {
    const char* real_data = GetTime();
    MakeDir(real_data);

    const char* param_file = "../params.txt";
    const char* tex_file = "../dump.tex";

    Forest_t* function = ForestCtor();
    double x0 = 0;
    int der_cnt = 4;
    char* var_name = (char*)"y";
    double x_delta = 0.05, y_delta = 1;
    
    CodeError_t error_code = SetParams(param_file, &function, &x0, &der_cnt, &var_name, &x_delta, &y_delta);
    printf("\"%s\"\n", function->buf);
    printf("x0 = %lg, der_cnt = %d, var_name = %s\n", x0, der_cnt, var_name);
    printf("x_delta = %lg, y_delta = %lg\n", x_delta, y_delta);

    if (error_code != NOTHING)
        return 0;
    
    ParseForest(function);
    SelectForestVars(function);
    DUMP(function, 0, NOTHING, "Before Optimization:");
    Optimization(&function->tree[0]->root);
    DUMP(function, 0, NOTHING, "Let's check this function:");
    PRINT_TO_TEX(function, "Her graphic:\n");
    PrintPlot(function->tree[0], tex_file, x0, x_delta, y_delta);

    Forest_t* taylor = ForestCtor();
    Forest_t* tangent = ForestCtor();
    taylor->tree[0] = Taylor(function->tree[0], der_cnt, x0, var_name);
    tangent->tree[0] = Tangent(function->tree[0], x0, var_name);
    
    PRINT_TO_TEX(function, "\\section*{Graphics}");
    PRINT_TO_TEX(function, "Tangent's graphic and Taylor's graphic with original function:\n");
    double y0 = CalcConstNode(function->tree[0]->root, x0);
    char* plot_name = GnuPlot(x0, x_delta, y0, y_delta, 3, function->tree[0], taylor->tree[0], tangent->tree[0]);
    AddPlot(tex_file, plot_name);
    
    PrintPdf(function->tex_file_name);
}
