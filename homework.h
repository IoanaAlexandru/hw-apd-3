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

typedef struct {
  image *in;
  image *out;
  int thread_id;
} thread_func_args;

void readInput(const char *fileName, image *img);

void writeData(const char *fileName, image *img);

void resize(image *in, image *out);

#endif /* HOMEWORK_H */