#ifndef IMG_MANAGER_H
#define IMG_MANAGER_H

#include <cstdint>

/**
 * @brief a luminance view of an Img_Manager, containing
 * luminance of the origianl image.
 */
class Luminance_View {
private:
public:
    static constexpr char artscii_chars[] = " .-=+*#@";
    uint32_t height, width;
    float * luminance;

    /**
     * @brief create Luminance_View object that has initialized empty luminance array
     *
     * @param height height of luminance image
     * @param width width of luminance image
     */
    Luminance_View(uint32_t, uint32_t);
    ~Luminance_View();

    /**
     * create a char array size width * height where each character is
     * its artscii representation of the luminance view.
     * 
     * @return char array of the char array
     *
     * @note reminder to delete the char arr after using.
     */
    char * gen_artscii();

    void gamma_correction(float);
};

class Img_Manager {
public:
    uint8_t * rgba;
    uint32_t height, width;

    // constructor

    /**
     * @brief create Img_Manager object that has initialized empty rgb arrays
     */
    Img_Manager(uint32_t, uint32_t);
    ~Img_Manager();

    // methods

    /**
     * @brief downsize the image's height by a factor of height_downsize
     * and width by width_downsize
     *
     * @param height_downsize The factor to downsize height
     * @param width_downsize The factor to downsize width
     * @return The pointer to a new Img_Manager downsized by
     * those factors.
     */
    void downsize(uint8_t, uint8_t);

    /**
     * @brief recolor the image to its luminance analysis
     *
     * Each pixel's luminance value is calculated by first summing 
     * the rgb value with their corresponding factor (subject to change
     * during development) then divide by 255 (since 0-255 is range of rgb)
     *
     * @param r_background red value of background
     * @param g_background green value of background
     * @param b_background blue value of background
     *
     * @return a vector of luminance value.
     * @note for the image's size and how to imagine the luminance on a picture,
     * use this image's height and width.
     */
    Luminance_View * luminance(uint8_t r, uint8_t g, uint8_t b);
    
};

#endif
