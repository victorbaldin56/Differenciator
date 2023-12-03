//////////////////////////////////////////////////////////////////////////////
/// @file main.cpp
///
/// @brief Copyright (c) Victor Baldin, 2023.
///
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "ddx.h"
#include "tree.h"
#include "expr_parser.h"

int main(int argc, char *argv[])
{
    if (argc == 1) {
        printf("Differenciator (c) Victor Baldin, 2023\n");
        return 0;
    }

    char *buffer = LoadFile(argv[1]); // TODO: read from standart input
    struct TreeNode *node = ParseExpression(buffer);
    free(buffer);
    printf("Result = " TREE_NODE_NUM_FORMAT "\n", EvalTree(node));
    TreeNodeDtor(node);

    return 0;
}
