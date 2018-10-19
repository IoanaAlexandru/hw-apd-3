#include "homework.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

int num_threads;
int resize_factor;

void alloc_image(image *img) {
  img->image = (unsigned char **) malloc(
      (size_t) img->height * sizeof(unsigned char **));

  int real_width = img->type == 5 ? img->width : img->width * 3;

  for (int i = 0; i < img->height; i++)
    img->image[i] =
        (unsigned char *) malloc((size_t) real_width * sizeof(unsigned char *));
}

void* resize_bw(void *args) {
  thread_func_args *imgs = (thread_func_args *) args;
  image *in = imgs->in, *out = imgs->out;
  int thread_id = imgs->thread_id;

  int i1, j1, i2, j2, sum;
  int gaussian_kernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
  int count = resize_factor == 3 ? 16 : resize_factor * resize_factor;

  int start = thread_id * (out->height) / num_threads,
      end = (thread_id + 1) * (out->height) / num_threads;

  for (i1 = start; i1 < end; i1++) {
    for (j1 = 0; j1 < out->width; j1++) {
      sum = 0;
      for (i2 = i1 * resize_factor; i2 < (i1 + 1) * resize_factor; i2++) {
        for (j2 = j1 * resize_factor; j2 < (j1 + 1) * resize_factor; j2++) {
          if (resize_factor == 3)
            sum += in->image[i2][j2] * gaussian_kernel[i2 % 3][j2 % 3];
          else
            sum += in->image[i2][j2];
        }
      }
      out->image[i1][j1] = (unsigned char) (sum / count);
    }
  }

  return 0;
}

void* resize_color(void *args) {
  thread_func_args *imgs = (thread_func_args *) args;
  image *in = imgs->in, *out = imgs->out;
  int thread_id = imgs->thread_id;

  int i1, j1, i2, j2, sum_red, sum_green, sum_blue;
  int gaussian_kernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
  int count = resize_factor == 3 ? 16 : resize_factor * resize_factor;

  int start = thread_id * (out->height) / num_threads,
      end = (thread_id + 1) * (out->height) / num_threads;

  for (i1 = start; i1 < end; i1++) {
    for (j1 = 0; j1 < out->width * 3; j1 += 3) {
      sum_red = 0;
      sum_green = 0;
      sum_blue = 0;
      for (i2 = i1 * resize_factor; i2 < (i1 + 1) * resize_factor; i2++) {
        for (j2 = j1 * resize_factor; j2 < (j1 + 3) * resize_factor;
             j2 += 3) {
          if (resize_factor == 3) {
            sum_red += in->image[i2][j2] * gaussian_kernel[i2 % 3][j2 / 3 % 3];
            sum_green +=
                in->image[i2][j2 + 1] * gaussian_kernel[i2 % 3][j2 / 3 % 3];
            sum_blue +=
                in->image[i2][j2 + 2] * gaussian_kernel[i2 % 3][j2 / 3 % 3];
          } else {
            sum_red += in->image[i2][j2];
            sum_green += in->image[i2][j2 + 1];
            sum_blue += in->image[i2][j2 + 2];
          }
        }
      }
      out->image[i1][j1] = (unsigned char) (sum_red / count);
      out->image[i1][j1 + 1] = (unsigned char) (sum_green / count);
      out->image[i1][j1 + 2] = (unsigned char) (sum_blue / count);
    }
  }

  return 0;
}

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

  alloc_image(img);
  int real_width = img->type == 5 ? img->width : img->width * 3;

  for (int i = 0; i < img->height; i++)
    fread(img->image[i], sizeof(char), (size_t) real_width, in);

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
  out->height = in->height / resize_factor;
  out->width = in->width / resize_factor;
  out->maxval = in->maxval;
  alloc_image(out);

  void* (*thread_func)(void *) = in->type == 5 ? resize_bw : resize_color;

  int i;
  pthread_t tid[num_threads];
  thread_func_args *args[num_threads];

  for(i = 0; i < num_threads; i++) {
    args[i] = (thread_func_args*) malloc(sizeof(thread_func_args));
    args[i]->in = in;
    args[i]->out = out;
    args[i]->thread_id = i;
    pthread_create(&(tid[i]), NULL, thread_func, args[i]);
  }

  for(i = 0; i < num_threads; i++) {
    pthread_join(tid[i], NULL);
  }
}