#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
  /*
   * 5 -> image is PGM
   * 6 -> image is PPM
   */
  int type;
  int width, height;
  unsigned char maxval;
  unsigned char **image;
} image;

void readInput(const char *fileName, image *img);

void writeData(const char *fileName, image *img);

#endif /* HOMEWORK_H */