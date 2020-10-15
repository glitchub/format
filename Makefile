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

COPTS=-O2 -Wall -Werror -Wformat=0 -fstack-usage

# 'make BROKEN=1' to induce test failure
ifdef BROKEN
COPTS += -DBROKEN
endif

# these must build sequentially
test.printf: test.c test.format
	cc ${COPTS} -DREFERENCE -o $@ $<
	mv test.su $@.su
	cat $@.su

# note format.c is #included by test.c
test.format: test.c format.c
	cc ${COPTS} -o $@ $<
	mv test.su $@.su
	cat $@.su

.PHONY: clean
clean:; rm -f test.format test.printf *.su
