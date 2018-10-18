#ifndef HOMEWORK_H1
#define HOMEWORK_H1

#define A -1
#define B 2
#define C 0

#define IMG_SIZE 100
#define BACKGROUND_COLOR 255
#define LINE_COLOR 0
#define LINE_WIDTH 3

typedef struct {
  int width, height;
  unsigned char maxval;
  unsigned char **image;
} image;

void initialize(image *im);
void render(image *im);
void writeData(const char *fileName, image *img);

#endif /* HOMEWORK_H1 */