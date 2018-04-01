#ifndef __LIBS_STDIO_H__
#define __LIBS_STDIO_H__

#include "defs.h"
#include "stdarg.h"

typedef void (*putchar_func_t)(int ch);
typedef int (*getchar_func_t)();

void setup_stdio(putchar_func_t pf, getchar_func_t gf);

/* stdio.c */
int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list ap);
void putchar(int c);
int puts(const char *str);
int getchar(void);

/* printfmt.c */
void printfmt(void (*putch)(int, void *, int), int fd, void *putdat, const char *fmt, ...);
void vprintfmt(void (*putch)(int, void *, int), int fd, void *putdat, const char *fmt, va_list ap);    
int snprintf(char *str, size_t size, const char *fmt, ...);
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap);

#endif /* !__LIBS_STDIO_H__ */

