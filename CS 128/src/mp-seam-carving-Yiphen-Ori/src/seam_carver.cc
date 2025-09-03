#include "seam_carver.hpp"

// implement the rest of SeamCarver's functions here

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image) : image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}

const ImagePPM& SeamCarver::GetImage() const { return image_; }
int SeamCarver::GetHeight() const { return height_; }
int SeamCarver::GetWidth() const { return width_; }
int SeamCarver::GetEnergy(int row, int col) const {
  Pixel left_pixel =
      image_.GetPixel(row % height_, (col + width_ - 1) % width_);
  Pixel right_pixel =
      image_.GetPixel(row % height_, (col + width_ + 1) % width_);
  Pixel upper_pixel =
      image_.GetPixel((row + height_ - 1) % height_, col % width_);
  Pixel lower_pixel =
      image_.GetPixel((row + height_ + 1) % height_, col % width_);
  int total_energy =
      GetEnergy(left_pixel, right_pixel) + GetEnergy(upper_pixel, lower_pixel);
  return total_energy;
}
int SeamCarver::GetEnergy(Pixel a, Pixel b) const {
  int red_diff = a.GetRed() - b.GetRed();
  int green_diff = a.GetGreen() - b.GetGreen();
  int blue_diff = a.GetBlue() - b.GetBlue();
  int energy =
      red_diff * red_diff + green_diff * green_diff + blue_diff * blue_diff;
  return energy;
}
bool SeamCarver::ValidCoordinate(int row, int col) const {
  return (row >= 0 && row < height_ && col >= 0 && col < width_);
}
int SeamCarver::BestHorizontalEnergy(int row, int col) const {
  if (col == width_ - 1) {
    return -1;
  }
  int best_row = row;
  int best_energy = GetEnergy(row, col + 1);
  if (ValidCoordinate(row - 1, col + 1)) {
    int tested_energy = GetEnergy(row - 1, col + 1);
    if (best_energy > tested_energy) {
      best_row = row - 1;
      best_energy = tested_energy;
    }
  }
  if (ValidCoordinate(row + 1, col + 1)) {
    int tested_energy = GetEnergy(row + 1, col + 1);
    if (best_energy > tested_energy) {
      best_row = row + 1;
    }
  }
  return best_row;
}
int SeamCarver::BestHorizontalEnergy(int row, int col,
                                     int** best_energy_path) const {
  if (col == width_ - 1) {
    return -1;
  }
  int best_row = row;
  int best_energy = best_energy_path[row][col + 1];
  if (ValidCoordinate(row - 1, col + 1)) {
    int tested_energy = best_energy_path[row - 1][col + 1];
    if (best_energy > tested_energy) {
      best_row = row - 1;
      best_energy = tested_energy;
    }
  }
  if (ValidCoordinate(row + 1, col + 1)) {
    int tested_energy = best_energy_path[row + 1][col + 1];
    if (best_energy > tested_energy) {
      best_row = row + 1;
    }
  }
  return best_row;
}
int SeamCarver::BestVerticalEnergy(int row, int col) const {
  if (row == height_ - 1) {
    return -1;
  }
  int best_col = col;
  int best_energy = GetEnergy(row + 1, col);
  if (ValidCoordinate(row + 1, col - 1)) {
    int tested_energy = GetEnergy(row + 1, col - 1);
    if (best_energy > tested_energy) {
      best_col = col - 1;
      best_energy = tested_energy;
    }
  }
  if (ValidCoordinate(row + 1, col + 1)) {
    int tested_energy = GetEnergy(row + 1, col + 1);
    if (best_energy > tested_energy) {
      best_col = col + 1;
    }
  }
  return best_col;
}
int SeamCarver::BestVerticalEnergy(int row, int col,
                                   int** best_energy_path) const {
  if (row == height_ - 1) {
    return -1;
  }
  int best_col = col;
  int best_energy = best_energy_path[row + 1][col];
  if (ValidCoordinate(row + 1, col - 1)) {
    int tested_energy = best_energy_path[row + 1][col - 1];
    if (best_energy > tested_energy) {
      best_col = col - 1;
      best_energy = tested_energy;
    }
  }
  if (ValidCoordinate(row + 1, col + 1)) {
    int tested_energy = best_energy_path[row + 1][col + 1];
    if (best_energy > tested_energy) {
      best_col = col + 1;
    }
  }
  return best_col;
}
void SeamCarver::GetHorizontalEnergy(int**& best_energy_path) const {
  for (int col = width_ - 1; col >= 0; col--) {
    for (int row = 0; row < height_; row++) {
      int cur_energy = GetEnergy(row, col);
      int best_row = BestHorizontalEnergy(row, col);
      if (best_row == -1) {
        best_energy_path[row][col] = cur_energy;
      } else {
        best_energy_path[row][col] =
            cur_energy + best_energy_path[best_row][col + 1];
      }
    }
  }
}
int* SeamCarver::GetHorizontalSeam() const {
  int** best_energy_path = new int*[height_];
  for (int row = 0; row < height_; row++) {
    best_energy_path[row] = new int[width_]{0};
  }
  GetHorizontalEnergy(best_energy_path);
  for (int col = width_ - 1; col >= 0; col--) {
    for (int row = 0; row < height_; row++) {
      int cur_energy = GetEnergy(row, col);
      int best_row = BestHorizontalEnergy(row, col, best_energy_path);
      if (best_row == -1) {
        best_energy_path[row][col] = cur_energy;
      } else {
        best_energy_path[row][col] =
            cur_energy + best_energy_path[best_row][col + 1];
      }
    }
  }
  int* best_path = new int[width_];
  int best_row = 0;
  int best_value = best_energy_path[0][0];
  // Get the starting row
  for (int row = 1; row < height_; row++) {
    if (best_energy_path[row][0] < best_value) {
      best_row = row;
      best_value = best_energy_path[row][0];
    }
  }
  best_path[0] = best_row;
  // Find the path of least energy
  for (int col = 1; col < width_; col++) {
    int cur_row = best_row;
    best_row = BestHorizontalEnergy(cur_row, col - 1, best_energy_path);
    best_path[col] = best_row;
  }
  for (int row = 0; row < height_; row++) {
    delete[] best_energy_path[row];
  }
  delete[] best_energy_path;
  return best_path;
}

