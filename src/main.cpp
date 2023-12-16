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
    TexDumpSource(tf, node);
    TakeDerivative(tf, node);
    TexEnd(tf);
    TreeNodeDtor(node);
    return 0;
}

static inline struct TreeNode *ReadInput()
{
    const size_t inputSize = 16384;
    char *input = (char *) calloc(inputSize, sizeof(*input));
    if (!input)
        return NULL;
    fgets(input, inputSize, stdin);
    struct TreeNode *node = ParseExpression(input);
    free(input);
    return node;
}
