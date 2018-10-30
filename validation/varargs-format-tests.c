
extern void pf(char *msg, ...) __attribute__((format (printf, 1, 2)));

static int test(void)
{
	pf("%*d\n", 5, 10);		/* value 10, print width is 5 */
	pf("%2$*1$d\n", 5, 10);		/* value 10, print width is 5 */
	pf("%3$*2$d\n", 1, 5, 10);	/* ok, skipping the '1' */
	pf("%3$-*2$d\n", 1, 5, 10);	/* ok, skipping the '1' */
	pf("%3$*2$-d\n", 1, 5, 10);	/* bad, the "-" shouldn't be before the 'd' */
	pf("%3$ *2$d\n", 1, 5, 10);	/* ok, skipping the '1' */
	pf("%3$+*2$d\n", 1, 5, 10);	/* ok, skipping the '1' */
	pf("%3$0+*2$d\n", 1, 5, 10);	/* ok, skipping the '1' */
	pf("%3$+0*2$d\n", 1, 5, 10);	/* ok, skipping the '1' */
	pf("%3$+#*2$d\n", 1, 5, 10);	/* ok, skipping the '1' */
	pf("%3$+#*2$.5d\n", 1, 5, 10);	/* ok, skipping the '1' */

	/* go with some precision as well as width strings */
	pf("%2$+*1$.6d\n", 5, 10);	/* ok */
	pf("%2$+*1$.*3$d\n", 5, 10, 6);	/* ok */
	pf("%2$+*3$.*1$d\n", 6, 10, 5);	/* ok */
	pf("%2$+*1$.*d\n", 5, 10, 6);	/* not ok */

	pf("%s", "msg");
	return 0;
}

static void test2(int x, int y, const void *p)
{
	pf("%02x%02x %8p\n", x, y, p);
}

static inline void fn(int x) { pf("%08x\n", x); }
static void test3(int x)
{
	fn;
	fn(x);
}

static void test4(int i, unsigned int u)
{
	pf("%d\n", i);
	pf("%x\n", u);
}

/*
 * check-name: variadic formatting tests for width/precisions
 * check-command: sparse -Wformat $file
 *
 * check-error-start
varargs-format-tests.c:10:12: warning: cannot evaluate type '%3$*2$-d'
varargs-format-tests.c:10:12: warning: cannot evaluate format string
varargs-format-tests.c:22:12: warning: format 3: position: no position specified
 * check-error-end
 */
