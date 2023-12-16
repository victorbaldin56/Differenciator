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

void TakeDerivative(struct TexFile tf, const struct TreeNode *node);

struct TreeNode *dTree(struct TexFile tf, const struct TreeNode *node);

#endif // DDX_H_
