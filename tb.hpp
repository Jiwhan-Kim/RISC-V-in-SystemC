#include <systemc.h>

SC_MODULE(tb) {
  sc_in<bool> clk;
  sc_out<bool> rst;
  sc_out<bool> en;
  sc_in<sc_uint<2>> status;

  void tb_matmul();

  SC_CTOR(tb) {
    SC_CTHREAD(tb_matmul, clk.pos());
    reset_signal_is(rst, true);
  }
};
