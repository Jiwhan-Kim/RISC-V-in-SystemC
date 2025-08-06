#include <systemc.h>

#define MEM_SIZE (1 << 20)
#define ADDR_WIDTH sc_unit<32>
#define DATA_WIDTH sc_unit<32>

SC_MODULE(tb) {
  sc_in<bool> clk;
  sc_out<bool> rst;
  sc_out<bool> en;
  sc_in<sc_uint<2>> status;

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

  // Memory Model
  unsigned int memory[1 << 20];

  void tb_main();
  void tb_matmul();

  FILE *outfp;

  SC_CTOR(tb) {
    SC_CTHREAD(tb_main, clk.pos());
    SC_CTHREAD(tb_matmul, clk.pos());
    reset_signal_is(rst, true);
  }
};
