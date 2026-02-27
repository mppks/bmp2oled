Two command line utilities for converting a bmp (black and white) image to a data array (1 byte per 8 pixels) and back, for use in firmware with OLED displays.

## Usage:
```bash
./bmp2oled.out <input_image.bmp> <output_data_file.txt>
```

```bash
./oled2bmp.out <input_data_file.txt> <width_px> <height_px> <output_image.bmp>
```