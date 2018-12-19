#ifndef HOMEWORK_H
#define HOMEWORK_H

#include <string.h>

#define DEFAULT_TAG 0

typedef struct {
  /*
   * 5 -> image_t is PGM
   * 6 -> image_t is PPM
   */
  int type;
  int width, height;
  unsigned char maxval;
  unsigned char **image;
} image_t;

typedef struct {
  const int kernel[3][3];
  const int weight;
  const char *name;
} filter_t;

const filter_t IDENTITY_FILTER = {.kernel = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}},
    .weight = 1,
    .name = "identity"};
const filter_t SMOOTH_FILTER = {.kernel = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}},
    .weight = 9,
    .name = "smooth"};
const filter_t BLUR_FILTER = {.kernel = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}},
    .weight = 16,
    .name = "blur"};
const filter_t SHARPEN_FILTER = {.kernel = {{0, -2, 0}, {-2, 11, -2}, {0, -2, 0}},
    .weight = 3,
    .name = "sharpen"};
const filter_t MEAN_FILTER = {.kernel = {{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}},
    .weight = 1,
    .name = "mean"};
const filter_t EMBOSS_FILTER = {.kernel = {{0, 1, 0}, {0, 0, 0}, {0, -1, 0}},
    .weight = 1,
    .name = "emboss"};

const filter_t getFilter(char name[]) {
  if (strcmp(name, SMOOTH_FILTER.name) == 0)
    return SMOOTH_FILTER;
  if (strcmp(name, BLUR_FILTER.name) == 0)
    return BLUR_FILTER;
  if (strcmp(name, SHARPEN_FILTER.name) == 0)
    return SHARPEN_FILTER;
  if (strcmp(name, MEAN_FILTER.name) == 0)
    return MEAN_FILTER;
  if (strcmp(name, EMBOSS_FILTER.name) == 0)
    return EMBOSS_FILTER;

  // Not a valid filter_t name -> identity
  return IDENTITY_FILTER;
}

void readInput(const char *fileName, image_t *img);

void writeData(const char *fileName, image_t *img);

void sendImage(image_t *img, int receiver);

image_t *recvImage(int sender);

#endif /* HOMEWORK_H */