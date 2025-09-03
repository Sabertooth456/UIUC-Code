/**
 * @file StickerSheet.h
 * Contains your declaration of the interface for the StickerSheet class.
 */
#include "Image.h"
#include <vector>

struct Sticker {
    // /**
    //  * Initializes this Sticker with a deep copy of the base sticker
    //  * @param _sticker The base sticker to be deep copied
    // */
    // Sticker(Image _image, unsigned _x, unsigned _y) : image(Image(_image)), x(_x), y(_y) {};

    // /**
    //  * The copy constructor makes this Sticker an independent copy of the source.
    //  * @param other The source object to be copied
    //  */
    // Sticker(const Sticker& _sticker);

    // /**
    //  * Frees all space that was dynamically allocated by this Sticker
    // */
    // ~Sticker();

    // const Sticker& operator=(const Sticker& other);
    Image image;
    unsigned x = 0;
    unsigned y = 0;
};

class StickerSheet {
    public:

    /**
     * Initializes this StickerSheet with a deep copy of the base picture and
     * the ability to hold a max number of stickers (Images) with indices 0 through max - 1.
     * @param picture The base picture to use in the StickerSheet
     * @param max The maximum number of stickers (Images) on this StickerSheet
     */
    StickerSheet(const Image &picture, unsigned max);
    
 	// /**
    //  * Frees all space that was dynamically allocated by this StickerSheet
    //  * */
    // ~StickerSheet();

    // /**
    //  * The copy constructor makes this StickerSheet an independent copy of the source.
    //  * @param other The source object to be copied
    //  */
    // StickerSheet(const StickerSheet &other);

    // /**
    //  * The assignment operator for the StickerSheet class.
    //  * @param other The other StickerSheet object to copy data from
    // */
    // const StickerSheet& operator=(const StickerSheet &other);

    /**
     * Modifies the maximum number of stickers that can be stored on this StickerSheet without changing existing stickers' indices.
     * @param max The new value for the maximum number of Images in the StickerSheet
     */
    void changeMaxStickers(unsigned max);

    /**
     * Adds a sticker to the StickerSheet, so that the top-left of the sticker's Image is at (x, y) on the StickerSheet.
     * @param sticker The Image of the sticker
     * @param x The x location of the sticker on the StickerSheet
     * @param y The y location of the sticker on the StickerSheet
     */
    int addSticker(Image &sticker, unsigned x, unsigned y);

    /**
     * Changes the x and y coordinates of the Image in the specified layer.
     * @param index Zero-based layer index of the sticker
     * @param x The new x location of the sticker on the StickerSheet
     * @param y The new y location of the sticker on the StickerSheet
     */
    bool translate(unsigned index, unsigned x, unsigned y);
    
    /**
     * Removes the sticker at the given zero-based layer index.
     * @param index The layer in which to delete the PNG
     */
    void removeSticker (unsigned index);
 	
    /**
     * Returns a pointer to the sticker at the specified index, not a copy of it.
     * @param index The layer in which to get the sticker
     */
    Image* getSticker(unsigned index);
    
    /**
     * Renders the whole StickerSheet on one Image and returns that Image.
     */
    Image render() const;
    
    // private:

    /**
     * Stamps a Sticker onto the base image
     * @param baseImage The image that the Sticker will be stamped on
     * @param sticker The sticker to stamp on the image
    */
    void stamp(Image& baseImage, Sticker sticker) const;

    // /**
    //  * Copies the contents of other to self
    // */
    // void _copy(StickerSheet const & other);

    unsigned maxStickers = 0;
    Image basePicture;
    std::vector<Sticker> stickerSheet;
};


#pragma once