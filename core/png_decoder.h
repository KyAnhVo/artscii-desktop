#ifndef PNG_DECODER_H
#define PNG_DECODER_H

#include "img_manager.h"
#include <string>
#include <png.h>

class Png_Input {
public:
    uint32_t width, height;
    FILE * fp;
    png_structp     img_ptr;
    png_infop       info_ptr;

    uint8_t bit_depth, color_type;
    
    /**
     * @brief create a Png wrapper for a file given its path
     *
     * @param file_name the path to the file
     */
    Png_Input(std::string);
    ~Png_Input();

    /**
     * @brief read image to this->img_manager
     */
    Img_Manager * read_image();
};

#endif
