#include "differ.h"
#include "tree.h"

Node_t* Differ(Node_t* node, char* var_name) {
    my_assert(node, NULLPTR, NULL);

    if (node->type == NUM) {
        // WriteToTex("../dump.tex", "\\begin{center}Constant's derivative equal 0:\n \\begin{math}\\frac{d}{dx}\\end{math}(\n");
        // PRINT_TEX_NODE(node);
        // WriteToTex("../dump.tex", ") = 0 \\end{center}");
        return NUM_NODE(0);
    }

    if (node->type == VAR) {
        // WriteToTex("../dump.tex", "\\begin{center}Var's derivative equal 0 or 1:\n \\begin{math}\\frac{d}{dx}\\end{math}(\n");
        // PRINT_TEX_NODE(node);
        if (!strcmp(node->value->name, var_name)) {
            // WriteToTex("../dump.tex", ") = 1 \\end{center}");
            return NUM_NODE(1);
        }
        
        // WriteToTex("../dump.tex", ") = 0 \\end{center}");
        return NUM_NODE(0);
    }

    #define d(node) Differ(node, var_name)

    Node_t* new_node = NULL;

    switch (node->value->type) {
        case OP_ADD:
            new_node = OP_NODE(OP_ADD, d(node->left), d(node->right));
            break;

        case OP_SUB:
            new_node = OP_NODE(OP_SUB, d(node->left), d(node->right));
            break;

        case OP_MUL:
            new_node = OP_NODE(OP_ADD, NULL, NULL);
            new_node->left = OP_NODE(OP_MUL, d(node->left), c(node->right));
            new_node->right = OP_NODE(OP_MUL, c(node->left), d(node->right));
            break;
        
        case OP_DIV:
            new_node = OP_NODE(OP_DIV, NULL, NULL);

            new_node->left = OP_NODE(OP_SUB, NULL, NULL);
            new_node->left->left = OP_NODE(OP_MUL, d(node->left), c(node->right));
            new_node->left->right = OP_NODE(OP_MUL, c(node->left), d(node->right));

            new_node->right = OP_NODE(OP_POW, c(node->right), NUM_NODE(2));

            break;
        
        case OP_COS:
            new_node = OP_NODE(OP_MUL, NULL, d(node->right));

            new_node->left = OP_NODE(OP_MUL, NUM_NODE(-1), OP_NODE(OP_SIN, NULL, c(node->right)));
            break;

        case OP_SIN:
            new_node = OP_NODE(OP_MUL, OP_NODE(OP_COS, NULL, c(node->right)), d(node->right));
            break;

        case OP_POW:
            if (node->right && node->right->type == NUM) {
                int power = GetValue(node->right);
                new_node = OP_NODE(OP_MUL, OP_NODE(OP_MUL, NUM_NODE(power), OP_NODE(OP_POW, c(node->left), NUM_NODE(power - 1))), d(node->left));
            }

            else if (node->left && node->left->type == NUM) {
                new_node = OP_NODE(OP_MUL, c(node), OP_NODE(OP_LN, NULL, c(node->left)));
            }

            else {
                new_node = d(OP_NODE(OP_EXP, NULL, OP_NODE(OP_MUL, c(node->right), OP_NODE(OP_LN, NULL, c(node->left)))));
            }

            break;

        case OP_LN:
            new_node = OP_NODE(OP_DIV, d(node->right), c(node->right));
            break;

        case OP_LOG:
            new_node = d(OP_NODE(OP_DIV, OP_NODE(OP_LN, NULL, c(node->right)), OP_NODE(OP_LN, NULL, c(node->left))));
            break;

        case OP_EXP:
            new_node = OP_NODE(OP_MUL, c(node), d(node->right));
            break;
        
        case OP_TAN:
            new_node = OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_POW, OP_NODE(OP_COS, NULL, c(node->right)), NUM_NODE(2)));
            break;
        
        case OP_COT:
            new_node = OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_POW, OP_NODE(OP_SIN, NULL, c(node->right)), NUM_NODE(2)));
            break;
        
        case OP_ASIN:
            new_node = OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_POW, OP_NODE(OP_SUB, NUM_NODE(1), OP_NODE(OP_POW, c(node->right), NUM_NODE(2))), NUM_NODE(0.5)));
            break;
        
        case OP_ACOS:
            new_node = OP_NODE(OP_MUL, NUM_NODE(-1), OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_POW, OP_NODE(OP_SUB, NUM_NODE(1), OP_NODE(OP_POW, c(node->right), NUM_NODE(2))), NUM_NODE(0.5))));
            break;

        case OP_ATAN:
            new_node = OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_ADD, NUM_NODE(1), OP_NODE(OP_POW, c(node->right), NUM_NODE(2))));
            break;
        
        case OP_ACOT:
            new_node = OP_NODE(OP_MUL, NUM_NODE(-1), OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_ADD, NUM_NODE(1), OP_NODE(OP_POW, c(node->right), NUM_NODE(2)))));
            break;
    }

    #undef d

    /*
    WriteToTex("../dump.tex", "\\begin{center}Derivative of complex function:\n \\begin{math}\\frac{d}{dx}\\end{math}(\n");
    PRINT_TEX_NODE(node);
    WriteToTex("../dump.tex", ") = \\end{center}\n\\begin{center}");
    PRINT_TEX_NODE(new_node);
    WriteToTex("../dump.tex", "\\end{center}]\n");
    */
    return new_node;
}

