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

    FILE *output = TexBegin("latex/output.tex");

    if (!output) {
        perror("");
        return EXIT_FAILURE;
    }
    MathBegin(output);
    TexDump(output, node);
    MathEnd(output);

    struct TreeNode *dt = dTree(node);
    TREE_DUMP(node);
    TreeNodeDtor(node);
    fprintf(output, "$$ f'(x) = ");
    TexDump(output, dt);
    TREE_DUMP(dt);
    MathEnd(output);
    // TODO: struct
    TexEnd(output, "latex/output.tex");

    TreeNodeDtor(dt);

    return 0;
}

static inline char *ReadInput()
{
    char *input  = NULL;
    size_t nread = 0;
    getline(&input, &nread, stdin);
    return input;
}
