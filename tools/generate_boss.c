#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Massive Size for a Boss (Odd width 39 for perfect center symmetry)
#define WIDTH  39
#define HEIGHT 32

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

/* MATERIAL PALETTE:
   0 = Background (Space)
   1 = Dark Chassis (Grey/Black)
   2 = Heavy Armor (Crimson Red)
   3 = Armor Highlight (Bright Red)
   4 = Gold Trim / Heat Sinks
   5 = Mechanical Guts (Dark Grey/Wires)
   6 = ENGINE (Animates: Orange <-> Yellow)
   7 = CORE EYE (Animates: Cyan <-> White)
   8 = WEAPON CHARGE (Animates: Off <-> Green)
*/

// A complex 39x32 Sprite Map (Symmetric)
const int boss_map[HEIGHT][WIDTH] = {
    // Top Spikes / Antennae
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,4,4,4,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    
    // Upper Bridge / Head
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,3,4,4,4,3,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,3,3,2,2,2,3,3,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,3,2,2,5,5,5,2,2,3,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,2,2,4,4,2,5,7,7,7,5,2,4,4,2,2,1,0,0,0,0,0,0,0,0,0,0,0},
    
    // The Core Eye Section
    {0,0,0,0,0,0,0,0,0,0,1,2,2,4,2,2,1,5,7,7,7,5,1,2,2,4,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,2,2,4,2,2,1,1,5,7,8,7,5,1,1,2,2,4,2,2,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,2,2,4,2,2,1,0,1,5,7,7,7,5,1,0,1,2,2,4,2,2,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,2,2,4,2,2,1,0,0,1,2,5,5,5,2,1,0,0,1,2,2,4,2,2,1,0,0,0,0,0,0,0},
    
    // Shoulder Pads Expansion
    {0,0,0,0,0,1,1,2,2,4,2,2,1,0,0,0,1,2,2,4,2,2,1,0,0,0,1,2,2,4,2,2,1,1,0,0,0,0,0},
    {0,0,0,0,1,4,4,2,3,2,2,1,0,0,0,1,2,2,3,4,3,2,2,1,0,0,0,1,2,2,3,2,4,4,1,0,0,0,0},
    {0,0,0,1,2,4,4,3,2,2,1,0,0,1,1,2,2,3,3,4,3,3,2,2,1,1,0,0,1,2,2,3,4,4,2,1,0,0,0},
    {0,0,1,2,2,4,4,2,2,1,0,1,1,5,5,2,3,3,3,4,3,3,3,2,5,5,1,1,0,1,2,2,4,4,2,2,1,0,0},
    
    // Main Body Bulk
    {0,1,2,2,3,2,2,2,1,0,1,5,5,5,5,5,2,2,3,4,3,2,2,5,5,5,5,5,1,0,1,2,2,2,3,2,2,1,0},
    {1,2,2,3,2,2,2,1,0,1,5,5,5,5,5,5,5,2,3,4,3,2,5,5,5,5,5,5,5,1,0,1,2,2,2,3,2,2,1},
    {1,2,3,2,2,2,1,0,1,5,5,5,5,5,5,5,5,5,2,2,2,5,5,5,5,5,5,5,5,5,1,0,1,2,2,2,3,2,1},
    {1,2,3,2,2,1,0,1,2,2,5,5,5,5,5,5,5,5,5,1,5,5,5,5,5,5,5,5,2,2,1,0,1,2,2,3,2,1},
    
    // Weapon Systems (Sides)
    {1,2,4,2,1,0,0,1,2,2,2,5,5,5,5,5,5,5,1,6,1,5,5,5,5,5,5,2,2,2,1,0,0,1,2,4,2,1},
    {1,4,8,4,1,0,0,1,2,2,2,2,2,2,5,5,5,1,6,6,6,1,5,5,5,2,2,2,2,2,2,1,0,0,1,4,8,4,1},
    {1,4,8,4,1,0,1,2,2,2,2,3,3,2,2,2,1,6,6,6,6,6,1,2,2,2,3,3,2,2,2,2,1,0,1,4,8,4,1},
    {1,2,4,2,1,0,1,2,2,2,3,3,3,3,2,1,6,6,6,6,6,6,6,1,2,3,3,3,3,2,2,2,1,0,1,2,4,2,1},
    
    // Heavy Engines
    {0,1,1,1,0,0,1,2,2,3,4,4,4,3,2,1,6,6,6,6,6,6,6,1,2,3,4,4,4,3,2,2,1,0,0,1,1,1,0},
    {0,0,0,0,0,0,1,2,3,4,4,6,4,4,3,1,6,6,6,6,6,6,6,1,3,4,4,6,4,4,3,2,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,2,3,4,6,6,6,4,3,1,6,6,6,6,6,6,6,1,3,4,6,6,6,4,3,2,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,2,2,4,6,6,6,4,2,2,1,6,6,6,6,6,1,2,2,4,6,6,6,4,2,2,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,2,4,6,6,6,4,2,1,0,1,1,6,1,1,0,1,2,4,6,6,6,4,2,1,0,0,0,0,0,0,0},
    
    // Exhaust Plumes
    {0,0,0,0,0,0,0,0,1,1,6,6,6,1,1,0,0,0,0,6,0,0,0,0,1,1,6,6,6,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,6,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0},
};

