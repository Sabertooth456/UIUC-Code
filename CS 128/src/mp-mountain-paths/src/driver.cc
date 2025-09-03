#include <iostream>
#include <vector>

#include "color.hpp"
#include "elevation_dataset.hpp"
#include "grayscale_image.hpp"
#include "path.hpp"
#include "path_image.hpp"

int main() {
  ElevationDataset dataset{
      "/home/vagrant/src/mp-mountain-paths-Yiphen-Ori/example-data/"
      "ex_input_data/map-input-w51-h55.dat",
      51, 55};
  GrayscaleImage gray_image{dataset};
  PathImage path_image{gray_image, dataset};
}