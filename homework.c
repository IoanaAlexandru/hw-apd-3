#include "homework.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

/*
 * Allocate matrix of pixels in image structure.
 * Returns 0 after a successful allocation, -1 otherwise.
 */
int allocImage(image *img) {
  img->image = (unsigned char **) malloc(
      (size_t) img->height * sizeof(unsigned char **));
  if (img->image == NULL)
    return -1;

  int real_width = img->type == 5 ? img->width : img->width * 3;

  for (int i = 0; i < img->height; i++) {
    img->image[i] =
        (unsigned char *) malloc((size_t) real_width * sizeof(unsigned char *));
    if (img->image[i] == NULL) {
      for (int j = 0; j < i; j++)
        free(img->image[j]);
      return -1;
    }
  }

  return 0;
}

/*
 * Free matrix of pixels in image structure.
 */
void freeImage(image *img) {
  for (int i = 0; i < img->height; i++)
    free(img->image[i]);
  free(img->image);
}

/*
 * Read data from file <fileName> and save it in <img>.
 */
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

  if (allocImage(img) != 0) {
    fprintf(stderr, "Cannot allocate memory!\n");
  } else {
    int real_width = img->type == 5 ? img->width : img->width * 3;

    for (int i = 0; i < img->height; i++)
      fread(img->image[i], sizeof(char), (size_t) real_width, in);
  }

  fclose(in);
}

/*
 * Write data from <img> to file <fileName> and free <img>.
 */
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

  int i, real_width = img->type == 5 ? img->width : img->width * 3;

  for (i = 0; i < img->height; i++) {
    fwrite(img->image[i], sizeof(char), (size_t) real_width, out);
  }

  fclose(out);
  freeImage(img);
}

int main(int argc, char *argv[]) {
  // argv[1]  input
  // argv[2]  output
  // argv[3]+ filter
  if (argc < 4) {
    printf(
        "Usage: mpirun -np N ./%s image_in.pnm image_out.pnm filter1 filter2 ... filterX\n",
        argv[0]);
    exit(-1);
  }

  image input;
  image output;

  readInput(argv[1], &input);

  // TODO apply filters
  output = input;

  writeData(argv[2], &output);

  return 0;
}