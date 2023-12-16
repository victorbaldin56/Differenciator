//////////////////////////////////////////////////////////////////////////////
/// @file main.cpp
///
/// @brief Copyright (c) Victor Baldin, 2023.
///
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "ddx.h"
#include "dump_tree.h"
#include "tree.h"
#include "expr_parser.h"
#include "tex_dump.h"

static inline struct TreeNode *ReadInput();

int main(int argc, char *argv[])
{
    struct TreeNode *node = ReadInput();

    struct TexFile tf = TexBegin("latex/output.tex");
    if (!tf.stream) {
        perror("");
        return EXIT_FAILURE;
    }
    TexDump(tf, node);

    struct TreeNode *dt = dTree(node);
    TREE_DUMP(node);
    TreeNodeDtor(node);
    TexDump(tf, dt);
    TREE_DUMP(dt);
    TexEnd(tf);
    TreeNodeDtor(dt);

    return 0;
}

static inline struct TreeNode *ReadInput()
{
    char input[8000] = {};
    fgets(input, sizeof(input), stdin);
    struct TreeNode *node = ParseExpression(input);
    return node;
}
