#include "homework.h"
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

    for (int i = 1; i < nProcesses; i++) {
      sendImage(&image, i);
    }

    applyFilter(&image, getFilter(argv[3]));

    writeData(argv[2], &image);
  } else {
    image_t *img = recvImage(0);
    char name[20];
    sprintf(name, "%s%d.pgm", argv[2], rank);
    writeData(name, img);
  }

  MPI_Finalize();
  return 0;
}