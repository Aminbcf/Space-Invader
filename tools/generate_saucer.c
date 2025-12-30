#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Width 24, Height 14 (Aerodynamic shape)
#define WIDTH  24
#define HEIGHT 14

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
   1 = Shadow/Outline (Dark Red/Maroon)
   2 = Main Hull (Bright Red)
   3 = Hull Highlight (Pink/White)
   4 = Dome Glass (Cyan)
   5 = Rim Lights A (On in Frame 0, Off in Frame 1)
   6 = Rim Lights B (Off in Frame 0, On in Frame 1)
   7 = Engine Glow (Yellow)
*/

const int saucer_map[HEIGHT][WIDTH] = {
    // Top Antennae
    {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,0,0,0},
    
    // Dome (Glass Cockpit)
    {0,0,0,0,0,0,0,0,0,1,2,4,4,2,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,2,4,4,4,4,2,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,2,2,4,4,4,4,2,2,1,1,0,0,0,0,0,0},
    
    // Upper Hull
    {0,0,0,0,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0},
    {0,0,0,1,2,2,3,3,2,2,2,2,2,2,2,2,3,3,2,2,1,0,0,0},
    {0,0,1,2,2,3,3,3,3,2,2,2,2,2,2,3,3,3,3,2,2,1,0,0},
    
    // The Light Rim (Alternating Pattern 5 and 6)
    {0,1,2,2,5,5,1,6,6,1,5,5,6,6,1,5,5,1,6,6,2,2,1,0},
    {1,2,2,2,5,5,1,6,6,1,5,5,6,6,1,5,5,1,6,6,2,2,2,1},
    
    // Lower Hull / Shadow
    {1,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,1},
    {0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0},
    
    // Engine Ports (Underside)
    {0,0,1,1,2,2,1,1,7,7,1,1,1,1,7,7,1,1,2,2,1,1,0,0},
    {0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0},
};

void generate_saucer_frame(int frame_num, const char* filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Error: Could not create %s. Check 'pictures' folder.\n", filename);
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

    // Draw from Bottom to Top
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < WIDTH; x++) {
            unsigned char b=0, g=0, r=0;
            int pixel_type = saucer_map[y][x];

            // --- ANIMATION LOGIC (Running Lights) ---
            // 5: Light Group A
            if (pixel_type == 5) {
                // Frame 0: On (Cyan/White), Frame 1: Off (Dark Red)
                pixel_type = (frame_num == 0) ? 8 : 1; 
            }
            // 6: Light Group B
            else if (pixel_type == 6) {
                // Frame 1: On (Cyan/White), Frame 0: Off (Dark Red)
                pixel_type = (frame_num == 1) ? 8 : 1;
            }

            // --- PALETTE ---
            switch (pixel_type) {
                case 0: // Background (5, 10, 30)
                    r=5; g=10; b=30; break;
                case 1: // Shadow/Outline (Deep Maroon)
                    r=80; g=0; b=20; break;
                case 2: // Main Hull (Bright Red)
                    r=220; g=20; b=40; break;
                case 3: // Highlight (Rose/Pink)
                    r=255; g=150; b=180; break;
                case 4: // Dome (Teal/Cyan)
                    r=0; g=200; b=220; break;
                case 7: // Engine Glow (Gold)
                    r=255; g=200; b=0; break;
                case 8: // LIGHT ON (Bright White/Cyan)
                    r=200; g=255; b=255; break;
                default: // Fallback (Same as BG)
                    r=5; g=10; b=30; break;
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
    generate_saucer_frame(0, "pictures/bonus_saucer_f1.bmp");
    generate_saucer_frame(1, "pictures/bonus_saucer_f2.bmp");
    printf("Bonus Saucer assets created.\n");
    return 0;
}