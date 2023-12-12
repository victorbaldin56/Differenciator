//////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @brief Copyright (c) Victor Baldin, 2023.
///
//////////////////////////////////////////////////////////////////////////////

#include "tex_dump.h"

#include <assert.h>

#include "tree.h"

static void TexPrintOp(FILE *output, const struct TreeNode *node);
static inline void TexPrintFrac(FILE *output, const struct TreeNode *node);
static inline void TexPrintPow(FILE *output, const struct TreeNode *node);

void TexDump(FILE *output, const struct TreeNode *node)
{
    assert(output);
    assert(node);

    switch (node->type) {
        case TYPE_VARIABLE:
            fputc(node->data.varname, output);
            return;
        case TYPE_NUMBER:
            fprintf(output, TREE_NODE_NUM_FORMAT, node->data.num);
            return;
        case TYPE_OPERATOR:
            TexPrintOp(output, node);
            return;
        default:
            assert(0 && "Unhandled enum value");
    }
}

static void TexPrintOp(FILE *output, const struct TreeNode *node)
{
    assert(output);
    assert(node);

    switch (node->data.op) {
        case OP_ADD:
        case OP_SUB:
            TexDump(output, node->left);
            fprintf(output, "%c", node->data.op);
            TexDump(output, node->right);
            return;
        case OP_MUL:
        case OP_DIV:
            TexPrintFrac(output, node);
            return;
        case OP_POW:
            TexPrintPow(output, node);
            return;
        default:
            assert(0 && "Unhandled enum value");
    }
}

static inline void TexPrintFrac(FILE *output, const struct TreeNode *node)
{
    assert(output);
    assert(node);
    fprintf(output, "\\frac{");
    TexDump(output, node->left);
    fprintf(output, "}{");
    TexDump(output, node->right);
    fprintf(output, "}");
}

static inline void TexPrintPow(FILE *output, const struct TreeNode *node)
{
    assert(output);
    assert(node);
    TexDump(output, node->left);
    fprintf(output, "^{");
    TexDump(output, node->right);
    fprintf(output, "}");
}
