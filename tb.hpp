#include <systemc.h>

SC_MODULE(tb) {
  // unit time
  sc_time start_time[64], end_time[64], clock_period;

  sc_in<bool> clk;
  sc_out<bool> rst;

  sc_out<sc_int<16>> inp;
  sc_out<bool> inp_vld;
  sc_in<bool> inp_rdy;

  sc_in<sc_int<16>> outp;
  sc_in<bool> outp_vld;
  sc_out<bool> outp_rdy;

  void source();
  void sink();

  FILE *outfp;

  SC_CTOR(tb) {
    // gives output data (rst, inp, ...)
    SC_CTHREAD(source, clk.pos());

    // gets input data (outp, ...)
    SC_CTHREAD(sink, clk.pos());
  }
};
