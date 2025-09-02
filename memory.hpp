#include <systemc.h>
#include "axi_slave.hpp"

#define MEM_SIZE (1 << 20)

SC_MODULE(mem) {
  // Local clock/reset for internal thread
  sc_in<bool> clk;
  sc_in<bool> rst;

  // AXI slave bundle
  axi_slave *axi;

  unsigned int memory[MEM_SIZE];

  void mem_main();

  SC_CTOR(mem) {
    axi = new axi_slave("axi");
    SC_CTHREAD(mem_main, clk.pos());
    reset_signal_is(rst, true);
  }
};
