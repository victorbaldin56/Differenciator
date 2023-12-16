//////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @brief Functions for taking derivatives.
///        Copyright (c) Victor Baldin, 2023.
///
//////////////////////////////////////////////////////////////////////////////

#include "ddx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "ddx_fun.h"
#include "dump_tree.h"

static struct TreeNode *dSubExpr(struct TexFile tf,
                                 const struct TreeNode *node);
static struct TreeNode *cTree(const struct TreeNode *node);

//////////////////////////////////////////////////////////////////////////////
/// @defgroup DSL Short functions for convinience purposes.
/// @{

static inline struct TreeNode *Add(struct TreeNode *left,
                                   struct TreeNode *right);
static inline struct TreeNode *Sub(struct TreeNode *left,
                                   struct TreeNode *right);
static inline struct TreeNode *Mul(struct TreeNode *left,
                                   struct TreeNode *right);
static inline struct TreeNode *Div(struct TreeNode *left,
                                   struct TreeNode *right);
static inline struct TreeNode *Pow(struct TreeNode *left,
                                   struct TreeNode *right);
static inline struct TreeNode *ln (struct TreeNode *node);
static inline struct TreeNode *Num(TreeNodeNumType num);

/// @}
//////////////////////////////////////////////////////////////////////////////

void TakeDerivative(struct TexFile tf, const struct TreeNode *node)
{
    assert(tf.stream);
    fprintf(tf.stream, "\\section{Дифференцирование}\n"
                       "$$f'(x)=\\left(");
    TexDumpNode(tf.stream, node);
    fprintf(tf.stream, "\\right)'$$\n");
    struct TreeNode *dnode = dTree(tf, node);
    TreeNodeDtor(dnode);
}

struct TreeNode *dTree(struct TexFile tf, const struct TreeNode *node)
{
    assert(node);
    TREE_ASSERT(node);

    switch (node->type) {
        case TYPE_NUMBER:
            return TreeNodeCtor(TYPE_NUMBER, 0, NULL, NULL);
        case TYPE_VARIABLE:
            return TreeNodeCtor(TYPE_NUMBER, 1, NULL, NULL);
        case TYPE_OPERATOR: {
            struct TreeNode *dnode = dSubExpr(tf, node);
            fprintf(tf.stream, "%s", FUN_PHRASES[(unsigned long) rand() %
                                     (sizeof(FUN_PHRASES) /
                                      sizeof(FUN_PHRASES[0]))]);
            fprintf(tf.stream, "$$\\left(");
            TexDumpNode(tf.stream, node);
            fprintf(tf.stream, "\\right)'=");
            TexDumpNode(tf.stream, dnode);
            fprintf(tf.stream, "$$\n");
            return dnode;
        }
        default:
            assert(0 && "Unhandled enum value");
    }
}

static struct TreeNode *dSubExpr(struct TexFile tf,
                                 const struct TreeNode *node)
{
    TREE_ASSERT(node);

    // TODO: better solution.
    // I know this is really bad, but
    // it's just impossible to write differenciation without this (:
    #define dTree(node) dTree(tf, node)
    switch (node->data.op) {
        case OP_ADD:
            return Add(dTree(node->left), dTree(node->right));
        case OP_SUB:
            return Sub(dTree(node->left), dTree(node->right));
        case OP_MUL:
            return Add(Mul(dTree(node->left), cTree(node->right)),
                       Mul(cTree(node->left), dTree(node->right)));
        case OP_DIV:
            return Div(Sub(Mul(dTree(node->left), cTree(node->right)),
                           Mul(cTree(node->left), dTree(node->right))),
                       Pow(cTree(node->right), Num(2)));
        case OP_POW:
            return Mul(cTree(node),
                       Add(Mul(dTree(node->right), ln(cTree(node->left))),
                           Mul(cTree(node->right), Div(dTree(node->left),
                                                       cTree(node->left)))));
        case OP_LN:
            return Div(dTree(node->left), cTree(node->left));
        default:
            assert(0 && "Unhandled enum value");
    }
    #undef dTree
}

static struct TreeNode *cTree(const struct TreeNode *node)
{
    TREE_ASSERT(node);

    if (!node)
        return NULL;

    switch (node->type) {
        case TYPE_OPERATOR:
            return TreeNodeCtor(TYPE_OPERATOR, node->data.op,
                                cTree(node->left), cTree(node->right));
        case TYPE_NUMBER:
            return TreeNodeCtor(TYPE_NUMBER, node->data.num, NULL, NULL);
        case TYPE_VARIABLE:
            return TreeNodeCtor(TYPE_VARIABLE, node->data.varname,
                                NULL, NULL);
        default:
            assert(0 && "Unhandled enum value");
    }
}

static inline struct TreeNode *Add(struct TreeNode *left,
                                   struct TreeNode *right)
{
    return TreeNodeCtor(TYPE_OPERATOR, OP_ADD, left, right);
}

static inline struct TreeNode *Sub(struct TreeNode *left,
                                   struct TreeNode *right)
{
    return TreeNodeCtor(TYPE_OPERATOR, OP_SUB, left, right);
}

static inline struct TreeNode *Mul(struct TreeNode *left,
                                   struct TreeNode *right)
{
    return TreeNodeCtor(TYPE_OPERATOR, OP_MUL, left, right);
}

static inline struct TreeNode *Div(struct TreeNode *left,
                                   struct TreeNode *right)
{
    return TreeNodeCtor(TYPE_OPERATOR, OP_DIV, left, right);
}

static inline struct TreeNode *Pow(struct TreeNode *left,
                                   struct TreeNode *right)
{
    return TreeNodeCtor(TYPE_OPERATOR, OP_POW, left, right);
}

static inline struct TreeNode *ln (struct TreeNode *node)
{
    return TreeNodeCtor(TYPE_OPERATOR, OP_LN, node, NULL);
}

static inline struct TreeNode *Num(TreeNodeNumType num)
{
    return TreeNodeCtor(TYPE_NUMBER, num, NULL, NULL);
}
