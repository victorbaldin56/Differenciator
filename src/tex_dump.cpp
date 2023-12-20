//////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @brief Copyright (c) Victor Baldin, 2023.
///
//////////////////////////////////////////////////////////////////////////////

#include "tex_dump.h"

#include <assert.h>

#include "ddx.h"
#include "dump_tree.h"
#include "tree.h"

static void TexPrintOp(FILE *output, const struct TreeNode *node);
static inline void TexPrintSubExpr(FILE *output, const struct TreeNode *node,
                                   TreeOperators lastop);

static inline void TexPrintMul(FILE *output, const struct TreeNode *node);
static inline void TexPrintFrac(FILE *output, const struct TreeNode *node);
static inline void TexPrintPow(FILE *output, const struct TreeNode *node);

void TexDumpSource(struct TexFile tf, struct TreeNode *node)
{
    assert(tf.stream);
    fprintf(tf.stream, "\\section{Анализ данной функции}\n"
                       "В качестве примера рассмотрим следующую функцию:\n"
                       "$$f(x)=");
    TexDumpNode(tf.stream, node);
    fprintf(tf.stream, "$$\n");
    fprintf(tf.stream, "После очевидных преобразований:");

    TREE_DUMP(node);
    TreeOptimize(node->left);
    fprintf(tf.stream, "$$f(x)=");
    TexDumpNode(tf.stream, node);
    fprintf(tf.stream, "$$\n");
}

void TexDumpNode(FILE *output, const struct TreeNode *node)
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
            TexPrintSubExpr(output, node->left, node->data.op);
            fprintf(output, "%s", OP_SYMBOLS[node->data.op]);
            TexPrintSubExpr(output, node->right, node->data.op);
            return;
        case OP_MUL:
            TexPrintMul(output, node);
            return;
        case OP_DIV:
            TexPrintFrac(output, node);
            return;
        case OP_POW:
            TexPrintPow(output, node);
            return;
        case OP_LN: // TODO
            fprintf(output, "\\ln ");
            TexPrintSubExpr(output, node->left, node->data.op);
            return;
        case OP_EQU:
            TexPrintSubExpr(output, node->left, node->data.op);
            return;
        default:
            assert(0 && "Unhandled enum value");
    }
}

static inline void TexPrintSubExpr(FILE *output, const struct TreeNode *node,
                                   TreeOperators lastop)
{
    assert(output);
    assert(node);
    if (node->type == TYPE_OPERATOR && OP_PRIORITIES[node->data.op] >=
        OP_PRIORITIES[lastop]) {
        fprintf(output, "\\left(");
        TexDumpNode(output, node);
        fprintf(output, "\\right)");
    }
    else
        TexDumpNode(output, node);
}

static inline void TexPrintMul(FILE *output, const struct TreeNode *node)
{
    assert(output);
    assert(node);
    TexPrintSubExpr(output, node->left, OP_MUL);
    fprintf(output, " \\cdot ");
    TexPrintSubExpr(output, node->right, OP_MUL);
}

static inline void TexPrintFrac(FILE *output, const struct TreeNode *node)
{
    assert(output);
    assert(node);
    fprintf(output, "\\frac{");
    TexDumpNode(output, node->left);
    fprintf(output, "}{");
    TexDumpNode(output, node->right);
    fprintf(output, "}");
}

static inline void TexPrintPow(FILE *output, const struct TreeNode *node)
{
    assert(output);
    assert(node);
    TexPrintSubExpr(output, node->left, OP_POW);
    fprintf(output, "^{");
    TexDumpNode(output, node->right);
    fprintf(output, "}");
}
