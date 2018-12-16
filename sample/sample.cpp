#include "../mpistream.hpp"
#include <mpi.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, procs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &procs);
  mout.init(rank, procs);
  mout << "My rank is " << rank << " out of " << procs << " procs."
       << std::endl;
  MPI_Finalize();
}
