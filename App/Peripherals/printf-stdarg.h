/* printf-stdarg.h */

/* Header for a minimal printf implementation based on stdarg */

#ifndef PRINTF_STDARG_H
#define PRINTF_STDARG_H

#include <stdarg.h> // Necessary for va_list

int printf(const char *format, ...);

#endif // PRINTF_STDARG_H

#ifndef putchar
#define putchar(c) UART_putChar(c)
#endif
