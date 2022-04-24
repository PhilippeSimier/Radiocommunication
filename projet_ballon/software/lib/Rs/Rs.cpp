/* 
 * File:   Rs.cpp
 * Author: Anthony Le Cren & Philippe Simier
 * 
 * Created on 14 septembre 2021, 13:18
 */


#include "Rs.h"

Rs::Rs() {
    init();
}

Rs::Rs(const Rs& orig) {
}

Rs::~Rs() {
}

/**
 *  __builtin_popcountll Cette fonction renvoie le nombre de bits définis dans un entier long long
 *                       (le nombre de uns dans la représentation binaire de l'entier). 
 */
void Rs::init() {

    for (int i = 0; i < NTAB; i++) {
        Tab[i].rs = initRS(Tab[i].symsize, Tab[i].genpoly, Tab[i].fcs, Tab[i].prim, Tab[i].nroots);
        if (Tab[i].rs == NULL) {
            Serial.printf("FX.25 internal error: init_rs_char failed!\n");
            exit(EXIT_FAILURE);
        }
    }

    // Vérifier l'intégrité des tableaux et des hypothèses. 
    // Cela effectue également une vérification rapide de la fonction __builtin_popcountll. 
    // La fonction assert affiche un message et stop l'éxecution du programme s'il y a une erreur

    for (int j = 0; j < 16; j++) {
        for (int k = 0; k < 16; k++) {
            if (j == k) {
                assert(__builtin_popcountll(tags[j].value ^ tags[k].value) == 0);
            } else {
                assert(__builtin_popcountll(tags[j].value ^ tags[k].value) == 32);
            }
        }
    }

    for (int j = CTAG_MIN; j <= CTAG_MAX; j++) {
        assert(tags[j].n_block_radio - tags[j].k_data_radio == Tab[tags[j].itab].nroots);
        assert(tags[j].n_block_rs - tags[j].k_data_rs == Tab[tags[j].itab].nroots);
        assert(tags[j].n_block_rs == FX25_BLOCK_SIZE);
    }

    assert(pick_mode(100 + 1, 239) == 1);
    assert(pick_mode(100 + 1, 240) == -1);

    assert(pick_mode(100 + 5, 223) == 5);
    assert(pick_mode(100 + 5, 224) == -1);

    assert(pick_mode(100 + 9, 191) == 9);
    assert(pick_mode(100 + 9, 192) == -1);

    assert(pick_mode(16, 32) == 4);
    assert(pick_mode(16, 64) == 3);
    assert(pick_mode(16, 128) == 2);
    assert(pick_mode(16, 239) == 1);
    assert(pick_mode(16, 240) == -1);

    assert(pick_mode(32, 32) == 8);
    assert(pick_mode(32, 64) == 7);
    assert(pick_mode(32, 128) == 6);
    assert(pick_mode(32, 223) == 5);
    assert(pick_mode(32, 234) == -1);

    assert(pick_mode(64, 64) == 11);
    assert(pick_mode(64, 128) == 10);
    assert(pick_mode(64, 191) == 9);
    assert(pick_mode(64, 192) == -1);

    assert(pick_mode(1, 32) == 4);
    assert(pick_mode(1, 33) == 3);
    assert(pick_mode(1, 64) == 3);
    assert(pick_mode(1, 65) == 6);
    assert(pick_mode(1, 128) == 6);
    assert(pick_mode(1, 191) == 9);
    assert(pick_mode(1, 223) == 5);
    assert(pick_mode(1, 239) == 1);
    assert(pick_mode(1, 240) == -1);

} // init

// Get properties of specified CTAG number.

struct rs *Rs::get_rs(int ctag_num) {
    assert(ctag_num >= CTAG_MIN && ctag_num <= CTAG_MAX);
    assert(tags[ctag_num].itab >= 0 && tags[ctag_num].itab < NTAB);
    assert(Tab[tags[ctag_num].itab].rs != NULL);
    return (Tab[tags[ctag_num].itab].rs);
}

uint64_t Rs::get_ctag_value(int ctag_num) {
    assert(ctag_num >= CTAG_MIN && ctag_num <= CTAG_MAX);
    return (tags[ctag_num].value);
}

int Rs::get_k_data_radio(int ctag_num) {
    assert(ctag_num >= CTAG_MIN && ctag_num <= CTAG_MAX);
    return (tags[ctag_num].k_data_radio);
}

