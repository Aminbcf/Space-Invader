#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Width 22, Height 16
#define WIDTH  22
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
   1 = Outline/Shadow (Dark Blue-Grey)
   2 = Main Body (Cyan)
   3 = Highlight (White)
   4 = Eyes (Yellow/Red animation)
   5 = Animation A (Inner Legs - Frame 1)
   6 = Animation B (Outer Legs - Frame 2)
*/

// Perfectly Symmetric Map (Width 22)
// Columns 0-10 are mirrored by 21-11
const int invader_map[HEIGHT][WIDTH] = {
    // Top of head (Flat top)
    {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,1,1,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0},
    {0,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0,0,0},
    {0,0,1,2,2,2,3,3,3,3,3,3,3,3,2,2,2,2,2,1,0,0},

    // Eyes (Wide)
    {0,1,2,2,3,3,2,2,2,2,2,2,2,2,3,3,2,2,2,2,1,0},
    {1,2,2,3,3,2,1,1,4,4,4,4,1,1,2,3,3,2,2,2,2,1},
    {1,2,2,3,2,1,0,0,4,4,4,4,0,0,1,2,3,2,2,2,2,1},
    {1,2,2,3,2,1,0,0,4,4,4,4,0,0,1,2,3,2,2,2,2,1},

    // Mouth / Mid-section
    {1,2,2,2,2,2,1,1,1,1,1,1,1,1,2,2,2,2,2,2,1,0},
    {0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0,0},
    {0,0,1,2,2,2,1,2,2,1,1,2,2,1,2,2,2,2,1,0,0,0},
    
    // --- TENTACLES (Fixed Symmetry) ---
    // Row 12: Base connectors
    {0,0,0,1,2,1,0,5,5,0,0,5,5,0,1,2,1,0,0,0,0,0},
    
    // Row 13: 
    // Frame 1 (5): Legs go straight down
    // Frame 2 (6): Legs go out to the sides
    {0,0,6,6,1,0,0,5,5,0,0,5,5,0,0,1,6,6,0,0,0,0},
    
    // Row 14:
    {0,6,6,1,0,0,0,5,5,0,0,5,5,0,0,0,1,6,6,0,0,0},
    
    // Row 15: Tips
    {6,6,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,6,6,0,0},
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
            // If pixel is 5 (Inner Legs), show in Frame 0, hide in Frame 1
            if (pixel_type == 5) {
                pixel_type = (frame_num == 0) ? 2 : 0;
            }
            // If pixel is 6 (Outer Legs), hide in Frame 0, show in Frame 1
            else if (pixel_type == 6) {
                pixel_type = (frame_num == 1) ? 2 : 0;
            }

            // --- PALETTE ---
            switch (pixel_type) {
                case 0: // Background (Black)
                    r=0; g=0; b=0; 
                    break;
                case 1: // Shadow/Outline
                    r=0; g=30; b=50; 
                    break;
                case 2: // Main Skin (Cyan)
                    if(frame_num == 1) { r=0; g=220; b=240; } // Pulse Bright
                    else { r=0; g=160; b=190; } // Pulse Dim
                    break;
                case 3: // Highlight
                    if(frame_num == 1) { r=200; g=255; b=255; }
                    else { r=100; g=200; b=220; }
                    break;
                case 4: // Eyes
                    if(frame_num == 1) { r=255; g=50; b=50; } // Red Alert
                    else { r=255; g=200; b=0; } // Yellow Warning
                    break;
                default:
                    // Default to Background
                    r=0; g=0; b=0; 
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
    // UPDATED PATHS: removed 'src/'
    generate_invader_frame(0, "pictures/invader2_1.bmp");
    generate_invader_frame(1, "pictures/invader2_2.bmp");
    
    printf("Invader 2 assets created with corrected symmetry.\n");
    return 0;
}