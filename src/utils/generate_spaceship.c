#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Increased size for better detail (Odd width 31 for perfect center symmetry)
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
   0 = Empty/Black
   1 = Dark Outline (Dark Grey)
   2 = Main Hull Color (Blue)
   3 = Highlight (Light Blue/White)
   4 = Shadow (Dark Blue)
   5 = Cockpit (Glass/Cyan)
   6 = Metal/Guns (Grey)
   7 = Engine Glow (Orange/Yellow)
*/

// A 31x32 Sprite Map (Visualized sideways here, but top-down in game)
const int sprite_map[HEIGHT][WIDTH] = {
    // 0: Nose Tip
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    
    // 4: Nose Body
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,3,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,2,4,4,4,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,2,4,5,5,5,4,2,1,0,0,0,0,0,0,0,0,0,0,0},
    
    // 8: Cockpit Area
    {0,0,0,0,0,0,0,0,0,0,0,1,2,4,5,5,5,4,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,2,4,5,5,5,4,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,6,2,2,4,4,4,2,2,6,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,6,2,2,2,3,2,2,2,6,1,0,0,0,0,0,0,0,0,0,0},
    
    // 12: Guns appear
    {0,0,0,0,0,0,0,0,0,1,6,6,1,2,2,3,2,2,1,6,6,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,6,6,1,2,2,3,2,2,1,6,6,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,2,1,1,1,2,2,3,2,2,1,1,1,2,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,3,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0},
    
    // 16: Wings Start
    {0,0,0,0,0,0,1,2,2,2,2,4,4,4,4,4,4,4,4,4,2,2,2,2,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,2,2,2,4,4,6,6,6,6,6,6,6,6,6,4,4,2,2,2,1,0,0,0,0,0},
    {0,0,0,0,1,2,2,4,4,6,6,2,2,2,2,3,2,2,2,2,6,6,4,4,2,2,1,0,0,0,0},
    {0,0,0,1,2,2,4,6,6,2,2,2,2,2,2,3,2,2,2,2,2,2,6,6,4,2,2,1,0,0,0},
    
    // 20: Wings Wide
    {0,0,1,2,2,4,6,2,2,2,4,4,4,4,4,4,4,4,4,4,4,2,2,2,6,4,2,2,1,0,0},
    {0,1,2,2,4,6,2,2,4,4,2,2,2,2,2,3,2,2,2,2,2,4,4,2,2,6,4,2,2,1,0},
    {1,2,2,4,6,2,2,4,2,2,2,2,2,2,2,3,2,2,2,2,2,2,2,4,2,2,6,4,2,2,1},
    {1,3,3,4,6,2,4,2,2,2,2,1,1,1,1,1,1,1,1,1,2,2,2,2,4,2,6,4,3,3,1},
    
    // 24: Rear Section
    {1,3,3,4,6,2,4,2,2,2,1,6,6,6,6,6,6,6,6,6,1,2,2,2,4,2,6,4,3,3,1},
    {1,3,3,4,6,2,4,2,2,1,6,6,6,6,6,6,6,6,6,6,6,1,2,2,4,2,6,4,3,3,1},
    {1,1,1,4,6,2,4,2,1,6,6,6,6,6,6,6,6,6,6,6,6,6,1,2,4,2,6,4,1,1,1},
    {0,1,1,1,6,2,4,1,6,6,6,1,1,1,1,1,1,1,1,1,6,6,6,1,4,2,6,1,1,1,0},
    
    // 28: Engines / Exhaust
    {0,0,0,1,1,2,4,1,6,6,1,0,0,7,7,7,7,7,0,0,1,6,6,1,4,2,1,1,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,0,0,0,7,7,7,7,7,0,0,0,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int main(void) {
    FILE *f = fopen("spaceship_detailed.bmp", "wb");
    if (!f) {
        printf("Error creating file.\n");
        return 1;
    }

    // BMP Padding calc
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

    // Draw from Bottom to Top (BMP Standard)
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            unsigned char b=0, g=0, r=0;

            switch (sprite_map[y][x]) {
                case 0: // Empty/Black
                    r=0; g=0; b=0; break;
                case 1: // Dark Outline (Dark Grey/Black)
                    r=20; g=20; b=25; break;
                case 2: // Main Hull (Royal Blue) -> CHANGE HERE FOR RED SHIP
                    r=30; g=60; b=180; break;
                case 3: // Highlight (Cyan/White)
                    r=150; g=220; b=255; break;
                case 4: // Shadow (Darker Blue)
                    r=10; g=30; b=100; break;
                case 5: // Cockpit Glass (Teal)
                    r=0; g=200; b=220; break;
                case 6: // Metal Details (Grey)
                    r=120; g=125; b=130; break;
                case 7: // Engine Glow (Bright Orange/Yellow)
                    r=255; g=160; b=0; break;
            }

            unsigned char pixel[3] = {b, g, r}; // BMP is BGR
            fwrite(pixel, 1, 3, f);
        }
        fwrite(pad, 1, padding, f);
    }

    fclose(f);
    printf("Generated spaceship_detailed.bmp (%dx%d)\n", WIDTH, HEIGHT);
    return 0;
}