#include <stdlib.h>
#include <stdio.h>
#include "bmp24.h"

void file_rawRead(uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

void file_rawWrite(uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}

t_pixel **bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = malloc(height * sizeof(t_pixel *));
    if (pixels == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire échouée pour les lignes.\n");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        if (pixels[i] == NULL) {
            fprintf(stderr, "Erreur : allocation mémoire échouée pour la ligne %d.\n", i);
            // Libérer ce qui a été alloué avant
            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            return NULL;
        }
    }
    return pixels;
}

void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (pixels == NULL) return;
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (img == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire échouée pour l'image.\n");
        return NULL;
    }

    img->data = bmp24_allocateDataPixels(width, height);
    if (img->data == NULL) {
        free(img);
        return NULL;
    }

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;

    return img;
}

void bmp24_free(t_bmp24 *img) {
    if (img == NULL) return;
    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}

void bmp24_readPixelValue(t_bmp24 * image, int x, int y, FILE * file) {
    uint8_t colors[3];
    fread(colors, sizeof(uint8_t), 3, file);
    image->data[y][x].blue = colors[0];
    image->data[y][x].green = colors[1];
    image->data[y][x].red = colors[2];
}


void bmp24_writePixelValue(t_bmp24 * image, int x, int y, FILE * file) {
    uint8_t colors[3];
    colors[0] = image->data[y][x].blue;
    colors[1] = image->data[y][x].green;
    colors[2] = image->data[y][x].red;
    fwrite(colors, sizeof(uint8_t), 3, file);
}

void bmp24_readPixelData(t_bmp24 * image, FILE * file) {
    int padding = (4 - (image->width * 3) % 4) % 4;

    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            bmp24_readPixelValue(image, j, i, file);
        }
        fseek(file, padding, SEEK_CUR);
    }
}


void bmp24_writePixelData(t_bmp24 * image, FILE * file) {
    int padding = (4 - (image->width * 3) % 4) % 4;
    uint8_t pad[3] = {0, 0, 0}; // 3 octets de padding (au pire)

    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            bmp24_writePixelValue(image, j, i, file);
        }
        fwrite(pad, sizeof(uint8_t), padding, file);
    }
}


t_bmp24 * bmp24_loadImage(const char * filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        return NULL;
    }

    unsigned char header[54];
    fread(header, 1, 54, f);

    int32_t width = *(int32_t *)&header[BITMAP_WIDTH];
    int32_t height = *(int32_t *)&header[BITMAP_HEIGHT];
    uint16_t colorDepth = *(uint16_t *)&header[BITMAP_DEPTH];

    t_bmp24 *img = bmp24_allocate(width, height, colorDepth);
    if (img == NULL) {
        fclose(f);
        return NULL;
    }

    uint32_t offset = *(uint32_t *)&header[BITMAP_OFFSET];
    fseek(f, offset, SEEK_SET);

    bmp24_readPixelData(img, f);

    fclose(f);
    return img;
}

void bmp24_saveImage(t_bmp24 * img, const char * filename) {
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        return;
    }

    int padding = (4 - (img->width * 3) % 4) % 4;
    uint32_t rowSize = img->width * 3 + padding;
    uint32_t imageSize = rowSize * img->height;
    uint32_t fileSize = HEADER_SIZE + INFO_SIZE + imageSize;

    t_bmp_header header;
    header.type = BMP_TYPE;
    header.size = fileSize;
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.offset = HEADER_SIZE + INFO_SIZE;

    fwrite(&header.type, sizeof(uint16_t), 1, f);
    fwrite(&header.size, sizeof(uint32_t), 1, f);
    fwrite(&header.reserved1, sizeof(uint16_t), 1, f);
    fwrite(&header.reserved2, sizeof(uint16_t), 1, f);
    fwrite(&header.offset, sizeof(uint32_t), 1, f);

    t_bmp_info info;
    info.size = INFO_SIZE;
    info.width = img->width;
    info.height = img->height;
    info.planes = 1;
    info.bits = 24;
    info.compression = 0;
    info.imagesize = imageSize;
    info.xresolution = 0x0B13;
    info.yresolution = 0x0B13;
    info.ncolors = 0;
    info.importantcolors = 0;

    fwrite(&info, sizeof(t_bmp_info), 1, f);

    bmp24_writePixelData(img, f);

    fclose(f);
}

void bmp24_negative(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red   = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue  = 255 - img->data[y][x].blue;
        }
    }
}

void bmp24_grayscale(t_bmp24 *img) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            uint8_t gray = (img->data[y][x].red + img->data[y][x].green + img->data[y][x].blue) / 3;
            img->data[y][x].red   = gray;
            img->data[y][x].green = gray;
            img->data[y][x].blue  = gray;
        }
    }
}

void bmp24_brightness(t_bmp24 *img, int value) {
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int r = img->data[y][x].red + value;
            int g = img->data[y][x].green + value;
            int b = img->data[y][x].blue + value;

            // Clamp entre 0 et 255
            if (r > 255) r = 255;
            if (r < 0)   r = 0;
            if (g > 255) g = 255;
            if (g < 0)   g = 0;
            if (b > 255) b = 255;
            if (b < 0)   b = 0;

            img->data[y][x].red   = (uint8_t)r;
            img->data[y][x].green = (uint8_t)g;
            img->data[y][x].blue  = (uint8_t)b;
        }
    }
}

