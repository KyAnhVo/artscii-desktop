#include "png_decoder.h"
#include "img_manager.h"

#include <cstdio>
#include <png.h>
#include <string>
#include <stdio.h>
#include <stdexcept>

#define BACKGROUND_IMG 255, 255, 255


Png_Input::Png_Input(std::string file_name) {
    // open file

    this->fp = fopen(file_name.c_str(), "rb");

    if (!this->fp) {
        throw std::runtime_error("Cannot read file");
    }

    // check if file is actually png

    uint8_t header[8];
    size_t read_size = fread(header, 1, 8, fp);
    if (read_size != 8) {
        fclose(this->fp);
        throw std::runtime_error("File is too small");
    }

    uint8_t is_not_png = png_sig_cmp(header, 0, 8);
    if (is_not_png) {
        fclose(this->fp);
        throw std::runtime_error("File is not png");
    }

    // initialize png_struct and png_info

    this->img_ptr   = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!this->img_ptr) {
        fclose(this->fp);
        throw std::runtime_error("Cannot create PNG struct");
    }
    this->info_ptr = png_create_info_struct(this->img_ptr);
    if (!this->info_ptr) {
        png_destroy_read_struct(&this->img_ptr, &this->info_ptr, nullptr);
        fclose(this->fp);
        throw std::runtime_error("Cannot create PNG info");
    }

    // error handling hook, destroy the read structs, close fp, throw error

    if (setjmp(png_jmpbuf(this->img_ptr))) {
        png_destroy_read_struct(&this->img_ptr, &this->info_ptr, nullptr);
        fclose(this->fp);
        throw std::runtime_error("Fatal libpng error");
    }

    // initiate I/O
    png_init_io(this->img_ptr, this->fp);
    png_set_sig_bytes(this->img_ptr, 8); // the 8 png header bytes
    png_read_info(this->img_ptr, this->info_ptr);

    // load metadata to ram
    uint32_t width, height;
    int bit_depth, color_type;
    png_get_IHDR(this->img_ptr, this->info_ptr, 
            &width, &height, (int* ) &bit_depth, (int *) &color_type, 
            nullptr, nullptr, nullptr);

    // change format to rgba

    // ensure bit_depth max is 8
    if (bit_depth == 16) {
        png_set_strip_16(this->img_ptr);
        bit_depth = 8;
    }

    // ensure if grey then it is grey 8.
    if ((color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(this->img_ptr);
        bit_depth = 8;
    }

    // change GRAY to RGB
    if (color_type == PNG_COLOR_TYPE_GRAY) {
        png_set_gray_to_rgb(this->img_ptr);
        color_type = PNG_COLOR_TYPE_RGB;
    }

    // change GRAY_ALPHA to RGBA
    if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(this->img_ptr);
        color_type = PNG_COLOR_TYPE_RGBA;
    }

    // change PALLETE to RGB
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(this->img_ptr);
        color_type = PNG_COLOR_TYPE_RGB;
    }

    // change RGB to RGBA
    if (color_type == PNG_COLOR_TYPE_RGB) {
        png_set_add_alpha(img_ptr, 0xFF, PNG_FILLER_AFTER);
        color_type = PNG_COLOR_TYPE_RGBA;
    }

    // reset data of info_ptr and pull down ihdr metadata values for safety
    png_read_update_info(img_ptr, info_ptr);
    png_get_IHDR(this->img_ptr, this->info_ptr, 
            &width, &height, (int* ) &bit_depth, (int *) &color_type, 
            nullptr, nullptr, nullptr);
    
    // Stores metadata in object <this>

    this->bit_depth = static_cast<uint8_t>(bit_depth);
    this->color_type = static_cast<uint8_t>(color_type);
    this->width = width;
    this->height = height;
}

Png_Input::~Png_Input() {
    if (this->fp)
        fclose(this->fp);
    if (this->img_ptr || this->info_ptr)
        png_destroy_read_struct(&this->img_ptr, &this->info_ptr, nullptr);
}

Img_Manager * Png_Input::read_image() {
    // poing all of row pointers to its corresponding place
    // in this->img_manager->rgba
    
    Img_Manager * img_manager = new Img_Manager(this->height, this->width);
    
    png_bytep * row_ptr = new png_bytep[img_manager->height];
    for (uint32_t i = 0; i < img_manager->height; i++) {
        row_ptr[i] = img_manager->rgba + 4 * i * img_manager->width;
    }

    png_read_image(this->img_ptr, row_ptr);
    delete[] row_ptr;

    return img_manager;
}
