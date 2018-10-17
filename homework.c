#include "homework.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

int num_threads;
int resize_factor;

void readInput(const char *fileName, image *img) {
  FILE *in = fopen(fileName, "rb");
  if (in == NULL) {
    fprintf(stderr, "Could not open %s!\n", fileName);
    return;
  }

  char buf[10];
  fscanf(in, "%2s", buf);

  if (strcmp(buf, "P5") == 0) {
    img->type = 5;
  } else if (strcmp(buf, "P6") == 0) {
    img->type = 6;
  } else {
    fprintf(stderr, "Invalid file type!\n");
    return;
  }

  fscanf(in, "%10s", buf);
  img->width = (int) strtol(buf, NULL, 10);
  fscanf(in, "%10s", buf);
  img->height = (int) strtol(buf, NULL, 10);
  fscanf(in, "%10s", buf);
  img->maxval = (unsigned char) strtol(buf, NULL, 10);

  fseek(in, 1, SEEK_CUR);  // skip whitespace

  img->image = (unsigned char **) malloc(
      (size_t) img->height * sizeof(unsigned char **));
  int real_width = img->type == 5 ? img->width : img->width * 3;

  for (int i = 0; i < img->height; i++) {
    img->image[i] =
        (unsigned char *) malloc((size_t) real_width * sizeof(unsigned char *));
    fread(img->image[i], sizeof(char), (size_t) real_width, in);
  }

  fclose(in);
}

void writeData(const char *fileName, image *img) {
  FILE *out = fopen(fileName, "wb");
  if (out == NULL) {
    fprintf(stderr, "Could not open %s!\n", fileName);
    return;
  }

  if (img->type == 5) {
    fprintf(out, "P5\n");
  } else if (img->type == 6) {
    fprintf(out, "P6\n");
  } else {
    fprintf(stderr, "Invalid image type %d!\n", img->type);
    return;
  }

  fprintf(out, "%d ", img->width);
  fprintf(out, "%d\n", img->height);
  fprintf(out, "%d\n", img->maxval);

  int real_width = img->type == 5 ? img->width : img->width * 3;

  for (int i = 0; i < img->height; i++) {
    fwrite(img->image[i], sizeof(char), (size_t) real_width, out);
  }

  fclose(out);
}

void resize(image *in, image *out) {
  out->type = in->type;
  out->height = in->height;
  out->width = in->width;
  out->maxval = in->maxval;
  out->image = in->image;
}