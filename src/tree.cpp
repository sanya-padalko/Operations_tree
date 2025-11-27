#include "tree.h"

#define READ(name)  scanf("%[^\n]", name); \
                    getchar();

#define ull unsigned long long

Forest_t* ForestCtor() {
    Forest_t* forest = (Forest_t*)calloc(1, sizeof(Forest_t));
    my_assert(forest, CALLOC_ERR, NULL);

    forest->html_file_name = "dump.html";
    forest->dot_file_name = "tree.dot";
    forest->tex_file_name = "dump.tex";

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

int CheckOper(const char* str) {
    for (int i = 0; i < OPER_CNT; ++i) {
        if (!strcmp(str, opers[i].base_view))
            return i;
    }

    return -1;
}

Node_t* ParseBase(char** cur_pos, Forest_t* forest) {
    my_assert(cur_pos, NULLPTR, NULL);
    my_assert(*cur_pos, NULLPTR, NULL);
    my_assert(forest, NULLPTR, NULL);

    ValueType* value = (ValueType*)calloc(1, sizeof(ValueType));
    value->value = 0;

    if (**cur_pos == '(') {
        ++*cur_pos;

        Node_t* node = NodeCtor(NUM, value, NULL, NULL);
        int read_bytes = 0;

        char* str = (char*)calloc(100, sizeof(char));
        sscanf(*cur_pos, " %[^ ] %n", str, &read_bytes);
        *cur_pos += read_bytes;

        int oper_ind = CheckOper(str);

        if (oper_ind != -1) {
            node->type = OPER;
            node->value->type = opers[oper_ind].type;
        }
        else if (isdigit(str[0])) {
            node->type = NUM;
            node->value->value = atoi(str);
        }
        else {
            node->type = VAR;
            node->value->name = str;

            forest->vars[forest->var_cnt++] = VarCtor(str);
        }

        node->left = ParseBase(cur_pos, forest);
        node->right = ParseBase(cur_pos, forest);

        if (**cur_pos != ')') {
            printerr(RED_COLOR "ERRORS IN TEXT OF BASE\n" RESET_COLOR);
            return node;
        }

        ++*cur_pos;
        sscanf(*cur_pos, " %n", &read_bytes);
        *cur_pos += read_bytes;

        return node;
    }

    if (**cur_pos == 'n') {
        *cur_pos += strlen("nil");

        int read_bytes = 0;
        sscanf(*cur_pos, " %n", &read_bytes);
        *cur_pos += read_bytes;
        return NULL;
    }

    printerr(RED_COLOR "ERRORS IN TEXT OF BASE\n" RESET_COLOR);
    return NULL;
}

Var_t* VarCtor(char* name) {
    Var_t* new_var = (Var_t*)calloc(1, sizeof(Var_t));
    new_var->name = name;
    
    return new_var;
}

CodeError_t SeparateVars(Forest_t* forest) {
    my_assert(forest, NULLPTR, NULLPTR);

    for (int i = 0; i < forest->var_cnt; ++i) {
        char* name = forest->vars[i]->name;
        while (*name != '\0')
            ++name;

        *name = '\0';
    }

    return NOTHING;
}

CodeError_t TexDump(Forest_t* forest, int ind, const char* add_msg) {
    my_assert(forest, NULLPTR, NULLPTR);
    my_assert(ind >= 0, IND_ERR, IND_ERR);
    my_assert(ind < MAX_DIFFER, IND_ERR, IND_ERR);

    static int tex_counter = 0;

    FILE* tex_file = fopen(forest->tex_file_name, "a");

    if (tex_counter++ == 0)
        fprintf(tex_file, "\\documentclass{article}\n\\usepackage[utf8x]{inputenc}\n\\usepackage[english,russian]{babel}\n\\begin{document}\n");

    fprintf(tex_file, "%s", add_msg);
    fprintf(tex_file, "\\[");
    TexPrinting(forest->tree[ind]->root, tex_file);
    fprintf(tex_file, "\\]\n");

    fclose(tex_file);

    return NOTHING;
} 

CodeError_t TexPrinting(Node_t* node, FILE* file) {
    if (!node)
        return NOTHING;
    
    if (node->type == NUM) {
        fprintf(file, "%lg", node->value->value);
        return NOTHING;
    }

    if (node->type == VAR) {
        fprintf(file, "%s", node->value->name);
        return NOTHING;
    }

    fprintf(file, "%s", opers[node->value->type].first_tex_view);

    TexPrinting(node->left, file);
    fprintf(file, "%s", opers[node->value->type].second_tex_view);

    TexPrinting(node->right, file);

    fprintf(file, "%s", opers[node->value->type].third_tex_view);

    return NOTHING;
}

CodeError_t CloseTex(Forest_t* forest) {
    my_assert(forest, NULLPTR, NULLPTR);

    FILE* tex_file = fopen(forest->tex_file_name, "a");
    fprintf(tex_file, "\n\\end{document}\n");
    fclose(tex_file);

    return NOTHING;
}

CodeError_t Dump(Forest_t* forest, int ind, VarInfo varinfo) {
    my_assert(forest, NULLPTR, NULLPTR);
    my_assert(ind >= 0, IND_ERR, IND_ERR);
    my_assert(ind < MAX_DIFFER, IND_ERR, IND_ERR);

    static int dump_counter = 0;

    FILE* dump_file = fopen(forest->html_file_name, "a");
    fprintf(dump_file, "<h2 style = \"color: blue\"> %s </h2>\n", varinfo.add_info);
    const char* color = "green";
    if (varinfo.error_code != NOTHING) {
        fprintf(dump_file, "<h2 style = \"color: red\"> ERROR: %s</h2>\n", ErrorType(varinfo.error_code));
        color = "red";
    }

    fprintf(dump_file, "<h2 style=\"color: %s\"> TreeDump called from %s, function: %s, line %d, list name: %s</h2>\n", color, varinfo.file_name, varinfo.func_name, varinfo.line, varinfo.name);

    Tree_t* tree = forest->tree[ind];

    if (tree->root == NULL) {
        fprintf(dump_file, "<h3 style=\"color: red\">Root of tree pointer equal NULL</h3>\n");
        return NULLPTR;
    }

    fprintf(dump_file, "<h3>");
    Printing(forest->tree[ind]->root, dump_file);
    fprintf(dump_file, "</h3>");

    TreeImgDump(forest, ind);

    char dot_str[100] = {};
    sprintf(dot_str, "\"C:\\Program Files\\Graphviz\\bin\\dot.exe\" tree.dot -T svg -o result%d.svg", dump_counter);
    system(dot_str);

    char img_path[100] = {};
    sprintf(img_path, "result%d.svg", dump_counter++);
    fprintf(dump_file, "<img src=\"%s\">\n", img_path);
    fprintf(dump_file, "<hr>\n");
    fclose(dump_file);

    TexDump(forest, ind, varinfo.add_info);

    return NOTHING;
}

void TreeImgDump(Forest_t* forest, int ind) {
    FILE* dot_file = fopen(forest->dot_file_name, "w");
    fprintf(dot_file, "digraph G {\n");

    RecDump(forest->tree[ind]->root, dot_file);

    fprintf(dot_file, "}");
    fclose(dot_file);
}

void RecDump(Node_t* root, FILE* dot_file) {
    fprintf(dot_file, "\tNode%llX[shape = Mrecord, style = \"filled\", fillcolor = \"#%06x\", label = <\n\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"5\">\n\t\t<TR>\n\t\t\t<TD> ptr: 0x%llX </TD>\n\t\t</TR>\n\n\t\t<TR>\n\t\t\t<TD> ", (ull)root, (unsigned int)CalcHash((long long)root), (ull)root);
    
    if (root->type == OPER) {
        fprintf(dot_file, "%s", opers[root->value->type].dump_view);
    } 
    else if (root->type == NUM) {
        fprintf(dot_file, "%lg", root->value->value);
    }
    else {
        fprintf(dot_file, "%s", root->value->name);
    }

    fprintf(dot_file, " </TD>\n\t\t</TR>\n\n\t\t<TR>\n\t\t\t<TD BGCOLOR = \"#%06x\"> left: ", (unsigned int)CalcHash((long long)root->left));
    if (root->left)
        fprintf(dot_file, "0x%llX", (ull)root->left);
    else
        fprintf(dot_file, "NULL");

    fprintf(dot_file, " </TD>\n\t\t</TR>\n\n\t\t<TR>\n\t\t\t<TD BGCOLOR = \"#%06x\"> right: ", (unsigned int)CalcHash((long long)root->right));
    if (root->right)
        fprintf(dot_file, "0x%llX", (ull)root->right);
    else
        fprintf(dot_file, "NULL");

    fprintf(dot_file, "</TD>\n\t\t</TR>\n\t</TABLE>>];\n\n");

    if (root->left) {
        RecDump(root->left, dot_file);
        fprintf(dot_file, "\tNode%llX->Node%llX;\n", (ull)root, (ull)root->left);
    }
    if (root->right) {
        RecDump(root->right, dot_file);
        fprintf(dot_file, "\tNode%llX->Node%llX;\n", (ull)root, (ull)root->right);
    }
}

int CalcHash(long long p) {
    int c = (int)p;

    c = (c ^ (c >> 16)) * 0xC4CEB9FE;
    c = (c ^ (c >> 13)) * 0xFF51AFD7;
    c = c ^ (c >> 16);

    return (c >> 8) | 0x00808080;
}

CodeError_t Printing(Node_t* node, FILE* file) {
    if (!node)
        return NOTHING;
    
    if (node->type == NUM) {
        fprintf(file, "%lg", node->value->value);
        return NOTHING;
    }

    if (node->type == VAR) {
        fprintf(file, "%c", node->value->name);
        return NOTHING;
    }

    fprintf(file, "%s", opers[node->value->type].first_print_view);
    Printing(node->left, file);

    fprintf(file, "%s", opers[node->value->type].second_print_view);
    Printing(node->right, file);

    fprintf(file, "%s", opers[node->value->type].third_print_view);

    return NOTHING;
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

#define NUM_Ctor(x) NodeCtor(NUM, ValueNumCtor(x), NULL, NULL)
double values[26] = {0};

double CalculatingTree(Forest_t* forest, int ind) {
    my_assert(forest, NULLPTR, NAN);
    my_assert(ind >= 0, IND_ERR, NAN);
    my_assert(ind < MAX_DIFFER, IND_ERR, NAN);

    printf("Enter the values of vars:\n");
    for (int i = 0; i < forest->var_cnt; ++i) {
        printf("%s: ", forest->vars[i]->name);
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

    return opers[node->value->type].func(NUM_Ctor(left_res), NUM_Ctor(right_res));
}

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
    #define c(node) CopyNode(node)
    #define OP_NODE(OPNAME, left, right) NodeCtor(OPER, ValueOperCtor(OPNAME), left, right)

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

            new_node->right = OP_NODE(OP_POW_C, c(node->right), NUM_Ctor(2));
            return new_node;
        
        case OP_COS:
            new_node = OP_NODE(OP_MUL, NULL, d(node->right));

            new_node->left = OP_NODE(OP_MUL, NUM_Ctor(-1), OP_NODE(OP_SIN, NULL, c(node->right)));
            return new_node;

        case OP_SIN:
            new_node = OP_NODE(OP_MUL, OP_NODE(OP_COS, NULL, c(node->right)), d(node->right));
            return new_node;

        case OP_POW_C:
            my_assert(node->right->type == NUM, VALUE_ERR, NULL);

            return OP_NODE(OP_MUL, d(node->left), OP_NODE(OP_MUL, NUM_Ctor(GetValue(node->right)), OP_NODE(OP_POW_C, c(node->left), NUM_Ctor(GetValue(node->right) - 1))));
        
        case OP_LN:
            return OP_NODE(OP_DIV, d(node->right), c(node->right));

        case OP_LOG:
            return Differ(OP_NODE(OP_DIV, OP_NODE(OP_LN, NULL, c(node->right)), OP_NODE(OP_LN, NULL, c(node->left))), var_name);
    }

    #undef d
    #undef c
    #undef OP_NODE

    return NULL;
}

Node_t* CopyNode(Node_t* node) {
    if (!node)
        return NULL;

    Node_t* new_node = (Node_t*)calloc(1, sizeof(Node_t));
    new_node->type = node->type;
    new_node->value = node->value;

    new_node->left = CopyNode(node->left);
    new_node->right = CopyNode(node->right);

    return new_node;
}

double GetValue(Node_t* node) {
    if (!node || !node->value)
        return NAN;

    return node->value->value;
}

Node_t* ConvolConst(Node_t* node) {
    if (!node)
        return node;

    if (node->type == NUM) 
        return node;

    if (node->type == VAR)
        return node;

    node->left = RemovingNeutral(node->left);
    node->right = RemovingNeutral(node->right);

    node->left = ConvolConst(node->left);
    node->right = ConvolConst(node->right);

    #define is_const(node, dir) (node->dir && node->dir->type == NUM) 
    if (node->value->type == OP_COS || node->value->type == OP_SIN) { //добавить случай для NULL
        if (!is_const(node, right))
            return node;

        node->type = NUM;
        if (node->value->type == OP_COS) 
            node->value->value = cos(GetValue(node->right));
        else
            node->value->value = sin(GetValue(node->right));

        node->left = node->right = NULL;
    }
    else if (is_const(node, left) && is_const(node, right)) {
        node->type = NUM;
        node->value->value = opers[node->value->type].func(node->left, node->right);

        node->left = node->right = NULL;
    }
    #undef is_const

    return node;
}

Node_t* RemovingNeutral(Node_t* node) {
    if (!node)
        return NULL;

    node->left = ConvolConst(node->left);
    node->right = ConvolConst(node->right);

    node->left = RemovingNeutral(node->left);
    node->right = RemovingNeutral(node->right);

    if (node->type == NUM || node->type == VAR)
        return node;

    #define is_null(node, dir) (node->dir && node->dir->type == NUM && node->dir->value->value == 0)
    #define is_one(node, dir) (node->dir && node->dir->type == NUM && node->dir->value->value == 1)
    
    if (node->value->type == OP_MUL) {
        if (is_null(node, left) || is_null(node, right)) {
            node->type = NUM;
            node->value->value = 0;
            return node;
        }
    }

    if (node->value->type != OP_SIN && node->value->type != OP_COS) {
        switch (node->value->type) {
            case OP_ADD:
                if (is_null(node, right))
                    node = node->left;
                else if (is_null(node, left))
                    node = node->right;
            case OP_SUB:
                if (is_null(node, right))
                    node = node->left;
                break;
            case OP_DIV:
                if (is_one(node, right))
                    node = node->left;
                break;
            case OP_MUL:
                if (is_one(node, left)) {
                    node = node->right;
                }
                else if (is_one(node, right)) {
                    node = node->left;
                }
                break;
            case OP_POW_C:
                if (is_one(node, right)) {
                    node = node->left;
                }
                else if (is_null(node, right) || is_one(node, left)) {
                    node = NUM_Ctor(1);
                }
                break;
        }
    }

    #undef is_null
    #undef is_one

    return node;
}