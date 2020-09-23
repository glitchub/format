// Format strings in the style of printf. This is intended for use with gcc in
// embedded systems.
//
// See https://github.com/glitchub/format for more information.
//
// This software is released as-is into the public domain, as described at
// https://unlicense.org. Do whatever you like with it.

// This file may be linked or #include'd by another file.

#ifndef va_start
// mimic stdarg.h
#define _format_mimic
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
#define va_list __builtin_va_list
#endif

#ifdef FORMAT_CHAR
// If FORMAT_CHAR is defined, it invokes putchar-style function to be
// called directly. This is smallest and fastest.
void format(char *fmt, ...)
#else
// Otherwise FORMAT_CHAR is passed as a pointer, this increases stack but
// allows format() to be used for printf and sprintf-style functions.
void format(void (*FORMAT_CHAR) (unsigned int), char *fmt, ...)
#endif
{
    va_list ap;
    va_start(ap, fmt);

    for (; *fmt; fmt++)
    {
        if (*fmt != '%')
        {
#if FORMAT_CRLF
            if (*fmt == '\n') FORMAT_CHAR('\r'); // expand \n to \r\n
#endif
            FORMAT_CHAR(*fmt);
            continue;
        }

        if (*++fmt == '%')
        {
            FORMAT_CHAR('%');
            continue;
        }

        // wedge format options into 32 bits
        struct
        {
            unsigned char left:1;       // true if flag '-'
            unsigned char zero:1;       // true if flag '0'
            unsigned char precise:1;    // true if precision was set
            unsigned char binary:1;     // true if decoding binary
            unsigned char octal:1;      // true if decoding octal
            unsigned char hex:1;        // true if decoding hex (if neither, then decimal)
            unsigned char lower:1;      // true if lowercase hex
            unsigned char space:1;      // true if sign is ' '
            unsigned char plus:1;       // true if sign is '+' (overrides space)
            unsigned char minus:1;      // true if sign is '-' (overrides plus and space)
            unsigned char islong:1;     // true if size long
            unsigned char islonglong:1; // true if size long long
            unsigned char width;        // width 0-255
            unsigned char precision;    // precision 0-255
        } opts = {0};

        // flags
        for(;;fmt++)
        {
            switch (*fmt)
            {
                case '-':               // align left
                    opts.left = 1;
                    continue;
                case '0':               // pad with '0' instead of space
                    opts.zero = 1;
                    continue;
                case '+':               // prefix positive signed with '+'
                    opts.plus = 1;
                    continue;
                case ' ':               // prefix positive signed with ' '
                    opts.space = 1;
                    opts.plus = 0;
                    continue;
            }
            break;
        }

        // width 0 to 255
        if (*fmt == '*')
        {
            // use arg
            opts.width = va_arg(ap, unsigned int);
            fmt++;
        } else
            // inline
            while (*fmt >= '0' && *fmt <= '9') opts.width = (opts.width * 10) + (*fmt++ - '0');

        // precision 0 to 255
        if (*fmt == '.')
        {
            if (*++fmt == '*')
            {
                // use arg
                opts.precision = va_arg(ap, unsigned int);
                fmt++;
            } else
                // inline
                while (*fmt >= '0' && *fmt <= '9') opts.precision = (opts.precision * 10) + (*fmt++ - '0');
            opts.precise = 1; // remember precision is set, for %s"
        }

        // length, 'l' and 'll' only
        if (*fmt == 'l')
        {
            opts.islong = 1;
            if (*++fmt == 'l')
            {
                opts.islonglong = 1;
                fmt++;
            }
        }

        if (*fmt == 'c')
        {
            // char
            if (!opts.left)
                while (opts.width-- > 1) FORMAT_CHAR(' ');      // pad right before char
            FORMAT_CHAR(va_arg(ap, int) & 0xff);                // note char arg promotes to int
            if (opts.left)
                while (opts.width-- > 1) FORMAT_CHAR(' ');      // pad left after char
        } else
        if (*fmt == 's')
        {
            // string
            unsigned char chars = 0;
            char *s = va_arg(ap, char *);
            if (!s)
                s = "(null)";
            while (s[chars] && chars < 255) chars++;            // 255 chars max
            if (opts.precise && opts.precision < chars)
                chars = opts.precision;                         // truncate to precision, possibly 0
            if (opts.width > chars)
                opts.width -= chars;
            else
                opts.width = 0;
            if (!opts.left)
                while (opts.width--) FORMAT_CHAR(' ');          // pad right before string
            while (chars--) FORMAT_CHAR(*s++);
            if (opts.left)
                while (opts.width--) FORMAT_CHAR(' ');          // pad left after string
        } else
        {
            // must be numeric
            unsigned long long number;

            // prepare for decimal       18446744073709551615
            unsigned long long divisor = 10000000000000000000ULL;
            unsigned char digits = 20;

            switch (*fmt)
            {
                case 'i':
                case 'd':
                {
                    // get signed number
                    if (opts.islonglong) number = (long long)va_arg(ap, long long int);
                    else if (opts.islong) number = (long long)va_arg(ap, long int);
                    else number = (long long)va_arg(ap, int);

                    // if negative, set sign char and make it unsigned
                    if ((long long)number < 0)
                    {
                        opts.minus = 1;
                        number = -(long long)number;
                    }
                    break;
                }

                case 'b':
                    opts.binary = 1;
                    divisor = 0x8000000000000000ULL;
                    digits = 64;
                    goto getnum;

                case 'o':
                    opts.octal = 1;
                    //         1777777777777777777777
                    divisor = 01000000000000000000000ULL;
                    digits = 22;
                    goto getnum;

                case 'x':
                    opts.lower = 1;
                    // fall thru

                case 'X':
                    opts.hex = 1;
                    //          ffffffffffffffff
                    divisor = 0x1000000000000000ULL;
                    digits = 16;
                    goto getnum;

                case 'u':
                    // fall thru

                getnum:
                    // get unsigned number
                    opts.plus = 0; // ignore flags
                    opts.space = 0;
                    if (opts.islonglong) number = va_arg(ap, unsigned long long int);
                    else if (opts.islong) number = va_arg(ap, unsigned long int);
                    else number = va_arg(ap, unsigned int);
                    break;

                default:
                    // ignore others
                    continue;
            }

            // shift divisor to first non zero
            #define RADIX (opts.binary ? 2 : opts.octal ? 8 : opts.hex ? 16 : 10)
            #define SIGN (opts.minus ? '-' : opts.plus ? '+' : opts.space ? ' ' : 0)
            while (digits > 1 && divisor > number)
            {
                divisor /= RADIX;
                digits--;
            }

            if (opts.width && SIGN) opts.width--;           // adjust width for sign

            if (!opts.left && opts.zero && !opts.precise)   // maybe right justify with zeros
                opts.precision = opts.width;

            if (opts.precision < digits)
                opts.precision = digits;                    // minimum precision is number of digits

            if (opts.width > opts.precision)                // adjust width for precision
                opts.width -= opts.precision;
            else
                opts.width = 0;

            if (!opts.left)                                 // right justified
                while (opts.width--) FORMAT_CHAR(' ');

            if (SIGN) FORMAT_CHAR(SIGN);                    // put sign

            while (opts.precision-- > digits)               // pad with zeros to precision
                FORMAT_CHAR('0');

            while (digits--)                                // output digits in left-to-right order
            {
                unsigned char n = (number / divisor) % RADIX;
                FORMAT_CHAR((n < 10) ? n+'0' : opts.lower ? n-10+'a' : n-10+'A');
                divisor /= RADIX;
            }

            if (opts.left)                                  // left justified
                while (opts.width--) FORMAT_CHAR(' ');
        }
    }
    va_end(ap);
}

// Remove transient macros
#ifdef _format_mimic
  #undef _format_mimic
  #undef va_start
  #undef va_end
  #undef va_arg
  #undef va_list
#endif
