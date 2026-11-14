/* Ghidra-derived field model; not recovered OEM source and not executable. */
struct scale_descriptor_60 {
    unsigned mode;               /* +0x00 = 9 */
    unsigned short field_04;     /* request +0x14, low 16 */
    unsigned short field_06;     /* request +0x08, low 16 */
    unsigned word_08;            /* request +0x20 */
    unsigned word_0c;            /* request +0x24 */
    unsigned word_10;            /* request +0x28 */
    unsigned reserved_14;
    unsigned word_18;            /* request +0x1c */
    unsigned short width_1c;     /* min(request +4, LCD width) */
    unsigned short height_1e;    /* min(request +8, LCD height) */
    unsigned word_20;            /* request +0x2c */
    short rect_x_24;             /* 0 */
    short rect_y_26;             /* 0 */
    unsigned short rect_w_28;    /* request +4 */
    unsigned short rect_h_2a;    /* request +8 */
    unsigned reserved_2c[4];
};

/* FUN_003497c0 builds the structure on its stack, then calls
 * FUN_002e6958(&descriptor, request_word_30 != 0).
 */
