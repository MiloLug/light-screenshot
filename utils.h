#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <png.h>


#define CHANNELS 4
#define ALIGNMENT (sizeof(void*))
#define CHECK_ERR(e) if (e) return false


inline void *
lmalloc(size_t size)
{
    void * ptr = aligned_alloc(ALIGNMENT, (size + ALIGNMENT - 1) & -ALIGNMENT);
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
  
    fread(sig, 1, 8, file);
    if (!png_check_sig(sig, 8))
        return false;   /* bad signature */
    
    void * err = NULL;
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, err, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    
    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
        png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
    png_set_bgr(png_ptr);
    
    png_set_sig_bytes(png_ptr, 8);
    png_init_io(png_ptr, file);
    CHECK_ERR(err);
    png_read_info(png_ptr, info_ptr);
    CHECK_ERR(err);

    *w = png_get_image_width(png_ptr, info_ptr);
    *h = png_get_image_height(png_ptr, info_ptr);
    uint8_t * dest = *destp = (uint8_t*)lmalloc(*w * *h * CHANNELS);
    CHECK_ERR(!dest);

    int line_bytes = *w * CHANNELS;
    uint8_t ** rows = (uint8_t**)lmalloc(*h * sizeof(uint8_t*));
    CHECK_ERR(!rows);

    for (int i = 0; i < *h; i++, dest += line_bytes)
        rows[i] = dest;

    png_read_image(png_ptr, rows);
    CHECK_ERR(err);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    free(rows);
    return true;
}
