# Build test.printf and test.format, verify they have identical output
SHELL=bash

.PHONY: default
default: test.printf test.format
        # sanity check
	nm test.printf | grep -q printf
	(( $$(stat -c%s test.format) > $$(stat -c%s test.printf) + 2048 ))
	! nm test.format | grep -q printf
	! diff -U0 --label=printf --label=format --suppress-common <(./test.printf) <(./test.format) | grep -v '^@'
	@echo "TEST PASS!"

COPTS=-O3 -Wall -Werror -Wformat=0

# 'make BROKEN=1' to induce test failure
ifdef BROKEN
COPTS += -DBROKEN
endif

test.printf: test.c; cc ${COPTS} -DREFERENCE -o test.printf test.c

test.format: test.c format.c; cc ${COPTS} -o test.format test.c

.PHONY: clean
clean:; rm -f test.format test.printf
