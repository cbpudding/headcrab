.PHONY: all
all: headcrab headcrab.so

headcrab: src/main.c injector/src/linux/libinjector.a
	$(CC) -g -Iinjector/include -lelf -o $@ $^

headcrab.so: src/payload.c
	$(CC) -shared -fPIC -llua5.4 -o $@ $^

injector/src/linux/libinjector.a: injector/Makefile
	$(MAKE) -C injector

.PHONY: clean
clean:
	$(RM) headcrab headcrab.so
	$(MAKE) -C injector clean