#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Dimensions
#define WIDTH  31
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

/* PALETTE MAP:
   0 = Empty
   7 = Engine Core (Will animate)
   8 = Engine Outer Flame (Will animate)
*/

// A 31x32 Sprite Map
const int sprite_map[HEIGHT][WIDTH] = {
    // 0-3: Nose Tip
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    
    // 4-7: Nose Body
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,3,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,2,4,4,4,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,2,4,5,5,5,4,2,1,0,0,0,0,0,0,0,0,0,0,0},
    
    // 8-11: Cockpit Area
    {0,0,0,0,0,0,0,0,0,0,0,1,2,4,5,5,5,4,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,2,4,5,5,5,4,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,6,2,2,4,4,4,2,2,6,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,6,2,2,2,3,2,2,2,6,1,0,0,0,0,0,0,0,0,0,0},
    
    // 12-15: Guns appear
    {0,0,0,0,0,0,0,0,0,1,6,6,1,2,2,3,2,2,1,6,6,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,6,6,1,2,2,3,2,2,1,6,6,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,2,1,1,1,2,2,3,2,2,1,1,1,2,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,3,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0},
    
    // 16-19: Wings Start
    {0,0,0,0,0,0,1,2,2,2,2,4,4,4,4,4,4,4,4,4,2,2,2,2,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,2,2,2,4,4,6,6,6,6,6,6,6,6,6,4,4,2,2,2,1,0,0,0,0,0},
    {0,0,0,0,1,2,2,4,4,6,6,2,2,2,2,3,2,2,2,2,6,6,4,4,2,2,1,0,0,0,0},
    {0,0,0,1,2,2,4,6,6,2,2,2,2,2,2,3,2,2,2,2,2,2,6,6,4,2,2,1,0,0,0},
    
    // 20-23: Wings Wide
    {0,0,1,2,2,4,6,2,2,2,4,4,4,4,4,4,4,4,4,4,4,2,2,2,6,4,2,2,1,0,0},
    {0,1,2,2,4,6,2,2,4,4,2,2,2,2,2,3,2,2,2,2,2,4,4,2,2,6,4,2,2,1,0},
    {1,2,2,4,6,2,2,4,2,2,2,2,2,2,2,3,2,2,2,2,2,2,2,4,2,2,6,4,2,2,1},
    {1,3,3,4,6,2,4,2,2,2,2,1,1,1,1,1,1,1,1,1,2,2,2,2,4,2,6,4,3,3,1},
    
    // 24-27: Rear Section
    {1,3,3,4,6,2,4,2,2,2,1,6,6,6,6,6,6,6,6,6,1,2,2,2,4,2,6,4,3,3,1},
    {1,3,3,4,6,2,4,2,2,1,6,6,6,6,6,6,6,6,6,6,6,1,2,2,4,2,6,4,3,3,1},
    {1,1,1,4,6,2,4,2,1,6,6,6,6,6,6,6,6,6,6,6,6,6,1,2,4,2,6,4,1,1,1},
    {0,1,1,1,6,2,4,1,6,6,6,1,1,1,1,1,1,1,1,1,6,6,6,1,4,2,6,1,1,1,0},
    
    // 28-31: Engines / Exhaust (Using 7 for core, 8 for outer flame)
    {0,0,0,1,1,2,4,1,6,6,1,7,7,7,7,7,7,7,7,7,1,6,6,1,4,2,1,1,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,0,8,7,7,7,7,7,7,7,8,0,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,8,8,7,7,7,7,7,8,8,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,8,8,7,7,7,8,8,0,0,0,0,0,0,0,0,0,0,0,0},
};

void generate_frame(int player_id, int frame_num, const char* filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) return;

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
            int pixel_type = sprite_map[y][x];

            // ANIMATION LOGIC FOR ENGINE
            if (pixel_type == 7 || pixel_type == 8) {
                if (frame_num == 1) {
                    // Frame 1: Turbo Mode (Brighter, Whiter, Hotter)
                    if (pixel_type == 7) { r=255; g=255; b=150; } // White Hot Core
                    else { r=255; g=200; b=50; } // Bright Yellow Outer
                } else {
                    // Frame 0: Standard Cruise (Orange/Yellow)
                    if (pixel_type == 7) { r=255; g=120; b=0; } // Orange Core
                    else { r=200; g=60; b=0; } // Darker Orange Outer
                }
                
                // P2 Flame variation (Blueish flames for P2?)
                if (player_id == 1) {
                    unsigned char temp = r;
                    r = b;
                    b = temp;
                }
            } else {
                // Standard Colors
                switch (pixel_type) {
                    case 0: r=0, g=0, b=0; break; // Background: PURE BLACK
                    case 1: r=20, g=20, b=25; break; // Outline
                    case 2: // Main Hull
                        if (player_id == 0) { r=30; g=60; b=180; } // P1: Blue
                        else { r=180; g=30; b=50; } // P2: Red
                        break;
                    case 3: // Highlight
                        if (player_id == 0) { r=150; g=220; b=255; }
                        else { r=255; g=150; b=180; }
                        break;
                    case 4: // Shadow
                        if (player_id == 0) { r=10; g=30; b=100; }
                        else { r=100; g=10; b=30; }
                        break;
                    case 5: // Cockpit
                        r=0, g=200, b=220; 
                        break;
                    case 6: r=120, g=125, b=130; break; // Metal
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
    // Player 1
    generate_frame(0, 0, "pictures/player_p1_f1.bmp");
    generate_frame(0, 1, "pictures/player_p1_f2.bmp");
    // Player 2
    generate_frame(1, 0, "pictures/player_p2_f1.bmp");
    generate_frame(1, 1, "pictures/player_p2_f2.bmp");
    return 0;
}