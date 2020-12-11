// Lightweigt text formatting in the style of vprintf. Does not require heap or
// large memory buffers. This is intended for use with gcc in embedded systems.
//
// See https://github.com/glitchub/format for more information.
//
// This software is released as-is into the public domain, as described at
// https://unlicense.org. Do whatever you like with it.

// This file may be linked or #include'd by another file. In the latter case,
// keywords such as 'static' appearing immediately before the #include will
// apply to the format function definition.

#ifndef va_list
#include <stdarg.h>
#endif

// Given a pointer to a putchar-like function, a format string, and a va_list,
// write the formatted text character-by-character to outchar()
int format(int(*outchar)(int), char *fmt, va_list ap)
{
    // Get numeric argument of specified type and return as unsigned long long:
    // 0 = unsigned int, 1 = unsigned long, 2 = unsigned long long, ~0 = int,
    // ~1 = long, ~2 = long long.
    unsigned long long getn(int type)
    {
        switch (type)
        {
            case 0:  return (unsigned long long) va_arg(ap, unsigned int);
            case ~0: return (unsigned long long) va_arg(ap, int);
            case 1:  return (unsigned long long) va_arg(ap, unsigned long);
            case ~1: return (unsigned long long) va_arg(ap, long);
        }
#ifndef FORMAT_MIPS_BUG
        return va_arg(ap, unsigned long long);
#elif MIPSEB
        // Turn this on if long longs (or fields after a long long) don't print
        // correctly or segfault on MIPSEB. Possibly a bug in va_start, long
        // longs are aligned on 4 and 12 instead of 0 and 8
        if (!(((void *)ap - (void *)0) & 7)) ap = (void *)ap + 4;   // force the misalignment
        unsigned int a = va_arg(ap, unsigned int);                  // read as two ints
        unsigned int b = va_arg(ap, unsigned int);
        return ((unsigned long long) a << 32) | b;                  // return long long
#else
#error FORMAT_MIPS_BUG not supported on this platform
#endif
    }

    for (; *fmt; fmt++)
    {
        if (*fmt != '%')
        {
            if (outchar(*fmt) < 0) return -1;
            continue;
        }

        if (*++fmt == '%')
        {
            if (outchar('%') < 0) return -1;
            continue;
        }

        // flags
        unsigned int left = 0;      // true if flag '-' (left justify)
        unsigned int zero = 0;      // true if flag '0' (pad with zeros)
        unsigned int sign = 0;      // decimal prefix, or 0
        for(;;fmt++)
        {
            switch (*fmt)
            {
                case '-':               // align left
                    left = 1;
                    continue;
                case '0':               // pad with '0' instead of space
                    zero = 1;
                    continue;
                case '+':               // prefix positive signed with '+'
                    sign = '+';
                    continue;
                case ' ':               // prefix positive signed with ' '
                    sign = ' ';
                    continue;
                case '#':               // alternate form, ignored
                    continue;
            }
            break;
        }

        // width, 0 if not set
        int width = 0;
        if (*fmt == '*')
        {
            // use arg
            width = va_arg(ap, int);
            fmt++;
        } else
            // inline
            while (*fmt >= '0' && *fmt <= '9') width = (width * 10) + (*fmt++ - '0');
        if (width < 0) width = 0;

        // precision, -1 if not set
        int precision = -1;
        if (*fmt == '.')
        {
            if (*++fmt == '*')
            {
                // use arg
                precision = va_arg(ap, int);
                fmt++;
            } else
            {
                // inline
                precision = 0;
                while (*fmt >= '0' && *fmt <= '9') precision = (precision * 10) + (*fmt++ - '0');
            }
        }

        // size 0 = int, 1 = 'l', 2 = 'll'
        unsigned char size = 0;
        if (*fmt == 'l')
        {
            size = 1;
            if (*++fmt == 'l')
            {
                size = 2;
                fmt++;
            }
        }

        if (*fmt == 'c')
        {
            // char
            int c = va_arg(ap, int) & 0xff;                 // note char arg promotes to int
            if (!left)
                while (width-- > 1)
                    if (outchar(' ') < 0) return -1;        // pad right before char
            if (outchar(c & 0xff) < 0) return -1;
            if (left)
                while (width-- > 1)
                    if (outchar(' ') < 0) return -1;        // pad left after char
        } else
        if (*fmt == 's')
        {
            // string
            int chars = 0;
            char *s = va_arg(ap, char *);
            if (!s)
                s = "(null)";
            while (s[chars]) chars++;                       // strlen
            if (precision >= 0 && precision < chars)
                chars = precision;                          // truncate to precision, possibly 0
            width -= chars;
            if (!left)
                while (width-- > 0)
                    if (outchar(' ') < 0) return -1;        // pad right before string
            while (chars--)
            {
                char c = *s++;
                if (outchar(c) < 0) return -1;
            }
            if (left)
                while (width-- > 0)
                    if (outchar(' ') < 0) return -1;        // pad left after string
        } else
        {
            // must be numeric
            unsigned long long number;

            // prepare for decimal       18446744073709551615
            unsigned long long divisor = 10000000000000000000ULL;
            unsigned char digits = 20;
            unsigned char radix = 10;
            unsigned char lower = 0;  // true if lowercase

            switch (*fmt)
            {
                case 'i':
                case 'd':
                {
                    // get signed number
                    number = (long long)getn(~size); // invert to retrieve as signed

                    // if negative, set sign char and make it unsigned
                    if ((long long)number < 0)
                    {
                        sign = '-';
                        number = -(long long)number;
                    }
                    break;
                }

                case 'b':
                    radix = 2;
                    divisor = 0x8000000000000000ULL;
                    digits = 64;
                    goto getuns;

                case 'o':
                    radix = 8;
                    //         1777777777777777777777
                    divisor = 01000000000000000000000ULL;
                    digits = 22;
                    goto getuns;

                case 'x':
                    lower = 1;
                    // fall thru

                case 'X':
                    radix = 16;
                    //          ffffffffffffffff
                    divisor = 0x1000000000000000ULL;
                    digits = 16;
                    goto getuns;

                case 'u':
                    lower = 1;
                    // fall thru

                getuns:
                    // get unsigned number
                    sign = 0; // ignore flags
                    number = getn(size);
                    break;

                default:
                    // ignore others
                    continue;
            }

            // shift divisor to first non zero
            while (digits > 1 && divisor > number)
            {
                divisor /= radix;
                digits--;
            }

            if (width && sign) width--;                     // adjust width for sign

            if (!left && zero && precision < 0)             // maybe right justify with zeros
                precision = width;

            if (precision < digits) precision = digits;     // minimum precision is number of digits

            if (width > precision)                          // adjust width for precision
                width -= precision;
            else
                width = 0;

            if (!left)
                while (width-- > 0)
                    if (outchar(' ') < 0) return -1;        // right justified

            if (sign)
                if (outchar(sign) < 0) return -1;           // put sign

            while (precision-- > digits)
                if (outchar('0') < 0) return -1;            // pad with zeros to precision

            while (digits--)                                // output digits in left-to-right order
            {
                unsigned char n = (number / divisor) % radix;
                n = (n < 10) ? n+'0' : n-10+(lower ? 'a' : 'A');
                if (outchar(n) < 0) return -1;
                divisor /= radix;
            }

            if (left)
                while (width-- > 0)
                    if (outchar(' ') < 0) return -1;        // left justified
        }
    }
    return 0; // success
}
