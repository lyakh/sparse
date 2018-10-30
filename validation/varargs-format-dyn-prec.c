extern void pf(const char *msg, ...) __attribute__((format(printf, 1, 2)));

static void test(int prec)
{
	pf("%*s\n", prec, "xyz");
}

/*
 * check-name: variadic formatting test dynamic precision
 * check-command: sparse -Wformat $file
 */
