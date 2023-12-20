//////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @brief Functions for taking derivatives.
///        Copyright (c) Victor Baldin, 2023.
///
//////////////////////////////////////////////////////////////////////////////

#include "ddx.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "ddx_fun.h"
#include "dump_tree.h"

static int Factorial(int num);

static struct TreeNode *dSubExpr(struct TexFile tf,
                                 const struct TreeNode *node, bool with_dump);
static inline void TexDumpStep(struct TexFile tf, const struct TreeNode *node,
                               const struct TreeNode *dnode, bool with_dump);
static void PrintTaylor(struct TexFile tf, const TreeNodeNumType coeffs[],
                        int prec);
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

/// @brief Prints a random phrase from FUN_PHRASES global array.
/// @param tf TexFile to printf.
static inline void PrintRandomMathShit(struct TexFile tf);

void PrintDifferenciationReport(struct TexFile tf,
                                const struct TreeNode *node)
{
    assert(tf.stream);
    fprintf(tf.stream, "\\section{Дифференцирование}\n"
                       "$$f'(x)=\\left(");
    TexDumpNode(tf.stream, node);
    fprintf(tf.stream, "\\right)'$$\n");

    struct TreeNode *dnode = dTree(tf, node, true);
    TreeOptimize(dnode->left);
    PrintRandomMathShit(tf);
    fprintf(tf.stream, "$$f'(x)=");
    TexDumpNode(tf.stream, dnode);
    fprintf(tf.stream, "$$\n");
    TreeNodeDtor(dnode);
}

void PrintTaylorExpansionReport(struct TexFile tf,
                                const struct TreeNode *node)
{
    assert(tf.stream);
    fprintf(tf.stream, "\\section{Разложение по формуле Маклорена}\n");

    const int precision = 5; ///< for Taylor expansion
    struct TreeNode *prev = dTree(tf, node, false);
    TreeNodeNumType coeffs[precision + 1] = {EvalTree(node, 0)};
    for (int i = 1; i <= precision; ++i) {
        coeffs[i] = EvalTree(prev, 0) / Factorial(i);
        struct TreeNode *next = dTree(tf, prev, false);
        TreeNodeDtor(prev);
        prev = next;
    }
    TreeNodeDtor(prev);
    PrintTaylor(tf, coeffs, precision);
}

void PrintReferences(struct TexFile tf)
{
    assert(tf.stream);

    size_t bib_size = sizeof(REF_PHRASES) / sizeof(REF_PHRASES[0]);
    fprintf(tf.stream, "\\begin{thebibliography}{%zu}\n", bib_size);
    for (size_t i = 0; i < bib_size; ++i)
        fprintf(tf.stream, "\\bibitem{r%zu} \n%s\n",
                           i, REF_PHRASES[(size_t)rand() % bib_size]);
    fprintf(tf.stream, "\\end{thebibliography}");
}

static void PrintTaylor(struct TexFile tf, const TreeNodeNumType coeffs[],
                        int prec)
{
    assert(tf.stream);
    assert(coeffs);
    assert(prec >= 0);

    fprintf(tf.stream, "$$f(x)=");
    fprintf(tf.stream, TREE_NODE_NUM_FORMAT, coeffs[0]);
    for (int i = 1; i <= prec; ++i)
        fprintf(tf.stream, "+" TREE_NODE_NUM_FORMAT "x^{%d}", coeffs[i], i);
    fprintf(tf.stream, "$$\n");
}

static int Factorial(int num)
{
    int res = 1;
    for (int i = 1; i <= num; ++i)
        res *= i;
    return res;
}

struct TreeNode *dTree(struct TexFile tf, const struct TreeNode *node,
                       bool with_dump)
{
    assert(node);
    TREE_ASSERT(node);

    switch (node->type) {
        case TYPE_NUMBER:
            return TreeNodeCtor(TYPE_NUMBER, 0, NULL, NULL);
        case TYPE_VARIABLE:
            return TreeNodeCtor(TYPE_NUMBER, 1, NULL, NULL);
        case TYPE_OPERATOR: {
            struct TreeNode *dnode = dSubExpr(tf, node, with_dump);
            TexDumpStep(tf, node, dnode, with_dump);
            return dnode;
        }
        default:
            assert(0 && "Unhandled enum value");
    }
}

static inline void TexDumpStep(struct TexFile tf, const struct TreeNode *node,
                               const struct TreeNode *dnode, bool with_dump)
{
    assert(tf.stream);
    if (!with_dump)
        return;

    PrintRandomMathShit(tf);
    fprintf(tf.stream, "$$\\left(");
    TexDumpNode(tf.stream, node);
    fprintf(tf.stream, "\\right)'=");
    TexDumpNode(tf.stream, dnode);
    fprintf(tf.stream, "$$\n");
}

static inline void PrintRandomMathShit(struct TexFile tf)
{
    assert(tf.stream);
    fprintf(tf.stream, "%s", FUN_PHRASES[(size_t) rand() %
                                         (sizeof(FUN_PHRASES) /
                                          sizeof(FUN_PHRASES[0]))]);
}

