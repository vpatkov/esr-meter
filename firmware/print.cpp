#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>
#include "common.hpp"
#include "print.hpp"

/*
 * For Harvard architectures define "pm_read(p)" to read one byte from a
 * program memory pointer "p", and "pm_attr" to the attribute for data to be
 * placed in the program memory.
 */
#if defined(__AVR_ARCH__)       /* AVR */
#  include <avr/pgmspace.h>
#  define pm_read(p) pgm_read_byte(p)
#  define pm_attr PROGMEM
#else                           /* von Neumann */
#  define pm_read(p) (*(p))
#  define pm_attr
#endif

void Print::printf(const char *format, ...)
{
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
}

void Print::vprintf(const char *format, va_list args)
{
        struct Options {
                bool signed_: 1;
                bool force_sign: 1;
                bool space_for_plus: 1;
                bool negative: 1;
                bool left_justify: 1;
                bool uppercase: 1;
                bool length_32: 1;
        };

        for (char c = pm_read(format++); c != 0; c = pm_read(format++))
        {
                if (c != '%') {
                        putc(c);
                        continue;
                }

                /* ======== Flags ======== */

                Options options = {};
                char pad = ' ';
next_flag:
                switch (c = pm_read(format++)) {
                case '+':
                        options.force_sign = 1;
                        goto next_flag;
                case '-':
                        options.left_justify = 1;
                        goto next_flag;
                case '0':
                        pad = '0';
                        goto next_flag;
                case ' ':
                        options.space_for_plus = 1;
                        goto next_flag;
                case '=':
                        c = pm_read(format++);
                        if (c == 0)
                                break;
                        if (c == '*')
                                pad = va_arg(args, int);
                        else
                                pad = c;
                        goto next_flag;
                }

                if (c == 0)
                        break;

                /* ======== Width ======== */

                uint8_t width = 0;
                if (c == '*') {
                        width = va_arg(args, int);
                        c = pm_read(format++);
                } else {
                        while (c >= '0' && c <= '9') {
                                width = width * 10 + c - '0';
                                c = pm_read(format++);
                        }
                }

                if (c == 0)
                        break;

                /* ======== Precision ======== */

                uint8_t precision = 0;
                if (c == '.') {
                        c = pm_read(format++);
                        if (c == '*') {
                                precision = va_arg(args, int);
                                c = pm_read(format++);
                        } else {
                                while (c >= '0' && c <= '9') {
                                        precision = precision * 10 + c - '0';
                                        c = pm_read(format++);
                                }
                        }
                }

                /* ======== Length ======== */

                if (c == 'l') {
                        options.length_32 = 1;
                        c = pm_read(format++);
                }

                if (c == 0)
                        break;

                /* ======== Specifier ======== */

                uint8_t base;
                switch (c) {
                case 's':
                case 'S': {
                        if (precision == 0)
                                precision = 254;

                        uint8_t len = 0;
                        const char *s = va_arg(args, const char *);
                        while (len < precision && (c == 's' ? s[len] : pm_read(&s[len])) != 0)
                                len++;

                        if (!options.left_justify)
                                for (uint8_t i = len; i < width; i++)
                                        putc(pad);

                        for (uint8_t i = 0; i < len; i++)
                                putc(c == 's' ? s[i] : pm_read(&s[i]));

                        if (options.left_justify)
                                for (uint8_t i = len; i < width; i++)
                                        putc(pad);

                        continue;
                }
                case 'c':
                        if (!options.left_justify)
                                for (uint8_t i = 1; i < width; i++)
                                        putc(pad);
                        putc(va_arg(args, int));
                        if (options.left_justify)
                                for (uint8_t i = 1; i < width; i++)
                                        putc(pad);
                        continue;
                case 'i':
                case 'd':
                        options.signed_ = 1;
                case 'u':
                        base = 10;
                        break;
                case 'o':
                        base = 8;
                        break;
                case 'b':
                        base = 2;
                        break;
                case 'X':
                        options.uppercase = 1;
                case 'x':
                        base = 16;
                        break;
                case '%':
                        putc('%');
                default:
                        continue;
                }

                /* ======== Print the integer ======== */

                static_assert(UINT32_MAX >= UINT_MAX, "");
                uint32_t val;
                if (options.length_32)
                        val = va_arg(args, uint32_t);
                else
                        val = va_arg(args, int);

                if (options.signed_ && (val & 0x80000000)) {
                        val = -val;
                        options.negative = 1;
                }

                char buf[256];
                uint8_t len = 0;
                do {
                        char d = val % base;
                        val /= base;
                        if (d > 9)
                                d += options.uppercase ? 0x07 : 0x27;
                        buf[len++] = d + '0';
                } while (val > 0);

                while (len < precision)
                        buf[len++] = '0';

                char sign_char =
                        options.negative ? '-' :
                        options.force_sign ? '+' :
                        options.space_for_plus ? ' ' : 0;

                uint8_t len_ = len;
                if (sign_char != 0) {
                        len_++;
                        if (pad == '0')
                                putc(sign_char);
                        else
                                buf[len++] = sign_char;
                }

                if (!options.left_justify)
                        for (uint8_t i = len_; i < width; i++)
                                putc(pad);

                for (uint8_t i = len; i; i--)
                        putc(buf[i-1]);

                if (options.left_justify)
                        for (uint8_t i = len_; i < width; i++)
                                putc(pad);
        }
}
