#include <systemc.h>
#include "axi_master.hpp"

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

  // AXI master bundle
  axi_master *axi;

  float acc;

  // A (M * N), B (N * K), M, N, K, C
  signed int data[6];

  signed int A[1024];
  signed int B[1024];

  // AXI helpers
  void axi_read32(uint32_t addr, uint32_t &data);
  void axi_write32(uint32_t addr, uint32_t data);
  uint32_t axi_read32_blocking(uint32_t addr);
  void axi_write32_blocking(uint32_t addr, uint32_t data);
  void fault_trap();

  void matmul_main();

  SC_CTOR(matmul) {
    axi = new axi_master("axi");
    SC_CTHREAD(matmul_main, clk.pos());
    reset_signal_is(rst, true);
  }
};
