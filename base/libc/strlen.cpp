#include <stdint.h>
void dbgputchar(char c);

size_t strlen(const char *txt) {
    size_t len = 0;
    for (len=0;txt[len];len++);
	return len;
}