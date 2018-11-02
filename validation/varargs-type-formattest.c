
extern void pf1(char *msg, ...) __attribute__((format (printf, 1, 2)));
extern void pf2(int m, char *msg, ...) __attribute__((format (printf, 2, 3)));

/* run all the tests with both of these printf formatted types */
#define pf(x...) do { pf1(x); pf2(1, x); } while(0);

static void test(void) {
	/* first two are valid */
	pf("%*d", 5, 10);	/* value 10, print width is 5 */
	pf("%2$*1$d", 5, 10);	/* value 10, print width is 5 */
	pf("%2$*3$d", 5, 10);	/* value 10, print width is ?? */

	pf("%*d", 5, 10);	/* value 10, print width is 5 */
	pf("%*d", 5, 10L);	/* value 10, print width is 5 (bad type) */
	pf("%*d", 5UL, 10L);	/* value 10, print width is 5 (bad type) */

	pf("%3$*2$d", 1, 5, 10);	/* ok, skipping the '1' */
	pf("%3$*2$d", 1, 5, 10L);	/* bad print type */
	pf("%2$*3$d", 1UL, 10, 5);	/* ok, try with swapping width/val */
	pf("%2$*3$d", 1UL, 10L, 5);	/* bad, try with swapping width/val */

	/* and now try with precision specifiers */

	pf("%*.6d", 5, 10);	/* value 10, print width is 5 */
	pf("%*.6d", 5, 10L);	/* value 10, print width is 5 (bad type) */
	pf("%*.6d", 5UL, 10L);	/* value 10, print width is 5 (bad type) */

	pf("%*.*d", 5, 6, 10);	/* value 10, print width is 5 */
	pf("%*.*d", 5, 6, 10L);	/* value 10, print width is 5 (bad type) */
	pf("%*.*d", 5UL, 6, 10L); /* value 10, print width is 5 (bad type) */
	pf("%*.*d", 5, 6UL, 10); /* value 10, print width is 5 (bad type) */
}

/*
 * check-name: variadic formatting test position checking types
 * check-command: sparse -Wformat $file
 * check-known-to-fail
 *
 * check-error-start
varargs-type-formattest.c:12:9: warning: width: no argument at position 4
varargs-type-formattest.c:12:9: warning: width: no argument at position 5
varargs-type-formattest.c:15:9: warning: incorrect type in argument 3 (different types)
varargs-type-formattest.c:15:9:    expected int
varargs-type-formattest.c:15:9:    got long
varargs-type-formattest.c:15:9: warning: incorrect type in argument 4 (different types)
varargs-type-formattest.c:15:9:    expected int
varargs-type-formattest.c:15:9:    got long
varargs-type-formattest.c:16:9: warning: incorrect type for width argument 2
varargs-type-formattest.c:16:9:    expected int
varargs-type-formattest.c:16:9:    got unsigned long
varargs-type-formattest.c:16:9: warning: incorrect type in argument 3 (different types)
varargs-type-formattest.c:16:9:    expected int
varargs-type-formattest.c:16:9:    got long
varargs-type-formattest.c:16:9: warning: incorrect type for width argument 3
varargs-type-formattest.c:16:9:    expected int
varargs-type-formattest.c:16:9:    got unsigned long
varargs-type-formattest.c:16:9: warning: incorrect type in argument 4 (different types)
varargs-type-formattest.c:16:9:    expected int
varargs-type-formattest.c:16:9:    got long
varargs-type-formattest.c:19:9: warning: incorrect type in argument 4 (different types)
varargs-type-formattest.c:19:9:    expected int
varargs-type-formattest.c:19:9:    got long
varargs-type-formattest.c:19:9: warning: incorrect type in argument 5 (different types)
varargs-type-formattest.c:19:9:    expected int
varargs-type-formattest.c:19:9:    got long
varargs-type-formattest.c:21:9: warning: incorrect type in argument 3 (different types)
varargs-type-formattest.c:21:9:    expected int
varargs-type-formattest.c:21:9:    got long
varargs-type-formattest.c:21:9: warning: incorrect type in argument 4 (different types)
varargs-type-formattest.c:21:9:    expected int
varargs-type-formattest.c:21:9:    got long
varargs-type-formattest.c:26:9: warning: incorrect type in argument 3 (different types)
varargs-type-formattest.c:26:9:    expected int
varargs-type-formattest.c:26:9:    got long
varargs-type-formattest.c:26:9: warning: incorrect type in argument 4 (different types)
varargs-type-formattest.c:26:9:    expected int
varargs-type-formattest.c:26:9:    got long
varargs-type-formattest.c:27:9: warning: incorrect type for width argument 2
varargs-type-formattest.c:27:9:    expected int
varargs-type-formattest.c:27:9:    got unsigned long
varargs-type-formattest.c:27:9: warning: incorrect type in argument 3 (different types)
varargs-type-formattest.c:27:9:    expected int
varargs-type-formattest.c:27:9:    got long
varargs-type-formattest.c:27:9: warning: incorrect type for width argument 3
varargs-type-formattest.c:27:9:    expected int
varargs-type-formattest.c:27:9:    got unsigned long
varargs-type-formattest.c:27:9: warning: incorrect type in argument 4 (different types)
varargs-type-formattest.c:27:9:    expected int
varargs-type-formattest.c:27:9:    got long
varargs-type-formattest.c:30:9: warning: incorrect type in argument 4 (different types)
varargs-type-formattest.c:30:9:    expected int
varargs-type-formattest.c:30:9:    got long
varargs-type-formattest.c:30:9: warning: incorrect type in argument 5 (different types)
varargs-type-formattest.c:30:9:    expected int
varargs-type-formattest.c:30:9:    got long
varargs-type-formattest.c:31:9: warning: incorrect type for width argument 2
varargs-type-formattest.c:31:9:    expected int
varargs-type-formattest.c:31:9:    got unsigned long
varargs-type-formattest.c:31:9: warning: incorrect type in argument 4 (different types)
varargs-type-formattest.c:31:9:    expected int
varargs-type-formattest.c:31:9:    got long
varargs-type-formattest.c:31:9: warning: incorrect type for width argument 3
varargs-type-formattest.c:31:9:    expected int
varargs-type-formattest.c:31:9:    got unsigned long
varargs-type-formattest.c:31:9: warning: incorrect type in argument 5 (different types)
varargs-type-formattest.c:31:9:    expected int
varargs-type-formattest.c:31:9:    got long
varargs-type-formattest.c:32:9: warning: incorrect type for position argument 3
varargs-type-formattest.c:32:9:    expected int
varargs-type-formattest.c:32:9:    got unsigned long
varargs-type-formattest.c:32:9: warning: incorrect type for position argument 4
varargs-type-formattest.c:32:9:    expected int
varargs-type-formattest.c:32:9:    got unsigned long
 * check-error-end
 *
 */
