#include <systemc.h>

SC_MODULE(tb) {
  sc_in<bool> clk;
  sc_out<bool> rst;
  sc_out<bool> en[2];
  sc_in<sc_uint<2>> status[2];

  void tb_matmul();
  void tb_riscv();

  SC_CTOR(tb) {
    SC_CTHREAD(tb_matmul, clk.pos());
    SC_CTHREAD(tb_riscv, clk.pos());
    reset_signal_is(rst, true);
  }
};
