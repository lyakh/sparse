
extern void pf(char *msg, ...) __attribute__((format (printf, 1, 2)));

static void test(void) {
	pf("%2$d %u\n", 1U, 1L);
	pf("%3$d %2$u\n", 1U, 1);
	pf("%1$d %2$d\n", 1L, 1);
}

/*
 * check-name: variadic formatting test position checking
 * check-command: sparse -Wformat $file
 * check-known-to-fail
 *
 * check-error-start
varargs-format-position.c:5:29: warning: incorrect type in argument 3 (different types)
varargs-format-position.c:5:29:    expected int
varargs-format-position.c:5:29:    got long
varargs-format-position.c:5:12: warning: format 3: position: no position specified
varargs-format-position.c:5:29: warning: incorrect type in argument 3 (different types)
varargs-format-position.c:5:29:    expected unsigned int
varargs-format-position.c:5:29:    got long
varargs-format-position.c:6:12: warning: no argument at position '4'
varargs-format-position.c:6:31: warning: incorrect type in argument 3 (different types)
varargs-format-position.c:6:31:    expected unsigned int
varargs-format-position.c:6:31:    got int
varargs-format-position.c:7:27: warning: incorrect type in argument 2 (different types)
varargs-format-position.c:7:27:    expected int
varargs-format-position.c:7:27:    got long
 * check-error-end
 *
 */
