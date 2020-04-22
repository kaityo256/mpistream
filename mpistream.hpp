#pragma once
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <sstream>
#include <string>

#ifdef _MSC_VER
#define SNPRINTF(x, len, ...) _snprintf_s(x, len - 1, __VA_ARGS__)
#else
#define SNPRINTF(x, len, ...) snprintf(x, len, __VA_ARGS__)
#endif

class mpistream {
private:
  int rank;
  int size;
  std::ostringstream oss;
  std::ostringstream os_backup;
  std::string filename;
  bool initialized;

public:
  bool save_to_file;  // save outputs of each rank to file
  bool output_ifroot; // output to stdout if rank == 0

  mpistream(void) {
    rank = 0;
    size = 0;
    initialized = false;
    save_to_file = false;
    output_ifroot = true;
  }

  void init(void) {
    if (initialized) return;
    initialized = true;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    char f[256];
    SNPRINTF(f, sizeof(f), "r%04d.out", rank);
    filename = f;
    if (save_to_file) {
      std::ofstream ofs(filename.c_str());
    }
  }

  template <class T>
  mpistream &operator<<(const T &a) {
    init();
    oss << a;
    os_backup << a;
    return *this;
  }

  mpistream &operator<<(std::ostream &(*pf)(std::ostream &)) {
    init();
    os_backup << pf;
    if (output_ifroot && rank == 0) {
      std::cout << oss.str() << pf;
    }
    if (save_to_file) {
      std::ofstream ofs(filename.c_str(), std::ios_base::app);
      ofs << oss.str() << pf;
      oss.str("");
    }
    oss.clear();
    return *this;
  }
};

#ifdef _MSC_VER
#pragma comment(lib, "msmpi.lib")
#endif

__attribute__((weak)) mpistream mout;
