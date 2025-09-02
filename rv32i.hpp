#include <systemc.h>
#include "axi_master.hpp"

SC_MODULE(rv32i) {
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

  unsigned int pc;
  signed int reg[32];

  void rv32i_main();

  inline sc_uint<32> sext(sc_uint<32> v, int bits) {
    sc_uint<32> mask = (bits == 32) ? 0xffffffffu : ((1u << bits) - 1u);
    sc_uint<32> x = v & mask;
    sc_uint<32> sign = (bits == 32) ? 0u : (1u << (bits - 1));
    return (x ^ sign) - sign;
  }

  void axi_read32(uint32_t addr, uint32_t &data);
  void axi_write32(uint32_t addr, uint32_t data);
  uint32_t axi_read32_blocking(uint32_t addr);
  void axi_write32_blocking(uint32_t addr, uint32_t data);
  void fault_trap();

  SC_CTOR(rv32i) {
    axi = new axi_master("axi");
    SC_CTHREAD(rv32i_main, clk.pos());
    reset_signal_is(rst, true);
  }
};
