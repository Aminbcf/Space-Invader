/* tools/assets_gen.c - Enhanced Space Invaders Assets with Better Designs */
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
        printf("Error: Could not create %s\n", filename);
        return;
    }

    int image_size = w * h * 4; 
    int file_size = 54 + image_size;

    BMPFileHeader fh = {0x4D42, file_size, 0, 0, 54};
    BMPInfoHeader ih = {40, w, h, 1, 32, 0, image_size, 2835, 2835, 0, 0};

    fwrite(&fh, 1, sizeof(fh), f);
    fwrite(&ih, 1, sizeof(ih), f);

    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            char p = pattern[y * w + x];
            uint8_t b = 0, g = 0, r = 0, a = 255;

            switch(p) {
                case '.': r=0; g=0; b=0; a=0; break; // Transparent
                case 'W': r=255; g=255; b=255; break; // White
                case 'K': r=50;  g=50;  b=50;  break; // Dark Gray
                case 'k': r=100; g=100; b=100; break; // Medium Gray
                
                // Player (Blue)
                case 'L': r=0;   g=80;  b=220; break; // Bright Blue
                case 'l': r=0;   g=40;  b=150; break; // Dark Blue
                case 'A': r=100; g=180; b=255; break; // Light Blue
                
                // Saucer (Red/Blue)
                case 'D': r=100; g=149; b=237; break; // Cornflower Blue
                case 'd': r=50;  g=100; b=180; break; // Dark Blue
                case 'R': r=220; g=20;  b=60;  break; // Crimson
                case 'r': r=139; g=0;   b=0;   break; // Dark Red
                case 'Y': r=255; g=215; b=0;   break; // Gold
                
                // Boss (Red)
                case 'B': r=180; g=0;   b=0;   break; 
                case 'b': r=100; g=0;   b=0;   break; 
                case '!': r=255; g=50;  b=50;  break; 
                case '@': r=255; g=255; b=0;   break; 
                
                // Purple (Invader 1)
                case 'P': r=128; g=0;   b=200; break; // Purple
                case 'p': r=75;  g=0;   b=130; break; // Dark Purple
                
                // Cyan/Teal (Invader 2)
                case 'C': r=0;   g=200; b=200; break; // Cyan
                case 'c': r=0;   g=100; b=150; break; // Dark Cyan
                
                // Red/Green Striped (Invader 3)
                case 'G': r=50;  g=205; b=50;  break; // Green
                case 'g': r=0;   g=120; b=0;   break; // Dark Green
                
                // Pink (Invader 4)
                case 'N': r=255; g=105; b=180; break; // Hot Pink
                case 'n': r=199; g=21;  b=133; break; // Medium Pink
                
                // Blue (Invader 5)
                case 'U': r=30;  g=144; b=255; break; // Dodger Blue
                case 'u': r=0;   g=80;  b=180; break; // Dark Blue
                
                // Orange (Invader 6)
                case 'O': r=255; g=140; b=0;   break; // Orange
                case 'o': r=200; g=80;  b=0;   break; // Dark Orange
                
                // Bullets
                case 'y': r=255; g=255; b=0;   break; 
                case 'M': r=255; g=0;   b=0;   break; 
                
                default:  r=0;   g=0;   b=0;   a=0; break;
            }
            
            uint8_t pixel[4] = {b, g, r, a};
            fwrite(pixel, 1, 4, f);
        }
    }
    fclose(f);
    printf("Generated: %s\n", filename);
}

