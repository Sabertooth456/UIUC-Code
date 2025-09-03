#include "grayscale_image.hpp"

#include <cmath>
#include <fstream>
#include <vector>

#include "color.hpp"
#include "elevation_dataset.hpp"

GrayscaleImage::GrayscaleImage(const ElevationDataset& dataset) {
  height_ = dataset.Height();
  width_ = dataset.Width();
  int min_ele = dataset.MinEle();
  int max_ele = dataset.MaxEle();
  image_ = std::vector<std::vector<Color>>(height_);
  std::vector<std::vector<int>> data = dataset.GetData();
  for (size_t y = 0; y < height_; y++) {
    for (size_t x = 0; x < width_; x++) {
      int shade_of_gray = 0;
      if (max_ele - min_ele != 0) {
        int elevation = data[y][x];
        shade_of_gray = std::round(static_cast<double>(elevation - min_ele) /
                                   (max_ele - min_ele) * kMaxColorValue);
      }
      image_[y].push_back(Color(shade_of_gray, shade_of_gray, shade_of_gray));
    }
  }
}
GrayscaleImage::GrayscaleImage(const std::string& filename, size_t width,
                               size_t height)
    : GrayscaleImage(ElevationDataset(filename, width, height)) {}
size_t GrayscaleImage::Width() const { return width_; }
size_t GrayscaleImage::Height() const { return height_; }
unsigned int GrayscaleImage::MaxColorValue() const { return kMaxColorValue; }
const Color& GrayscaleImage::ColorAt(int row, int col) const {
  return image_[row][col];
}
const std::vector<std::vector<Color>>& GrayscaleImage::GetImage() const {
  return image_;
}
void GrayscaleImage::ToPpm(const std::string& name) const {
  std::ofstream ofs(name);
  ofs << "P3" << std::endl;
  ofs << width_ << " " << height_ << std::endl;
  ofs << kMaxColorValue << std::endl;
  for (size_t y = 0; y < height_; y++) {
    for (size_t x = 0; x < width_; x++) {
      ofs << image_[y][x].Red() << " " << image_[y][x].Green() << " "
          << image_[y][x].Blue();
      if (x < width_ - 1) {
        ofs << " ";
      }
    }
    ofs << std::endl;
  }
}