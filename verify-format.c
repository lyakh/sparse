/*
 * sparse/verify-format.c
 *
 * Copyright (C) 2019 Codethink Ltd.
 *	Written by Ben Dooks <ben.dooks@codethink.co.uk>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Verification code for format-attributes (currently printf)
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include "evaluate.h"
#include "lib.h"
#include "allocate.h"
#include "parse.h"
#include "token.h"
#include "symbol.h"
#include "target.h"
#include "expression.h"
#include "verify-format.h"

struct format_type {
	const char	*format;
	int		(*test)(struct format_type *fmt,
				struct expression **expr,
				struct symbol *ctype,
				struct symbol **target,
				const char **typediff);
	struct symbol	*type;
};

struct format_state {
	struct expression	*expr;
	unsigned int		first;
	unsigned int		fmt_index;
	unsigned int		arg_index;
	unsigned int		used_position: 1;
};

static int printf_fmt_numtype(struct format_type *fmt,
			      struct expression **expr,
			      struct symbol *ctype,
			      struct symbol **target, const char **typediff)
{
	struct symbol *type = fmt->type;
	*target = type;
	return check_assignment_types(*target, expr, typediff);
}

static int printf_fmt_string(struct format_type *fmt,
			     struct expression **expr,
			     struct symbol *ctype,
			     struct symbol **target, const char **typediff)
{
	*target = fmt->type;
	return check_assignment_types(*target, expr, typediff);
}

static int printf_fmt_pointer(struct format_type *fmt,
			      struct expression **expr,
			      struct symbol *ctype,
			      struct symbol **target, const char **typediff)
{
	*target = &const_ptr_ctype;
	return check_assignment_types(*target, expr, typediff);
}

static int printf_fmt_print_pointer(struct format_type *fmt,
				    struct expression **expr,
				    struct symbol *ctype,
				    struct symbol **target,
				    const char **typediff)
{
	// TODO TODO: fix this here!!!
	int ret;
	*target = &const_ptr_ctype;
	ret = check_assignment_types(*target, expr, typediff);
	if (ret == 0) {
		/* if just printing, ignore address-space mismatches */
		if (strcmp(*typediff, "different address spaces") == 0)
			ret = 1;
	}
	return ret;
}

static struct format_type printf_fmt_ptr_ref = {
	.format = "p",
	.test = printf_fmt_pointer,
};

static int is_float_spec(char t)
{
	return t == 'f' || t == 'g' || t == 'F' || t == 'G';
}