Tree_t* N_Differ(Tree_t* tree, int n, char* var_name) {
    Tree_t* n_tree = TreeCtor();
    *n_tree = *tree;

    for (int i = 0; i < n; ++i) {
        n_tree->root = Differ(n_tree->root, var_name);
        Optimization(&n_tree->root);
    }

    n_tree->nodes_cnt = GetSize(n_tree->root);
    n_tree->name = "differ";
    
    return n_tree;
}

Tree_t* Tangent(Tree_t* tree, double x0, char* var_name) {
    PrintToTex("../dump.tex", "\\section*{Tangent}");
    Tree_t* deriv = N_Differ(tree, 1, var_name);
    double k = CalcConstNode(deriv->root, x0);
    double b = CalcConstNode(tree->root, x0);

    Tree_t* tangent = TreeCtor();
    tangent->root = OP_NODE(OP_ADD, OP_NODE(OP_MUL, NUM_NODE(k), OP_NODE(OP_SUB, VAR_NODE(var_name), NUM_NODE(x0))), NUM_NODE(b));
    tangent->nodes_cnt = GetSize(tangent->root);

    TREE_DUMP(tangent, "Tangent:");
    tangent->name = "tangent";

    return tangent;
}

Tree_t* Taylor(Tree_t* tree, int n, double x0, char* var_name) {
    char* sec_name = (char*)calloc(MAX_VAR_SIZE, sizeof(char));
    sprintf(sec_name, "\\section*{Taylor to %d degree}", n);
    PrintToTex("../dump.tex", sec_name);
    
    Forest_t* forest = ForestCtor();
    forest->tree[0] = tree;

    Tree_t* answer = TreeCtor();
    double fact = 1;
    char* deriv = (char*)calloc(MAX_VAR_SIZE, sizeof(char));

    for (int i = 0; i <= n; ++i) {
        Optimization(&answer->root);
        Optimization(&forest->tree[i]->root);
        if (i <= 5) {
            sprintf(deriv, "Derivative %d:", i);
            DUMP(forest, i, NOTHING, deriv);
        }

        double coef = CalcConstNode(forest->tree[i]->root, x0) / fact;
        answer->root = OP_NODE(OP_ADD, c(answer->root), OP_NODE(OP_MUL, NUM_NODE(coef), OP_NODE(OP_POW, OP_NODE(OP_SUB, VAR_NODE(var_name), NUM_NODE(x0)), NUM_NODE(i))));

        forest->tree[i + 1] = TreeCtor();
        forest->tree[i + 1]->root = Differ(forest->tree[i]->root, var_name);
        fact *= (i + 1);
    }

    Optimization(&answer->root);
    answer->nodes_cnt = GetSize(answer->root);

    TREE_DUMP(answer, "Taylor:");
    answer->name = "taylor";

    return answer;
}
