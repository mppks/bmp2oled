#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION 
#include "vendor/stb/stb_image.h"

// Функция конвертации
// input: массив width*height байт
// output: массив width*height/8 байт (1 бит на пиксель, формат страниц SSD1306)
void convert_to_oled(const unsigned char* input, unsigned char* output, int width, int height) {

    // Обнуляем выходной массив
    memset(output, 0, (width * height) / 8);

    for (int page = 0; page < 8; page++) {         // 8 страниц по высоте
        for (int x = 0; x < width; x++) {          // 128 пикселей по ширине
            unsigned char byte = 0;
            for (int bit = 0; bit < 8; bit++) {    // 8 бит в одном байте (вертикально)
                int y = page * 8 + bit;
                int input_index = y * width + x;

                // Если яркость пикселя > 127, считаем его "белым" (1)
                if (input[input_index] > 127) {
                    byte |= (1 << bit);
                }
            }
            output[page * width + x] = byte;
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Usage: %s <input_image.bmp> <output_data_file.txt>\n", argv[0]);
        return 1;
    }
    int width, height, comp;

    char *input_file = argv[1];
    char *output_file = argv[2];

    if (!output_file || *output_file == '\0') {
        fprintf(stderr, "Error: Invalid input for the output file name\n"); 
        return 1;
    }

    if (!input_file || *input_file == '\0') {
        fprintf(stderr, "Error: Invalid input for the input file name\n"); 
        return 1;
    }   

    unsigned char *image_data = stbi_load(input_file, &width, &height, &comp, 1);

    if (image_data == NULL) {
        printf("Error loading image: %s\n", stbi_failure_reason());
        return 1;
    }

    printf("Image loaded successfully!\n");
    printf("Width: %d, Height: %d, Channels in file: %d, Desired channels: %d\n", width, height, comp);
    
    unsigned char *oled_buffer = (unsigned char*)malloc(width * height / 8);

    // Конвертируем
    convert_to_oled(image_data, oled_buffer, width, height);

    free(image_data);
    
    FILE *file = fopen(output_file, "w");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Вывод результата в формате Си-массива для вставки в прошивку
    for (int i = 0; i < width * height / 8; i++) {
        fprintf(file, "0x%02X, ", oled_buffer[i]);
    }

    fclose(file);
    free(oled_buffer);

    return 0;
}
