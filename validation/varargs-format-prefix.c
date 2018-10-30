
extern int __attribute__((format (printf, 1, 2))) variadic(char *msg, ...);

static int test(void) {
	void __attribute__((noderef, address_space(1))) *a;

	variadic("%s\n", a);
}

/*
 * check-name: variadic formatting test prefix based __attribute__
 * check-command: sparse -Wformat $file
 *
 * check-error-start
varargs-format-prefix.c:7:26: warning: incorrect type in argument 2 (different address spaces)
varargs-format-prefix.c:7:26:    expected char const *
varargs-format-prefix.c:7:26:    got void [noderef] <asn:1> *a
 * check-error-end
 */
