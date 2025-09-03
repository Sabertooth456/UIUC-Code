#include "Image.h"
#include "StickerSheet.h"

#include <iostream>

int main() {
  //
  // Reminder:
  //   Before exiting main, save your creation to disk as myImage.png
  //

  Image alma;
  alma.readFromFile("../alma.png");
  StickerSheet stickerSheet = StickerSheet(alma, 4);

  Image illiniLogo;
  illiniLogo.readFromFile("../i.png");
  illiniLogo.scale(0.25);

  stickerSheet.addSticker(illiniLogo, 10, 10);
  
  Image sparkle;
  sparkle.readFromFile("../sparkle.png");
  sparkle.scale(0.66);

  stickerSheet.addSticker(sparkle, 35, 500);

  Image stare;
  stare.readFromFile("../stare.png");
  stare.scale(0.27);
  stare.desaturate(0.5);
  stare.rotateColor(240);

  stickerSheet.addSticker(stare, 431, 42);

  Image walker;
  walker.readFromFile("../walker.png");
  walker.scale(0.23);

  stickerSheet.addSticker(walker, 60, 290);

  stickerSheet.render().writeToFile("../myImage.png");

  return 0;
}
