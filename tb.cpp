#include "tb.hpp"
#include <cstring>

void tb::tb_matmul() {
  en[0].write(false);
  wait();

  for (unsigned int i = 0; i < 5; i++)
    wait();

  en[0].write(true);
  wait();

  en[0].write(false);
  do {
    wait();
  } while (status[0].read() == 1);

  // uncomment the following line to stop the simulation after matmul test
  // otherwise, cpu starts to run
  // sc_stop();
}

/**
 *
 */
void tb::tb_riscv() {
  en[1].write(false);
  wait();

  do {
    wait();
  } while (status[0].read() == 1);

  en[1].write(true);
  wait();

  en[1].write(false);
  do {
    wait();
  } while (status[1].read() == 1);

  sc_stop();
}
