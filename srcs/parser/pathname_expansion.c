/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathname_expansion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmervin <vmervin@student-21.school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/07 13:35:37 by vmervin           #+#    #+#             */
/*   Updated: 2022/08/03 16:21:02 by vmervin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static char	*skip_quote(char *newstr, char *str, int *tmp, t_token *tok)
{
	int	end;
	int	begin;

	begin = *tmp;
	end = tok->begin;
	newstr = ft_strjoin_free(newstr, ft_substr(str, begin, end - begin));
	begin = end + 1;
	end = tok->end;
	newstr = ft_strjoin_free(newstr, ft_substr(str, begin, end - begin));
	begin = tok->end + 1;
	*tmp = begin;
	return (newstr);
}

static char	*remove_quotes(t_list *lst, char *str)
{
	char	*newstr;
	int		begin;

	begin = 0;
	if (!str || !lst)
		return (str);
	newstr = ft_strdup("");
	while (lst)
	{
		if (((t_token *)lst->content)->tokentype != '$')
			newstr = skip_quote(newstr, str, &begin,
					((t_token *)lst->content));
		lst = lst->next;
	}
	if (ft_strlen(str) > (size_t)begin)
		newstr = ft_strjoin_free(newstr, ft_substr(str, begin,
					ft_strlen(str) - begin));
	free(str);
	return (newstr);
}

static char	*expand(char *string, int herdoc)
{
	t_parser	service;

	service.tokens = NULL;
	service.string = string;
	service.error = 0;
	if (ft_strchr(string, '$') || ft_strchr(string, '\'')
		|| ft_strchr(string, '\"'))
	{
		quote_token_search(&service);
		dollar_sign_token_search(&service.tokens, '$', string);
		ft_list_sort(&service.tokens, compare_tokens);
		if (!herdoc)
		{
			string = extractor(string, &service);
		}
		string = remove_quotes(service.tokens, string);
		ft_lstclear(&service.tokens, free);
	}
	return (string);
}

void	parse_word(t_list *lst, int vars)
{
	while (lst)
	{
		if (vars == VARIABLE)
		{
			((t_file *)(lst->content))->value
				= expand(((t_file *)(lst->content))->value, 0);
		}
		else if (vars == INPUT && ((t_file *)(lst->content))->append)
		{
			if (ft_strchr(((t_file *)(lst->content))->name, '\'')
			|| ft_strchr(((t_file *)(lst->content))->name, '\"'))
				((t_file *)(lst->content))->append = HERDOC_NOEXPAND;
			((t_file *)(lst->content))->name
				= expand(((t_file *)(lst->content))->name, 1);
		}
		else
			((t_file *)(lst->content))->name
				= expand(((t_file *)(lst->content))->name, 0);
		lst = lst->next;
	}
}
