#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
  /*
   * 5 -> image is PPM
   * 6 -> image is PGM
   */
  int type;
  int width, height;
  unsigned char maxval;
  unsigned char **image;
} image;

void readInput(const char *fileName, image *img);

void writeData(const char *fileName, image *img);

void resize(image *in, image *out);

#endif /* HOMEWORK_H */