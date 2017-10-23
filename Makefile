# 64bit compiler tool commands
include config.mk

# utils directory must be build first
all:
	mkdir ./image
	make -C utils
	make -C src

clean:
	make -C utils clean
	make -C src clean
	rm -rf ./image
