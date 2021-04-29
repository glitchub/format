// Exercise various format options and word sizes
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#ifdef REFERENCE
// Reference build, output using printf
#define output(...) printf(__VA_ARGS__)
#else
// Test build, output using format
#include <stdarg.h>

int format(int (*outchar)(int), char* fmt, va_list ap);

void __attribute__((format (printf, 1, 2))) output(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    format(putchar, fmt, ap); // output via putchar
    va_end(ap);
}
#endif

// Given a format string and a value, output a line
#define test(format, value) output(#value " '%" format "' -> '" format "'\n", value)

// Test values
#define CHR     'x'
#define STR     "abcdefghijklmnop"
#define POS     INT_MAX
#define LLPOS   LLONG_MAX
#define NEG     INT_MIN
#define LLNEG   LLONG_MIN
#define UNS     UINT_MAX
#define LLUNS   ULLONG_MAX

int main(void)
{
    test("%c", CHR);
    test("%25c", CHR);
    test("%-25c", CHR);
    test("%s", STR);
    test("%25s", STR);
    test("%25.8s", STR);
    test("%25.0s", STR);
    test("%-25s", STR);
    test("%-25.8s", STR);

    test("%d", POS);
    test("%25d", POS);
    test("%-25d", POS);
    test("%.22d", POS);
    test("%25.22d", POS);
    test("%-25.22d", POS);

    test("%0d", POS);
    test("%025d", POS);
    test("%-25d", POS);
    test("%.22d", POS);
    test("%25.22d", POS);
    test("%-25.22d", POS);

    test("%+d", POS);
    test("%+25d", POS);
    test("%+-25d", POS);
    test("%+.22d", POS);
    test("%+25.22d", POS);
    test("%+-25.22d", POS);

    test("%0+d", POS);
    test("%0+25d", POS);
    test("%+-25d", POS);
    test("%+.22d", POS);
    test("%+25.22d", POS);
    test("%+-25.22d", POS);

    test("% d", POS);
    test("% 25d", POS);
    test("% -25d", POS);
    test("% .22d", POS);
    test("% 25.22d", POS);
    test("% -25.22d", POS);

    test("%0 d", POS);
    test("%0 25d", POS);
    test("% -25d", POS);
    test("% .22d", POS);
    test("% 25.22d", POS);
    test("% -25.22d", POS);

    test("%d", NEG);
    test("%25d", NEG);
    test("%-25d", NEG);
    test("%.22d", NEG);
    test("%25.22d", NEG);

    test("%0d", NEG);
    test("%025d", NEG);
    test("%-25d", NEG);
    test("%.22d", NEG);
    test("%25.22d", NEG);

    test("%+d", NEG);
    test("%+25d", NEG);
    test("%+-25d", NEG);
    test("%+.22d", NEG);
    test("%+25.22d", NEG);
    test("%+-25.22d", NEG);

    test("%0+d", NEG);
    test("%0+25d", NEG);
    test("%+-25d", NEG);
    test("%+.22d", NEG);
    test("%+25.22d", NEG);
    test("%+-25.22d", NEG);

    test("% d", NEG);
    test("% 25d", NEG);
    test("% -25d", NEG);
    test("% .22d", NEG);
    test("% 25.22d", NEG);
    test("% -25.22d", NEG);

    test("%0 d", NEG);
    test("%0 25d", NEG);
    test("% -25d", NEG);
    test("% .22d", NEG);
    test("% 25.22d", NEG);
    test("% -25.22d", NEG);

    test("%lld", LLPOS);
    test("%25lld", LLPOS);
    test("%-25lld", LLPOS);
    test("%.22lld", LLPOS);
    test("%25.22lld", LLPOS);
    test("%-25.22lld", LLPOS);

    test("%0lld", LLPOS);
    test("%025lld", LLPOS);
    test("%-25lld", LLPOS);
    test("%.22lld", LLPOS);
    test("%25.22lld", LLPOS);
    test("%-25.22lld", LLPOS);

    test("%+lld", LLPOS);
    test("%+25lld", LLPOS);
    test("%+-25lld", LLPOS);
    test("%+.22lld", LLPOS);
    test("%+25.22lld", LLPOS);
    test("%+-25.22lld", LLPOS);

    test("%0+lld", LLPOS);
    test("%0+25lld", LLPOS);
    test("%+-25lld", LLPOS);
    test("%+.22lld", LLPOS);
    test("%+25.22lld", LLPOS);
    test("%+-25.22lld", LLPOS);

    test("% lld", LLPOS);
    test("% 25lld", LLPOS);
    test("% -25lld", LLPOS);
    test("% .22lld", LLPOS);
    test("% 25.22lld", LLPOS);
    test("% -25.22lld", LLPOS);

    test("%0 lld", LLPOS);
    test("%0 25lld", LLPOS);
    test("% -25lld", LLPOS);
    test("% .22lld", LLPOS);
    test("% 25.22lld", LLPOS);
    test("% -25.22lld", LLPOS);

    test("%lld", LLNEG);
    test("%25lld", LLNEG);
    test("%-25lld", LLNEG);
    test("%.22lld", LLNEG);
    test("%25.22lld", LLNEG);
    test("%-25.22lld", LLNEG);

    test("%0lld", LLNEG);
    test("%025lld", LLNEG);
    test("%-25lld", LLNEG);
    test("%.22lld", LLNEG);
    test("%25.22lld", LLNEG);
    test("%-25.22lld", LLNEG);

    test("%+lld", LLNEG);
    test("%+25lld", LLNEG);
    test("%+-25lld", LLNEG);
    test("%+.22lld", LLNEG);
    test("%+25.22lld", LLNEG);
    test("%+-25.22lld", LLNEG);

    test("%0+lld", LLNEG);
    test("%0+25lld", LLNEG);
    test("%+-25lld", LLNEG);
    test("%+.22lld", LLNEG);
    test("%+25.22lld", LLNEG);
    test("%+-25.22lld", LLNEG);

    test("% lld", LLNEG);
    test("% 25lld", LLNEG);
    test("% -25lld", LLNEG);
    test("% .22lld", LLNEG);
    test("% 25.22lld", LLNEG);
    test("% -25.22lld", LLNEG);

    test("%0 lld", LLNEG);
    test("%0 25lld", LLNEG);
    test("% -25lld", LLNEG);
    test("% .22lld", LLNEG);
    test("% 25.22lld", LLNEG);
    test("% -25.22lld", LLNEG);

    test("%u", UNS);
    test("%25u", UNS);
    test("%-25u", UNS);
    test("%.22u", UNS);
    test("%25.22u", UNS);
    test("%-25.22u", UNS);

    test("%0u", UNS);
    test("%025u", UNS);
    test("%-25u", UNS);
    test("%.22u", UNS);
    test("%25.22u", UNS);
    test("%-25.22u", UNS);

    test("%x", UNS);
    test("%25x", UNS);
    test("%-25x", UNS);
    test("%.22x", UNS);
    test("%25.22x", UNS);
    test("%-25.22x", UNS);

    test("%0x", UNS);
    test("%025x", UNS);
    test("%-25x", UNS);
    test("%.22x", UNS);
    test("%25.22x", UNS);
    test("%-25.22x", UNS);

    test("%X", UNS);
    test("%25X", UNS);
    test("%-25X", UNS);
    test("%.22X", UNS);
    test("%25.22X", UNS);
    test("%-25.22X", UNS);

    test("%0X", UNS);
    test("%025X", UNS);
    test("%-25X", UNS);
    test("%.22X", UNS);
    test("%25.22X", UNS);
    test("%-25.22X", UNS);

    test("%o", UNS);
    test("%25o", UNS);
    test("%-25o", UNS);
    test("%.22o", UNS);
    test("%25.22o", UNS);
    test("%-25.22o", UNS);

    test("%0o", UNS);
    test("%025o", UNS);
    test("%-25o", UNS);
    test("%.22o", UNS);
    test("%25.22o", UNS);
    test("%-25.22o", UNS);

    test("%llu", LLUNS);
    test("%25llu", LLUNS);
    test("%-25llu", LLUNS);
    test("%.22llu", LLUNS);
    test("%25.22llu", LLUNS);
    test("%-25.22llu", LLUNS);

    test("%0llu", LLUNS);
    test("%025llu", LLUNS);
    test("%-25llu", LLUNS);
    test("%.22llu", LLUNS);
    test("%25.22llu", LLUNS);
    test("%-25.22llu", LLUNS);

    test("%llx", LLUNS);
    test("%25llx", LLUNS);
    test("%-25llx", LLUNS);
    test("%.22llx", LLUNS);
    test("%25.22llx", LLUNS);
    test("%-25.22llx", LLUNS);

    test("%0llx", LLUNS);
    test("%025llx", LLUNS);
    test("%-25llx", LLUNS);
    test("%.22llx", LLUNS);
    test("%25.22llx", LLUNS);
    test("%-25.22llx", LLUNS);

    test("%llX", LLUNS);
    test("%25llX", LLUNS);
    test("%-25llX", LLUNS);
    test("%.22llX", LLUNS);
    test("%25.22llX", LLUNS);
    test("%-25.22llX", LLUNS);

    test("%0llX", LLUNS);
    test("%025llX", LLUNS);
    test("%-25llX", LLUNS);
    test("%.22llX", LLUNS);
    test("%25.22llX", LLUNS);
    test("%-25.22llX", LLUNS);

    test("%llo", LLUNS);
    test("%25llo", LLUNS);
    test("%-25llo", LLUNS);
    test("%.22llo", LLUNS);
    test("%25.22llo", LLUNS);
    test("%-25.22llo", LLUNS);

    test("%0llo", LLUNS);
    test("%025llo", LLUNS);
    test("%-25llo", LLUNS);
    test("%.22llo", LLUNS);
    test("%25.22llo", LLUNS);
    test("%-25.22llo", LLUNS);

#ifdef BROKEN
    // printf doesn't support '%b' so building with -DBROKEN will cause
    // the test to fail. Just to make sure it's really working!
    test("%b", UNS);
#endif
    return 0;
}