static struct TreeNode *dSubExpr(struct TexFile tf,
                                 const struct TreeNode *node, bool with_dump)
{
    TREE_ASSERT(node);

    // TODO: better solution.
    // I know this is really bad, but
    // it's just impossible to write differenciation without this (:
    #define dTree(node) dTree(tf, node, with_dump)
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
        case OP_EQU:
            return dTree(node->left);
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

//============================================================================

static void MergeConstants(struct TreeNode *node);
static void DeleteNeutrals(struct TreeNode *node);

static inline void OptimizeAdd(struct TreeNode *node);
static inline void OptimizeMul(struct TreeNode *node);
static inline void OptimizeDiv(struct TreeNode *node);
static inline void OptimizePow(struct TreeNode *node);

static inline void OptimizeSubtrees(struct TreeNode *node);

static inline bool Equal(TreeNodeNumType a, TreeNodeNumType b);

/// @brief Deletes subtree starting with node and replaces it with newnode of
///        numeric type.
/// @param node
/// @param newnode
static void ReplaceSubTree(struct TreeNode *node, struct TreeNode *newnode);

void TreeOptimize(struct TreeNode *node)
{
    MergeConstants(node);
    DeleteNeutrals(node);
    MergeConstants(node);
}

static void MergeConstants(struct TreeNode *node)
{
    TREE_ASSERT(node);

    if (!node)
        return;
    TreeNodeNumType val = EvalTree(node, NAN);
    if (!isnan(val)) {
        ReplaceSubTree(node, Num(val));
        return;
    }
    MergeConstants(node->left);
    MergeConstants(node->right);
}

static void DeleteNeutrals(struct TreeNode *node)
{
    TREE_ASSERT(node);

    if (!node)
        return;
    if (node->type != TYPE_OPERATOR)
        return;
    switch (node->data.op) {
        case OP_ADD:
        case OP_SUB:
            OptimizeAdd(node);
            return;
        case OP_MUL:
            OptimizeMul(node);
            return;
        case OP_DIV:
            OptimizeDiv(node);
            return;
        case OP_POW:
            OptimizePow(node);
            return;
        case OP_LN:
            DeleteNeutrals(node->left);
            return;
        case OP_EQU:
            DeleteNeutrals(node->left);
            return;
        default:
            assert(0 && "Unhandled enum value");
    }
}

static inline void OptimizeAdd(struct TreeNode *node)
{
    TREE_ASSERT(node);
    if (node->left->type == TYPE_NUMBER && Equal(node->left->data.num, 0)) {
        DeleteNeutrals(node->right);
        ReplaceSubTree(node, cTree(node->right));
    }
    else if (node->right->type == TYPE_NUMBER &&
             Equal(node->right->data.num, 0)) {
        DeleteNeutrals(node->left);
        ReplaceSubTree(node, cTree(node->left));
    }
    else
        OptimizeSubtrees(node);
}

static inline void OptimizeMul(struct TreeNode *node)
{
    TREE_ASSERT(node);
    if (node->left->type == TYPE_NUMBER) {
        if (Equal(node->left->data.num, 0)) {
            ReplaceSubTree(node, Num(0));
            return;
        }
        if (Equal(node->left->data.num, 1)) {
            DeleteNeutrals(node->right);
            ReplaceSubTree(node, cTree(node->right));
            return;
        }
    }
    if (node->right->type == TYPE_NUMBER) {
        if (Equal(node->right->data.num, 0)) {
            ReplaceSubTree(node, Num(0));
            return;
        }
        if (Equal(node->right->data.num, 1)) {
            DeleteNeutrals(node->right);
            ReplaceSubTree(node, cTree(node->left));
            return;
        }
    }
    OptimizeSubtrees(node);
}

static inline void OptimizeDiv(struct TreeNode *node)
{
    TREE_ASSERT(node);
    if (node->left->type == TYPE_NUMBER &&
            Equal(node->left->data.num, 0))
        ReplaceSubTree(node, Num(0));
    else if (node->right->type == TYPE_NUMBER &&
            Equal(node->right->data.num, 1))
        ReplaceSubTree(node, cTree(node->left));
    else
        OptimizeSubtrees(node);
}

static inline void OptimizePow(struct TreeNode *node)
{
    TREE_ASSERT(node);
    if (node->left->type == TYPE_NUMBER) {
        if (Equal(node->left->data.num, 0)) {
            ReplaceSubTree(node, Num(0));
            return;
        }
        if (Equal(node->right->data.num, 1)) {
            ReplaceSubTree(node, Num(1));
            return;
        }
    }
    if (node->right->type == TYPE_NUMBER) {
        if (Equal(node->right->data.num, 0)) {
            ReplaceSubTree(node, Num(1));
            return;
        }
        if (Equal(node->right->data.num, 1)) {
            ReplaceSubTree(node, cTree(node->left));
            return;
        }
    }
    OptimizeSubtrees(node);
}

static void ReplaceSubTree(struct TreeNode *node, struct TreeNode *newnode)
{
    assert(node && newnode);
    TREE_ASSERT(node);
    TREE_ASSERT(newnode);

    if (node->parent->left == node) {
        node->parent->left = newnode;
        newnode->parent    = node->parent;
    }
    else {
        node->parent->right = newnode;
        newnode->parent     = node->parent;
    }
    TreeNodeDtor(node);
}

static inline bool Equal(TreeNodeNumType a, TreeNodeNumType b)
{
    return abs(b - a) < 1e-15 ? true : false;
}

static inline void OptimizeSubtrees(struct TreeNode *node)
{
    DeleteNeutrals(node->left);
    DeleteNeutrals(node->right);
}