void bmp24_equalize(t_bmp24 * img) {
    int Y = 0;
    int U = 0;
    int V = 0;

    t_pixel pixel;
    int *histogram = calloc(256, sizeof(int));
    for (int y = 0; y < img -> height; y ++) {
        for (int x = 0; x < img -> width; x++){
            pixel = img->data[x][y];
            Y = (int)(0.299 * pixel.red + 0.587 * pixel.green + 0.114 * pixel.blue);
            if (Y < 0) Y = 0;
            if (Y > 255) Y = 255;
            histogram[Y] ++;
        }
    }

    unsigned int CDF[256] = {0};
    unsigned int *new_hist = calloc(256, sizeof(unsigned int));
    CDF[0] = histogram[0];
    for (int i =1; i < 256; i ++) {
        CDF[i] = histogram[i] + CDF[i-1];
    }

    int i =0;
    while (CDF[i] == 0) {
        i++;
    }
    unsigned int cdf_min = CDF[i];
    for (; i < 256; i ++) {
        if (CDF[i] > 0 && CDF[i] < cdf_min) {
            cdf_min = CDF[i];
        }
    }

    if (CDF[255] == cdf_min) {
        free(histogram);
        free(new_hist);
        return;
    }

    for (int i =0; i <= 255; i++) {
        new_hist[i] = ((double)((CDF[i] - cdf_min) * 255 )/ (CDF[255] - cdf_min) );
    }


    for (int y = 0; y < img -> height; y ++) {
        for (int x = 0; x < img -> width; x++){
            pixel = img->data[x][y];
            Y = (int)(0.299 * pixel.red + 0.587 * pixel.green + 0.114 * pixel.blue);
            U = (int)(-0.14713 * pixel.red -0.28886 * pixel.green + 0.436 * pixel.blue);
            V = (int)(0.615 * pixel.red -0.51499 * pixel.green -0.10001 * pixel.blue);

            if (Y < 0) Y = 0;
            if (Y > 255) Y = 255;

            Y = new_hist[Y];

            int R = (int)(Y + 1.13983 * V);
            int G = (int)(Y - 0.39465 * U - 0.58060 * V);
            int B = (int)(Y + 2.03211 * U);

            if (R < 0) R = 0;
            if (R > 255) R = 255;
            if (G < 0) G = 0;
            if (G > 255) G = 255;
            if (B < 0) B = 0;
            if (B > 255) B = 255;

            pixel.red = R;
            pixel.green = G;
            pixel.blue = B;

            img->data[x][y] = pixel;
        }
    }
    free(histogram);
    free(new_hist);
}

t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    int offset = kernelSize / 2;
    float r = 0.0, g = 0.0, b = 0.0;

    for (int ky = -offset; ky <= offset; ky++) {
        for (int kx = -offset; kx <= offset; kx++) {
            int px = x + kx;
            int py = y + ky;

            if (px < 0) px = 0;
            if (py < 0) py = 0;
            if (px >= img->width)  px = img->width - 1;
            if (py >= img->height) py = img->height - 1;

            t_pixel p = img->data[py][px];
            float coeff = kernel[ky + offset][kx + offset];

            r += p.red   * coeff;
            g += p.green * coeff;
            b += p.blue  * coeff;
        }
    }

    t_pixel result;
    result.red   = (uint8_t)(r < 0 ? 0 : (r > 255 ? 255 : r));
    result.green = (uint8_t)(g < 0 ? 0 : (g > 255 ? 255 : g));
    result.blue  = (uint8_t)(b < 0 ? 0 : (b > 255 ? 255 : b));
    return result;
}

void bmp24_applyFilter(t_bmp24 *img, float **kernel, int kernelSize) {
    t_bmp24 *copy = bmp24_allocate(img->width, img->height, img->colorDepth);

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            copy->data[y][x] = bmp24_convolution(img, x, y, kernel, kernelSize);
        }
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x] = copy->data[y][x];
        }
    }

    bmp24_free(copy);
}

void bmp24_boxBlur(t_bmp24 *img) {
    int size = 3;
    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            kernel[i][j] = 1.0 / 9.0;
        }
    }

    bmp24_applyFilter(img, kernel, size);

    for (int i = 0; i < size; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_gaussianBlur(t_bmp24 *img) {
    int size = 3;
    float values[3][3] = {
        {1/16.0, 2/16.0, 1/16.0},
        {2/16.0, 4/16.0, 2/16.0},
        {1/16.0, 2/16.0, 1/16.0}
    };

    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            kernel[i][j] = values[i][j];
        }
    }

    bmp24_applyFilter(img, kernel, size);

    for (int i = 0; i < size; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_outline(t_bmp24 *img) {
    int size = 3;
    float values[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            kernel[i][j] = values[i][j];
        }
    }

    bmp24_applyFilter(img, kernel, size);

    for (int i = 0; i < size; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_emboss(t_bmp24 *img) {
    int size = 3;
    float values[3][3] = {
        {-2, -1, 0},
        {-1,  1, 1},
        { 0,  1, 2}
    };

    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            kernel[i][j] = values[i][j];
        }
    }

    bmp24_applyFilter(img, kernel, size);

    for (int i = 0; i < size; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_sharpen(t_bmp24 *img) {
    int size = 3;
    float values[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };

    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++) {
            kernel[i][j] = values[i][j];
        }
    }

    bmp24_applyFilter(img, kernel, size);

    for (int i = 0; i < size; i++) free(kernel[i]);
    free(kernel);
}
