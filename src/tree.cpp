#include "tree.h"

#define READ(name)  scanf("%[^\n]", name); \
                    getchar();

Forest_t* ForestCtor() {
    Forest_t* forest = (Forest_t*)calloc(1, sizeof(Forest_t));
    my_assert(forest, CALLOC_ERR, NULL);

    forest->html_file_name = "dump.html";
    forest->dot_file_name = "tree.dot";
    forest->tex_file_name = "../dump.tex";

    for (int i = 0; i < MAX_DIFFER; ++i)
        forest->tree[i] = TreeCtor();

    forest->var_cnt = 0;
    
    return forest;
}

Tree_t* TreeCtor() {
    Tree_t* tree = (Tree_t*)calloc(1, sizeof(Tree_t));
    my_assert(tree, CALLOC_ERR, NULL);

    ValueType* value = (ValueType*)calloc(1, sizeof(ValueType));
    value->value = 0;
    tree->root = NodeCtor(NUM, value, NULL, NULL);
    tree->nodes_cnt = 1;
    tree->name = "function";

    return tree;
}

Node_t* NodeCtor(TYPES type, ValueType* value, Node_t* left, Node_t* right) {
    Node_t* node = (Node_t*)calloc(1, sizeof(Node_t));
    my_assert(node, CALLOC_ERR, NULL);

    node->type = type;
    node->value = value;
    node->left = left;
    node->right = right;

    return node;
}

CodeError_t ForestDtor(Forest_t* forest) {
    my_assert(forest, NULLPTR, NULLPTR);

    free(forest->buf);

    forest->html_file_name = NULL;
    forest->dot_file_name = NULL;
    forest->tex_file_name = NULL;

    for (int i = 0; i < MAX_DIFFER; ++i)
        TreeDtor(forest->tree[i]->root);

    for (int i = 0; i < MAX_VARS_CNT; ++i)
        forest->vars[i] = NULL;

    free(forest);

    return NOTHING;
}

CodeError_t TreeDtor(Node_t* root) {
    my_assert(root, NULLPTR, NULLPTR);

    if (root->left) 
        TreeDtor(root->left);

    if (root->right) 
        TreeDtor(root->right);

    free(root);

    return NOTHING;
}

CodeError_t TreeVerify(Tree_t* tree) {
    if (!tree)
        return NULLPTR;

    if (!tree->root)
        return NULLPTR;

    if (tree->nodes_cnt <= 0)
        return SIZE_ERR;

    int real_size = GetSize(tree->root);

    if (real_size != tree->nodes_cnt)
        return CYCLE_ERR;
    
    return NOTHING;
}

int GetSize(Node_t* root) {
    my_assert(root, NULLPTR, MAX_NODES_CNT);

    int sz = 1;
    if (root->left)
        sz += GetSize(root->left);
    
    if (sz > MAX_NODES_CNT)
        return sz;

    if (root->right)
        sz += GetSize(root->right);

    return sz;
}

CodeError_t SetParams(const char* param_file, Forest_t** forest, double* x0, int* der_cnt, char** var_name, double* x_delta, double* y_delta) {
    if (!param_file || !forest || !x0 || !der_cnt || !var_name || !x_delta || !y_delta)
        return NULLPTR;

    FILE* params = fopen(param_file, "r");
    (*forest)->buf = (char*)calloc(100, sizeof(char));
    fscanf(params, "%[^\n]", (*forest)->buf);

    char* cmd = (char*)calloc(50, sizeof(char));

    while (fscanf(params, "%s", cmd) == 1) {
        if (strcmp(cmd, "set")) {
            printf(RED_COLOR "Wrong param's function\n" RESET_COLOR);
            return VALUE_ERR;
        }

        char* type = (char*)calloc(100, sizeof(char));
        fscanf(params, "%s", type);

        if (!strcmp(type, "x0")) {
            fscanf(params, " %lf", x0);
        }
        else if (!strcmp(type, "var_name")) {
            *var_name = (char*)calloc(50, sizeof(char));
            fscanf(params, "%s", *var_name);
        }
        else if (!strcmp(type, "differ")) {
            fscanf(params, "%d", der_cnt);
        }
        else if (!strcmp(type, "x_delta")) {
            fscanf(params, "%lf", x_delta);
        }
        else if (!strcmp(type, "y_delta")) {
            fscanf(params, "%lf", y_delta);
        }
    }

    fclose(params);

    return NOTHING;
}

