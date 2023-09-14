void dbgputchar(char c);

int strlen(const char *txt) {
    int len = 0;
	while (*txt)
		len++;
        txt++;
	return len;
}