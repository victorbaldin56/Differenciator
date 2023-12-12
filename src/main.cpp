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

static inline char *ReadInput();

int main(int argc, char *argv[])
{
    char *input = ReadInput();
    struct TreeNode *node = ParseExpression(input);
    free(input);
    TREE_DUMP(node);

    FILE *output = TexBegin("latex/output.tex");

    if (!output) {
        perror("");
        return EXIT_FAILURE;
    }

    MathBegin(output);
    TexDump(output, node);
    MathEnd(output);

    TexEnd(output, "latex/output.tex");

    printf("Result = " TREE_NODE_NUM_FORMAT "\n", EvalTree(node));
    TreeNodeDtor(node);

    return 0;
}

static inline char *ReadInput()
{
    char *input  = NULL;
    size_t nread = 0;
    getline(&input, &nread, stdin);
    return input;
}
