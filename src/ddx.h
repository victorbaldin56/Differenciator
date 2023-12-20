//////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @brief Copyright (c) Victor Baldin, 2023.
///
//////////////////////////////////////////////////////////////////////////////

#ifndef DDX_H_
#define DDX_H_

#include "tex_dump.h"
#include "tree.h"

void PrintDifferenciationReport(struct TexFile tf,
                                const struct TreeNode *node);
void PrintTaylorExpansionReport(struct TexFile tf,
                                const struct TreeNode *node);
void PrintReferences(struct TexFile tf);
struct TreeNode *dTree(struct TexFile tf, const struct TreeNode *node,
                       bool with_dump);

void TreeOptimize(struct TreeNode *node);

#endif // DDX_H_
