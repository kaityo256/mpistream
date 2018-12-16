#pragma once
#include <fstream>
#include <iostream>
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
  bool initialized;
  std::ostringstream oss;
  std::ostringstream os_backup;
  std::string filename;

public:
  mpistream(void) {
    initialized = false;
    rank = 0;
    size = 0;
  };
  void init(int r, int s) {
    rank = r;
    size = s;
    char f[256];
    SNPRINTF(f, sizeof(f), "r%04d.out", rank);
    filename = f;
    std::ofstream ofs(filename.c_str());
    initialized = true;
  }
  template <class T> mpistream &operator<<(const T &a) {
    if (!initialized) {
      std::cerr << "mpistream is not initialized." << std::endl;
      abort();
    }
    oss << a;
    os_backup << a;
    return *this;
  }
  mpistream &operator<<(std::ostream &(*pf)(std::ostream &)) {
    if (!initialized) {
      std::cerr << "mpistream is not initialized." << std::endl;
      abort();
    }

    os_backup << pf;
    if (size == 1) {
      std::cout << oss.str() << pf;
    }
    std::ofstream ofs(filename.c_str(), std::ios_base::app);
    ofs << oss.str() << pf;
    oss.str("");
    oss.clear();
    return *this;
  }
  void save_to_file(std::string filename) {
    if (rank != 0) {
      return;
    }
    std::ofstream ofs(filename.c_str());
    ofs << os_backup.str() << std::endl;
  }
  void append_to_file(std::string filename) {
    if (rank != 0) {
      return;
    }
    std::ofstream ofs(filename.c_str(), std::ios_base::app);
    ofs << os_backup.str() << std::endl;
  }
};

#ifdef _MSC_VER
#pragma comment(lib, "msmpi.lib")
#endif

extern mpistream mout;
