#include "tex_utils.h"
#include "tree.h"

CodeError_t TexDump(Forest_t* forest, int ind, const char* add_msg) {
    my_assert(forest, NULLPTR, NULLPTR);
    my_assert(ind >= 0, IND_ERR, IND_ERR);
    my_assert(ind < MAX_DIFFER, IND_ERR, IND_ERR);

    static int tex_counter = 0;
    if (tex_counter++ == 0) {
        FILE* tex_top = fopen(forest->tex_file_name, "w");
        fprintf(tex_top, "\\documentclass{article}\n\\usepackage{graphicx}\n\\usepackage[utf8x]{inputenc}\n\\usepackage[english,russian]{babel}\n\\begin{document}\n");
        fclose(tex_top);
    }

    FILE* tex_file = fopen(forest->tex_file_name, "a");

    fprintf(tex_file, "%s", add_msg);

    fprintf(tex_file, "\\[");
    TexPrinting(forest->tree[ind]->root, tex_file, OPER_CNT, 0);
    fprintf(tex_file, "\\]\n");

    fclose(tex_file);

    return NOTHING;
} 

CodeError_t TexPrinting(Node_t* node, FILE* file, int prev_oper, int son_type) {
    if (!node)
        return NOTHING;
    
    if (node->type == NUM) {
        if (prev_oper == OP_EXP || (prev_oper == OP_LOG && son_type == 0) || (prev_oper == OP_POW && son_type == 1)) {
            fprintf(file, "%lg", node->value->value);
        }
        else {
            if (node->value->value < 0)
                fprintf(file, "(%lg)", node->value->value);
            else
                fprintf(file, "%lg", node->value->value);
        }
        return NOTHING;
    }

    if (node->type == VAR) {
        fprintf(file, "%s", node->value->name);
        return NOTHING;
    }

    bool is_par = false;
    int oper = node->value->type;

    if (oper == OP_ADD || oper == OP_SUB || oper == OP_MUL || prev_oper == OP_POW) {
        if (son_type == 1) {
            if (prev_oper != OP_DIV && prev_oper != OP_ADD && prev_oper != OP_POW && prev_oper != OP_EXP)
                is_par = true;
        }
        else {
            if (prev_oper == OP_MUL || prev_oper == OP_POW)
                is_par = true;
        }
    }

    if (is_par)
        fprintf(file, "(");

    if (oper == OP_LOG)
        fprintf(file, "\\log_");
    else if (oper == OP_DIV)
        fprintf(file, "\\frac");

    fprintf(file, "{");
    TexPrinting(node->left, file, oper, 0);
    fprintf(file, "} %s {", opers[oper].tex_view);
    TexPrinting(node->right, file, oper, 1);
    fprintf(file, "}");

    if (is_par)
        fprintf(file, ")");

    return NOTHING;
}

void PrintToTex(const char* tex_file_name, const char* message) {
    FILE* tex_file = fopen(tex_file_name, "a");
    fprintf(tex_file, "%s", message);
    fclose(tex_file);
}

CodeError_t CloseTex(const char* tex_file_name) {
    FILE* tex_file = fopen(tex_file_name, "a");
    fprintf(tex_file, "\n\\end{document}\n");
    fclose(tex_file);

    return NOTHING;
}