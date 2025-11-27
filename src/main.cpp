#include "folders.h"
#include "tree.h"

int main() {
    MakeDir();

    const char* base_file = "../base.txt";

    Forest_t* forest = ForestCtor();

    ReadBase(forest, base_file);

    char* cur_pos = forest->buf;
    forest->tree[0]->root = ParseBase(&cur_pos, forest);
    SeparateVars(forest);
    dump(forest, 0, NOTHING, "Dump after ParseBase:");

    forest->tree[0]->root = ConvolConst(forest->tree[0]->root);
    forest->tree[0]->root = RemovingNeutral(forest->tree[0]->root);
    dump(forest, 0, NOTHING, "Dump after ConvolConst and RemovingNeutral:");

    forest->tree[1]->root = Differ(forest->tree[0]->root, (char*)"x");
    dump(forest, 0, NOTHING, "Dump after Differentiation:");

    forest->tree[1]->root = ConvolConst(forest->tree[1]->root);
    forest->tree[1]->root = RemovingNeutral(forest->tree[1]->root);
    dump(forest, 1, NOTHING, "Dump after second ConvolConst and RemovingNeutral:");
    
    printf("Answer: %lg\n", CalculatingTree(forest, 1));
    CloseTex(forest);
}
