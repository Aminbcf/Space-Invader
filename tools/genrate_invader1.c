#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Dimensions (Width 24, Height 16) - Classic ratio, scaled up
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

/* PALETTE KEY:
   0 = Background
   1 = Outline/Shadow (Dark Green)
   2 = Skin Main (Medium Green)
   3 = Skin Highlight (Light Green)
   4 = Eyes (Will animate: Red -> Bright Pink)
   5 = Legs Frame 1 (Visible only in Frame 1)
   6 = Legs Frame 2 (Visible only in Frame 2)
*/

const int invader_map[HEIGHT][WIDTH] = {
    // Top Antennae
    {0,0,0,0,0,0,0,0,1,2,2,0,0,2,2,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,2,3,2,0,0,2,3,2,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,2,3,2,0,0,2,3,2,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0},
    
    // Forehead
    {0,0,0,0,0,1,1,2,2,3,3,3,3,3,3,2,2,1,1,0,0,0,0,0},
    {0,0,0,0,1,2,2,2,3,3,3,3,3,3,3,3,2,2,2,1,0,0,0,0},
    
    // Eyes Row
    {0,0,0,1,2,2,3,3,3,3,3,3,3,3,3,3,3,3,2,2,1,0,0,0},
    {0,0,1,2,2,3,2,1,4,4,4,1,1,4,4,4,1,2,3,2,2,1,0,0},
    {0,0,1,2,2,3,2,1,4,4,4,1,1,4,4,4,1,2,3,2,2,1,0,0},
    
    // Mouth / Lower Body
    {0,0,1,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,1,0,0,0},
    {0,0,0,1,2,2,3,2,1,1,1,2,2,1,1,1,2,3,2,2,1,0,0,0},
    {0,0,0,0,1,2,2,1,0,0,1,2,2,1,0,0,1,2,2,1,0,0,0,0},
    
    // Leg Roots
    {0,0,0,5,1,2,2,1,6,0,0,0,0,0,0,6,1,2,2,1,5,0,0,0},
    {0,0,5,5,0,1,1,0,6,6,0,0,0,0,6,6,0,1,1,0,5,5,0,0},
    {0,5,5,0,0,0,0,0,0,6,6,0,0,6,6,0,0,0,0,0,0,5,5,0},
    {5,5,0,0,0,0,0,0,0,0,6,0,0,6,0,0,0,0,0,0,0,0,5,5},
};

void generate_invader_frame(int frame_num, const char* filename) {
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
            int pixel_type = invader_map[y][x];

            // --- ANIMATION LOGIC ---
            
            // 1. Geometric Animation (Moving Legs)
            if (pixel_type == 5 && frame_num == 0) pixel_type = 2; // Frame 0: Show Leg Type 5
            else if (pixel_type == 5 && frame_num == 1) pixel_type = 0; // Frame 1: Hide Leg Type 5
            
            else if (pixel_type == 6 && frame_num == 1) pixel_type = 2; // Frame 1: Show Leg Type 6
            else if (pixel_type == 6 && frame_num == 0) pixel_type = 0; // Frame 0: Hide Leg Type 6

            // 2. Color Palette & Pulsing Effects
            switch (pixel_type) {
                case 0: // Background
                    r=5; g=10; b=30; 
                    break;
                case 1: // Shadow/Dark Outline
                    r=10; g=30; b=10; 
                    break;
                case 2: // Main Skin (Green)
                    // Pulse breathing effect: Lighter in frame 1
                    if(frame_num == 1) { r=40; g=180; b=40; } 
                    else { r=30; g=150; b=30; }
                    break;
                case 3: // Highlight
                    if(frame_num == 1) { r=100; g=255; b=100; }
                    else { r=80; g=220; b=80; }
                    break;
                case 4: // Eyes (The Glow Up)
                    if(frame_num == 1) {
                        // Glowing bright white/pinkish in frame 1
                        r=255; g=200; b=220; 
                    } else {
                        // menacing deep red in frame 0
                        r=220; g=0; b=50; 
                    }
                    break;
                default: // Should be 0 if hidden leg
                    r=5; g=10; b=25;
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
    // Generate the two animation states
    // These names match the pattern used in your resource loader (invader1_1, invader1_2)
    generate_invader_frame(0, "pictures/invader1_1.bmp");
    generate_invader_frame(1, "pictures/invader1_2.bmp");
    
    printf("Invader assets created.\n");
    return 0;
}