#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Width 24, Height 16 (Imposing Size)
#define WIDTH  24
#define HEIGHT 16

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

/* PALETTE:
   0 = Background (5, 10, 30)
   1 = Outline/Shadow (Dark Maroon)
   2 = Main Body (Red)
   3 = Highlight (Orange/Gold)
   4 = Eyes (Cyan/White Pulse)
   5 = Arms Up (Frame 1)
   6 = Arms Down (Frame 2)
*/

// Perfectly Symmetric Map (Columns 0-11 mirrored by 23-12)
const int invader_map[HEIGHT][WIDTH] = {
    // Sharp Pointy Head
    {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,2,3,3,2,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,2,2,3,3,2,2,1,0,0,0,0,0,0,0,0},
    
    // Upper Body
    {0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,2,2,3,3,3,3,3,3,2,2,1,0,0,0,0,0,0},
    
    // Eyes (Vertical Slits)
    {0,0,0,0,0,1,2,2,3,2,2,2,2,2,2,3,2,2,1,0,0,0,0,0},
    {0,0,0,0,1,2,2,3,2,1,4,4,4,4,1,2,3,2,2,1,0,0,0,0},
    {0,0,0,0,1,2,2,3,2,1,4,4,4,4,1,2,3,2,2,1,0,0,0,0},
    
    // Mouth / Tentacle Base
    {0,0,0,0,1,2,2,2,2,2,1,1,1,1,2,2,2,2,2,1,0,0,0,0},
    {0,0,0,1,2,2,2,2,2,1,0,0,0,0,1,2,2,2,2,2,1,0,0,0},
    
    // Animated Arms/Legs Area
    // 5 = Arms Up (Frame 0), 6 = Arms Down (Frame 1)
    
    // Row 11: Connections
    {0,0,1,2,2,2,1,0,0,5,1,0,0,1,5,0,0,1,2,2,2,1,0,0},
    
    // Row 12
    {0,1,2,2,1,0,0,5,5,0,0,6,6,0,0,5,5,0,0,1,2,2,1,0},
    
    // Row 13
    {1,2,2,1,0,0,5,5,0,0,6,6,6,6,0,0,5,5,0,0,1,2,2,1},
    
    // Row 14: Tips
    {1,5,5,1,0,0,0,0,0,6,6,0,0,6,6,0,0,0,0,0,1,5,5,1},
    
    // Row 15: Lowest Tips
    {5,5,0,0,0,0,0,0,1,6,0,0,0,0,6,1,0,0,0,0,0,0,5,5},
};

void generate_invader_frame(int frame_num, const char* filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Error: Could not create %s. Check if folder 'pictures' exists.\n", filename);
        return;
    }

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

    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            unsigned char b=0, g=0, r=0;
            int pixel_type = invader_map[y][x];

            // --- ANIMATION LOGIC ---
            // Pixel 5: Arms Up (Visible Frame 0)
            if (pixel_type == 5) {
                pixel_type = (frame_num == 0) ? 2 : 0;
            }
            // Pixel 6: Arms Down (Visible Frame 1)
            else if (pixel_type == 6) {
                pixel_type = (frame_num == 1) ? 2 : 0;
            }

            // --- PALETTE (Red Theme) ---
            switch (pixel_type) {
                case 0: // Background (Fixed: 5, 10, 30)
                    r=5; g=10; b=30; 
                    break;
                case 1: // Shadow/Outline (Deep Maroon)
                    r=60; g=0; b=20; 
                    break;
                case 2: // Main Body (Crimson Red)
                    if(frame_num == 1) { r=220; g=40; b=60; } // Pulse Brighter
                    else { r=180; g=20; b=40; } // Pulse Darker
                    break;
                case 3: // Highlight (Orange/Gold)
                    if(frame_num == 1) { r=255; g=200; b=50; }
                    else { r=255; g=140; b=0; }
                    break;
                case 4: // Eyes (Cold Contrast)
                    if(frame_num == 1) { r=255; g=255; b=255; } // Flash White
                    else { r=0; g=255; b=255; } // Steady Cyan
                    break;
                default:
                    r=5; g=10; b=30; 
                    break;
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
    generate_invader_frame(0, "pictures/invader3_1.bmp");
    generate_invader_frame(1, "pictures/invader3_2.bmp");
    
    printf("Invader 3 (The Red Squid) assets created.\n");
    return 0;
}