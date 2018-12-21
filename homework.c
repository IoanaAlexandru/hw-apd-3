#include "functions.c"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int rank;
  int nProcesses;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

  if (rank == 0) {
    // argv[1]  input
    // argv[2]  output
    // argv[3]+ filter
    if (argc < 4) {
      printf(
          "Usage: mpirun -np N ./%s Image_in.pnm Image_out.pnm filter1 filter2 ... filterX\n",
          argv[0]);
      exit(-1);
    }

    image_t image;

    readInput(argv[1], &image);

    int start_line, end_line;

    for (int i = 1; i < nProcesses; i++) {
      start_line = (i * image.height) / nProcesses;
      end_line = ((i + 1) * image.height) / nProcesses;
      start_line -= 1;
      if (i != nProcesses - 1)
        end_line++;
      sendImage(&image, i, start_line, end_line);
    }

    start_line = 0;
    end_line = image.height / nProcesses;
    if (end_line != image.height)
      end_line++;
    for (int i = 3; i < argc; i++) {
      applyFilter(&image, getFilter(argv[i]), start_line, end_line);

      if (nProcesses > 1) {
        int real_width = image.type == 5 ? image.width : image.width * 3;

        MPI_Send(image.image[end_line - 2],
                 real_width,
                 MPI_UNSIGNED_CHAR,
                 1,
                 DEFAULT_TAG,
                 MPI_COMM_WORLD);

        MPI_Recv(image.image[end_line - 1],
                 real_width,
                 MPI_UNSIGNED_CHAR,
                 1,
                 DEFAULT_TAG,
                 MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
      }
    }

    for (int i = 1; i < nProcesses; i++) {
      start_line = (i * image.height) / nProcesses;
      end_line = ((i + 1) * image.height) / nProcesses;

      image_t *img = recvImage(i);
      for (int j = start_line; j < end_line; j++) {
        image.image[j] = img->image[j - start_line + 1];
      }
    }

    writeData(argv[2], &image);
  } else {
    image_t *img = recvImage(0);


    for (int i = 3; i < argc; i++) {
      applyFilter(img, getFilter(argv[i]), 0, img->height);

      int real_width = img->type == 5 ? img->width : img->width * 3;

      MPI_Recv(img->image[0],
               real_width,
               MPI_UNSIGNED_CHAR,
               rank - 1,
               DEFAULT_TAG,
               MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);

      MPI_Send(img->image[1],
               real_width,
               MPI_UNSIGNED_CHAR,
               rank - 1,
               DEFAULT_TAG,
               MPI_COMM_WORLD);

      if (rank != nProcesses - 1) {
        MPI_Send(img->image[img->height - 2],
                 real_width,
                 MPI_UNSIGNED_CHAR,
                 rank + 1,
                 DEFAULT_TAG,
                 MPI_COMM_WORLD);

        MPI_Recv(img->image[img->height - 1],
                 real_width,
                 MPI_UNSIGNED_CHAR,
                 rank + 1,
                 DEFAULT_TAG,
                 MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
      }

    }

    sendImage(img, 0, 0, img->height);
  }

  MPI_Finalize();
  return 0;
}