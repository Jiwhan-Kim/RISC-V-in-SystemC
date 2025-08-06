#include <systemc.h>

#define MEM_SIZE (1 << 20)

SC_MODULE(mem) {
  sc_in<bool> clk;
  sc_in<bool> rst;

  // Write address channel
  sc_in<sc_uint<32>> awaddr;
  sc_in<bool> awvalid;
  sc_out<bool> awready;

  // Write data channel
  sc_in<sc_uint<32>> wdata;
  sc_in<bool> wvalid;
  sc_out<bool> wready;

  // Write response channel
  sc_out<bool> bvalid;
  sc_in<bool> bready;

  // Read address channel
  sc_in<sc_uint<32>> araddr;
  sc_in<bool> arvalid;
  sc_out<bool> arready;

  // Read data channel
  sc_out<sc_uint<32>> rdata;
  sc_out<bool> rvalid;
  sc_in<bool> rready;

  unsigned int memory[MEM_SIZE];

  void mem_main();

  SC_CTOR(mem) {
    SC_CTHREAD(mem_main, clk.pos());
    reset_signal_is(rst, true);
  }
};
