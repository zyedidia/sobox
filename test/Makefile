box.out: main.c libadd/libadd.lfi.box.so
	gcc $< -O2 -Llibadd -ladd.lfi.box -o $@ -DSOBOX -I../embed

native.out: main.c libadd/libadd.native.so
	gcc $< -O2 -Llibadd -ladd.native -o $@

run-box: box.out
	LD_LIBRARY_PATH=./libadd ./$<

run-native: native.out
	LD_LIBRARY_PATH=./libadd ./$<

libadd/libadd.lfi.so: libadd/add.c
	x86_64-lfi-linux-musl-clang $< -O2 -shared -o $@

libadd/libadd.native.so: libadd/add.c
	gcc $< -O2 -shared -o $@ -fPIC -ftls-model=initial-exec

libadd/libadd.lfi.box.so: libadd/libadd.lfi.so
	sobox -map /lib/ld-musl-x86_64.so.1=/opt/lfi-amd64-tp/x86_64-lfi-linux-musl/lib/ld-musl-x86_64.so.1 \
		-map /lib/libadd.lfi.so=$< \
		-o $@ $<

.PHONY: run-box run-native
