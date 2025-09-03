#include "path_image.hpp"

const Color kRed = Color(252, 25, 63);
const Color kGreen = Color(31, 253, 13);

PathImage::PathImage(const GrayscaleImage& image,
                     const ElevationDataset& dataset)
    : width_(dataset.Width()), height_(dataset.Height()) {
  path_image_ = image.GetImage();
  for (size_t y = 0; y < height_; y++) {
    paths_.push_back(GreedyWalk(y, dataset));
  }
  size_t best_starting_row = 0;
  bool initialized = false;
  size_t best_change = 0;
  for (Path path : paths_) {
    if (path.EleChange() < best_change || !initialized) {
      initialized = true;
      best_change = path.EleChange();
      best_starting_row = path.StartingRow();
    }
    std::vector<size_t> trail = path.GetPath();
    for (size_t i = 0; i < width_; i++) {
      path_image_[trail[i]][i] = kRed;
    }
  }
  std::vector<size_t> trail = paths_[best_starting_row].GetPath();
  for (size_t i = 0; i < width_; i++) {
    path_image_[trail[i]][i] = kGreen;
  }
}
Path PathImage::GreedyWalk(size_t starting_row,
                           const ElevationDataset& dataset) {
  Path path{width_, starting_row};
  size_t cur_row = starting_row;
  std::vector<std::vector<int>> data = dataset.GetData();
  for (size_t col = 0; col < width_ - 1; col++) {
    path.SetLoc(col, cur_row);
    bool initialized = false;
    int min_change = 0;
    int best_row_change = 0;
    if (LocationInBounds(col + 1, cur_row)) {
      int change = abs(data[cur_row][col] - data[cur_row][col + 1]);
      min_change = change;
      best_row_change = 0;
      initialized = true;
    }
    if (LocationInBounds(col + 1, cur_row + 1)) {
      int change = abs(data[cur_row][col] - data[cur_row + 1][col + 1]);
      if (change < min_change || !initialized) {
        min_change = change;
        best_row_change = 1;
        initialized = true;
      }
    }
    if (LocationInBounds(col + 1, cur_row - 1)) {
      int change = abs(data[cur_row][col] - data[cur_row - 1][col + 1]);
      if (change < min_change || !initialized) {
        min_change = change;
        best_row_change = -1;
      }
    }
    path.IncEleChange(min_change);
    cur_row += best_row_change;
  }
  path.SetLoc(width_ - 1, cur_row);

  return path;
}
bool PathImage::LocationInBounds(size_t col, size_t row) const {
  return (row < height_ && col < width_);
}

size_t PathImage::Width() const { return width_; }
size_t PathImage::Height() const { return height_; }
unsigned int PathImage::MaxColorValue() const { return kMaxColorValue; }
const std::vector<Path>& PathImage::Paths() const { return paths_; }
const std::vector<std::vector<Color>>& PathImage::GetPathImage() const {
  return path_image_;
}
void PathImage::ToPpm(const std::string& name) const {
  std::ofstream ofs(name);
  ofs << "P3" << std::endl;
  ofs << width_ << " " << height_ << std::endl;
  ofs << kMaxColorValue << std::endl;
  for (size_t y = 0; y < height_; y++) {
    for (size_t x = 0; x < width_; x++) {
      ofs << path_image_[y][x].Red() << " " << path_image_[y][x].Green() << " "
          << path_image_[y][x].Blue();
      if (x < width_ - 1) {
        ofs << " ";
      }
    }
    ofs << std::endl;
  }
}
