/* 
 * File:   Rs.h
 * Author: Anthony Le Cren & Philippe Simier
 *
 * Created on 14 septembre 2021, 13:18
 */

#ifndef RS_H
#define RS_H

#include <Arduino.h>

#define A0 (rs->nn)


#define CTAG_MIN 0x01
#define CTAG_MAX 0x0B

// Maximum sizes of "data" and "check" parts.

#define FX25_MAX_DATA 239 // i.e. RS(255,239)
#define FX25_MAX_CHECK 64 // e.g. RS(255, 191)
#define FX25_BLOCK_SIZE 255 // Block size always 255 for 8 bit symbols.

#define NTAB 3

/* Reed-Solomon codec control block */
struct rs {
    unsigned int mm; /* Bits per symbol */
    unsigned int nn; /* Symbols per block (= (1<<mm)-1) */
    uint8_t *alpha_to; /* log lookup table */
    uint8_t *index_of; /* Antilog lookup table */
    uint8_t *genpoly; /* Generator polynomial */
    unsigned int nroots; /* Number of generator roots = number of parity symbols */
    uint8_t fcr; /* First consecutive root, index form */
    uint8_t prim; /* Primitive element, index form */
    uint8_t iprim; /* prim-th root of 1, index form */
};

struct correlation_tag_s {
    uint64_t value; // 64 bit value, send LSB first.
    int n_block_radio; // Size of transmitted block, all in bytes.
    int k_data_radio; // Size of transmitted data part.
    int n_block_rs; // Size of RS algorithm block.
    int k_data_rs; // Size of RS algorithm data part.
    int itab; // Index into Tab array.
};

static struct {
    int symsize; // Symbol size, bits (1-8).  Always 8 for this application.
    int genpoly; // Field generator polynomial coefficients.
    int fcs; // First root of RS code generator polynomial, index form.
    int prim; // Primitive element to generate polynomial roots.
    int nroots; // RS code generator polynomial degree (number of roots).
    // Same as number of check bytes added.
    struct rs *rs; // Pointer to RS codec control block.  Filled in at init time.
} Tab[NTAB] = {
    {8, 0x11d, 1, 1, 16, NULL}, // RS(255,239)
    {8, 0x11d, 1, 1, 32, NULL}, // RS(255,223)
    {8, 0x11d, 1, 1, 64, NULL}, // RS(255,191)
};


static const struct correlation_tag_s tags[16] = {
    
    { 0x566ED2717946107ELL, 0, 0, 0, 0, -1},        //  Tag_00  Reserved
    { 0xB74DB7DF8A532F3ELL, 255, 239, 255, 239, 0}, //  Tag_01 RS(255, 239) 16-byte check value, 239 information bytes
    { 0x26FF60A600CC8FDELL, 144, 128, 255, 239, 0}, //  Tag_02 RS(144, 128) - shortened RS(255, 239), 128 info bytes
    { 0xC7DC0508F3D9B09ELL, 80, 64, 255, 239, 0},   //  Tag_03 RS( 80,  64) - shortened RS(255, 239), 64 info bytes
    { 0x8F056EB4369660EELL, 48, 32, 255, 239, 0},   //  Tag_04 RS( 48, 32)  - shortened RS(255, 239), 32 info bytes
    { 0x6E260B1AC5835FAELL, 255, 223, 255, 223, 1}, //  Tag_05 RS(255, 223) 32-byte check value, 223 information bytes
    { 0xFF94DC634F1CFF4ELL, 160, 128, 255, 223, 1}, //  Tag_06 RS(160,128)  - shortened RS(255, 223), 128 info bytes
    { 0x1EB7B9CDBC09C00ELL, 96, 64, 255, 223, 1},   //  Tag_07 RS( 96, 64)  - shortened RS(255, 223), 64 info bytes
    { 0xDBF869BD2DBB1776LL, 64, 32, 255, 223, 1},   //  Tag_08 RS( 64,  32) - shortened RS(255, 223), 32 info bytes
    { 0x3ADB0C13DEAE2836LL, 255, 191, 255, 191, 2}, //  Tag_09 RS(255, 191) 64-byte check value, 191 information bytes
    { 0xAB69DB6A543188D6LL, 192, 128, 255, 191, 2}, //  Tag_0A RS(192, 128) - shortened RS(255, 191), 128 info bytes
    { 0x4A4ABEC4A724B796LL, 128, 64, 255, 191, 2},  //  Tag_0B RS(128,  64) - shortened RS(255, 191), 64 info bytes
    { 0x0293D578626B67E6LL, 0, 0, 0, 0, -1},        //  Tag_0C Undefined
    { 0xE3B0B0D6917E58A6LL, 0, 0, 0, 0, -1},        //  Tag_0D Undefined
    { 0x720267AF1BE1F846LL, 0, 0, 0, 0, -1},        //  Tag_0E Undefined
    { 0x93210201E8F4C706LL, 0, 0, 0, 0, -1}         //  Tag_0F Undefined
};

__attribute__ ((always_inline))
static inline int modnn(struct rs *rs, int x) {
    while (x >= rs->nn) {
        x -= rs->nn;
        x = (x >> rs->mm) + (x & rs->nn);
    }
    return x;
}

#define put_bit(value)  {      \
                        if (olen >= osize) return(-1);   \
                        if (value) out[olen>>3] |= 1 << (olen & 0x7); \
                        olen++;      \
                        }

class Rs {
public:
    Rs();
    Rs(const Rs& orig);
    virtual ~Rs();

    int  generate(uint8_t *fbuf, int flen, uint8_t *dest, uint16_t *destLen, int fx_mode);
    void hex_dump(uint8_t *p, int len);
    int  stuff_it(unsigned char *in, int ilen, unsigned char *out, int osize);

private:
    void      init();
    void      encodeRs(struct rs *rs, uint8_t *data, uint8_t *bb);

    int       pick_mode(int fx_mode, int dlen);
    uint64_t  get_ctag_value(int ctag_num);
    int       get_k_data_radio(int ctag_num);
    int       get_k_data_rs(int ctag_num);
    struct rs *get_rs(int ctag_num);
    struct rs *initRS(unsigned int symsize, unsigned int gfpoly, unsigned fcr, unsigned prim, unsigned int nroots);
    int       get_nroots(int ctag_num);
    

    uint8_t frameRadio[263];
};

#endif /* RS_H */