void SeamCarver::GetVerticalEnergy(int**& best_energy_path) const {
  for (int row = height_ - 1; row >= 0; row--) {
    for (int col = 0; col < width_; col++) {
      int cur_energy = GetEnergy(row, col);
      int best_col = BestVerticalEnergy(row, col, best_energy_path);
      if (best_col == -1) {
        best_energy_path[row][col] = cur_energy;
      } else {
        best_energy_path[row][col] =
            cur_energy + best_energy_path[row + 1][best_col];
      }
    }
  }
}

int* SeamCarver::GetVerticalSeam() const {
  int** best_energy_path = new int*[height_];
  for (int row = 0; row < height_; row++) {
    best_energy_path[row] = new int[width_]{0};
  }
  for (int row = height_ - 1; row >= 0; row--) {
    for (int col = 0; col < width_; col++) {
      int cur_energy = GetEnergy(row, col);
      int best_col = BestVerticalEnergy(row, col);
      if (best_col == -1) {
        best_energy_path[row][col] = cur_energy;
      } else {
        best_energy_path[row][col] =
            cur_energy + best_energy_path[row + 1][best_col];
      }
    }
  }
  GetVerticalEnergy(best_energy_path);
  int* best_path = new int[height_];
  int best_col = 0;
  int best_value = best_energy_path[0][0];
  // Get the starting row
  for (int col = 1; col < width_; col++) {
    if (best_energy_path[0][col] < best_value) {
      best_col = col;
      best_value = best_energy_path[0][col];
    }
  }
  best_path[0] = best_col;
  // Find the path of least energy
  for (int row = 1; row < height_; row++) {
    int cur_col = best_col;
    best_col = BestVerticalEnergy(row - 1, cur_col, best_energy_path);
    best_path[row] = best_col;
  }
  for (int row = 0; row < height_; row++) {
    delete[] best_energy_path[row];
  }
  delete[] best_energy_path;
  return best_path;
}
void SeamCarver::RemoveHorizontalSeam() {
  int* horizontal_seam = GetHorizontalSeam();
  ImagePPM new_image = ImagePPM(width_, height_ - 1, image_.GetMaxColorValue());
  for (int row = 0; row < height_ - 1; row++) {
    for (int col = 0; col < width_; col++) {
      int adjusted_row = row;
      if (row >= horizontal_seam[col]) {
        adjusted_row++;
      }
      Pixel image_pixel = image_.GetPixel(adjusted_row, col);
      new_image.SetPixel(row, col,
                         Pixel(image_pixel.GetRed(), image_pixel.GetGreen(),
                               image_pixel.GetBlue()));
    }
  }
  image_.~ImagePPM();
  image_ = new_image;
  height_--;
  delete[] horizontal_seam;
}
void SeamCarver::RemoveVerticalSeam() {
  int* vertical_seam = GetVerticalSeam();
  ImagePPM new_image = ImagePPM(width_ - 1, height_, image_.GetMaxColorValue());
  for (int row = 0; row < height_; row++) {
    for (int col = 0; col < width_ - 1; col++) {
      int adjusted_col = col;
      if (col >= vertical_seam[row]) {
        adjusted_col++;
      }
      Pixel image_pixel = image_.GetPixel(row, adjusted_col);
      new_image.SetPixel(row, col,
                         Pixel(image_pixel.GetRed(), image_pixel.GetGreen(),
                               image_pixel.GetBlue()));
    }
  }
  image_.~ImagePPM();
  image_ = new_image;
  width_--;
  delete[] vertical_seam;
}

// std::string SeamCarver::PrintImage() {
//   std::string output = "";
//   for (int row = 0; row < height_; row++) {
//     for (int col = 0; col < width_; col++) {
//       output += std::to_string(GetEnergy(row, col)) + " ";
//     }
//     output += "\n";
//   }
//   output += "\n";
//   int** best_energy_path = new int*[height_];
//   for (int row = 0; row < height_; row++) {
//     best_energy_path[row] = new int[width_]{0};
//   }
//   for (int row = height_ - 1; row >= 0; row--) {
//     for (int col = 0; col < width_; col++) {
//       int cur_energy = GetEnergy(row, col);
//       int best_col = BestVerticalEnergy(row, col, best_energy_path);
//       if (best_col == -1) {
//         best_energy_path[row][col] = cur_energy;
//       } else {
//         best_energy_path[row][col] =
//             cur_energy + best_energy_path[row + 1][best_col];
//       }
//     }
//   }
//   for (int row = 0; row < height_; row++) {
//     for (int col = 0; col < width_; col++) {
//       output += std::to_string(best_energy_path[row][col]) + " ";
//     }
//     output += "\n";
//   }
//   std::cout << "\n";
//   int* path = GetVerticalSeam();
//   for (int row = 0; row < height_; row++) {
//     std::cout << path[row] << " ";
//   }
//   std::cout << std::endl;
//   for (int row = 0; row < height_; row++) {
//     delete[] best_energy_path[row];
//   }
//   delete[] best_energy_path;
//   delete[] path;
//   return output;
// }