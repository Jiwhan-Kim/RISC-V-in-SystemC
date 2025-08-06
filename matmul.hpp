#include <systemc.h>

#define ADDR_WIDTH sc_uint<32>
#define DATA_WIDTH sc_uint<32>

SC_MODULE(matmul) {
  sc_in<bool> clk;
  sc_in<bool> rst;
  sc_in<bool> en;
  /**
   * 0x00 Idle
   * 0x01 Busy
   * 0x02 Fault
   */
  sc_out<sc_uint<2>> status;

  // Write Address Channel
  sc_out<ADDR_WIDTH> awaddr;
  sc_out<bool> awvalid;
  sc_in<bool> awready;

  // Write Data Channel
  sc_out<DATA_WIDTH> wdata;
  sc_out<bool> wvalid;
  sc_in<bool> wready;

  // Write Response Channel
  sc_in<bool> bvalid;
  sc_out<bool> bready;

  // Read Address Channel
  sc_out<ADDR_WIDTH> araddr;
  sc_out<bool> arvalid;
  sc_in<bool> arready;

  // Read Data Channel
  sc_in<DATA_WIDTH> rdata;
  sc_in<bool> rvalid;
  sc_out<bool> rready;

  float acc;

  // A (M * N), B (N * K), M, N, K, C
  signed int data[6];

  signed int A[1024];
  signed int B[1024];

  void matmul_main();

  SC_CTOR(matmul) {
    SC_CTHREAD(matmul_main, clk.pos());
    reset_signal_is(rst, true);
  }
};
