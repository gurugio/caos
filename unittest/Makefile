include ../config.mk


CFLAGS=-Wall -Winline -O2


C_SRCS = test_atomic.c

C_EXEC = $(C_SRCS:.c=)


all:$(C_EXEC) EXEC_TEST


$(C_EXEC):$(C_SRCS)
	$(GCC64) $(CFLAGS) -o $@ $<


EXEC_TEST:$(C_EXEC)
	echo "UNITTEST<"$(C_EXEC)">"
	./$<
