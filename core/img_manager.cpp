#include "img_manager.h"

#include <cstdint>
#include <iostream>
#include <cmath>

#define HEIGHT_DOWNSIZE 2
#define WIDTH_DOWNSIZE  1

// ==== CLASS: LUMINANCE_VIEW ====

Luminance_View::Luminance_View(uint32_t height, uint32_t width) {
    this->height    = height;
    this->width     = width;
    uint64_t area   = static_cast<uint64_t>(height) * static_cast<uint64_t>(width); 
    this->luminance = new float[area];
}

Luminance_View::~Luminance_View() {
    delete[] this->luminance;
}

Luminance_View * Img_Manager::luminance(uint8_t r, uint8_t g, uint8_t b) {
    Luminance_View * luminance = new Luminance_View(this->height, this->width);

    for (uint32_t i = 0; i < this->height * this->width * 4; i += 4) {
        float alpha = this->rgba[i + 3] / 255.0;
        float not_alpha = 1 - alpha;

        float pixel_luminance = 
            (0.2126f * rgba[i] + 0.7152f * rgba[i + 1] + 0.0722f * rgba[i + 2]) / 255;
        float background_luminance =
            (0.2126f * r + 0.7152f * g + 0.0722f * b) / 255;

        luminance->luminance[i / 4] = alpha * pixel_luminance + not_alpha * background_luminance;
    }

    return luminance;
}

char * Luminance_View::gen_artscii() {
    char * artscii = new char[this->width * this->height];
    for (uint64_t i = 0; i < this->width * this->height; i++) {
        uint8_t brightness_level = static_cast<uint8_t>(this->luminance[i] * 7.999f);
        artscii[i] = Luminance_View::artscii_chars[brightness_level];
    }
    return artscii;
}

void Luminance_View::gamma_correction(float gamma) {
    if (gamma < 0) {
        std::cerr << "gamma must be more than or equal to 0, reset to 1" << std::endl;
        return;
    }
    for (uint64_t i = 0; i < this->width * this->height; i++) {
        this->luminance[i] = std::pow(this->luminance[i], 1 / gamma);
    }
}




// ==== CLASS: IMG_MANAGER ====

Img_Manager::Img_Manager(uint32_t height, uint32_t width) {
    this->height    = height;
    this->width     = width;
    uint64_t area   = static_cast<uint64_t>(height) * static_cast<uint64_t>(width);
    this->rgba      = new uint8_t[area * 4]; // because r, g, b, a is 4 vals.
}

Img_Manager::~Img_Manager() {
    delete[] this->rgba;
}

void Img_Manager::downsize(uint8_t height_downsize_ratio, uint8_t width_downsize_ratio) {
    // each pixel of the downsized image is map to its
    // height_downsize_ratio x width_downsize_ratio 
    // corresponding square.

    uint16_t block_size = height_downsize_ratio * width_downsize_ratio;
    uint32_t downsized_width = this->width / width_downsize_ratio,
             downsized_height = this->height / height_downsize_ratio;
    uint8_t * downsized = new uint8_t[downsized_width * downsized_height * 4];
    
    for (uint32_t downsized_row = 0; downsized_row < downsized_height; downsized_row++) {
        for (uint32_t downsized_col = 0; downsized_col < downsized_width; downsized_col++) {

            uint64_t downsized_ind = (downsized_row * downsized_width + downsized_col) * 4;

            uint32_t starting_row = downsized_row * height_downsize_ratio,
                     starting_col = downsized_col * width_downsize_ratio;
            uint64_t r_tot = 0, 
                     g_tot = 0, 
                     b_tot = 0, 
                     a_tot = 0;
            uint8_t max_a = 0;

            for (uint32_t row = starting_row; row < starting_row + height_downsize_ratio; row++) {
                for (uint32_t col = starting_col; col < starting_col + width_downsize_ratio; col++) {
                    uint64_t original_ind = (row * this->width + col) * 4;
                    uint8_t curr_a = this->rgba[original_ind + 3];
                    
                    r_tot += curr_a * this->rgba[original_ind + 0];
                    g_tot += curr_a * this->rgba[original_ind + 1];
                    b_tot += curr_a * this->rgba[original_ind + 2];
                    a_tot += curr_a;

                    max_a = max_a < curr_a ? curr_a : max_a;
                }
            }
            
            if (max_a > 0) {
                downsized[downsized_ind + 0] = static_cast<uint8_t>(r_tot / a_tot);
                downsized[downsized_ind + 1] = static_cast<uint8_t>(g_tot / a_tot);
                downsized[downsized_ind + 2] = static_cast<uint8_t>(b_tot / a_tot);
            }
            else {
                downsized[downsized_ind + 0] = 0;
                downsized[downsized_ind + 1] = 0;
                downsized[downsized_ind + 2] = 0;
            }

            downsized[downsized_ind + 3] = max_a;
        }
    }

    // change pointer
    delete[] this->rgba;
    this->rgba = downsized;

    // change sizes
    this->height    = downsized_height;
    this->width     = downsized_width;
}


