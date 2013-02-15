
CFLAGS=-Wall -O2


TARGETS = gen_section test_utils 



all: $(TARGETS)

gen_section: gen_section.c
	gcc $(CFLAGS) -o $@ $<

test_utils: test_utils.c
	gcc $(CFLAGS) -o $@ $<



clean:
	rm -f *.o $(TARGETS)
