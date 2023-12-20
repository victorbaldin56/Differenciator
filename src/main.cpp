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
    PrintDifferenciationReport(tf, node);
    PrintTaylorExpansionReport(tf, node);
    PrintReferences(tf);
    TexEnd(tf);
    TreeNodeDtor(node);
    return 0;
}

static inline struct TreeNode *ReadInput()
{
    const size_t input_size = 16384;
    char *input = (char *)calloc(input_size, sizeof(*input));
    if (!input)
        return NULL;
    fgets(input, input_size, stdin);
    struct TreeNode *node = ParseExpression(input);
    free(input);
    return node;
}
