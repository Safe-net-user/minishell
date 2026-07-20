#include "lexer.h"

static void	lx_begin_squote(t_lx *lx)
{
    lx->state = LX_SQUOTE;
    lx->tk->flags |= TOKF_SQUOTE;
    append_ch_sb(lx->sb, '\'');
    lx->index++;
}

static void	lx_begin_dquote(t_lx *lx)
{
    lx->state = LX_DQUOTE;
    lx->tk->flags |= TOKF_DQUOTE;
    append_ch_sb(lx->sb, '"');
    lx->index++;
}

static void	lx_dollar(t_lx *lx)
{
    if (lx->cmdl[lx->index + 1] == '\'')
    {
        lx->state = LX_SQUOTE;
        lx->tk->flags |= TOKF_SQUOTE;
        append_sb(lx->sb, "$'");
        lx->index += 2;
    }
    else
    {
        lx->tk->flags |= TOKF_EXPANSION;
        append_ch_sb(lx->sb, '$');
        lx->index++;
    }
}

static t_val_lx	lx_space(t_lx *lx)
{
    if (lx->sb->str[0])
        if (!emit_tk(lx))
            return (LX_ERROR);
    lx->index++;
    return (LX_SUCCESS);
}

t_val_lx	lx_normal(t_lx *lx)
{
    char	c;

    c = lx->cmdl[lx->index];
    if (c == '|' || c == '&' || c == '<' || c == '>')
    {
        lx->state = LX_OPERATOR;
        return (LX_SUCCESS);
    }
    if (c == '\'')
        lx_begin_squote(lx);
    else if (c == '"')
        lx_begin_dquote(lx);
    else if (c == '$')
        lx_dollar(lx);
    else if (c == ' ' || c == '\t')
        return (lx_space(lx));
    else
    {
        append_ch_sb(lx->sb, c);
        lx->index++;
    }
    return (LX_SUCCESS);
}