/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>

#include "maptiles.h"

using namespace std;


Point<3> convertToXYZ(LUVAPixel pixel) {
    return Point<3>( pixel.l, pixel.u, pixel.v );
}

MosaicCanvas* mapTiles(SourceImage const& theSource,
                       vector<TileImage>& theTiles)
{
    /**
     * @todo Implement this function!
     */
    MosaicCanvas* canvas = new MosaicCanvas(theSource.getRows(), theSource.getColumns());
    vector<Point<3>> tileset(theTiles.size());
    std::map<Point<3>, int> aveToTileIndex;
    Point<3> average = Point<3>();
    for (size_t i = 0; i < theTiles.size(); ++i) {
        average = convertToXYZ(theTiles[i].getAverageColor());
        tileset[i] = average;
        aveToTileIndex[average] = i;
    }
    KDTree<3> tileTree(tileset);

    for (int row = 0; row < theSource.getRows(); ++row) {
        for (int col = 0; col < theSource.getColumns(); ++col) {
            Point<3> sourceAve = convertToXYZ(theSource.getRegionColor(row, col));
            Point<3> approxTile = tileTree.findNearestNeighbor(sourceAve);
            canvas->setTile(row, col, &theTiles[aveToTileIndex[approxTile]]);
        }
    }
    return canvas;
}