int Rs::get_k_data_rs(int ctag_num) {
    assert(ctag_num >= CTAG_MIN && ctag_num <= CTAG_MAX);
    return (tags[ctag_num].k_data_rs);
}

int Rs::get_nroots(int ctag_num) {
    assert(ctag_num >= CTAG_MIN && ctag_num <= CTAG_MAX);
    return (Tab[tags[ctag_num].itab].nroots);
}

/*-------------------------------------------------------------
 *
 * Name:	fx25_pick_mode
 *
 * Purpose:	Choisir le format de transmission approprié en fonction des préférences de l'utilisateur 
 *              et de la taille de la partie de données requise. 
 *		
 *
 * Inputs:	fx_mode	- 0 = none.
 *			1 = pick a tag automatically.
 *			16, 32, 64 = use this many check bytes.
 *			100 + n = use tag n.
 *
 *			0 et 1 seraient les choix les plus courants. 
 *			Lrs autres sont principalement destinés à des tests.
 *
 *		dlen - 	Taille requise pour la partie "données" transmise, en octets. 
 *                      Cela inclut la trame AX.25 avec les bits de stuffing et un motif de préambule à chaque extrémité. 
 *
 * Returns:	Numéro du tag de corrélation dans la plage de CTAG_MIN à CTAG_MAX.
 *              -1 est renvoyé en cas d'échec. Le programme appelant doit revenir à l'utilisation de l'AX.25 sans FEC. 
 *
 *--------------------------------------------------------------*/

int Rs::pick_mode(int fx_mode, int dlen) {
    if (fx_mode <= 0) return (-1);

    // Spécifiez une balise spécifique en ajoutant 100 au nombre. 
    // Échoue si les données ne rentrent pas. 

    if (fx_mode - 100 >= CTAG_MIN && fx_mode - 100 <= CTAG_MAX) {
        if (dlen <= get_k_data_radio(fx_mode - 100)) {
            return (fx_mode - 100);
        } else {
            return (-1); // En supposant que l'appelant imprime un message d'échec.
        }
    }        // Specify number of check bytes.
        // Pick the shortest one that can handle the required data length.

    else if (fx_mode == 16 || fx_mode == 32 || fx_mode == 64) {
        for (int k = CTAG_MAX; k >= CTAG_MIN; k--) {
            if (fx_mode == get_nroots(k) && dlen <= get_k_data_radio(k)) {
                return (k);
            }
        }
        return (-1);
    }

    // Pour tout autre numéro, ou si la préférence n'était pas possible ?? essaye de trouver quelque chose de raisonnable. 
    // Pour des trames plus courtes, utilise des champs de contrôles plus petits. 
    // Pour les trames plus longues, où une erreur est plus probable, utilisez plus d'octets de contrôle. 
    // Lorsque les données deviennent encore plus volumineuses, les octets de contrôle doivent être réduits 
    // pour s'adapter à la taille du bloc. Lorsque tout le reste échoue, revenez à AX.25 sans FEC.
    // Une partie de cela est issue de l'observation du comportement du Soundmodem UZ7HO. 
    //
    //	Tag 	Data 	Check 	Max Num
    //	Number	Bytes	Bytes	Repaired
    //	------	-----	-----	-----
    //	0x04	32	16	8
    //	0x03	64	16	8
    //	0x06	128	32	16
    //	0x09	191	64	32
    //	0x05	223	32	16
    //	0x01	239	16	8
    //	none	larger		
    //
    // Le PRUG FX.25 TNC dispose de modes supplémentaires qui gèrent des trames plus grandes 
    // en utilisant plusieurs blocs RS. Il s'agit d'une possibilité future, 
    // mais elle doit être coordonnée avec d'autres développeurs FX.25 afin que nous maintenions la compatibilité. 

    static const int prefer[6] = {0x04, 0x03, 0x06, 0x09, 0x05, 0x01};
    for (int k = 0; k < 6; k++) {
        int m = prefer[k];
        if (dlen <= get_k_data_radio(m)) {
            return (m);
        }
    }
    return (-1);

    // TODO: revisit error messages, produced by caller, when this returns -1.

}

/*   Initialise un codeur décodeur (codec) Reed-Solomon 
 *   symsize = taille du symbole, bits (1-8) - toujours 8 pour cette application. 
 *   gfpoly = Coefficients polynomiaux du générateur de champ 
 *   fcr = première racine du polynôme générateur de code Reed Solomon,, index form
 *   prim = élément primitif pour générer des racines polynomiales 
 *   nroots = Degré polynomial du générateur de code RS (nombre de racines) 
 */

