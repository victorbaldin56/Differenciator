//////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @brief Copyright (c) Victor Baldin, 2023.
///
//////////////////////////////////////////////////////////////////////////////

#ifndef TEX_DUMP_H_
#define TEX_DUMP_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct TexFile {
    const char *pathname;
    FILE *stream;
};

void TexDumpSource(struct TexFile tf, const struct TreeNode *node);

void TexDumpNode(FILE *output, const struct TreeNode *node);

//////////////////////////////////////////////////////////////////////////////
/// @defgroup Dumps tree as math expression in TEX format.
/// @{

inline struct TexFile TexBegin(const char pathname[])
{
    assert(pathname);

    FILE *output = fopen(pathname, "w");
    if (!output)
        return {pathname, NULL};
    fprintf(output, "\\documentclass{article}\n"
                    "\\usepackage[utf8]{inputenc}\n"
                    "\\usepackage[english, russian]{babel}\n"
                    "\\title{Символьное дифференцирование функций}\n"
                    "\\author{Балдин Виктор\\\\РТ РТ РТ РТ РТ РТ РТ РТ РТ РТ}\n"
                    "\n"
                    "\\usepackage[a4paper,top=1.3cm,bottom=2cm,left=1.5cm,"
                    "right=1.5cm,marginparwidth=0.75cm]{geometry}"
                    "\\begin{document}\n"
                    "\\maketitle\n"
                    "\\section{Введение}\n"
                    "Одним из самых простых действий над функцией является "
                    "дифференнцирование, так как оно подчиняется лишь "
                    "нескольким тривиальным правилам. "
                    "Так, каждому советскому школьнику известно, что:\n"
                    "$$(f+g)'=f'+g'$$\n"
                    "$$(fg)' =f'g+fg'$$\n"
                    "$$\\left(\\frac{f}{g}\\right)'=\\frac{f'g-fg'}{g^2}$$\n"
                    "$$(f^g)'=f^g\\left(g'\\ln f+g\\frac{f'}{f}\\right)$$"
                    "Теперь рассмотрим применение этих правил на простом примере.");
    return {pathname, output};
}

inline void TexEnd(struct TexFile tf)
{
    assert(tf.stream && tf.pathname);

    fprintf(tf.stream, "\\end{document}\n");
    fclose(tf.stream);

    char cmd[5000] = {};
    // TODO: output dir
    snprintf(cmd, sizeof(cmd), "pdflatex -output-directory latex %s",
                                tf.pathname);
    system(cmd);
}

/// @}
//////////////////////////////////////////////////////////////////////////////

#endif // TEX_DUMP_H_
