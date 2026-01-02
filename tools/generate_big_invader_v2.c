#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t type;      // Magic identifier "BM"
    uint32_t size;      // File size in bytes
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;    // Offset to image data
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t img_size;
    int32_t x_res;
    int32_t y_res;
    uint32_t colors;
    uint32_t imp_colors;
} BMPHeader;
#pragma pack(pop)

void set_pixel(uint8_t* data, int x, int y, int width, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= width) return;
    int row_stride = (width * 3 + 3) & ~3;
    int index = (y * row_stride) + (x * 3);
    data[index] = b;
    data[index + 1] = g;
    data[index + 2] = r;
}

// Helper to draw a filled rectangle
void draw_rect(uint8_t* data, int w, int x, int y, int rw, int rh, uint8_t r, uint8_t g, uint8_t b) {
    for(int iy=0; iy<rh; iy++) {
        for(int ix=0; ix<rw; ix++) {
            set_pixel(data, x+ix, y+iy, w, r, g, b);
        }
    }
}

void create_invader(const char* filename, int frame) {
    int w = 60;
    int h = 50;
    int row_stride = (w * 3 + 3) & ~3;
    uint32_t img_size = row_stride * h;
    
    BMPHeader header = {0};
    header.type = 0x4D42;
    header.size = sizeof(BMPHeader) + img_size;
    header.offset = sizeof(BMPHeader);
    header.header_size = 40;
    header.width = w;
    header.height = h;
    header.planes = 1;
    header.bpp = 24;
    header.img_size = img_size;
    
    uint8_t* data = calloc(1, img_size);
    if (!data) return;
    
    // Background is Black (0,0,0) - Transparent in game context if black is key (usually handling blending)
    
    // Design: Detailed Alien Battleship
    // Use symmetric drawing
    
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // Symmetry
            int cx = w/2;
            int dx = abs(x - cx);
            
            // Normalized coords
            float ny = (float)y/h;
            float nx = (float)dx/(w/2);
            
            // Main Hull (Saucer shape)
            // Top dome
            if (y >= 25 && y < 45 && dx < 20 - (y-25)*0.5) {
                set_pixel(data, x, y, w, 150, 50, 200); // Purple dome
            }
            // Rim
            if (y >= 15 && y < 25 && dx < 28) {
                set_pixel(data, x, y, w, 100, 100, 100); // Grey rim
                // Lights on rim
                if ((x % 10) < 5) {
                   if (frame == 1) set_pixel(data, x, y, w, 255, 255, 0); // Yellow lights
                   else  set_pixel(data, x, y, w, 255, 100, 0); // Orange lights
                }
            }
            // Bottom Spikes / Engines
            if (y < 15 && dx < 25) {
                if ((x % 12) > 4) {
                    set_pixel(data, x, y, w, 80, 80, 80); // Dark grey engines
                    // Thrust
                    if (y < 5) {
                         if (frame==1) set_pixel(data, x, y, w, 0, 255, 255); // Cyan thrust
                         else set_pixel(data, x, y, w, 0, 100, 255);
                    }
                }
            }
            
            // Big Eye / Cockpit
            if (y >= 28 && y < 40 && dx < 8) {
                if (frame == 1) set_pixel(data, x, y, w, 255, 0, 0); // Red Glowing Eye
                else set_pixel(data, x, y, w, 200, 0, 0);
            }
            
            // Borders for clarity
            // Simple logic: if transparent neighbor, paint pixel border? Too complex for here.
        }
    }
    
    FILE* f = fopen(filename, "wb");
    if (f) {
        fwrite(&header, sizeof(header), 1, f);
        fwrite(data, img_size, 1, f);
        fclose(f);
    }
    free(data);
}

int main() {
    create_invader("src/pictures/big_invader1.bmp", 1);
    create_invader("src/pictures/big_invader2.bmp", 2);
    printf("Generated V2 Big Invader assets.\n");
    return 0;
}
