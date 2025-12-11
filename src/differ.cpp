#include "differ.h"
#include "tree.h"

Node_t* Differ(Node_t* node, char* var_name) {
    my_assert(node, NULLPTR, NULL);

    if (node->type == NUM)
        return NodeCtor(NUM, ValueNumCtor(0), NULL, NULL);

    if (node->type == VAR) {
        if (!strcmp(node->value->name, var_name))
            return NodeCtor(NUM, ValueNumCtor(1), NULL, NULL);
        
        return NodeCtor(NUM, ValueNumCtor(0), NULL, NULL);
    }

    #define d(node) Differ(node, var_name)

    Node_t* new_node = NULL;

    switch (node->value->type) {
        case OP_ADD:
            new_node = OP_NODE(OP_ADD, d(node->left), d(node->right));
            return new_node;

        case OP_SUB:
            new_node = OP_NODE(OP_SUB, d(node->left), d(node->right));
            return new_node;

        case OP_MUL:
            new_node = OP_NODE(OP_ADD, NULL, NULL);
            new_node->left = OP_NODE(OP_MUL, d(node->left), c(node->right));
            new_node->right = OP_NODE(OP_MUL, c(node->left), d(node->right));
            return new_node;
        
        case OP_DIV:
            new_node = OP_NODE(OP_DIV, NULL, NULL);

            new_node->left = OP_NODE(OP_SUB, NULL, NULL);
            new_node->left->left = OP_NODE(OP_MUL, d(node->left), c(node->right));
            new_node->left->right = OP_NODE(OP_MUL, c(node->left), d(node->right));

            new_node->right = OP_NODE(OP_POW, c(node->right), NUM_NODE(2));
            return new_node;
        
        case OP_COS:
            new_node = OP_NODE(OP_MUL, NULL, d(node->right));

            new_node->left = OP_NODE(OP_MUL, NUM_NODE(-1), OP_NODE(OP_SIN, NULL, c(node->right)));
            return new_node;

        case OP_SIN:
            new_node = OP_NODE(OP_MUL, OP_NODE(OP_COS, NULL, c(node->right)), d(node->right));
            return new_node;

        case OP_POW:
            if (node->right && node->right->type == NUM) {
                int power = GetValue(node->right);
                return OP_NODE(OP_MUL, OP_NODE(OP_MUL, NUM_NODE(power), OP_NODE(OP_POW, c(node->left), NUM_NODE(power - 1))), d(node->left));
            }

            if (node->left && node->left->type == NUM) {
                
            }

            return d(OP_NODE(OP_EXP, NULL, OP_NODE(OP_MUL, c(node->right), OP_NODE(OP_LN, NULL, c(node->left)))));
        
        case OP_LN:
            return OP_NODE(OP_DIV, d(node->right), c(node->right));

        case OP_LOG:
            return d(OP_NODE(OP_DIV, OP_NODE(OP_LN, NULL, c(node->right)), OP_NODE(OP_LN, NULL, c(node->left))));

        case OP_EXP:
            return OP_NODE(OP_MUL, c(node), d(node->right));
        
        case OP_TAN:
            return OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_POW, OP_NODE(OP_COS, NULL, c(node->right)), NUM_NODE(2)));
        
        case OP_COT:
            return OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_POW, OP_NODE(OP_SIN, NULL, c(node->right)), NUM_NODE(2)));
        
        case OP_ASIN:
            return OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_POW, OP_NODE(OP_SUB, NUM_NODE(1), OP_NODE(OP_POW, c(node->right), NUM_NODE(2))), NUM_NODE(0.5)));
        
        case OP_ACOS:
            return OP_NODE(OP_MUL, NUM_NODE(-1), OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_POW, OP_NODE(OP_SUB, NUM_NODE(1), OP_NODE(OP_POW, c(node->right), NUM_NODE(2))), NUM_NODE(0.5))));

        case OP_ATAN:
            return OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_ADD, NUM_NODE(1), OP_NODE(OP_POW, c(node->right), NUM_NODE(2))));
        
        case OP_ACOT:
            return OP_NODE(OP_MUL, NUM_NODE(-1), OP_NODE(OP_DIV, d(node->right), OP_NODE(OP_ADD, NUM_NODE(1), OP_NODE(OP_POW, c(node->right), NUM_NODE(2)))));
    }

    #undef d

    return NULL;
}

Tree_t* N_Differ(Tree_t* tree, int n, char* var_name) {
    Tree_t* n_tree = TreeCtor();
    *n_tree = *tree;

    for (int i = 0; i < n; ++i) {
        n_tree->root = Differ(n_tree->root, var_name);
        Optimization(&n_tree->root);
    }

    n_tree->nodes_cnt = GetSize(n_tree->root);

    return n_tree;
}

Tree_t* Tangent(Tree_t* tree, double x0, char* var_name) {
    Tree_t* deriv = N_Differ(tree, 1, var_name);
    double k = CalcConstNode(deriv->root, x0);
    double b = CalcConstNode(tree->root, x0);

    Tree_t* tangent = TreeCtor();
    tangent->root = OP_NODE(OP_ADD, OP_NODE(OP_MUL, NUM_NODE(k), OP_NODE(OP_SUB, VAR_NODE(var_name), NUM_NODE(x0))), NUM_NODE(b));
    tangent->nodes_cnt = GetSize(tangent->root);

    return tangent;
}

Tree_t* Taylor(Tree_t* tree, int n, double x0, char* var_name) {
    Forest_t* forest = ForestCtor();
    forest->tree[0] = tree;

    Tree_t* answer = TreeCtor();
    
    double fact = 1;

    for (int i = 0; i <= n; ++i) {
        Optimization(&answer->root);
        Optimization(&forest->tree[i]->root);
        //DUMP(forest, i, NOTHING, "Derivative:");

        double coef = CalcConstNode(forest->tree[i]->root, x0);

        answer->root = OP_NODE(OP_ADD, c(answer->root), OP_NODE(OP_MUL, OP_NODE(OP_DIV, NUM_NODE(coef), NUM_NODE(fact)), OP_NODE(OP_POW, OP_NODE(OP_SUB, VAR_NODE(var_name), NUM_NODE(x0)), NUM_NODE(i))));

        forest->tree[i + 1] = TreeCtor();
        forest->tree[i + 1]->root = Differ(forest->tree[i]->root, var_name);

        fact *= (i + 1);
    }

    Optimization(&answer->root);
    answer->nodes_cnt = GetSize(answer->root);

    return answer;
}
