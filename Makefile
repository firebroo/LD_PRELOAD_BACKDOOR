all:
	gcc -fPIC hook.c -shared -o hack.so
	mv hack.so /tmp/hack.so
clean:
	rm -rf /tmp/hack.so
