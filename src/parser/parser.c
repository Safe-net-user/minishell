/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 10:27:18 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/19 13:46:22 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "minishell.h"

t_token *next_token(t_token *cur)
{
    t_header *h;

    h = (t_header *)cur - 1;
    return ((t_token *)((uint8_t *)cur + h->size + sizeof(t_header)));
}

int parse_or()
{

}

int parse_and()
{
    
}

int parse_pipe()
{

}

int parser(t_mms *mms)
{
    t_ast *ast;
}

//besoin de fonction utils: ajouter un noeud, l'arbre sera creer dans la fonction principale et place dans la stack allocator