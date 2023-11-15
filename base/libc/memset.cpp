extern "C" void *memset(void *dst, char v, int n) {
    char *d = (char *)dst;
    for (int i=0;i<n;i++) d[i]=v;
    return dst;
}