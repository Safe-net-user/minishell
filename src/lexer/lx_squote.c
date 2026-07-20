#include "lexer.h"

t_val_lx    lx_squote(t_lx *lx)
{
    char    c;

    c = lx->cmdl[lx->index];
    if (c == '\'')
    {
        append_ch_sb(lx->sb, c);
        lx->state = LX_NORMAL;
    }
    else
        append_ch_sb(lx->sb, c);
    lx->index++;
    return (LX_SUCCESS);
}