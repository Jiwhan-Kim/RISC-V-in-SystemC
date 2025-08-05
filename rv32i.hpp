#include <systemc.h>

SC_MODULE(rv32i) {
  sc_in<bool> clk;
  sc_in<bool> rst;

  void main();

  SC_CTOR(rv32i) {
    SC_CTHREAD(main, clk.pos());
    reset_signal_is(rst, true);
  }
};
