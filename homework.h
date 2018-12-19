#ifndef HOMEWORK_H
#define HOMEWORK_H

#define DEFAULT_TAG 0

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

void sendImage(image *img, int receiver);

image *recvImage(int sender);

#endif /* HOMEWORK_H */