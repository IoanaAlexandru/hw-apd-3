#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include "homework.h"

/*
 * Allocate matrix of pixels in image structure.
 * Returns 0 after a successful allocation, -1 otherwise.
 */
int allocImage(image_t *img) {
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
      free(img->image);
      return -1;
    }
  }

  return 0;
}

/*
 * Free matrix of pixels in image structure.
 */
void freeImage(image_t *img) {
  for (int i = 0; i < img->height; i++)
    free(img->image[i]);
  free(img->image);
}

/*
 * Read data from file <fileName> and save it in <img>.
 */
void readInput(const char *fileName, image_t *img) {
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
void writeData(const char *fileName, image_t *img) {
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

void sendImage(image_t *img, int receiver) {
  MPI_Send(&(img->type), 1, MPI_INT, receiver, DEFAULT_TAG, MPI_COMM_WORLD);
  MPI_Send(&(img->width), 1, MPI_INT, receiver, DEFAULT_TAG, MPI_COMM_WORLD);
  MPI_Send(&(img->height), 1, MPI_INT, receiver, DEFAULT_TAG, MPI_COMM_WORLD);
  MPI_Send(&(img->maxval),
           1,
           MPI_UNSIGNED_CHAR,
           receiver,
           DEFAULT_TAG,
           MPI_COMM_WORLD);

  int real_width = img->type == 5 ? img->width : img->width * 3;
  for (int i = 0; i < img->height; i++) {
    MPI_Send(img->image[i],
             real_width,
             MPI_UNSIGNED_CHAR,
             receiver,
             DEFAULT_TAG,
             MPI_COMM_WORLD);
  }
}

image_t *recvImage(int sender) {
  image_t *img = (image_t *) malloc(sizeof(image_t));

  MPI_Recv(&(img->type),
           1,
           MPI_INT,
           sender,
           DEFAULT_TAG,
           MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);
  MPI_Recv(&(img->width),
           1,
           MPI_INT,
           sender,
           DEFAULT_TAG,
           MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);
  MPI_Recv(&(img->height),
           1,
           MPI_INT,
           sender,
           DEFAULT_TAG,
           MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);
  MPI_Recv(&(img->maxval),
           1,
           MPI_UNSIGNED_CHAR,
           sender,
           DEFAULT_TAG,
           MPI_COMM_WORLD,
           MPI_STATUS_IGNORE);

  allocImage(img);

  int real_width = img->type == 5 ? img->width : img->width * 3;
  for (int i = 0; i < img->height; i++) {
    MPI_Recv(img->image[i],
             real_width,
             MPI_UNSIGNED_CHAR,
             sender,
             DEFAULT_TAG,
             MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
  }

  return img;
}

int multiplyMatrices(const int m1[3][3], unsigned char m2[3][3]) {
  int result = 0;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      result += m1[i][j] * m2[i][j];
  return result;
}

void applyFilter(image_t *img, filter_t filter) {
  image_t result = *img;
  allocImage(&result);

  int step = img->type == 5 ? 1 : 3;
  int real_width = img->width * step;

  for (int i = 0; i < img->height; i++) {
    for (int j = 0; j < real_width - step; j++) {
      if (i == 0 || j < step || i == img->height - 1
          || j > real_width - 2 * (step - 1)) {
        result.image[i][j] = img->image[i][j];
      } else {
        unsigned char **p = img->image;
        unsigned char pixels[3][3] =
            {{p[i - 1][j - step], p[i - 1][j], p[i - 1][j + step]},
             {p[i][j - step], p[i][j], p[i][j + step]},
             {p[i + 1][j - step], p[i + 1][j], p[i + 1][j + step]}};
        result.image[i][j] =
            (unsigned char) (multiplyMatrices(filter.kernel, pixels)
                / filter.weight);
      }
    }
  }

  for (int i = 0; i < img->height; i++)
    for (int j = 0; j < real_width - step; j++)
      img->image[i][j] = result.image[i][j];

  freeImage(&result);
}