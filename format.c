// Format strings in the style of printf. This is intended for use with gcc in
// embedded systems.
//
// See https://github.com/glitchub/format for more information.
//
// This software is released as-is into the public domain, as described at
// https://unlicense.org. Do whatever you like with it.

// This file may be linked or #include'd by another file.

// Special #defines:
//     FORMAT_STATIC causes format to be defined static (only if #included by another file)
//     FORMAT_CHAR(c) if format should write character via specific function, see below
//     FORMAT_CRLF causes all '\n's to be expanded to '\r\n'

#ifndef va_start
// mimic stdarg.h
#define format_stdarg
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg
#define va_list __builtin_va_list
#endif

#ifdef FORMAT_STATIC
static
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

        // flags
        unsigned char left = 0;         // true if flag '-' (left justify)
        unsigned char zero = 0;         // true if flag '0' (pad with zeros)
        unsigned char sign = 0;         // decimal prefix, or 0
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
                case '#':               // ignore other possible flags
                case '\'':
                case 'I':
                    continue;
            }
            break;
        }

        // width 0-255
        unsigned char width = 0;
        if (*fmt == '*')
        {
            // use arg
            width = va_arg(ap, int);
            fmt++;
        } else
            // inline
            while (*fmt >= '0' && *fmt <= '9') width = (width * 10) + (*fmt++ - '0');

        // precision 0-255, or -1 if not set
        short precision = -1;
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
            precision &= 255;
        }

        // size 0=int, 1='l', 2='ll'
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
            char c = va_arg(ap, int) & 0xff;               // note char arg promotes to int
            if (!left)
                while (width-- > 1) FORMAT_CHAR(' ');      // pad right before char
#if FORMAT_CRLF
            if (c == '\n') FORMAT_CHAR('\r');
#endif
            FORMAT_CHAR(c);
            if (left)
                while (width-- > 1) FORMAT_CHAR(' ');      // pad left after char
        } else
        if (*fmt == 's')
        {
            // string
            unsigned char chars = 0;
            char *s = va_arg(ap, char *);
            if (!s)
                s = "(null)";
            while (s[chars] && chars < 255) chars++;       // 255 chars max
            if (precision >= 0 && precision < chars)
                chars = precision;                         // truncate to precision, possibly 0
            if (width > chars) width -= chars;
            else width = 0;
            if (!left)
                while (width--) FORMAT_CHAR(' ');          // pad right before string
            while (chars--)
            {
                char c = *s++;
#if FORMAT_CRLF
                if (c == '\n') FORMAT_CHAR('\r');
#endif
                FORMAT_CHAR(c);
            }
            if (left)
                while (width-- > 0) FORMAT_CHAR(' ');     // pad left after string
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
                    switch(size)
                    {
                        case 0: number = (long long)va_arg(ap, int); break;
                        case 1: number = (long long)va_arg(ap, long int); break;
                        case 2: number = (long long)va_arg(ap, long long int); break;
                    }

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
                    goto getnum;

                case 'o':
                    radix = 8;
                    //         1777777777777777777777
                    divisor = 01000000000000000000000ULL;
                    digits = 22;
                    goto getnum;

                case 'x':
                    lower = 1;
                    // fall thru

                case 'X':
                    radix = 16;
                    //          ffffffffffffffff
                    divisor = 0x1000000000000000ULL;
                    digits = 16;
                    goto getnum;

                case 'u':
                    lower = 1;
                    // fall thru

                getnum:
                    // get unsigned number
                    sign = 0; // ignore flags
                    switch(size)
                    {
                        case 0: number = va_arg(ap, unsigned int); break;
                        case 1: number = va_arg(ap, unsigned long int); break;
                        case 2: number = va_arg(ap, unsigned long long int); break;
                    }
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

            if (!left) while (width--) FORMAT_CHAR(' ');    // right justified

            if (sign) FORMAT_CHAR(sign);                    // put sign

            while (precision-- > digits) FORMAT_CHAR('0');  // pad with zeros to precision

            while (digits--)                                // output digits in left-to-right order
            {
                unsigned char n = (number / divisor) % radix;
                FORMAT_CHAR((n < 10) ? n+'0' : n-10+(lower ? 'a' : 'A'));
                divisor /= radix;
            }

            if (left) while (width--) FORMAT_CHAR(' ');     // left justified
        }
    }
    va_end(ap);
}

// Remove transient macros
#undef FORMAT_CHAR
#undef FORMAT_STATIC
#undef FORMAT_CRLF
#undef format_sign
#ifdef format_stdarg
  #undef format_stdarg
  #undef va_start
  #undef va_end
  #undef va_arg
  #undef va_list
#endif
