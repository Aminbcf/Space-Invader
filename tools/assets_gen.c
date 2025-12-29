/* tools/assets_gen.c - Enhanced Version with Beautiful Detailed Sprites */
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

    // Enhanced Color Palette
    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            char p = pattern[y * w + x];
            uint8_t b = 0, g = 0, r = 0;
            switch(p) {
                // Base colors
                case '.': r=0;   g=0;   b=0;   break; // Black/Transparent
                case 'W': r=255; g=255; b=255; break; // White
                case 'K': r=50;  g=50;  b=50;  break; // Dark Gray
                case 'k': r=100; g=100; b=100; break; // Light Gray
                
                // Cyan (Player ship - gradient)
                case 'C': r=0;   g=255; b=255; break; // Bright Cyan
                case 'c': r=0;   g=200; b=200; break; // Medium Cyan
                case 'd': r=0;   g=150; b=150; break; // Dark Cyan
                case 'e': r=0;   g=100; b=100; break; // Darker Cyan
                
                // Purple (Squid - gradient)
                case 'P': r=200; g=50;  b=255; break; // Bright Purple
                case 'p': r=150; g=30;  b=200; break; // Medium Purple
                case 'q': r=100; g=20;  b=150; break; // Dark Purple
                
                // Green (Crab - gradient)
                case 'G': r=50;  g=255; b=50;  break; // Bright Green
                case 'g': r=30;  g=200; b=30;  break; // Medium Green
                case 'h': r=20;  g=150; b=20;  break; // Dark Green
                
                // Red (Octopus - gradient)
                case 'R': r=255; g=50;  b=50;  break; // Bright Red
                case 'r': r=200; g=30;  b=30;  break; // Medium Red
                case 's': r=150; g=20;  b=20;  break; // Dark Red
                
                // Yellow/Orange (Explosion/Saucer)
                case 'Y': r=255; g=255; b=0;   break; // Yellow
                case 'y': r=255; g=200; b=0;   break; // Golden
                case 'O': r=255; g=150; b=0;   break; // Orange
                case 'o': r=200; g=100; b=0;   break; // Dark Orange
                
                // Boss colors
                case 'B': r=180; g=0;   b=0;   break; // Boss Dark Red
                case 'b': r=120; g=0;   b=0;   break; // Boss Darker Red
                case '!': r=255; g=80;  b=80;  break; // Boss Core Bright
                case '@': r=255; g=120; b=120; break; // Boss Core Light
                
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
    // Enhanced Player Ship (20x14) - Sleek fighter design with gradient
    write_bmp("pictures/player.bmp", 20, 14,
        ".........CC........."
        "........CCCC........"
        ".......CCCCCC......."
        "......CCCCCCcc......"
        ".....CCCCCCcccc....."
        "....CCCCCCccccdd...."
        "...CCCCCCccccdddd..."
        "CCCCCCCCCCccccddddCC"
        "CCCCCCCCCCccccccccCC"
        "CCCCCCCCccccccccccCC"
        ".CCCCCCccccccccccC.."
        "..CCCCccccccccddC..."
        "...CCccccccdddd....."
        "....CccccdddC......."
    );

    // Enhanced Invader 1 - Squid (16x12) - Frame 1
    write_bmp("pictures/invader1_1.bmp", 16, 12,
        ".....PPPP......."
        "....PPPPPPp....."
        "...PPPPPPPPpp..."
        "..PPppPPPPppPP.."
        ".PPppppPPppppPP."
        "PPPPPPPPPPPPPPPq"
        "PPPPPPPPPPPPPPPq"
        "PPqq..PPPP..qqPP"
        "Pqq..........qqP"
        ".qq..........qq."
        "..q..........q.."
        "...q........q..."
    );
    
    // Enhanced Invader 1 - Squid Frame 2
    write_bmp("pictures/invader1_2.bmp", 16, 12,
        ".....PPPP......."
        "....PPPPPPp....."
        "...PPPPPPPPpp..."
        "..PPppPPPPppPP.."
        ".PPppppPPppppPP."
        "PPPPPPPPPPPPPPPq"
        "PPPPPPPPPPPPPPPq"
        "PPqq..PPPP..qqPP"
        "Pqq..........qqP"
        "q..............q"
        "q..............q"
        ".q............q."
    );

    // Enhanced Invader 2 - Crab (16x12) - Frame 1
    write_bmp("pictures/invader2_1.bmp", 16, 12,
        "...GG....GG....."
        "....GG..GG......"
        "...GGGGGGGGg...."
        "..GGggGGGGggGG.."
        ".GGggggGGggggGGh"
        "GGGGGGGGGGGGGGGh"
        "GhGGGGGGGGGGGGhG"
        "GhGhGGGGGGGGhGhG"
        "GhhG........Ghhh"
        ".hh..........hh."
        "hh............hh"
        "h..............h"
    );
    
    // Enhanced Invader 2 - Crab Frame 2
    write_bmp("pictures/invader2_2.bmp", 16, 12,
        "...GG....GG....."
        "....GG..GG......"
        "...GGGGGGGGg...."
        "..GGggGGGGggGG.."
        ".GGggggGGggggGGh"
        "GGGGGGGGGGGGGGGh"
        "GhGGGGGGGGGGGGhG"
        "GhGhGGGGGGGGhGhG"
        "GhhG........Ghhh"
        "h..............h"
        "hh............hh"
        ".h............h."
    );

    // Enhanced Invader 3 - Octopus (16x12) - Frame 1
    write_bmp("pictures/invader3_1.bmp", 16, 12,
        ".....RRRRRR....."
        "....RRRRRRRRr..."
        "...RRrrRRRRrrRR."
        "..RRrrrrRRrrrrRR"
        ".RRRRRRRRRRRRRRRs"
        "RRRRRRRRRRRRRRRRs"
        "RsRRRRRRRRRRRRsR"
        "RssRR......RRssR"
        "Rsss..RR..RR.sss"
        ".ss..RRRRRR..ss."
        ".s.RR......RR.s."
        "...R........R..."
    );
    
    // Enhanced Invader 3 - Octopus Frame 2
    write_bmp("pictures/invader3_2.bmp", 16, 12,
        ".....RRRRRR....."
        "....RRRRRRRRr..."
        "...RRrrRRRRrrRR."
        "..RRrrrrRRrrrrRR"
        ".RRRRRRRRRRRRRRRs"
        "RRRRRRRRRRRRRRRRs"
        "RsRRRRRRRRRRRRsR"
        "RssRR......RRssR"
        "Rsss..RR..RR.sss"
        "R..RR......RR..s"
        "R.R..........R.s"
        "..R..........R.."
    );

    // Enhanced Boss (32x16) - Massive detailed design
    write_bmp("pictures/boss.bmp", 32, 16,
        "..........BBBBBBBBBBBB.........."
        ".........BBBBBBBBBBBBBBb........"
        "........BBBBBBBBBBBBBBBBBB......"
        ".......BBBBBBBBBBBBBBBBBBBB....."
        "......BBBBb!!!!!!!!!!!!!bBBBB..."
        ".....BBBBbb!!!@@@@@@@@!!!bbBBBB."
        "....BBBBbbb!!!@@@@@@@@!!!bbbBBBB"
        "...BBBBBBbb!!!!!!!!!!!!!bbBBBBBB"
        "...BBBBBBBBBBBBBBBBBBBBBBBBBBBBb"
        "..BBBBBBBBBBBBBBBBBBBBBBBBBBBBBb"
        "..BBBb..bBBBBBBBBBBBBBBBBb..bBBB"
        "..BBb....bBBBBBBBBBBBBBBb....bBB"
        "..Bb......BBBBBBBBBBBBBb......bB"
        "...........BB..BB..BB..........."
        "..........BB..BB..BB............"
        ".........BB..BB..BB............."
    );

    // Enhanced Explosion (16x16) - More dramatic
    write_bmp("pictures/explosion.bmp", 16, 16,
        "...Y....O...O..."
        "..YYY..OOO.OOO.."
        ".Y.YYY.OOoOOOo.."
        ".YYYyYYOOOOOOoo."
        "Y.YyyyyOOOooooO."
        "YYYyyyyOooooooOO"
        ".YyyyyooooooOOOo"
        "..YyyoooooooOOo."
        "..YyyoooooooOOo."
        ".YyyyyooooooOOOo"
        "YYYyyyyOooooooOO"
        "Y.YyyyyOOOooooO."
        ".YYYyYYOOOOOOoo."
        ".Y.YYY.OOoOOOo.."
        "..YYY..OOO.OOO.."
        "...Y....O...O..."
    );

    // Enhanced Player Bullet (4x8) - Bright energy beam
    write_bmp("pictures/bullet_player.bmp", 4, 8,
        ".CC."
        "CCCC"
        "CCCC"
        "CCCC"
        "CCCC"
        "CCCC"
        "CCCC"
        ".CC."
    );
    
    // Enhanced Enemy Bullet (4x8) - Yellow/orange energy
    write_bmp("pictures/bullet_enemy.bmp", 4, 8,
        ".YY."
        "YYYY"
        "YYyy"
        "YYyy"
        "YYyy"
        "YYyy"
        "YYYY"
        ".YY."
    );

    // Enhanced Saucer (20x10) - UFO with more detail
    write_bmp("pictures/saucer.bmp", 20, 10,
        "........RRRR........"
        ".......RRRRRRr......"
        "......RRrrRRrrR....."
        ".....RRRRRRRRRRRr..."
        "....RRrrrrRRrrrrRR.."
        "...RR.rr.RR.RR.rr.RR"
        "..RRRRRRRRRRRRRRRRRr"
        "..RRRRRRRRRRRRRRRRRr"
        "...RRRRr....rRRRRr.."
        ".....RR......RR....."
    );

    printf("\n=================================\n");
    printf("Enhanced assets generated!\n");
    printf("All sprites now have:\n");
    printf("- Gradient shading\n");
    printf("- More detail and depth\n");
    printf("- Larger dimensions\n");
    printf("- Better visual quality\n");
    printf("=================================\n");
    return 0;
}