LFICC=/opt/aarch64-lfi-clang/bin/clang
LFISYSROOT=/opt/aarch64-lfi-clang/sysroot

box.out: main.c libadd/libadd.lfi.box.so
	gcc $< -O2 -Llibadd -ladd.lfi.box -o $@ -DSOBOX -I../embed

native.out: main.c libadd/libadd.native.so
	gcc $< -O2 -Llibadd -ladd.native -o $@

run-box: box.out
	LD_LIBRARY_PATH=./libadd ./$<

run-native: native.out
	LD_LIBRARY_PATH=./libadd ./$<

libadd/libadd.lfi.so: libadd/add.c
	$(LFICC) $< -O2 -shared -o $@

libadd/libadd.native.so: libadd/add.c
	gcc $< -O2 -shared -o $@

libadd/libadd.lfi.box.so: libadd/libadd.lfi.so
	sobox -map ld-musl-aarch64.so.1=$(LFISYSROOT)/lib/ld-musl-aarch64.so.1 \
		-map /lib/aarch64-linux-musl/libadd.lfi.so=$< \
		-o $@ $<

.PHONY: run-box run-native
