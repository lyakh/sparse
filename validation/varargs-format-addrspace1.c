
extern int variadic(char *msg, ...) __attribute__((format (printf, 1, 2)));
extern int variadic2(char *msg, int , ...) __attribute__((format (printf, 1, 3)));
extern int variadic3(int, char *msg,  ...) __attribute__((format (printf, 2, 3)));

static void test(void) {
	void __attribute__((noderef, address_space(1))) *a;
	void *b;

	variadic("%s\n", a);
	variadic("%s\n", b);
	variadic("%s %s\n", b, a);
	variadic2("%s %s\n", 1, b, a);
	variadic3(1, "%s %s\n", b, a);
	variadic3(1, "%s %p\n", b, a);
}

/*
 * check-name: variadic formatting test with address-space to %s
 * check-command: sparse -Wformat $file
 *
 * check-error-start
varargs-format-addrspace1.c:10:26: warning: incorrect type in argument 2 (different address spaces)
varargs-format-addrspace1.c:10:26:    expected char const *
varargs-format-addrspace1.c:10:26:    got void [noderef] <asn:1> *a
varargs-format-addrspace1.c:12:32: warning: incorrect type in argument 3 (different address spaces)
varargs-format-addrspace1.c:12:32:    expected char const *
varargs-format-addrspace1.c:12:32:    got void [noderef] <asn:1> *a
varargs-format-addrspace1.c:13:36: warning: incorrect type in argument 4 (different address spaces)
varargs-format-addrspace1.c:13:36:    expected char const *
varargs-format-addrspace1.c:13:36:    got void [noderef] <asn:1> *a
varargs-format-addrspace1.c:14:36: warning: incorrect type in argument 4 (different address spaces)
varargs-format-addrspace1.c:14:36:    expected char const *
varargs-format-addrspace1.c:14:36:    got void [noderef] <asn:1> *a
 * check-error-end
 */
