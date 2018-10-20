#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "homework1.h"

int num_threads;
int resolution;

void initialize(image *im) {
  im->width = resolution;
  im->height = resolution;
  im->maxval = 255;
  im->image =
      (unsigned char **) malloc((size_t) im->height * sizeof(unsigned char **));

  for (int i = 0; i < im->height; i++)
    im->image[i] =
        (unsigned char *) malloc((size_t) im->width * sizeof(unsigned char *));
}

/*
 * Convert from number of pixels to number of centimeters based on the resolution.
 *
 * resolution px......................IMG_SIZE cm
 *     pixels px......................       ? cm
 */
double pixelsToCm(double pixels) {
  return (pixels * IMG_SIZE) / resolution;
}

double getDistance(double x, double y) {
  return abs(A * x + B * y + C) / (sqrt(pow(A, 2) + pow(B, 2)));
}

void render(image *im) {
  double x, y;

  for (int i = 0; i < im->height; i++) {
    for (int j = 0; j < im->width; j++) {
      // Calculate coordinates to center of pixel
      x = pixelsToCm(j) + ((float) IMG_SIZE / resolution) / 2;
      y = pixelsToCm(im->height - i - 1) + ((float) IMG_SIZE / resolution) / 2;
      im->image[i][j] =
          (getDistance(x, y) <= LINE_WIDTH ? LINE_COLOR : BACKGROUND_COLOR);
    }
  }
}

void writeData(const char *fileName, image *img) {
  FILE *out = fopen(fileName, "wb");
  if (out == NULL) {
    fprintf(stderr, "Could not open %s!\n", fileName);
    return;
  }

  fprintf(out, "P5\n");
  fprintf(out, "%d ", img->width);
  fprintf(out, "%d\n", img->height);
  fprintf(out, "%d\n", img->maxval);

  for (int i = 0; i < img->height; i++) {
    fwrite(img->image[i], sizeof(char), (size_t) img->width, out);
  }

  fclose(out);
}