void generate_boss_frame(int frame_num, const char* filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Error: Could not create %s\n", filename);
        return;
    }

    // Standard Padding for BMP
    int padding = (4 - (WIDTH * 3) % 4) % 4;
    int imageSize = (WIDTH * 3 + padding) * HEIGHT;

    BMPFileHeader fh = {0};
    BMPInfoHeader ih = {0};

    fh.bfType = 0x4D42;
    fh.bfOffBits = sizeof(fh) + sizeof(ih);
    fh.bfSize = fh.bfOffBits + imageSize;

    ih.biSize = sizeof(ih);
    ih.biWidth = WIDTH;
    ih.biHeight = HEIGHT;
    ih.biPlanes = 1;
    ih.biBitCount = 24;

    fwrite(&fh, sizeof(fh), 1, f);
    fwrite(&ih, sizeof(ih), 1, f);

    unsigned char pad[3] = {0};

    // Draw from Bottom to Top
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            unsigned char b=0, g=0, r=0;
            int pixel_type = boss_map[y][x];

            // --- ANIMATION LOGIC ---
            
            // 6: ENGINES (Flicker)
            if (pixel_type == 6) {
                 if (frame_num == 1) { // Turbo Bright
                     r=255; g=200; b=50;
                 } else { // Standard Orange
                     r=255; g=100; b=0; 
                 }
            }
            // 7: CORE EYE (Pulse)
            else if (pixel_type == 7) {
                if (frame_num == 1) { // Active: Bright Cyan/White
                    r=200; g=255; b=255;
                } else { // Resting: Deep Blue
                    r=0; g=50; b=200;
                }
            }
            // 8: WEAPON CHARGE (Blink)
            else if (pixel_type == 8) {
                if (frame_num == 1) { // Charged: Green
                    r=50; g=255; b=50;
                } else { // Off: Dark
                    r=20; g=50; b=20;
                }
            }
            // STATIC MATERIALS
            else {
                switch (pixel_type) {
                    case 0: // Background (5, 10, 30)
                        r=5; g=10; b=30; break;
                    case 1: // Chassis (Dark Grey)
                        r=40; g=40; b=45; break;
                    case 2: // Heavy Armor (Crimson)
                        r=120; g=20; b=30; break;
                    case 3: // Armor Highlight (Red/Orange)
                        r=200; g=50; b=50; break;
                    case 4: // Gold/Brass Trim
                        r=218; g=165; b=32; break;
                    case 5: // Mechanical Guts (Wires/Dark)
                        r=20; g=20; b=20; break;
                }
            }

            unsigned char pixel[3] = {b, g, r};
            fwrite(pixel, 1, 3, f);
        }
        fwrite(pad, 1, padding, f);
    }

    fclose(f);
    printf("Generated %s\n", filename);
}

int main(void) {
    generate_boss_frame(0, "pictures/boss_dreadnought_f1.bmp");
    generate_boss_frame(1, "pictures/boss_dreadnought_f2.bmp");
    printf("BOSS DREADNOUGHT assets created.\n");
    return 0;
}