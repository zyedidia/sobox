box.out: main.c
	gcc $< -O2 -Llibadd -ladd.lfi.box -o $@

native.out: main.c
	gcc $< -O2 -Llibadd -ladd.native -o $@

run-box: box.out
	LD_LIBRARY_PATH=./libadd ./$<

run-native: native.out
	LD_LIBRARY_PATH=./libadd ./$<

.PHONY: run-box run-native