static struct format_type *parse_printf_get_fmt(struct format_type *type,
						const char *msg, const char **msgout)
{
	const char *ptr = msg;
	int szmod=0;

	type->test = NULL;
	*msgout = ptr;

	if (*ptr == 's') {
		ptr++;
		type->test = printf_fmt_string;
		type->type = &const_string_ctype;
	} else if (*ptr == 'c') {
		ptr++;
		type->test = printf_fmt_numtype;
		type->type = &char_ctype;
	} else if (*ptr == 'p') {
		ptr++;
		type->test = printf_fmt_print_pointer;
		/* check for pointer being printed as hex explicitly */
		if (*ptr == 'x' || *ptr == 'X') {
			ptr++;
		} else if (isalpha(*ptr)) {
			/* probably some extra specifiers after %p */
			ptr++;
			type->test = printf_fmt_pointer;
		}
	} else if (*ptr == 'z') {
		// todo - we should construct pointer to int/etc //

		ptr++;
		if (*ptr == 'd' || *ptr == 'i') {
			ptr++;
			type->test = printf_fmt_numtype;
			type->type = ssize_t_ctype;
		} else if (*ptr == 'u' || *ptr == 'x' || *ptr == 'X' ||
			   *ptr == 'o') {
			ptr++;
			type->test = printf_fmt_numtype;
			type->type = size_t_ctype;
		}
	} else {
		if (*ptr == 'l') {
			szmod++;
			ptr++;
			if (*ptr == 'l') {
				szmod++;
				ptr++;
			}
		} else if (*ptr == 'L') {
			szmod++;
			ptr++;
		} else {
			if (*ptr == 'h') { // short/char to int
				szmod = -1;
				ptr++;
				if (*ptr == 'h')  // promotion from char
					ptr++;
			}
			if (*ptr == 't') {  // ptrdiff_t
				szmod = 2;
				ptr++;
			}
			if (*ptr == 'j') { // intmax_t
				// todo - replace iwth intmax_ctype when added
				szmod = 1;
				ptr++;
			}
		}

		if (*ptr == 'x' || *ptr == 'X' || *ptr == 'u' || *ptr == 'o') {
			ptr++;
			type->test = printf_fmt_numtype;
			switch (szmod) {
			case -1:
				type->type = &ushort_ctype;
				break;
			case 0:
				type->type = &uint_ctype;
				break;
			case 1:
				type->type = &ulong_ctype;
				break;
			case 2:
				type->type = &ullong_ctype;
				break;
			default:
				type->test = NULL;
			}
		} else if (*ptr == 'i' || *ptr == 'd') {
			ptr++;
			type->test = printf_fmt_numtype;
			switch (szmod) {
			case -1:
				type->type = &short_ctype;
				break;
			case 0:
				type->type = &int_ctype;
				break;
			case 1:
				type->type = &long_ctype;
				break;
			case 2:
				type->type = &llong_ctype;
				break;
			default:
				type->test = NULL;
			}
		} else if (*ptr == 'L' && is_float_spec(ptr[1])) {
			type->test = printf_fmt_numtype;
			type->type = &ldouble_ctype;
			ptr += 2;
		} else if (is_float_spec(*ptr)) {
			type->test = printf_fmt_numtype;
			type->type = szmod == 1 ? &ldouble_ctype :  &double_ctype;
			ptr++;
		} else if (*ptr == 's') {
			type->test = printf_fmt_string;
			type->type = &const_wstring_ctype;
			ptr++;
		} else if (*ptr == 'n') {	/* pointer to an de-referenced int/etc */
			// todo - we should construct pointer to int/etc //
			// also should not have any flags or widths for this
			type->test = printf_fmt_pointer;
			ptr++;
		} else {
			// anything else here?
		}
	}

	if (type->test == NULL)
		return NULL;

	*msgout = ptr;
	return type;
}

static int is_printf_flag(char ch)
{
	return ch == '0' || ch == '+' || ch == '-' || ch == ' ' || ch == '#';
}

static int printf_check_position(const char **fmt)
{
	const char *ptr= *fmt;

	if (!isdigit(*ptr))
		return -1;
	while (isdigit(*ptr))
		ptr++;
	if (*ptr == '$') {
		const char *pos = *fmt;
		*fmt = ptr+1;
		return strtoul(pos, NULL, 10);
	}
	return -1;
}

static void parse_format_printf_checkpos(struct format_state *state, const char *which)
{
	if (state->used_position) {
		warning(state->expr->pos,
			"format %d: %s: no position specified",
			state->arg_index-1, which);
	}
}

static int parse_format_printf_argfield(const char **fmtptr,
					struct format_state *state,
					struct expression_list *args,
					int *pos, const char *which)
{
	struct expression *expr;
	struct symbol *ctype;
	const char *fmt = *fmtptr;
	int argpos = -1;

	/* check for simple digit-string width/precision specifier first */
	if (*fmt != '*') {
		while (isdigit(*fmt))
			fmt++;
		*fmtptr = fmt;
		return 0;
	}

	fmt++;
	argpos = printf_check_position(&fmt);

	if (argpos > 0) {
		argpos += state->first - 1;
		state->used_position = 1;
	} else {
		argpos = (*pos)++;
		state->arg_index++;
		parse_format_printf_checkpos(state, which);
	}

	*fmtptr = fmt;
	expr = get_nth_expression(args, argpos-1);
	if (!expr) {
		warning(state->expr->pos, "%s: no argument at position %d", which, argpos);
		return 1;
	}

	/* check the value we got was int/uint type */
	ctype = expr->ctype;
	if (ctype) {
		struct symbol *target = &int_ctype;

		if (ctype->type == SYM_NODE)
			ctype = ctype->ctype.base_type;

		if (ctype != &int_ctype && ctype != &uint_ctype) {
			warning(expr->pos, "incorrect type for %s argument %d", which, argpos);
			info(expr->pos, "   expected %s", show_typename(target));
			info(expr->pos, "   got %s", show_typename(ctype));
		}
	}

	return 0;
}

/*
 * printf format parsing code
 *
 * this code currently does not:
 * - check castable types (such as int vs long vs long long)
 * - validate all arguments specified are also used...
 */
