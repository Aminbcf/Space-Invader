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

int main() {
    int w = 60;
    int h = 50;
    int row_stride = (w * 3 + 3) & ~3;
    uint32_t img_size = row_stride * h;
    
    // Generate 2 frames
    for (int frame = 1; frame <= 2; frame++) {
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
        if (!data) return 1;
        
        // --- GREEN/PURPLE SAUCER DESIGN (Based on User Ref) ---
        // Colors:
        // Green: 0, 255, 100
        // Purple: 150, 0, 255
        // Dome High: 150, 255, 180
        
        int cx = w / 2;
        
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int dx = abs(x - cx);
                
                // 1. DOME (Top) [y: 30-45]
                // Semi-circleish
                if (y >= 30 && y <= 45) {
                    // Ellipse equationish
                    float ny = (y - 30) / 15.0f; // 0..1
                    float nx = dx / 15.0f; // 0..1
                    if (nx*nx + (1-ny)*(1-ny) < 1.0f) {
                        // Green Dome
                        set_pixel(data, x, y, w, 0, 220, 100); 
                        
                        // Highlight (Top Left)
                        if (x < cx-5 && y > 38) set_pixel(data, x, y, w, 200, 255, 200);
                    }
                }
                
                // 2. PURPLE RING (Middle Collar) [y: 25-30]
                if (y >= 25 && y < 30) {
                     if (dx < 20) {
                         set_pixel(data, x, y, w, 140, 0, 200); // Purple
                         if (y == 28) set_pixel(data, x, y, w, 180, 50, 240); // Highlight line
                     }
                }
                
                // 3. MAIN BODY (Green Saucer) [y: 15-25]
                // Wide ellipse
                if (y >= 15 && y < 25) {
                     float slope = (dx - 10) * 0.5f;
                     // Simple trapezoid shape
                     if (dx < 28 - (25-y)/2) {
                         set_pixel(data, x, y, w, 0, 255, 80); // Bright Green
                         // Panel lines
                         if (x % 10 == 0) set_pixel(data, x, y, w, 0, 180, 50);
                     }
                }
                
                // 4. LOWER RING (Purple) [y: 10-15]
                if (y >= 10 && y < 15) {
                    if (dx < 25) {
                        set_pixel(data, x, y, w, 100, 0, 180); // Darker Purple
                    }
                }
                
                // 5. LEGS / UNDERBODY [y: 0-10]
                if (y < 10) {
                    // Central strut
                    if (dx < 4) set_pixel(data, x, y, w, 0, 200, 50);
                    
                    // Side legs
                    if (dx > 12 && dx < 18) {
                         if (y > 2) set_pixel(data, x, y, w, 0, 200, 50);
                         // Tip glow (Frame dependent)
                         if (y < 4) {
                             if (frame == 1) set_pixel(data, x, y, w, 255, 255, 255); // Flash
                             else set_pixel(data, x, y, w, 0, 255, 0); 
                         }
                    }
                }
                
                // Glow Aura (Frame 2 only)
                if (frame == 2) {
                    // If pixel is black (0,0,0) and neighbor is not black -> Light Green
                    // Simple hack: Check if we just drew nothing
                    // (Actually difficult in single pass without buffer. Just draw aura circles)
                }
            }
        }
        
        char filename[64];
        sprintf(filename, "src/pictures/big_invader%d.bmp", frame);
        FILE* f = fopen(filename, "wb");
        if (f) {
            fwrite(&header, sizeof(header), 1, f);
            fwrite(data, img_size, 1, f);
            fclose(f);
        }
        free(data);
    }
    
    printf("Generated Green/Purple Saucer Assets.\n");
    return 0;
}
