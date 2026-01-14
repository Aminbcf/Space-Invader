#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t type;      // Magic identifier "BM"
    uint32_t size;      // File size in bytes
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;    // Offset to image data
    uint32_t header_size; // Header size in bytes
    int32_t width;
    int32_t height;
    uint16_t planes;    // Number of color planes
    uint16_t bpp;       // Bits per pixel
    uint32_t compression;
    uint32_t img_size;  // Image size in bytes
    int32_t x_res;
    int32_t y_res;
    uint32_t colors;
    uint32_t imp_colors;
} BMPHeader;
#pragma pack(pop)

void set_pixel(uint8_t* data, int x, int y, int width, uint8_t r, uint8_t g, uint8_t b) {
    // BMP stores rows bottom-to-top, but we'll just handle y inversion in logic if needed
    // Actually standard BMP is bottom-up.
    // Index = (y * row_stride) + (x * 3)
    // Row stride must be multiple of 4.
    int row_stride = (width * 3 + 3) & ~3;
    int index = (y * row_stride) + (x * 3);
    data[index] = b;     // Blue
    data[index + 1] = g; // Green
    data[index + 2] = r; // Red
}

void create_big_invader(const char* filename, int frame) {
    int w = 60;
    int h = 50;
    int row_stride = (w * 3 + 3) & ~3;
    uint32_t img_size = row_stride * h;
    
    BMPHeader header = {0};
    header.type = 0x4D42; // BM
    header.size = sizeof(BMPHeader) + img_size;
    header.offset = sizeof(BMPHeader);
    header.header_size = 40; // DIB Header size
    header.width = w;
    header.height = h;
    header.planes = 1;
    header.bpp = 24;
    header.img_size = img_size;
    
    uint8_t* data = calloc(1, img_size);
    if (!data) return;
    
    // Draw logic (Purple menacing shape)
    // Bottom-up: y=0 is bottom.
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // Default transparent (black or pink key?) -> We use black as background usually, check view_sdl.c transparency...
            // view_sdl.c usually treats black as transparent if colorkey set? Or uses alpha?
            // view_sdl uses TEXTURE BLEND mode. So valid alpha needed?
            // Wait, BMP doesn't support alpha channel normally (32-bit does, but I am writing 24-bit).
            // SDL_LoadBMP usually relies on Color Key. `sdl_view_load_resources` doesn't set color key explicitly...
            // It uses `SDL_SetTextureBlendMode(dest, SDL_BLENDMODE_BLEND)`.
            // But if texture has no alpha, blend mode BLEND calculates alpha?
            // For 24-bit BMP, alpha is 255.
            // If I want transparency, I should use specific color and SetColorKey.
            // But `view_sdl.c` used `IMG_Load`. `IMG_Load` handles formatting.
            // If I draw BLACK background, updates in `sdl_view_render` use `SDL_SetRenderDrawColor` black clear.
            
            // Shape:
            // Center body
            int dx = x - w/2;
            int dy = y - h/2;
            
            // Big circle/ellipse body
            if (dx*dx/(float)(25*25) + dy*dy/(float)(20*20) <= 1.0f) {
                // Purple body
                set_pixel(data, x, y, w, 100, 0, 150);
                
                // Details/Eyes
                if (dy > 0 && dy < 15) { // Upper half eyes
                   if (abs(dx) > 8 && abs(dx) < 18) {
                       if (frame == 1) set_pixel(data, x, y, w, 0, 255, 0); // Green eyes
                       else set_pixel(data, x, y, w, 255, 0, 0); // Red eyes
                   }
                }
                
                // Stripes
                if (y % 4 == 0) {
                     set_pixel(data, x, y, w, 80, 0, 120);
                }
            } else {
                // Tentacles/Legs
                if (y < 15 && (x % 10 < 5)) {
                    // Purple legs
                     set_pixel(data, x, y, w, 100, 0, 150);
                }
            }
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
    create_big_invader("bin/pictures/big_invader1.bmp", 1);
    create_big_invader("bin/pictures/big_invader2.bmp", 2);
    // Also copy to src/pictures if needed by Makefile asset copy rule?
    // Makefile copies `src/pictures/*` to `bin/pictures/`.
    // I should generate into `src/pictures/` so `make` copies them correctly on rebuild.
    // Or just putting them in bin is ephemeral.
    // I'll put in src/pictures.
    
    create_big_invader("src/pictures/big_invader1.bmp", 1);
    create_big_invader("src/pictures/big_invader2.bmp", 2);
    
    printf("Big invader assets generated.\n");
    return 0;
}
