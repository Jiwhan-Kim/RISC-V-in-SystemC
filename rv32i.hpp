#include <systemc.h>

#define ADDR_WIDTH sc_uint<32>
#define DATA_WIDTH sc_uint<32>

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

  SC_CTOR(rv32i) {
    SC_CTHREAD(rv32i_main, clk.pos());
    reset_signal_is(rst, true);
  }
};