CodeError_t ReadBase(Forest_t* forest, const char* file_name) {
    my_assert(forest, NULLPTR, NULLPTR);
    my_assert(file_name, NULLPTR, NULLPTR);

    int file_size = get_file_size(file_name);
    forest->buf = (char*)calloc(file_size + 1, sizeof(char));

    FILE* file_base = fopen(file_name, "r");
    fread(forest->buf, sizeof(char), file_size, file_base);
    fclose(file_base);

    return NOTHING;
}

int get_file_size(const char* file_name) {
    my_assert(file_name, NULLPTR, 0);

    struct stat file_stat; 
    int stat_result = stat(file_name, &file_stat);
    my_assert(!stat_result, FILE_ERR, 0);

    return file_stat.st_size;
}

CodeError_t ParseForest(Forest_t* forest) {
    if (!forest || !forest->buf)
        return NULLPTR;

    const char* cur_pos = forest->buf;
    forest->tree[0] = TreeCtor();
    forest->tree[0]->root = GetG(&cur_pos);

    return NOTHING;
}

CodeError_t SelectForestVars(Forest_t* forest) {
    if (!forest)
        return NULLPTR;

    SelectTreeVars(forest->tree[0]->root, forest);

    return NOTHING;
}

void SelectTreeVars(Node_t* node, Forest_t* forest) {
    if (!node || !forest) 
        return;

    if (node->type == NUM)
        return;

    if (node->type == VAR) {
        if (!CheckVars(forest, node->value->name))
            forest->vars[forest->var_cnt++] = VarCtor(node->value->name);
        return;
    }

    SelectTreeVars(node->left, forest);
    SelectTreeVars(node->right, forest);

    return;
}

bool CheckVars(Forest_t* forest, char* var_name) {
    if (!forest || !var_name)
        return false;

    for (int i = 0; i < forest->var_cnt; ++i) {
        if (!strcmp(forest->vars[i]->name, var_name))
            return true;
    }
    
    return false;
}

Var_t* VarCtor(char* name) {
    Var_t* new_var = (Var_t*)calloc(1, sizeof(Var_t));
    new_var->name = name;
    
    return new_var;
}

ValueType* ValueOperCtor(int type) {
    ValueType* value = (ValueType*)calloc(1, sizeof(ValueType));
    value->type = type;
    return value;
}

ValueType* ValueVarCtor(char* name) {
    ValueType* value = (ValueType*)calloc(1, sizeof(ValueType));
    value->name = name;
    return value;
}

ValueType* ValueNumCtor(double value) {
    ValueType* val_type = (ValueType*)calloc(1, sizeof(ValueType));
    val_type->value = value;
    return val_type;
}

double CalculatingTree(Forest_t* forest, int ind) {
    my_assert(forest, NULLPTR, NAN);
    my_assert(ind >= 0, IND_ERR, NAN);
    my_assert(ind < MAX_DIFFER, IND_ERR, NAN);

    printf("Enter the values of vars:\n");
    for (int i = 0; i < forest->var_cnt; ++i) {
        printf("%s = ", forest->vars[i]->name);
        scanf("%lg", &forest->vars[i]->value);
    }

    return CalculatingNode(forest->tree[ind]->root, forest);
}

double CalculatingNode(Node_t* node, Forest_t* forest) {
    if (!node)
        return NAN;

    if (node->type == NUM)
        return node->value->value;

    if (node->type == VAR) {
        for (int i = 0; i < forest->var_cnt; ++i) {
            if (!strcmp(forest->vars[i]->name, node->value->name))
                return forest->vars[i]->value;
        }

        return NAN;
    }

    double left_res = CalculatingNode(node->left, forest);
    double right_res = CalculatingNode(node->right, forest);

    double result = opers[node->value->type].func(NUM_NODE(left_res), NUM_NODE(right_res));

    return result;
}

double CalcConstNode(Node_t* node, double value) {
    if (!node)
        return NAN;

    if (node->type == NUM)
        return node->value->value;

    if (node->type == VAR)
        return value;

    double left_res = CalcConstNode(node->left, value);
    double right_res = CalcConstNode(node->right, value);

    double result = opers[node->value->type].func(NUM_NODE(left_res), NUM_NODE(right_res));

    return result;
}

