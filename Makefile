# Build test.printf and test.format, verify they have identical output
SHELL=bash

.PHONY: default
default: test.printf test.format
	@ echo "Testing..."
	@ nm test.printf | grep -q printf
	@ ! nm test.format | grep -q printf
	@ ! diff -U0 --label=printf --label=format --suppress-common <(./test.printf) <(./test.format) | grep -v '^@'
	@ echo "TEST PASS!"

COPTS = -O2 -Wall -Werror

# 'make BROKEN=1' induces test failure
ifdef BROKEN
COPTS += -Wformat=0 -DBROKEN
endif


# Always rebuild
.PHONY: test.printf test.format

# build the reference code with real printf
test.printf: test.c
	${CC} ${COPTS} -DREFERENCE -o $@ $<

# build the test code linked to format.c
test.format: test.c format.c
	rm -f *.su
	${CC} ${COPTS} -fstack-usage -o $@ $^
	cat *.su

.PHONY: clean
clean:; git clean -fx
