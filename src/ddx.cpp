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

#include "dump_tree.h"

static struct TreeNode *dSubExpr(const struct TreeNode *node);
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

struct TreeNode *dTree(const struct TreeNode *node)
{
    assert(node);
    TREE_ASSERT(node);

    switch (node->type) {
        case TYPE_NUMBER:
            return TreeNodeCtor(TYPE_NUMBER, 0, NULL, NULL);
        case TYPE_VARIABLE:
            return TreeNodeCtor(TYPE_NUMBER, 1, NULL, NULL);
        case TYPE_OPERATOR:
            return dSubExpr(node);
        default:
            assert(0 && "Unhandled enum value");
    }
}

static struct TreeNode *dSubExpr(const struct TreeNode *node)
{
    TREE_ASSERT(node);

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
