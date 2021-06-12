/*
 * Formatted output for small microcontrollers
 *
 * %[flags][width][.precison][length]specifier
 *
 * Some differences from the standard printf:
 *
 *   - No floating point support.
 *
 *   - Supported flags: -, +, 0, space, and also:
 *
 *       =<char>  Pad with <char> (except *).
 *       =*       Pad with a character taken from the argument.
 *
 *   - Width and precision have maximum value of 254. If '*' then take
 *     its value from the argument.
 *
 *   - Supported specifiers: d, i, u, o, x, X, c, s, %, and also:
 *
 *       S        On Harvard architectures, a string in the program memory.
 *                On von Neumann architectures, the same as 's'.
 *       b        Unsigned binary integer.
 *
 *   - Length: 'l' for 32 bits, nothing for int (at least 16 bits).
 *
 *   - On Harvard architectures the format string must be placed in the
 *     program memory.
 */

#ifndef PRINT_HPP_
#define PRINT_HPP_

#include <stdarg.h>

class Print {
public:
        void printf(const char *format, ...);
        void vprintf(const char *format, va_list args);
        virtual void putc(char c) = 0;
};

#endif
