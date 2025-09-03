#include "elevation_dataset.hpp"

#include <fstream>

ElevationDataset::ElevationDataset(const std::string& filename, size_t width,
                                   size_t height) {
  width_ = width;
  height_ = height;
  data_ = std::vector<std::vector<int>>(height_);
  size_t expected_datacount = width_ * height_;
  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    throw std::runtime_error("File is unable to be opened");
  }
  int value = 0;
  bool first_time = true;
  size_t index = 0;
  while (ifs.good()) {
    ifs >> value;
    if (ifs.fail()) {
      break;
    } else {
      if (index == expected_datacount) {
        index++;
        break;
      }
      if (first_time) {
        max_ele_ = value;
        min_ele_ = value;
        first_time = false;
      } else if (value > max_ele_) {
        max_ele_ = value;
      } else if (value < min_ele_) {
        min_ele_ = value;
      }
      data_[index / width].push_back(value);
      index++;
    }
  }
  if (index != expected_datacount) {
    throw std::runtime_error(
        "File contained the wrong number of datapoints for its given size");
  }
}

size_t ElevationDataset::Width() const { return width_; }
size_t ElevationDataset::Height() const { return height_; }
int ElevationDataset::MaxEle() const { return max_ele_; }
int ElevationDataset::MinEle() const { return min_ele_; }
int ElevationDataset::DatamAt(size_t row, size_t col) const {
  return data_[row][col];
}
const std::vector<std::vector<int>>& ElevationDataset::GetData() const {
  return data_;
}  // Returns const reference to data_