#include "folders.h"
#include "tree.h"
#include "parsing.h"

int main(int argc, char* argv[]) {
    const char* real_data = GetTime();
    MakeDir(real_data);

    const char* param_file = "../params.txt";

    Forest_t* forest = ForestCtor();

    double x0 = 0;
    int der_cnt = 4;
    char* var_name = (char*)"y";
    double x_delta = 0.05, y_delta = 1;
    
    CodeError_t error_code = SetParams(param_file, &forest, &x0, &der_cnt, &var_name, &x_delta, &y_delta);
    printf("\"%s\"\n", forest->buf);
    printf("x0 = %lg, der_cnt = %d, var_name = %s\n", x0, der_cnt, var_name);
    printf("x_delta = %lg, y_delta = %lg\n", x_delta, y_delta);

    if (error_code != NOTHING)
        return 0;
    
    ParseForest(forest);
    SelectForestVars(forest);

    DUMP(forest, 0, NOTHING, "Before Optimization:");
    Optimization(&forest->tree[0]->root);
    DUMP(forest, 0, NOTHING, "Her graphic:\n");

    DUMP(forest, 0, NOTHING, "Let's check this function:");
    PRINT_TO_TEX(forest, "Her graphic:\n");
    PrintPlot(forest->tree[0]->root, forest->tex_file_name, x0, x_delta, y_delta);

    Forest_t* taylor = ForestCtor();
    taylor->tree[0] = Taylor(forest->tree[0], der_cnt, x0, var_name);
    DUMP(taylor, 0, NOTHING, "Taylor:");

    Forest_t* tangent = ForestCtor();
    tangent->tree[0] = Tangent(forest->tree[0], x0, var_name);
    DUMP(tangent, 0, NOTHING, "Tangent:");
    PRINT_TO_TEX(forest, "Tangent's graphic and Taylor's graphic with original function:\n");

    PrintData(forest->tree[0]->root, "function.txt", x0, x_delta, y_delta);
    PrintData(taylor->tree[0]->root, "taylor.txt", x0, x_delta, y_delta);
    PrintData(tangent->tree[0]->root, "tangent.txt", x0, x_delta, y_delta);
    PrintData(forest->tree[0]->root, "x0.txt", x0, 0, 0);

    char* plot_name = GnuPlot(x0, x_delta, 4, "function.txt", "taylor.txt", "tangent.txt", "x0.txt");

    AddPlot(forest->tex_file_name, plot_name);

    PrintPdf(forest->tex_file_name);
}
