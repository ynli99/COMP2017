CC=gcc
CFLAGS=-fsanitize=address -Wvla -Wall -Werror -g -std=gnu11 -lasan

timetable: timetable.c
	$(CC) $(CFLAGS) $^ -o $@

test: export ASAN_OPTIONS=verify_asan_link_order=0
test: timetable
	bash test.sh

clean:
	rm timetable
