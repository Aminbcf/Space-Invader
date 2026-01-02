#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BULLET_WIDTH 8
#define BULLET_HEIGHT 16

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

void generate_bullet(const char* filename, int type) {
    FILE *f = fopen(filename, "wb");
    if (!f) return;

    int padding = (4 - (BULLET_WIDTH * 3) % 4) % 4;
    int imageSize = (BULLET_WIDTH * 3 + padding) * BULLET_HEIGHT;

    BMPFileHeader fh = {0};
    BMPInfoHeader ih = {0};

    fh.bfType = 0x4D42;
    fh.bfOffBits = sizeof(fh) + sizeof(ih);
    fh.bfSize = fh.bfOffBits + imageSize;

    ih.biSize = sizeof(ih);
    ih.biWidth = BULLET_WIDTH;
    ih.biHeight = BULLET_HEIGHT;
    ih.biPlanes = 1;
    ih.biBitCount = 24;

    fwrite(&fh, sizeof(fh), 1, f);
    fwrite(&ih, sizeof(ih), 1, f);

    unsigned char pad[3] = {0};

    for (int y = BULLET_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < BULLET_WIDTH; x++) {
            unsigned char r = 0, g = 0, b = 0;

            int cx = BULLET_WIDTH / 2;
            float dist = abs(x - cx);
            float fy = (float)y / BULLET_HEIGHT;

            if (type == 0) { // Standard Red Bullet
                if (dist <= 2) {
                    r = 255; g = 50 + (int)(fy * 100); b = 50;
                    if (dist == 0) { r = 255; g = 200; b = 150; }
                }
            } else if (type == 1) { // ZigZag Yellow Bullet
                int offset = (y % 4 < 2) ? -1 : 1;
                if (abs(x - cx - offset) <= 1) {
                    r = 255; g = 255; b = 50;
                    if (abs(x - cx - offset) == 0) { r = 255; g = 255; b = 200; }
                }
            } else if (type == 2) { // Laser Purple Bullet
                if (dist <= 1) {
                    r = 200; g = 50; b = 255;
                    if (dist == 0) { r = 255; g = 150; b = 255; }
                }
                // Add glow effect at top/bottom
                if ((y < 3 || y > BULLET_HEIGHT - 4) && dist <= 2) {
                    r = 180; g = 0; b = 200;
                }
            } else if (type == 3) { // Player Cyan Bullet
                if (dist <= 1) {
                    r = 50; g = 200; b = 255;
                    if (dist == 0) { r = 150; g = 255; b = 255; }
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
    generate_bullet("src/pictures/bullet_enemy.bmp", 0);     // Standard Red
    generate_bullet("src/pictures/bullet_zigzag.bmp", 1);    // ZigZag Yellow
    generate_bullet("src/pictures/bullet_laser.bmp", 2);     // Laser Purple
    generate_bullet("src/pictures/bullet_player.bmp", 3);    // Player Cyan
    return 0;
}
