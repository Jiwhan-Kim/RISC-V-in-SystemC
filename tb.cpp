#include "tb.hpp"
#include <cstring>

void tb::tb_matmul() {
  en.write(false);
  wait();

  for (unsigned int i = 0; i < 5; i++)
    wait();

  en.write(true);
  wait();

  en.write(false);
  do {
    wait();
  } while (status.read() == 1);

  sc_stop();
}
