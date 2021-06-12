#include "bubble.hpp"
#include "gpio.hpp"
#include "common.hpp"

/* Active high */
constexpr Gpio::Pin segments[] = {
        Gpio::B7,  /* a */
        Gpio::D5,  /* b */
        Gpio::D3,  /* c */
        Gpio::B0,  /* d */
        Gpio::D4,  /* e */
        Gpio::D7,  /* f */
        Gpio::B1,  /* g */
        Gpio::D1,  /* dp */
};

/* Active high */
constexpr Gpio::Pin digits[] = {
        Gpio::B6,
        Gpio::D6,
        Gpio::D2,
        Gpio::D0,
};

/* 
 * Font for ASCII table starting from space.
 * Values are bitmasks of segments, "a" is MSB, "dp" is LSB.
 *
 * Not-representable, not-readable, or ugly characters are replaced
 * by blank (so don't use it). For some characters its case is changed
 * for the sake of readability: "B" is the same as "b" (not "8"),
 * "l" is the same as "L" (not "I" or "1"), etc. To distinguish
 * between "I" and "1", the letter "I" uses left segments (e, f),
 * but the digit "1" uses the right ones (b, c).
 */
constexpr uint8_t font[128-32] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9c, 0xf0, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
        0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe0,
        0xfe, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xee, 0x3e, 0x9c, 0x7a, 0x9e, 0x8e, 0xbc,
        0x6e, 0x0c, 0x70, 0x00, 0x1c, 0x00, 0x2a, 0xfc,
        0xce, 0xe6, 0x0a, 0xb6, 0x1e, 0x7c, 0x00, 0x00,
        0x00, 0x76, 0xda, 0x9c, 0x00, 0xf0, 0xc4, 0x10,
        0x00, 0xee, 0x3e, 0x1a, 0x7a, 0xde, 0x8e, 0xbc,
        0x2e, 0x08, 0x70, 0x00, 0x1c, 0x00, 0x2a, 0x3a,
        0xce, 0xe6, 0x0a, 0xb6, 0x1e, 0x38, 0x00, 0x00,
        0x00, 0x76, 0xda, 0x00, 0x0c, 0x00, 0x00, 0x00,
};

Bubble::Bubble():
        scan_pos(0),
        putc_pos(0)
{
        clear();
}

void Bubble::init()
{
        for (auto d : digits)
                Gpio::write(d, 0);
}

void Bubble::clear()
{
        for (auto &b : buffer)
                b = 0;
}

void Bubble::scan()
{
        /* Turn off the current digit */
        Gpio::write(digits[scan_pos], 0);

        /* Draw the next digit and turn it on */
        scan_pos = (scan_pos + 1) % size(buffer);
        uint8_t b = buffer[scan_pos];
        Gpio::write(segments[0], b & 0x80);
        Gpio::write(segments[1], b & 0x40);
        Gpio::write(segments[2], b & 0x20);
        Gpio::write(segments[3], b & 0x10);
        Gpio::write(segments[4], b & 0x08);
        Gpio::write(segments[5], b & 0x04);
        Gpio::write(segments[6], b & 0x02);
        Gpio::write(segments[7], b & 0x01);
        Gpio::write(digits[scan_pos], 1);
}

void Bubble::putc(char c)
{
        uint8_t prev_pos = putc_pos > 0 ? putc_pos - 1 : 0;

        if (c == '\b') {
                putc_pos = prev_pos;
                return;
        }

        if (c == '\n') {
                clear();
                putc_pos = 0;
                return;
        }

        if (c == '\r') {
                putc_pos = 0;
                return;
        }

        if (c == '.' || c == ',') {
                if (prev_pos < size(buffer))
                        buffer[prev_pos] |= 1;
                return;
        }

        if (putc_pos < size(buffer))
                buffer[putc_pos] = (c >= 32 && c <= 127) ? font[c-32] : 0;

        if (putc_pos < 255)
                putc_pos++;
}
