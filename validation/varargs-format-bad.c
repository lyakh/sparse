
extern int variadic(char *msg, ...) __attribute__((format (printf, 0, 0)));
extern int variadic2(char *msg, int , ...) __attribute__((format (printf, 2, 2)));
extern int variadic3(char *msg, int , ...) __attribute__((format (printf, 2, 1)));

static void test(void) {
}

/*
 * check-name: variadic formatting test with bad formatting parameters
 * check-command: sparse -Wformat $file
 *
 * check-error-start
varargs-format-bad.c:2:73: warning: bad format positions
varargs-format-bad.c:3:80: warning: bad format positions
varargs-format-bad.c:4:80: warning: format cannot be after va_args
* check-error-end
 */
