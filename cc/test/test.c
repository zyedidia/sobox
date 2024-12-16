struct Hello {
    int a;
    float b;
    char c;
};

int foo(float x1, struct Hello x2, int x3, int x4, int x5, int x6, struct Hello x7) {
    return x1+x2.a+x3+x4+x5+x6+x7.c;
}
