#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <png.h>


#define CHANNELS 4


inline void *
emalloc(size_t size)
{
    void * ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Failed to allocate memory\n");
        return NULL;
    }
    return ptr;
}


bool
read_image(FILE * file, uint8_t ** destp, uint16_t * w, uint16_t * h)
{
    uint8_t sig[8];
    uint8_t * dest = NULL;
    uint8_t ** rows = NULL;
  
    if (fread(sig, 1, 8, file) != 8) {
        fprintf(stderr, "Failed to read PNG signature\n");
        return false;
    }
    if (!png_check_sig(sig, 8)) {
        fprintf(stderr, "Invalid PNG signature, possibly not a PNG file\n");
        return false;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
   
    if (!png_ptr || !info_ptr) {
        fprintf(stderr, "Failed to create PNG read struct\n");
        return false;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        goto error_end;
    }

    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
        png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
    png_set_bgr(png_ptr);
    
    png_set_sig_bytes(png_ptr, 8);
    png_init_io(png_ptr, file);
    png_read_info(png_ptr, info_ptr);

    *w = png_get_image_width(png_ptr, info_ptr);
    *h = png_get_image_height(png_ptr, info_ptr);
    dest = *destp = emalloc(*w * *h * CHANNELS);
    if (!dest) goto error_end;

    int line_bytes = *w * CHANNELS;
    rows = emalloc(*h * sizeof(uint8_t*));
    if (!rows) goto error_end;

    for (int i = 0; i < *h; i++, dest += line_bytes)
        rows[i] = dest;

    png_read_image(png_ptr, rows);

    free(rows);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    return true;

error_end:
    free(rows);
    free(dest);
    if (png_ptr) png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return false;
}
