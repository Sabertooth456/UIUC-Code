#include "StickerSheet.h"

StickerSheet::StickerSheet(const Image &picture, unsigned max) {
    maxStickers = max;
    basePicture = picture;
    stickerSheet = std::vector<Sticker>();
}

// StickerSheet::~StickerSheet() {
//     basePicture.~Image();
//     for (unsigned i = 0; i < stickerSheet.size(); i++) {
//         stickerSheet[i].image.~Image();
//     }
// }

// StickerSheet::StickerSheet(const StickerSheet &other) {
//     maxStickers = other.maxStickers;
//     basePicture = other.basePicture;
//     stickerSheet = std::vector<Sticker>();
//     for (unsigned i = 0; i < other.stickerSheet.size(); i++) {
//         stickerSheet.push_back(Sticker(other.stickerSheet[i]));
//     }
// }

// const StickerSheet& StickerSheet::operator=(const StickerSheet &other) {
//     if (this != &other) { _copy(other); }
//     return *this;
// }

// void StickerSheet::_copy(StickerSheet const & other) {
//     // // Clear self
//     // this->~StickerSheet();

//     // Copy other to self
//     maxStickers = other.maxStickers;
//     basePicture = other.basePicture;
//     stickerSheet = std::vector<Sticker>();
//     for (unsigned i = 0; i < other.stickerSheet.size(); i++) {
//         stickerSheet.push_back(Sticker(other.stickerSheet[i]));
//     }
// }

void StickerSheet::changeMaxStickers(unsigned max) {
    while (stickerSheet.size() > max) {
        this->removeSticker(max);
    }
    maxStickers = max;
}

int StickerSheet::addSticker(Image &sticker, unsigned x, unsigned y) {
    if (stickerSheet.size() == maxStickers) {
        return -1;
    }
    stickerSheet.push_back(Sticker{Image(sticker), x, y});
    return stickerSheet.size() - 1;
}

bool StickerSheet::translate(unsigned index, unsigned x, unsigned y) {
    if (index >= stickerSheet.size()) {
        return false;
    }
    stickerSheet[index].x = x;
    stickerSheet[index].y = y;
    return true;
}

void StickerSheet::removeSticker(unsigned index) {
    if (index >= stickerSheet.size()) {
        return;
    }
    stickerSheet.erase(stickerSheet.begin() + index);
}

Image* StickerSheet::getSticker(unsigned index) {
    if (index >= stickerSheet.size()) {
        return nullptr;
    }
    return &stickerSheet[index].image;
}

Image StickerSheet::render() const {
    Image renderedImage = Image(basePicture);
    for (unsigned i = 0; i < stickerSheet.size(); i++) {
        stamp(renderedImage, stickerSheet[i]);
    }
    return renderedImage;
}

void StickerSheet::stamp(Image& baseImage, Sticker sticker) const {
    Image* image = &sticker.image;
    unsigned x = sticker.x;
    unsigned y = sticker.y;
    unsigned width = image->width();
    unsigned height = image->height();
    if (x + width > baseImage.width()) {
        baseImage.resize(x + width, baseImage.height());
    }
    if (y + height > baseImage.height()) {
        baseImage.resize(baseImage.width(), y + height);
    }
    for (unsigned row = 0; row < height; row++) {
        for (unsigned col = 0; col < width; col++) {
            cs225::HSLAPixel& stickerPixel = image->getPixel(col, row);
            cs225::HSLAPixel& basePixel = baseImage.getPixel(x + col, y + row);
            if (stickerPixel.a != 0) {
                basePixel.h = stickerPixel.a * stickerPixel.h + (1 - stickerPixel.a) * basePixel.h;
                basePixel.l = stickerPixel.a * stickerPixel.l + (1 - stickerPixel.a) * basePixel.l;
                basePixel.s = stickerPixel.a * stickerPixel.s + (1 - stickerPixel.a) * basePixel.s;
            }
        }
    }
}