static int parse_format_printf(const char **fmtstring,
			       struct format_state *state,
			       struct expression_list *args)
{
	struct format_type ftype;	/* temp storage for format info */
	struct format_type *type;	/* type found from the parse */
	struct expression *expr;
	const char *fmt = *fmtstring;	/* pointer to parse position */
	const char *fmtpost = NULL;	/* moved to end of the parsed format */
	int pos = state->arg_index;	/* position of the argument */
	int error = 0;
	int ret;

	if (!fmt) {
		warning(state->expr->pos, "no format string passed");
		return -1;
	}

	/* trivial check for %% */
	fmt++;
	if (fmt[0] == '%') {
		*fmtstring = fmt+1;
		return 0;
	}

	state->arg_index++;
	state->fmt_index++;

	ret = printf_check_position(&fmt);
	if (ret == 0) {
		/* we got an invalid position argument */
		error++;
	} else if (ret < 0) {
		parse_format_printf_checkpos(state, "position");
	} else {
		state->used_position = 1;
		pos = ret + state->first - 1;
	}

	/* get rid of any formatting flag bits */
	while (is_printf_flag(*fmt))
		fmt++;

	/* now there is the posibility of a width specifier */
	if (parse_format_printf_argfield(&fmt, state, args, &pos, "width"))
		error++;

	/* now we might have the precision specifier */
	if (*fmt == '.') {
		fmt++;
		if (parse_format_printf_argfield(&fmt, state, args, &pos, "position"))
			error++;
	}

	type = parse_printf_get_fmt(&ftype, fmt, &fmtpost);

	if (!type && fmt[0] == 'p')
		type = &printf_fmt_ptr_ref;	/* probably some extension */

	if (type) {
		struct symbol *ctype, *target = NULL;
		const char *typediff = "different types";
		int ret;

		*fmtstring = fmtpost;
		expr = get_nth_expression(args, pos-1);
		if (!expr) {
			/* no argument, but otherwise valid argument string */
			warning(state->expr->pos, "no argument at position '%d'", pos);
			return 0;
		}

		ctype = expr->ctype;
		if (!ctype)
			return -3;

		ret = type->test(type, &expr, ctype, &target, &typediff);
		if (!target)	/* shouldn't happen, but catch anyway */
			return -4;

		if (ret == 0) {
			warning(expr->pos, "incorrect type in argument %d (%s)", pos, typediff);
			info(expr->pos, "   expected %s", show_typename(target));
			info(expr->pos, "   got %s", show_typename(ctype));
		}
	} else {
		/* try and find the end of this format string by looking for a space*/
		fmtpost = *fmtstring;
		while (*fmtpost > ' ')
			fmtpost++;
		warning(state->expr->pos, "cannot evaluate type '%.*s'",
			(int)(fmtpost - *fmtstring), *fmtstring);
		*fmtstring += 1;
		return -1;
	}

	return 1;
}

/*
 * attempt to run through a printf format string and work out the types
 * it specifies. The format is parsed from the __attribute__(format())
 * in the parser code which stores the positions of the message and arg
 * start in the ctype.
 */
void verify_format_attribute(struct symbol *fn, struct expression_list *args)
{
	struct format_state state = { };
	struct expression *expr;
	struct expression *init;
	const char *fmt_string;

	if (!fn || !Wformat)
		return;
	if (!fn->ctype.format.index)
		return;

	expr = get_nth_expression(args, fn->ctype.format.index-1);
	if (!expr)
		return;

	if (expr->type != EXPR_SYMBOL || expr->symbol->ident)
		return;			// not a literal
	init = expr->symbol->initializer;
	if (!init || init->type != EXPR_STRING)
		return;			// not a string
	fmt_string = init->string->data;

	state.expr = expr;
	state.first = fn->ctype.format.first;
	state.arg_index = fn->ctype.format.first;

	if (!fmt_string) {
		warning(expr->pos, "not a format string?");
	} else {
		const char *string = fmt_string;
		int fail = 0;

		while (string[0]) {
			if (string[0] != '%') {
				/* strip anything before the '%' */
				string++;
				continue;
			}

			if (parse_format_printf(&string, &state, args) < 0)
				fail++;
		}

		if (fail > 0)
			/* format string may have '\n' etc embedded in it */
			warning(expr->pos, "cannot evaluate format string");
	}
}
