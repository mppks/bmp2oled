#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION 
#include "vendor/stb/stb_image_write.h"

/*
Входной массив frame представлен в формате Bit-mapped (SSD1306 / OLED). В нем 1 байт хранит 8 вертикальных пикселей (битов), а не 1 пиксель на 1 байт.
Для stbi_write_bmp данные должны быть в формате: 1 байт = 1 пиксель (яркость 0-255).
*/

int main(int argc, char *argv[]) {
    
    if (argc < 5) {
        printf("Usage: %s <output_file.bmp> <width_px> <height_px> <input_hex_data_file.txt>\n", argv[0]);
        return 1;
    }
    
    int width, height;

    char *output_file = argv[1];
    char *input_file = argv[4];

    if (!output_file || *output_file == '\0') {
        fprintf(stderr, "Error: Invalid input for the output file name\n"); 
        return 1;
    }

    if (!input_file || *input_file == '\0') {
        fprintf(stderr, "Error: Invalid input for the input file name\n"); 
        return 1;
    }   

    if (sscanf(argv[2], "%d", &width) != 1) {
        fprintf(stderr, "Error: Invalid input for the image width\n");
        return 1;
    }
    
    if (sscanf(argv[3], "%d", &height) != 1) {
        fprintf(stderr, "Error: Invalid input for the image height\n");
        return 1;
    }

    FILE *file = fopen(input_file, "r");
    if (!file) {
        fprintf(stderr, "Error opening input file");
        return 1;
    }

    // Считываем данные из файла во входной массив
    unsigned char *frame = (unsigned char*)malloc(width * height / 8);

    int i = 0;
    while (fscanf(file,"%X, ", &frame[i]) != EOF) {
        i++;
    }

    fclose(file);
  
    // Создаем выходной массив (1 байт на пиксель)
    unsigned char* output = (unsigned char*)malloc(width * height);
    memset(output, 0, width * height);

    // Распаковка битов в байты
    // В OLED форматах данные обычно идут страницами по 8 пикселей в высоту
    for (int page = 0; page < (height / 8); page++) {
        for (int x = 0; x < width; x++) {
            // Берем байт из исходного массива
            unsigned char byte = frame[page * width + x];
            
            // Проходим по всем 8 битам этого байта
            for (int bit = 0; bit < 8; bit++) {
                int y = page * 8 + bit;
                int output_index = y * width + x;
                
                // Если бит установлен, ставим белый пиксель (255), иначе черный (0)
                if (byte & (1 << bit)) {
                    output[output_index] = 255;
                } else {
                    output[output_index] = 0;
                }
            }
        }
    }

    // Записываем результат
    stbi_write_bmp(output_file, width, height, 1, output);

    free(frame);
    free(output);

    return 0;
}