Node_t* CopyNode(Node_t* node) {
    if (!node)
        return NULL;

    Node_t* new_node = (Node_t*)calloc(1, sizeof(Node_t));
    new_node->type = node->type;
    new_node->value = (ValueType*)calloc(1, sizeof(ValueType));
    *(new_node->value) = *(node->value);

    new_node->left = CopyNode(node->left);
    new_node->right = CopyNode(node->right);

    return new_node;
}

double GetValue(Node_t* node) {
    if (!node || !node->value)
        return NAN;

    return node->value->value;
}

Node_t* GetLeft(Node_t* node) {
    if (!node)
        return NULL;

    return node->left;
}

Node_t* GetRight(Node_t* node) {
    if (!node)
        return NULL;

    return node->right;
}

void ConvolConst(Node_t** node) {
    if (!node || !(*node))
        return;

    if ((*node)->type == NUM || (*node)->type == VAR) 
        return;

    #define is_const(node, dir) ((node)->dir && (node)->dir->type == NUM) 
    int oper = (*node)->value->type;

    if (oper != OP_ADD && oper != OP_SUB && oper != OP_MUL && oper != OP_POW && oper != OP_LOG && oper != OP_DIV) {
        if (!is_const(*node, right))
            return;

        (*node)->type = NUM;
        (*node)->value->value = opers[oper].func(GetLeft(*node), GetRight(*node));

        (*node)->left = (*node)->right = NULL;
    }
    else if (is_const(*node, left) && is_const(*node, right)) {
        (*node)->type = NUM;
        (*node)->value->value = opers[oper].func(GetLeft(*node), GetRight(*node));

        (*node)->left = (*node)->right = NULL;
    }
    #undef is_const

    return;
}

void RemovingNeutral(Node_t** node) {
    if (!node || !(*node))
        return;

    if ((*node)->type == NUM || (*node)->type == VAR) 
        return;

    #define is_const(node, dir) ((node)->dir && (node)->dir->type == NUM)
    #define is_null(node, dir) (is_const(node, dir) && (GetValue((node)->dir) == double(0)))
    #define is_one(node, dir) (is_const(node, dir) && (GetValue((node)->dir) == double(1)))
    
    int oper = (*node)->value->type;
    
    if ((*node)->type == OPER && (*node)->value->type == OP_MUL && is_const(*node, right)) {
        Node_t* add_son = (*node)->right;
        (*node)->right = (*node)->left;
        (*node)->left = add_son;
    }

    switch (oper) {
        case OP_ADD:
            if (is_null(*node, right))
                *node = GetLeft(*node);
            else if (is_null(*node, left))
                *node = GetRight(*node);
            break;
        case OP_SUB:
            if (is_null(*node, right))
                *node = GetLeft(*node);
            break;
        case OP_DIV:
            if (is_one(*node, right))
                *node = GetLeft(*node);
            break;
        case OP_POW:
            if (is_one(*node, right))
                *node = GetLeft(*node);
            else if (is_null(*node, right) || is_one(*node, left))
                *node = NUM_NODE(1);
            break;
        case OP_MUL:
            Node_t* right = GetRight(*node);
            Node_t* last_node = *node;
            if (is_const(*node, left) && right &&  right->type == OPER && right->value->type == OP_MUL && is_const(right, left)) {
                double value = GetValue((*node)->left);
                *node = right;
                (*node)->left->value->value *= value; 
                return;
            }

            if (is_one(*node, left)) {
                *node = GetRight(*node);
                return;
            }
            else if (is_one(*node, right)) {
                *node = GetLeft(*node);
                return;
            }

            if (is_null(*node, left) || is_null(*node, right)) {
                *node = NUM_NODE(0);
                return;
            }
    }

    #undef is_num
    #undef is_null
    #undef is_one

    return;
}

CodeError_t Optimization(Node_t** node_ptr) {
    if (!node_ptr || !(*node_ptr))
        return NULLPTR;

    Optimization(&((*node_ptr)->left));
    Optimization(&((*node_ptr)->right));

    ConvolConst(node_ptr);
    RemovingNeutral(node_ptr);
    
    return NOTHING;
}