struct rs *Rs::initRS(unsigned int symsize, unsigned int gfpoly, unsigned fcr, unsigned prim,  unsigned int nroots) {
    struct rs *rs;
    int i, j, sr, root, iprim;

    if (symsize > 8 * sizeof (uint8_t))
        return NULL; /* Besoin d'une version avec des entiers plutôt que des caractères  */

    if (fcr >= (1 << symsize))
        return NULL;
    if (prim == 0 || prim >= (1 << symsize))
        return NULL;
    if (nroots >= (1 << symsize))
        return NULL; /* Ne peut pas avoir plus de racines que de valeurs de symboles !  */

    rs = (struct rs *) calloc(1, sizeof (struct rs));
    rs->mm = symsize;
    rs->nn = (1 << symsize) - 1;

    rs->alpha_to = (uint8_t *) malloc(sizeof (uint8_t)*(rs->nn + 1));
    if (rs->alpha_to == NULL) {
        free(rs);
        return NULL;
    }
    rs->index_of = (uint8_t *) malloc(sizeof (uint8_t)*(rs->nn + 1));
    if (rs->index_of == NULL) {
        free(rs->alpha_to);
        free(rs);
        return NULL;
    }

    /* Génére des tables de recherche de champs de Galois  */
    rs->index_of[0] = A0; /* log(zero) = -inf */
    rs->alpha_to[A0] = 0; /* alpha**-inf = 0 */
    sr = 1;
    for (i = 0; i < rs->nn; i++) {
        rs->index_of[sr] = i;
        rs->alpha_to[i] = sr;
        sr <<= 1;
        if (sr & (1 << symsize))
            sr ^= gfpoly;
        sr &= rs->nn;
    }
    if (sr != 1) {
        /* le polynôme générateur de champ n'est pas primitif !  */
        free(rs->alpha_to);
        free(rs->index_of);
        free(rs);
        return NULL;
    }

    /* Form RS code generator polynomial from its roots */
    rs->genpoly = (uint8_t *) malloc(sizeof (uint8_t)*(nroots + 1));
    if (rs->genpoly == NULL) {
        free(rs->alpha_to);
        free(rs->index_of);
        free(rs);
        return NULL;
    }
    rs->fcr = fcr;
    rs->prim = prim;
    rs->nroots = nroots;

    /* Find prim-th root of 1, used in decoding */
    for (iprim = 1; (iprim % prim) != 0; iprim += rs->nn)
        ;
    rs->iprim = iprim / prim;

    rs->genpoly[0] = 1;
    for (i = 0, root = fcr * prim; i < nroots; i++, root += prim) {
        rs->genpoly[i + 1] = 1;

        /* Multiply rs->genpoly[] by  @**(root + x) */
        for (j = i; j > 0; j--) {
            if (rs->genpoly[j] != 0)
                rs->genpoly[j] = rs->genpoly[j - 1] ^ rs->alpha_to[modnn(rs, rs->index_of[rs->genpoly[j]] + root)];
            else
                rs->genpoly[j] = rs->genpoly[j - 1];
        }
        /* rs->genpoly[0] can never be zero */
        rs->genpoly[0] = rs->alpha_to[modnn(rs, rs->index_of[rs->genpoly[0]] + root)];
    }
    /* convert rs->genpoly[] to index form for quicker encoding */
    for (i = 0; i <= nroots; i++) {
        rs->genpoly[i] = rs->index_of[rs->genpoly[i]];
    }
    return rs;
}

void Rs::hex_dump(uint8_t *p, int len) {
    int n, i, offset;

    offset = 0;
    while (len > 0) {
        n = len < 16 ? len : 16;
        Serial.printf("  %03x: ", offset);
        for (i = 0; i < n; i++) {
            Serial.printf(" %02x", p[i]);
        }
        for (i = n; i < 16; i++) {
            Serial.printf("   ");
        }
        Serial.printf("  ");
        for (i = 0; i < n; i++) {
            Serial.printf("%c", isprint(p[i]) ? p[i] : '.');
        }
        Serial.printf("\n\r");
        p += 16;
        offset += 16;
        len -= 16;
    }
}

