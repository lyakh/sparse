#ifndef EVALUATE_H
#define EVALUATE_H

struct expression;
struct expression_list;
struct statement;
struct symbol;
struct symbol_list;

///
// evaluate the type of an expression
// @expr: the expression to be evaluated
// @return: the type of the expression or ``NULL``
//	if the expression can't be evaluated
struct symbol *evaluate_expression(struct expression *expr);

///
// evaluate the type of a statement
// @stmt: the statement to be evaluated
// @return: the type of the statement or ``NULL``
//	if it can't be evaluated
struct symbol *evaluate_statement(struct statement *stmt);

///
// evaluate the type of a set of symbols
// @list: the list of the symbol to be evaluated
void evaluate_symbol_list(struct symbol_list *list);

///
// evaluate the arguments of a function
// @fn: the symbol of the prototype
// @argtypes: the list of the types in the prototype
// @args: the list of the effective arguments
int evaluate_arguments(struct symbol *fn, struct symbol_list *argtypes, struct expression_list *args);

///
// check if assignment types are compatible
// @target: the type of the right-hand side
// @rp: the expression corresponding to the left-hand side
// @typediff: a pointer for details about the type difference, if any
// @return: a non-zero value of the types are compatible, 0 otherwise
int check_assignment_types(struct symbol *target, struct expression **rp, const char **typediff);

#endif
