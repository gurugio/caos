
# 64bit compiler tool commands
include config.mk

REL_NAME=caos64_v0.1_`date +%F`.tgz

# utils directory must be build first
all:
	make -C utils
	make -C src
	
rel:clean
	rm -f *.tgz
	cd ..;tar zcf $(REL_NAME) caos64;mv $(REL_NAME) caos64/

clean:
	make -C utils clean
	make -C src clean
	rm -f image/*
