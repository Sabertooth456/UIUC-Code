#include "Image.h"
#include <iostream>

/**
 * Lighten an Image by increasing the luminance of every pixel by 0.1.
*/
void Image::lighten() {
    lighten(0.1);
}

/**
 * Lighten an Image by increasing the luminance of every pixel by a specified amount.
 * @param amount Amount to increase the luminance 
*/
void Image::lighten(double amount) {
    unsigned int height = this->height();
    unsigned int width = this->width();
    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {
            cs225::HSLAPixel& cur_pixel = this->getPixel(col, row);
            cur_pixel.l += amount;
            if (cur_pixel.l > 1.0) {
                cur_pixel.l = 1.0;
            }
        }
    }
}

/**
 * Darken an Image by decreasing the luminance of every pixel by 0.1.
*/
void Image::darken() {
    darken(0.1);
}

/**
 * Darken an Image by decreasing the luminance of every pixel by a specified amount.
 * @param amount Amount to decrease the luminance 
*/
void Image::darken(double amount) {
    unsigned int height = this->height();
    unsigned int width = this->width();
    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {
            cs225::HSLAPixel& cur_pixel = this->getPixel(col, row);
            cur_pixel.l -= amount;
            if (cur_pixel.l < 0.0) {
                cur_pixel.l = 0.0;
            }
        }
    }
}

/**
 * Saturate an Image by increasing the saturation of every pixel by 0.1.
*/
void Image::saturate() {
    saturate(0.1);
}

/**
 * Saturate an Image by increasing the saturation of every pixel by a specified amount.
 * @param amount Amount to increase the saturation 
*/
void Image::saturate(double amount) {
    unsigned int height = this->height();
    unsigned int width = this->width();
    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {
            cs225::HSLAPixel& cur_pixel = this->getPixel(col, row);
            cur_pixel.s += amount;
            if (cur_pixel.s > 1.0) {
                cur_pixel.s = 1.0;
            }
        }
    }
}

/**
 * Deaturate an Image by decreasing the saturation of every pixel by 0.1.
*/
void Image::desaturate() {
    desaturate(0.1);
}

/**
 * Desaturate an Image by decreasing the saturation of every pixel by a specified amount.
 * @param amount Amount to decrease the saturation 
*/
void Image::desaturate(double amount) {
    unsigned int height = this->height();
    unsigned int width = this->width();
    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {
            cs225::HSLAPixel& cur_pixel = this->getPixel(col, row);
            cur_pixel.s -= amount;
            if (cur_pixel.s < 0.0) {
                cur_pixel.s = 0.0;
            }
        }
    }
}

/**
 * Turns the image grayscale
*/
void Image::grayscale() {
    unsigned int height = this->height();
    unsigned int width = this->width();
    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {
            cs225::HSLAPixel& cur_pixel = this->getPixel(col, row);
            cur_pixel.s = 0.0;
        }
    }
}

/**
 * Rotate the color wheel by the specified degrees
 * @param degrees Degrees to rotate the color wheel
*/
void Image::rotateColor(double degrees) {
    unsigned int height = this->height();
    unsigned int width = this->width();
    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {
            cs225::HSLAPixel& cur_pixel = this->getPixel(col, row);
            cur_pixel.h = cur_pixel.h + degrees;
            while (cur_pixel.h < 0) {
                cur_pixel.h += 360;
            }
            while (cur_pixel.h >= 360) {
                cur_pixel.h -= 360;
            }
        }
    }
}

/**
 * Illinify the image
*/
void Image::illinify() {
    // Illini Orange is 11
    // Illini Blue is 216
    // 216 - 11 = 205
    // 205 / 2 = 102.5
    // 360 - 216 + 11 = 115
    // 115 / 2 = 57.5
    // 11 + 102.5 = 113.5
    // 216 + 57.5 = 273.5
    // Values between 113.5 and 273.5 are closer to Illini Blue
    // Values outside of that range are closer to Illini Orange
    unsigned int height = this->height();
    unsigned int width = this->width();
    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {
            cs225::HSLAPixel& cur_pixel = this->getPixel(col, row);
            if (cur_pixel.h > 113.5 && cur_pixel.h < 273.5) {
                cur_pixel.h = 216;
            } else {
                cur_pixel.h = 11;
            }
        }
    }
}

/**
 * Scale the image by the given factor
 * @param factor Factor to scale the image
*/
void Image::scale(double factor) {
    scale(this->width() * factor, this->height() * factor);
}

/**
 * Scale the image to fit within the size (w x h)
 * @param w Width of the new image
 * @param h Height of the new image
*/
void Image::scale(unsigned w, unsigned h) {
    if (this->height() == 0 || this->width() == 0) {
        return;
    }
    double cur_ratio = static_cast<double>(this->height()) / this->width();
    double new_ratio = static_cast<double>(h) / w;
    unsigned int new_width = 0;
    unsigned int new_height = 0;
    // If the new ratio is taller or wider than the current ratio
    // Adjust the width and height to equal the current ratio
    if (cur_ratio  < new_ratio) {
        new_width = w;
        new_height = w * cur_ratio;
    }
    else {
        new_height = h;
        new_width = h / cur_ratio;
    }
    double scaling_ratio = static_cast<double>(new_height) / this->height();
    cs225::HSLAPixel* new_image = new cs225::HSLAPixel[new_width * new_height];
    for (unsigned int x = 0; x < new_width; x++) {
        for (unsigned int y = 0; y < new_height; y++) {
            new_image[x + new_width * y] = this->getPixel(static_cast<int>(x / scaling_ratio), static_cast<int>(y / scaling_ratio));
        }
    }
    this->resize(new_width, new_height);
    for (unsigned int x = 0; x < new_width; x++) {
        for (unsigned int y = 0; y < new_height; y++) {
            cs225::HSLAPixel& cur_pixel = this->getPixel(x, y);
            cur_pixel = new_image[x + new_width * y];
        }
    }
    delete[] new_image;
}