int main() {
    /* PLAYER SHIP (19x20) - Blue Fighter */
    write_bmp("pictures/player.bmp", 19, 20,
        ".........A........."
        "........ALA........"
        "........LAL........"
        ".......lLALl......."
        ".......lLALl......."
        "......llLALll......"
        "......llLALll......"
        ".....llLAAALll....."
        "....llLLAAALLll...."
        "...llLLlAAAllLLll.."
        "..llLLllAAAllLLll.."
        ".llLLlllAAAlllLLll."
        "llLLllllAAAllllLLll"
        "lLLlllllAAAlllllLLl"
        "LL......AAA......LL"
        "LL......AAA......LL"
        "l.......AAA.......l"
        "........LKL........"
        "........l.l........"
        "........l.l........");

    write_bmp("pictures/player_f2.bmp", 19, 20,
        ".........A........."
        "........ALA........"
        "........LAL........"
        ".......lLALl......."
        ".......lLALl......."
        "......llLALll......"
        "......llLALll......"
        ".....llLAAALll....."
        "....llLLAAALLll...."
        "...llLLlAAAllLLll.."
        "..llLLllAAAllLLll.."
        ".llLLlllAAAlllLLll."
        "llLLllllAAAllllLLll"
        "lLLlllllAAAlllllLLl"
        "LL......AAA......LL"
        "LL......AAA......LL"
        "l.......AAA.......l"
        "........LKL........"
        ".........l........."
        "...................");

    /* SAUCER/UFO (17x14) - Classic Red/Blue UFO */
    write_bmp("pictures/saucer.bmp", 17, 14,
        ".....ddDDDdd....."
        "....dDDDDDDDd...."
        "...dDDDDDDDDDd..."
        "..dDDDDDDDDDDDd.."
        "..RRRRRRRRRRRRR.."
        ".RRRRRRRRRRRRRRR."
        "RRRRYYRYYRYYRRRRR"
        "rRRRRRRRRRRRRRRRr"
        ".rRRRRRRRRRRRRRr."
        "..K.....K.....K.."
        ".K......K......K."
        ".K......K......K."
        "KK......K......KK"
        "K.......K.......K");

    write_bmp("pictures/saucer_f2.bmp", 17, 14,
        ".....ddDDDdd....."
        "....dDDDDDDDd...."
        "...dDDDDDDDDDd..."
        "..dDDDDDDDDDDDd.."
        "..RRRRRRRRRRRRR.."
        ".RRRRRRRRRRRRRRR."
        "RRRRYYRYYRYYRRRRR"
        "rRRRRRRRRRRRRRRRr"
        ".rRRRRRRRRRRRRRr."
        "..K.....K.....K.."
        "..K.....K.....K.."
        ".KK....KK....KK.."
        ".K.....K.....K..."
        ".................");

    /* BOSS (32x16) - Large Red Boss */
    write_bmp("pictures/boss.bmp", 32, 16,
        "..........BBBBBBBBBBBB.........."
        ".........BBBBBBBBBBBBBB........."
        "........BBBBBBBBBBBBBBBB........"
        ".......BBBBBBBBBBBBBBBBBB......."
        "......BBBBb!!!!!!!!!!bBBBB......"
        ".....BBBBbb!!@@@@@@!!bbBBBB....."
        "....BBBBbbb!!@@@@@@!!bbbBBBB...."
        "...BBBBBBbb!!!!!!!!!!bbBBBBBB..."
        "...BBBBBBBBBBBBBBBBBBBBBBBBBB..."
        "..BBBBBBBBBBBBBBBBBBBBBBBBBBBB.."
        "..BBBb..bBBBBBBBBBBBBBBb..bBBB.."
        "..BBb....bBBBBBBBBBBBBb....bBB.."
        "..Bb......BBBBBBBBBBBB......bB.."
        "...........BB..BB..BB..........."
        "..........BB..BB..BB............"
        ".........BB..BB..BB.............");

    /* === INVADER 1 (16x12) - Purple Crab === */
    write_bmp("pictures/invader1_1.bmp", 16, 12,
        ".....PP..PP....."
        "....PPPPPPPP...."
        "...PPPPPPPPPP..."
        "..PPPPWWPPWWPP.."
        "..PPPPPPPPPPPP.."
        "..PPPPPPPPPPPP.."
        "...PPPPPPPPPP..."
        "..PP..PPPP..PP.."
        ".pp..PPPPPP..pp."
        "pp..pp....pp..pp"
        "p...p......p...p"
        "p..pp......pp..p");

    write_bmp("pictures/invader1_2.bmp", 16, 12,
        ".....PP..PP....."
        "....PPPPPPPP...."
        "...PPPPPPPPPP..."
        "..PPPPWWPPWWPP.."
        "..PPPPPPPPPPPP.."
        "..PPPPPPPPPPPP.."
        "...PPPPPPPPPP..."
        "..PP..PPPP..PP.."
        ".pp..PPPPPP..pp."
        ".p............p."
        ".p...pp..pp...p."
        ".....pp..pp.....");

    /* === INVADER 2 (16x12) - Cyan/Teal Squid === */
    write_bmp("pictures/invader2_1.bmp", 16, 12,
        ".....CC..CC....."
        "....CCCCCCCC...."
        "...CCCCCCCCCC..."
        "..CCWWCCCCWWCC.."
        "..CCCCCCCCCCCC.."
        "..CCCCCCCCCCCC.."
        "...CCCCCCCCCC..."
        "..CC..CCCC..CC.."
        ".cc..CCCCCC..cc."
        "cc..cc....cc..cc"
        "c...c......c...c"
        "c..cc......cc..c");

    write_bmp("pictures/invader2_2.bmp", 16, 12,
        ".....CC..CC....."
        "....CCCCCCCC...."
        "...CCCCCCCCCC..."
        "..CCWWCCCCWWCC.."
        "..CCCCCCCCCCCC.."
        "..CCCCCCCCCCCC.."
        "...CCCCCCCCCC..."
        "..CC..CCCC..CC.."
        ".cc..CCCCCC..cc."
        ".c............c."
        ".c...cc..cc...c."
        ".....cc..cc.....");

    /* === INVADER 3 (16x12) - Red/Green Striped === */
    write_bmp("pictures/invader3_1.bmp", 16, 12,
        "...M........M..."
        "....M......M...."
        "..GGGGGGGGGGGG.."
        ".GGGGGGGGGGGGGG."
        ".GGRRGGGGGGRRGG."
        ".GGRRGGGGGGRRGG."
        ".GGGGGGGGGGGGGG."
        "..GGGGGGGGGGGG.."
        "..MMMM....MMMM.."
        ".MM..........MM."
        "MM............MM"
        "M..............M");

    write_bmp("pictures/invader3_2.bmp", 16, 12,
        "...M........M..."
        "....M......M...."
        "..GGGGGGGGGGGG.."
        ".GGGGGGGGGGGGGG."
        ".GGRRGGGGGGRRGG."
        ".GGRRGGGGGGRRGG."
        ".GGGGGGGGGGGGGG."
        "..GGGGGGGGGGGG.."
        "..MMMM....MMMM.."
        ".MM..MM..MM..MM."
        ".....MM..MM....."
        "....M......M....");

    /* === INVADER 4 (16x12) - Teal Wide Squid === */
    write_bmp("pictures/invader4_1.bmp", 16, 12,
        "....CC....CC...."
        "...CCCC..CCCC..."
        "..CCCCCCCCCCCC.."
        ".CCWWCCCCCCWWCC."
        ".CCCCCCCCCCCCCC."
        ".CCCCCCCCCCCCCC."
        "..CCCCCCCCCCCC.."
        ".CC..CCCC..CC.."
        "cc..cc....cc..cc"
        "c...c......c...c"
        "c...c......c...c"
        "c...c......c...c");

    write_bmp("pictures/invader4_2.bmp", 16, 12,
        "....CC....CC...."
        "...CCCC..CCCC..."
        "..CCCCCCCCCCCC.."
        ".CCWWCCCCCCWWCC."
        ".CCCCCCCCCCCCCC."
        ".CCCCCCCCCCCCCC."
        "..CCCCCCCCCCCC.."
        ".CC..CCCC..CC.."
        "cc............cc"
        "c..cc......cc..c"
        "c..............c"
        "c..............c");

    /* === INVADER 5 (16x12) - Purple Robot === */
    write_bmp("pictures/invader5_1.bmp", 16, 12,
        "...p........p..."
        "....P......P...."
        "..PPPPPPPPPPPP.."
        ".PPPPPPPPPPPPPP."
        ".PPWWPPPPPPWWPP."
        ".PPWWPPPPPPWWPP."
        ".PPPPPPPPPPPPPP."
        "..PPPPPPPPPPP..."
        "..PPPP....PPPP.."
        ".PP..........PP."
        "PP............PP"
        "P..............P");

    write_bmp("pictures/invader5_2.bmp", 16, 12,
        "...p........p..."
        "....P......P...."
        "..PPPPPPPPPPPP.."
        ".PPPPPPPPPPPPPP."
        ".PPWWPPPPPPWWPP."
        ".PPWWPPPPPPWWPP."
        ".PPPPPPPPPPPPPP."
        "..PPPPPPPPPPP..."
        "..PPPP....PPPP.."
        ".PP..PP..PP..PP."
        ".....PP..PP....."
        "....P......P....");

    /* === INVADER 6 (16x12) - Pink Blocky === */
    write_bmp("pictures/invader6_1.bmp", 16, 12,
        "....NN....NN...."
        "...NNNN..NNNN..."
        "..NNNNNNNNNNNN.."
        ".NNWWNNNNNNWWNN."
        ".NNNNNNNNNNNNNN."
        ".NNNNNNNNNNNNNN."
        "..NNNNNNNNNNNN.."
        "...NNNNNNNNNN..."
        "..NN..NNNN..NN.."
        ".NN....NN....NN."
        "NN............NN"
        "N..............N");

    write_bmp("pictures/invader6_2.bmp", 16, 12,
        "....NN....NN...."
        "...NNNN..NNNN..."
        "..NNNNNNNNNNNN.."
        ".NNWWNNNNNNWWNN."
        ".NNNNNNNNNNNNNN."
        ".NNNNNNNNNNNNNN."
        "..NNNNNNNNNNNN.."
        "...NNNNNNNNNN..."
        "..NN..NNNN..NN.."
        "NN..NN....NN..NN"
        "NN............NN"
        "N..N........N..N");

    /* === INVADER 7 (16x12) - Blue/Yellow Robot === */
    write_bmp("pictures/invader7_1.bmp", 16, 12,
        "...M........M..."
        "....U......U...."
        "..UUUUUUUUUUUU.."
        ".UUUUWWUUWWUUUU."
        ".UUUUWWUUWWUUUU."
        ".UUUUUUUUUUUUUU."
        "...UUUUUUUUUU..."
        "..YYYY....YYYY.."
        ".YY..........YY."
        "YY............YY"
        "Y..............Y"
        "Y..............Y");

    write_bmp("pictures/invader7_2.bmp", 16, 12,
        "...M........M..."
        "....U......U...."
        "..UUUUUUUUUUUU.."
        ".UUUUWWUUWWUUUU."
        ".UUUUWWUUWWUUUU."
        ".UUUUUUUUUUUUUU."
        "...UUUUUUUUUU..."
        "..YYYY....YYYY.."
        ".YY..YY..YY..YY."
        ".....YY..YY....."
        "....YY....YY...."
        "....Y......Y....");

    /* === INVADER 8 (16x12) - Green Robot === */
    write_bmp("pictures/invader8_1.bmp", 16, 12,
        "...M........M..."
        "....G......G...."
        "..GGGGGGGGGGGG.."
        ".GGGGGGGGGGGGGG."
        ".GGWWGGGGGGWWGG."
        ".GGWWGGGGGGWWGG."
        ".GGGGGGGGGGGGGG."
        ".MGGGMMMMMMGGGM."
        ".M..GGGGGGGG..M."
        ".....M....M....."
        "....M......M...."
        "...MM......MM...");

    write_bmp("pictures/invader8_2.bmp", 16, 12,
        "...M........M..."
        "....G......G...."
        "..GGGGGGGGGGGG.."
        ".GGGGGGGGGGGGGG."
        ".GGWWGGGGGGWWGG."
        ".GGWWGGGGGGWWGG."
        ".GGGGGGGGGGGGGG."
        ".MGGGMMMMMMGGGM."
        ".M..GGGGGGGG..M."
        "M....M....M....M"
        "M...M......M...M"
        "MM............MM");

    /* === INVADER 9 (16x12) - Blue Robot === */
    write_bmp("pictures/invader9_1.bmp", 16, 12,
        "...u........u..."
        "....U......U...."
        "..UUUUUUUUUUUU.."
        ".UUUUUUUUUUUUUU."
        ".UUWWUUUUUUUWWUU."
        ".UUWWUUUUUUUWWUU."
        ".UUUUUUUUUUUUUU."
        "..UUUUUUUUUUUU.."
        "...UUUU..UUUU..."
        "..UU..UU..UU..UU"
        ".UU............UU"
        "UU............UU");

    write_bmp("pictures/invader9_2.bmp", 16, 12,
        "...u........u..."
        "....U......U...."
        "..UUUUUUUUUUUU.."
        ".UUUUUUUUUUUUUU."
        ".UUWWUUUUUUUWWUU."
        ".UUWWUUUUUUUWWUU."
        ".UUUUUUUUUUUUUU."
        "..UUUUUUUUUUUU.."
        "...UUUU..UUUU..."
        "UU..UUUUUUUU..UU"
        "U....UUUUUU....U"
        "U..............U");

    /* === INVADER 10 (16x12) - Orange/Pink Crab === */
    write_bmp("pictures/invader10_1.bmp", 16, 12,
        ".....OO..OO....."
        "....OOOOOOOO...."
        "...OOOOOOOOOO..."
        "..OOOOWWOOOWWOO.."
        "..OOOOOOOOOOOO.."
        "..OOOOOOOOOOOO.."
        "...OOOOOOOOOO..."
        "..OO..OOOO..OO.."
        ".nn..OOOOOO..nn."
        "nn..nn....nn..nn"
        "n...n......n...n"
        "n..nn......nn..n");

    write_bmp("pictures/invader10_2.bmp", 16, 12,
        ".....OO..OO....."
        "....OOOOOOOO...."
        "...OOOOOOOOOO..."
        "..OOOOWWOOOWWOO.."
        "..OOOOOOOOOOOO.."
        "..OOOOOOOOOOOO.."
        "...OOOOOOOOOO..."
        "..OO..OOOO..OO.."
        ".nn..OOOOOO..nn."
        ".n............n."
        ".n...nn..nn...n."
        ".....nn..nn.....");

    /* BULLET (4x8) */
    write_bmp("pictures/bullet.bmp", 4, 8,
        ".yy."
        "yyyy"
        "Oyym"
        "OyyO"
        "OyyO"
        "OyyO"
        "OOOO"
        ".OO.");

    printf("\n=================================\n");
    printf("All Assets Generated Successfully!\n");
    printf("=================================\n");
    printf("Player:     2 frames\n");
    printf("Saucer:     2 frames\n");
    printf("Boss:       1 frame\n");
    printf("Invaders:   10 types × 2 frames\n");
    printf("Bullet:     1 frame\n");
    printf("=================================\n");
    printf("Total: 26 sprite files\n");
    printf("Format: 32-bit BGRA (transparent)\n");
    printf("=================================\n");
    
    return 0;
}