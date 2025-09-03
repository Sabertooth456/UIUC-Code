#include "../lib/cs225/PNG.h"

/**
 * @file Image.h
 * Contains your declaration of the interface for the Image class.
 */

class Image : public cs225::PNG {
    public:
        using cs225::PNG::PNG;
        
        // /**
        //  * Creates an empty PNG image
        // */
        // Image() {
        //     PNG();
        // };

        // /**
        // * Creates a PNG image of the specified dimensions.
        // * @param width Width of the new image.
        // * @param height Height of the new image.
        // */
        // Image(unsigned int width, unsigned int height) {
        //     PNG(width, height);
        // };

        // /**
        //  * Copy constructor: creates a new Image that is a copy of
        //  * another.
        //  * @param other Image to be copied.
        //  */
        // Image(Image const & other) {
        //     PNG(other.PNG);
        // };

        // /**
        //  * Destructor: frees all memory associated with a given Image object.
        //  * Invoked by the system.
        //  */
        // ~Image() {
        //     ~PNG();
        // };

        // /**
        //  * Assignment operator for setting two PNGs equal to one another.
        //  * @param other Image to copy into the current image.
        //  * @return The current image for assignment chaining.
        //  */
        // Image const & operator=(Image const& other) {
        //     operator=(other.PNG);
        // }

        /**
         * Lighten an Image by increasing the luminance of every pixel by 0.1.
        */
        void lighten();

        /**
         * Lighten an Image by increasing the luminance of every pixel by a specified amount.
         * @param amount Amount to increase the luminance 
        */
        void lighten(double amount);

        /**
         * Darken an Image by decreasing the luminance of every pixel by 0.1.
        */
        void darken();

        /**
         * Darken an Image by decreasing the luminance of every pixel by a specified amount.
         * @param amount Amount to decrease the luminance 
        */
        void darken(double amount);

        /**
         * Saturate an Image by increasing the saturation of every pixel by 0.1.
        */
        void saturate();

        /**
         * Saturate an Image by increasing the saturation of every pixel by a specified amount.
         * @param amount Amount to increase the saturation 
        */
        void saturate(double amount);

        /**
         * Deaturate an Image by decreasing the saturation of every pixel by 0.1.
        */
        void desaturate();

        /**
         * Desaturate an Image by decreasing the saturation of every pixel by a specified amount.
         * @param amount Amount to decrease the saturation 
        */
        void desaturate(double amount);

        /**
         * Turns the image grayscale
        */
        void grayscale();

        /**
         * Rotate the color wheel by the specified degrees
         * @param degrees Degrees to rotate the color wheel
        */
        void rotateColor(double degrees);

        /**
         * Illinify the image
        */
        void illinify();

        /**
         * Scale the image by the given factor
         * @param factor Factor to scale the image
        */
        void scale(double factor);

        /**
         * Scale the image to fit within the size (w x h)
         * @param w Width of the new image
         * @param h Height of the new image
        */
        void scale(unsigned w, unsigned h);
};

#pragma once