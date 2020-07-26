#include "lighthouse2tools/include/lh2polys.h"

#ifdef CPU_MIMXRT1062DVL6A
#include "fsl_debug_console.h"
#define DEBUG(...) PRINTF(__VA_ARGS__)
#else
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifndef DEBUG
#define DEBUG(...)
#endif

lfsr_poly_t poly_pairs[32] =
        {
             // x^17 + x^13 + x^12 + x^10 + x^7 + x^4 + x^2 + x^1 + 1
             //   0123456789ABCDEF01
             // 0b11101001001011000
            0x0001D258,
            // x^17 + x^14 +  x^7 +  x^6 + x^5 + x^4 + x^3 + x^2 + 1
            //  0123456789ABCDEF01
            //0b10111111000000100
            0x00017E04,
            0x0001FF6B, 0x00013F67,
            0x0001B9EE, 0x000198D1,
            // x^17 + x^16 + x^14 + x^12 + x^10 + x^7 + x^5 + x^1 + 1
            // 0b11000101001010101
            // 0b11000101001010101
            0x000178C7, 0x00018A55,
            0x00015777, 0x0001D911,
            0x00015769, 0x0001991F,
            0x00012BD0, 0x0001CF73,
            0x0001365D, 0x000197F5,
            0x000194A0, 0x0001B279,
            0x00013A34, 0x0001AE41,
            0x000180D4, 0x00017891,
            0x00012E64, 0x00017C72,
            0x00019C6D, 0x00013F32,
            0x0001AE14, 0x00014E76,
            0x00013C97, 0x000130CB,
            0x00013750, 0x0001CB8D,
};

lfsr_lookup_t* poly_pair_lookups[32] = {};
static void init_lookups() {
    if(poly_pair_lookups[0] == 0) {
        for(int i = 0;i < 32;i++)
            poly_pair_lookups[i] = lfsr_lookup_ctor(poly_pairs[i]);
    }
}

uint32_t lh2_lookup_query(uint8_t channel, uint32_t q) {
    init_lookups();
    return lfsr_lookup_query(poly_pair_lookups[channel], q);
}

int find_best_poly(uint32_t sample, uint32_t mask, uint32_t start, uint32_t end) {
    //init_lookups();

    int best = 0;
    int best_error = 32;

    for(int i = start;i < end;i++) {
        uint32_t lookup_res = 0;

        /*
        for(uint8_t j = 0;j < 16 && lookup_res == 0;j++) {
            if( ((mask >> j) & 0x1FFFF) == 0x1FFFF) {
                lookup_res = lfsr_lookup_query(poly_pair_lookups[i], sample >> j) - j;
                DEBUG("Poly %2d -- %6d\t", i, lookup_res);
            }
        }
        */

        uint32_t error_bits = lfsr_error(poly_pairs[i], sample, mask);
        uint32_t error = popcnt(error_bits);

        DEBUG( "%2d error %d\r\n", i, error);

        if(error < best_error) {
            best_error = error;
            best = i;
        }
        //printf("Poly %d has %d errors %d\r\n", i, error, lookup_res);
    }

    DEBUG( "Best error was %d at %d\r\n", best, best_error);

    return best;
}
