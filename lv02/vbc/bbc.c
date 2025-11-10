#include "vbc.h"

node	*pars_nr(char **s)
{
	if (!isdigit(**s))
	{
		unexpected(**s);
		return NULL;
	}
	node	n = {.type = VAL, .val = (**s) - '0', .l = NULL, .r = NULL};
	(*s)++;
	return new_node(n);
}

node	*pars_parenthesis(char **s)
{
	if (accept(s, '('))
	{
		node	*expr = pars_expr(s);
		if (expect(s, ')'))
			return expr;
		else
		{
			destroy_tree(expr);
			return NULL;
		}
	}
	return pars_nr(s);
}

node	*pars_mult(char **s)
{
	//mult
	node *left = pars_parenthesis(s);
	while (accept(s, '*'))
	{
		node	*right = pars_mult(s);
		if (!right)
		{
			destroy_tree(left);
			return NULL;
		}
		node n = {.type = MULTI, .l = left, .r = right};
		left = new_node(n);
	}
	return (left);
}

node	*pars_expr(char **s)
{
	//mult
	node *left = pars_mult(s);
	while (accept(s, '+'))
	{
		node	*right = pars_expr(s);
		if (!right)
		{
			destroy_tree(left);
			return NULL;
		}
		node n = {.type = ADD, .l = left, .r = right};
		left = new_node(n);
	}
	return (left);
}