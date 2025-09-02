#ifndef AXI_MASTER_HPP_
#define AXI_MASTER_HPP_

#include <systemc.h>
#ifndef ADDR_WIDTH
#define ADDR_WIDTH sc_uint<32>
#endif

#ifndef DATA_WIDTH
#define DATA_WIDTH sc_uint<32>
#endif

// Generic AXI-like master port shell: clk, rst, and AXI channels
SC_MODULE(axi_master) {
  // Clock and reset
  sc_in<bool> clk;
  sc_in<bool> rst;

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
  sc_in<sc_uint<2>> bresp;

  // Read Address Channel
  sc_out<ADDR_WIDTH> araddr;
  sc_out<bool> arvalid;
  sc_in<bool> arready;

  // Read Data Channel
  sc_in<DATA_WIDTH> rdata;
  sc_in<bool> rvalid;
  sc_out<bool> rready;
  sc_in<sc_uint<2>> rresp;

  /**
   * response (bresp, rresp)
   *  0x00: OKAY
   *  0x01: EXOKAY
   *  0x10: SLVERR
   *  0x11: DECERR
   */

  SC_CTOR(axi_master) {}
};

#endif // AXI_MASTER_HPP_