void Rs::encodeRs(struct rs * rs, uint8_t * data, uint8_t * bb) {

    int i, j;
    uint8_t feedback;

    memset(bb, 0, rs->nroots * sizeof (uint8_t)); // clear out the FEC data area

    for (i = 0; i < rs->nn - rs->nroots; i++) {
        feedback = rs->index_of[data[i] ^ bb[0]];
        if (feedback != A0) { /* feedback term is non-zero */
            for (j = 1; j < rs->nroots; j++)
                bb[j] ^= rs->alpha_to[ modnn(rs, feedback + rs->genpoly[rs->nroots - j]) ];
        }
        /* Shift */
        memmove(&bb[0], &bb[1], sizeof (uint8_t)*(rs->nroots - 1));
        if (feedback != A0)
            bb[rs->nroots - 1] = rs->alpha_to[ modnn(rs, feedback + rs->genpoly[0]) ];
        else
            bb[rs->nroots - 1] = 0;
    }
}


int Rs::generate(uint8_t *fbuf, int flen, uint8_t *radio, uint16_t *radioLen, int fx_mode) {

    // Add bit-stuffing.
    const unsigned char fence = 0xaa;
    unsigned char data[FX25_MAX_DATA + 1];
    data[FX25_MAX_DATA] = fence;

    int dlen = stuff_it(fbuf, flen, data, FX25_MAX_DATA);

    assert(data[FX25_MAX_DATA] == fence);
    if (dlen < 0) {
        Serial.printf("FX.25[%d]: Frame length of %d + overhead is too large to encode.\n", flen);
        return (-1);
    }

    // Pick suitable correlation tag depending on
    // user's preference(fx_mode), for number of check bytes,
    // and the data size.

    int ctag_num = pick_mode(fx_mode, dlen);

    if (ctag_num < CTAG_MIN || ctag_num > CTAG_MAX) {
        Serial.printf("FX.25[%d]: Could not find suitable format for requested %d and data length %d.\n", fx_mode, dlen);
        return (-1);
    }

    uint64_t ctag_value = get_ctag_value(ctag_num);

    // Zero out part of data which won't be transmitted.
    // It should all be filled by extra HDLC "flag" patterns.

    int k_data_radio = get_k_data_radio(ctag_num);
    int k_data_rs    = get_k_data_rs(ctag_num);
    int shorten_by = FX25_MAX_DATA - k_data_radio;
    if (shorten_by > 0) {
        memset(data + k_data_radio, 0, shorten_by);
    }

    // Compute the check bytes.

    unsigned char check[FX25_MAX_CHECK + 1];
    check[FX25_MAX_CHECK] = fence;
    struct rs *rs = get_rs(ctag_num);

    assert(k_data_rs + rs->nroots == rs->nn);

    encodeRs(rs, data, check);
    assert(check[FX25_MAX_CHECK] == fence);

    int k;
    for (k = 0; k < 8; k++) {
        radio[k] = (ctag_value >> (k * 8)) & 0xff; // Should be portable to big endian too.
    }
    memcpy(radio + 8, data, k_data_radio);
    memcpy(radio + 8 + k_data_radio, check, rs->nroots);

    *radioLen = 8 + k_data_radio + rs->nroots;


    return 1;
}

/**
 * 
 * @param in    le buffer d'entrée 
 * @param ilen  la taille du buffer d'entrée 
 * @param out   le buffer de sortie
 * @param osize la taille du buffer de sortie
 * @return      le code de retour ? 
 */
int Rs::stuff_it(unsigned char *in, int ilen, unsigned char *out, int osize) {
    const unsigned char flag = 0x7e;
    int ret = -1;
    memset(out, 0, osize);
    out[0] = flag;
    int olen = 8; // Number of bits in output.
    osize *= 8; // Now in bits rather than bytes.
    int ones = 0;

    for (int i = 0; i < ilen; i++) {
        for (unsigned char imask = 1; imask != 0; imask <<= 1) {
            int v = in[i] & imask;
            put_bit(v);
            if (v) {
                ones++;
                if (ones == 5) {
                    put_bit(0);
                    ones = 0;
                }
            } else {
                ones = 0;
            }
        }
    }
    for (unsigned char imask = 1; imask != 0; imask <<= 1) {
        put_bit(flag & imask);
    }
    ret = (olen + 7) / 8; // Includes any partial byte.

    unsigned char imask = 1;
    while (olen < osize) {
        put_bit(flag & imask);
        imask = (imask << 1) | (imask >> 7); // Rotate.
    }

    return (ret);

} // end stuff_it

