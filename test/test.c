#include <stdio.h>
#include <assert.h>
#include "../functions.c"

void compareImages(const char *outFile, const char *refFile) {
  image_t out, ref;
  readInput(outFile, &out);
  readInput(refFile, &ref);

  assert(out.type == ref.type);
  assert(out.width == ref.width);
  assert(out.height == ref.height);
  assert(out.maxval == ref.maxval);
  int real_width = out.type == 5 ? out.width : out.width * 3;
  for (int i = 0; i < out.height; i++)
    for (int j = 0; j < real_width; j++)
      if (out.image[i][j] != ref.image[i][j]) {
        printf("out[%d][%d] = %hhu | ref[%d][%d] = %hhu\n",
               i,
               j,
               out.image[i][j],
               i,
               j,
               ref.image[i][j]);
        abort();
      }
}

void testFilter(const char *in,
                const char *out,
                const char *ref,
                const char *filter_string) {
  image_t image;
  readInput(in, &image);
  filter_t my_filter = getFilter(filter_string);
  assert(strcmp(my_filter.name, "identity") != 0);
  applyFilter(&image, my_filter);
  writeData(out, &image);
  compareImages(out, ref);
}

int main() {
  printf("Testing bw blur...\n");
  testFilter("../test/in/lenna_bw.pgm",
             "../test/out/lenna_bw_blur.pgm",
             "../test/ref/lenna_bw_blur.pgm",
             "blur");

  printf("Testing bw emboss...\n");
  testFilter("../test/in/lenna_bw.pgm",
             "../test/out/lenna_bw_emboss.pgm",
             "../test/ref/lenna_bw_emboss.pgm",
             "emboss");

  printf("Testing bw mean...\n");
  testFilter("../test/in/lenna_bw.pgm",
             "../test/out/lenna_bw_mean.pgm",
             "../test/ref/lenna_bw_mean.pgm",
             "mean");

  printf("Testing bw sharpen...\n");
  testFilter("../test/in/lenna_bw.pgm",
             "../test/out/lenna_bw_sharpen.pgm",
             "../test/ref/lenna_bw_sharpen.pgm",
             "sharpen");

  printf("Testing bw smooth...\n");
  testFilter("../test/in/lenna_bw.pgm",
             "../test/out/lenna_bw_smooth.pgm",
             "../test/ref/lenna_bw_smooth.pgm",
             "smooth");

  printf("Testing color blur...\n");
  testFilter("../test/in/lenna_color.pnm",
             "../test/out/lenna_color_blur.pnm",
             "../test/ref/lenna_color_blur.pnm",
             "blur");

  printf("Testing color emboss...\n");
  testFilter("../test/in/lenna_color.pnm",
             "../test/out/lenna_color_emboss.pnm",
             "../test/ref/lenna_color_emboss.pnm",
             "emboss");

  printf("Testing color mean...\n");
  testFilter("../test/in/lenna_color.pnm",
             "../test/out/lenna_color_mean.pnm",
             "../test/ref/lenna_color_mean.pnm",
             "mean");

  printf("Testing color sharpen...\n");
  testFilter("../test/in/lenna_color.pnm",
             "../test/out/lenna_color_sharpen.pnm",
             "../test/ref/lenna_color_sharpen.pnm",
             "sharpen");

  printf("Testing color smooth...\n");
  testFilter("../test/in/lenna_color.pnm",
             "../test/out/lenna_color_smooth.pnm",
             "../test/ref/lenna_color_smooth.pnm",
             "smooth");
}