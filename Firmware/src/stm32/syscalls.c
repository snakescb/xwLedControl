
#include <reent.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "stm32f10x.h"

#undef errno


extern int errno;

int _kill(int pid, int sig)
{
	pid = pid; sig = sig;
	errno = EINVAL;
	return -1;
}

void _exit(int status) {
	while(1) {;}
}

int _getpid(void) {
	return 1;
}


extern char _end;
static char* heap_end = 0;

char* get_heap_end(void) {
	return (char*) heap_end;
}

char* get_stack_top(void) {
	return (char*) __get_MSP();
}

caddr_t _sbrk(int incr) {
	char *prev_heap_end;
	if (heap_end == 0) {
		heap_end = &_end;
	}
	prev_heap_end = heap_end;
	heap_end += incr;
	return (caddr_t) prev_heap_end;
}

int _close(int file){
	file = file;
	return -1;
}

int _fstat(int file, struct stat *st){
	file = file;
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file){
	file = file;
	return 1;
}

int _lseek(int file, int ptr, int dir) {
	file = file;
	ptr = ptr;
	dir = dir;
	return 0;
}

int _read(int file, char *ptr, int len){
	file = file;
	ptr = ptr;
	len = len;
	return 0;
}

int _write(int file, char *ptr, int len){
	file = file;
	return len;
}
