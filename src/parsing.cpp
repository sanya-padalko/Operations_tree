#include "parsing.h"
#include "tree.h"

#define EQUAL(str, oper) !strncmp(str, oper, strlen(oper))

#define CHECK(ptr, symbol, add_info)    if (strncmp(*ptr, symbol, 1)) { \
                                            SyntaxError("( parsing " add_info " ), expected '" symbol "', but getting", **ptr); \
                                        } \
                                        else { \
                                            ++*ptr; \
                                            SkipSpaces(ptr); \
                                        }


Node_t* GetG(const char** s) {
    SkipSpaces(s);

    Node_t* val = GetE(s);

    CHECK(s, "$", "general");
    return val;
}

Node_t* GetE(const char** s) {
    SkipSpaces(s);

    Node_t* node = GetT(s);

    while (**s == '+' || **s == '-' || **s == '^') {
        int op = **s;
        ++*s;
        Node_t* node2 = GetT(s);
        if (op == '^')
            node = OP_NODE(OP_POW, c(node), c(node2));
        else if (op == '+')
            node = OP_NODE(OP_ADD, c(node), c(node2));
        else 
            node = OP_NODE(OP_SUB, c(node), c(node2));
    }

    SkipSpaces(s);
    return node;
}

Node_t* GetT(const char** s) {
    SkipSpaces(s);

    Node_t* node = GetPower(s);

    while (**s == '*' || **s == '/') {
        int op = **s;
        ++*s;
        Node_t* node2 = GetPower(s);
        if (op == '*')
            node = OP_NODE(OP_MUL, c(node), c(node2));
        else 
            node = OP_NODE(OP_DIV, c(node), c(node2));
    }

    SkipSpaces(s);
    return node;
}

Node_t* GetPower(const char** s) {
    SkipSpaces(s);

    Node_t* node = GetP(s);

    while (**s == '^') {
        ++*s;
        Node_t* node2 = GetP(s);
        
        node = OP_NODE(OP_POW, c(node), c(node2));
    }

    return node;
}

Node_t* GetP(const char** s) {
    SkipSpaces(s);

    if (**s == '(') {
        ++*s;
        Node_t* node = GetE(s);

        CHECK(s, ")", "parentheses");
        return node;
    }

    if ('a' <= **s && **s <= 'z') {
        if (EQUAL(*s, "ln")) {
            *s += strlen("ln");
            SkipSpaces(s);

            CHECK(s, "(", "ln");
            Node_t* node = GetE(s);
            CHECK(s, ")", "ln");

            return node;
        }

        if (EQUAL(*s, "log")) {
            *s += strlen("log");
            SkipSpaces(s);

            CHECK(s, "(", "log");
            Node_t* argue = GetE(s);
            CHECK(s, ",", "log");
            Node_t* base = GetE(s);
            CHECK(s, ")", "log");

            return OP_NODE(OP_LOG, c(base), c(argue));
        }

        if (EQUAL(*s, "e")) {
            *s += strlen("e");
            SkipSpaces(s);

            CHECK(s, "^", "exp");
            CHECK(s, "(", "exp");
            Node_t* power = GetE(s);
            CHECK(s, ")", "exp");

            return OP_NODE(OP_EXP, NULL, c(power));
        }
        
        if (EQUAL(*s, "sin") || EQUAL(*s, "cos")) {
            char first_char = **s;
            *s += strlen("sin");
            SkipSpaces(s);

            CHECK(s, "(", "sin/cos");
            Node_t* node = GetE(s);
            switch(first_char) {
                case 'c':
                    node = OP_NODE(OP_COS, NULL, c(node));
                    break;
                case 's':
                    node = OP_NODE(OP_SIN, NULL, c(node));
                    break;
            }
            CHECK(s, ")", "sin/cos");

            return node;
        }

        for (int op_ind = OP_TAN; op_ind <= OP_ACOT; ++op_ind) {
            if (EQUAL(*s, opers[op_ind].dump_view)) {
                *s += strlen(opers[op_ind].dump_view);
                SkipSpaces(s);

                CHECK(s, "(", "trigonometry");
                Node_t* node = GetE(s);
                node = OP_NODE(opers[op_ind].type, NULL, c(node));

                CHECK(s, ")", "trigonometry");

                return node;
            }
        }

        SkipSpaces(s);
        return GetV(s);
    }
    
    SkipSpaces(s);
    return GetN(s);
}

Node_t* GetV(const char** s) {
    SkipSpaces(s);

    char* name = (char*)calloc(MAX_VAR_SIZE, sizeof(char));
    int ind = 0;

    while ('a' <= **s && **s <= 'z') {
        name[ind++] = **s;
        ++*s;
    }
    name[ind] = '\0';

    SkipSpaces(s);
    return VAR_NODE(name);
}

Node_t* GetN(const char** s) {
    SkipSpaces(s);

    const char* old_s = *s;
    double val = 0;

    while ('0' <= **s && **s <= '9') {
        val = val * 10 + (double)(**s - '0');
        ++*s;
    }

    if (*s == old_s) {
        SyntaxError("(parsing number) expected digit, but getting", **s);
    }

    SkipSpaces(s);
    return NUM_NODE(val);
}

void SkipSpaces(const char** s) {
    while (**s == ' ')
        ++*s;
}