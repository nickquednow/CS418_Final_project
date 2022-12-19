DEBUG_FLAG = 

debug: DEBUG_FLAG += -ggdb
debug: all

all: clean index
index: base
	@gcc -c thread.c -lpthread -o out/thread.o ${DEBUG_FLAG}
	@gcc -c index.c -lpthread -o out/index.o ${DEBUG_FLAG}
	@gcc -c sql.c -o out/sql.o -I/usr/include/mariadb -I/usr/include/mariadb/mysql -L/usr/lib/x86_64-linux-gnu/ -lmariadb ${DEBUG_FLAG}
	@gcc out/* -o main.o -lpthread -I/usr/include/mariadb -I/usr/include/mariadb/mysql -L/usr/lib/x86_64-linux-gnu/ -lmariadb ${DEBUG_FLAG}

base:
	@mkdir out

test: all
	bash ../../../tests/test.sh

clean:
	@if [ -d "out" ]; then rm -r out; fi