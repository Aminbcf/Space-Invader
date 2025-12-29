/* tools/assets_gen.c */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BMPFileHeader;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPInfoHeader;
#pragma pack(pop)

void write_bmp(const char* filename, int w, int h, const char* pattern) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        printf("Error: Could not create %s (Check if directory exists)\n", filename);
        return;
    }

    int padding = (4 - (w * 3) % 4) % 4;
    int row_size = w * 3 + padding;
    int image_size = row_size * h;
    int file_size = 54 + image_size;

    BMPFileHeader fh = {0x4D42, file_size, 0, 0, 54};
    BMPInfoHeader ih = {40, w, h, 1, 24, 0, image_size, 2835, 2835, 0, 0};

    fwrite(&fh, 1, sizeof(fh), f);
    fwrite(&ih, 1, sizeof(ih), f);

    uint8_t pad_bytes[3] = {0, 0, 0};

    // Legend: . = Black, R=Red, G=Green, P=Purple, W=White, C=Cyan, Y=Yellow
    // B = Boss Body, ! = Boss Core
    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            char p = pattern[y * w + x];
            uint8_t b = 0, g = 0, r = 0;
            switch(p) {
                case 'W': r=255; g=255; b=255; break;
                case 'R': r=255; g=0;   b=0;   break;
                case 'G': r=0;   g=255; b=0;   break;
                case 'P': r=128; g=0;   b=128; break;
                case 'C': r=0;   g=255; b=255; break;
                case 'Y': r=255; g=255; b=0;   break;
                case 'O': r=255; g=165; b=0;   break; // Explosion
                case 'B': r=139; g=0;   b=0;   break; // Boss Dark
                case '!': r=255; g=50;  b=50;  break; // Boss Light
                default:  r=0;   g=0;   b=0;   break;
            }
            uint8_t pixel[3] = {b, g, r};
            fwrite(pixel, 1, 3, f);
        }
        fwrite(pad_bytes, 1, padding, f);
    }
    fclose(f);
    printf("Generated: %s\n", filename);
}

int main() {
    // Player (15x10)
    write_bmp("src/pictures/player.bmp", 15, 10,
        ".......C......."
        "......CCC......"
        "......CCC......"
        ".CCCCCCCCCCCCC."
        "CCCCCCCCCCCCCCC"
        "CCCCCCCCCCCCCCC"
        "CCCCCCCCCCCCCCC"
        "CCCCCCCCCCCCCCC"
        "CCCCCCCCCCCCCCC"
        "CCCCCCCCCCCCCCC"
    );

    // Invader 1 (Squid) - Purple
    write_bmp("src/pictures/invader1_1.bmp", 11, 8,
        "...PP.PP..."
        "..PPPPPPP.."
        ".PP.PPP.PP."
        "PPPPPPPPPPP"
        "P.PPPPPPP.P"
        "P.P.....P.P"
        "P.........P"
        ".P.......P."
    );
    write_bmp("src/pictures/invader1_2.bmp", 11, 8,
        "...PP.PP..."
        "..PPPPPPP.."
        ".PP.PPP.PP."
        "PPPPPPPPPPP"
        "P.PPPPPPP.P"
        "P.P.....P.P"
        ".P.......P."
        "P.........P"
    );

    // Invader 2 (Crab) - Green
    write_bmp("src/pictures/invader2_1.bmp", 11, 8,
        "..G.....G.."
        "...G...G..."
        "..GGGGGGG.."
        ".GG.GGG.GG."
        "GGGGGGGGGGG"
        "G.GGGGGGG.G"
        "G.G.....G.G"
        "...GG.GG..."
    );
    write_bmp("src/pictures/invader2_2.bmp", 11, 8,
        "..G.....G.."
        ".G.......G."
        ".GGGGGGGGG."
        "GG.GGG.GG.G"
        "GGGGGGGGGGG"
        ".GGGGGGGGG."
        "..G.....G.."
        ".G.......G."
    );

    // Invader 3 (Octopus) - Red
    write_bmp("src/pictures/invader3_1.bmp", 11, 8,
        "....RRRR..."
        ".RRRRRRRRRR"
        "RRR.RRR.RRR"
        "RRRRRRRRRRR"
        "..RRRRRRR.."
        ".RR.R.R.RR."
        "RR.......RR"
        "..........."
    );
    write_bmp("src/pictures/invader3_2.bmp", 11, 8,
        "....RRRR..."
        ".RRRRRRRRRR"
        "RRR.RRR.RRR"
        "RRRRRRRRRRR"
        "..RRRRRRR.."
        "..RR...RR.."
        "..RR...RR.."
        "..........."
    );

    // Boss
    write_bmp("src/pictures/boss.bmp", 24, 12,
        "........BBBBBBBB........"
        "......BBBBBBBBBBBB......"
        "....BBBBBBBBBBBBBBBB...."
        "...BBBBBBBBBBBBBBBBBB..."
        "..BBBB!!!!!!!!!!!!BBBB.."
        ".BBBBB!!!!!!!!!!!!BBBBB."
        ".BBBBBBBBBBBBBBBBBBBBBB."
        ".BBBB.BBBBBBBBBBBB.BBBB."
        ".BBB...BBBBBBBBBB...BBB."
        ".......BB.BB.BB........."
        "......BB..BB..BB........"
        ".....BB...BB...BB......."
    );

    // Explosion
    write_bmp("src/pictures/explosion.bmp", 12, 8,
        "...O....O..."
        ".O..O..O..O."
        "..O..OO..O.."
        "OOOOOOOOOOOO"
        "OOOOOOOOOOOO"
        "..O..OO..O.."
        ".O..O..O..O."
        "...O....O..."
    );

    // Bullets
    write_bmp("src/pictures/bullet_player.bmp", 3, 6, ".W..W..W..W..W..W.");
    write_bmp("src/pictures/bullet_enemy.bmp", 3, 6, ".Y.Y.Y.Y.Y.Y.Y.Y.Y");

    // Saucer
    write_bmp("src/pictures/saucer.bmp", 16, 7,
        ".....RRRRRR....."
        "...RRRRRRRRRR..."
        "..RRRRRRRRRRRR.."
        ".RR.RR.RR.RR.RR."
        "RRRRRRRRRRRRRRRR"
        "..RRR......RRR.."
        "....R......R...."
    );

    printf("Assets generation complete in src/pictures/\n");
    return 0;
}