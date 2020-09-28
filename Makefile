# Build test.printf and test.format, verify they have identical output
SHELL=bash

.PHONY: default
default: test.printf test.format
        # sanity check
	(( $$(stat -c%s test.format) > $$(stat -c%s test.printf) + 2048 ))
	nm test.printf | grep -q printf
	! nm test.format | grep -q printf
	! diff -U0 --label=printf --label=format --suppress-common <(./test.printf) <(./test.format) | grep -v '^@'
	@echo "TEST PASS!"

COPTS=-O3 -Wall -Werror -Wformat=0 -fstack-usage

# 'make BROKEN=1' to induce test failure
ifdef BROKEN
COPTS += -DBROKEN
endif

# these must build sequentially
test.printf: test.c
	cc ${COPTS} -DREFERENCE -o test.printf test.c
	mv test.su $@.su

test.format: test.c format.c test.printf
	cc ${COPTS} -o test.format test.c
	mv test.su $@.su

.PHONY: clean
clean:; rm -f test.format test.printf *.su
