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

void TexDump(FILE *output, const struct TreeNode *node);

//////////////////////////////////////////////////////////////////////////////
/// @defgroup Dumps tree as math expression in TEX format.
/// @{

inline FILE *TexBegin(const char pathname[])
{
    assert(pathname);

    FILE *output = fopen(pathname, "w");

    if (!output) return NULL;

    fprintf(output, "\\documentclass{article}\n"
                    "\\usepackage[utf8]{inputenc}\n"
                    "\\usepackage[english, russian]{babel}\n"
                    "\\title{Символьное дифференцирование функций}\n"
                    "\\author{Балдин Виктор\\\\РТ РТ РТ РТ РТ РТ РТ РТ РТ РТ}\n"
                    "\n"
                    "\\usepackage[a4paper,top=1.3cm,bottom=2cm,left=1.5cm,"
                    "right=1.5cm,marginparwidth=0.75cm]{geometry}"
                    "\\begin{document}\n"
                    "\\maketitle\n");

    return output;
}

inline void MathBegin(FILE *output)
{
    assert(output);
    fprintf(output, "$$");
}

inline void MathEnd(FILE *output)
{
    assert(output);
    fprintf(output, "$$\n");
}

inline void TexEnd(FILE *output, const char pathname[])
{
    assert(output && pathname);

    fprintf(output, "\\end{document}\n");
    fclose(output);

    char cmd[5000] = {};
    // TODO: output dir
    snprintf(cmd, sizeof(cmd), "pdflatex -output-directory latex %s",
                                pathname);
    system(cmd);
}

/// @}
//////////////////////////////////////////////////////////////////////////////

#endif // TEX_DUMP_H_
