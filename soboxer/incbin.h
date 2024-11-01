#define INCBIN_SECTION ".rodata"

#define STR2(x) #x
#define STR(x) STR2(x)

#define INCBIN(name, file) \
    __asm__(".section " INCBIN_SECTION "\n" \
            ".global __lfidata_" STR(name) "_start\n" \
            ".balign 16\n" \
            "__lfidata_" STR(name) "_start:\n" \
            ".incbin \"" file "\"\n" \
            \
            ".global __lfidata_" STR(name) "_end\n" \
            ".balign 1\n" \
            "__lfidata_" STR(name) "_end:\n" \
            ".byte 0\n" \
    ); \
    extern __attribute__((aligned(16))) uint8_t __lfidata_ ## name ## _start[]; \
    extern                              uint8_t __lfidata_ ## name ## _end[]
