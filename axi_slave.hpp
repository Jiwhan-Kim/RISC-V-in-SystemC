#ifndef AXI_SLAVE_HPP_
#define AXI_SLAVE_HPP_

#include <systemc.h>

#ifndef ADDR_WIDTH
#define ADDR_WIDTH sc_uint<32>
#endif

#ifndef DATA_WIDTH
#define DATA_WIDTH sc_uint<32>
#endif

// Generic AXI-like slave port shell: clk, rst, and AXI channels
SC_MODULE(axi_slave) {
  // Clock and reset
  sc_in<bool> clk;
  sc_in<bool> rst;

  // Write Address Channel
  sc_in<ADDR_WIDTH> awaddr;
  sc_in<bool> awvalid;
  sc_out<bool> awready;

  // Write Data Channel
  sc_in<DATA_WIDTH> wdata;
  sc_in<bool> wvalid;
  sc_out<bool> wready;

  // Write Response Channel
  sc_out<bool> bvalid;
  sc_in<bool> bready;

  // Read Address Channel
  sc_in<ADDR_WIDTH> araddr;
  sc_in<bool> arvalid;
  sc_out<bool> arready;

  // Read Data Channel
  sc_out<DATA_WIDTH> rdata;
  sc_out<bool> rvalid;
  sc_in<bool> rready;

  SC_CTOR(axi_slave) {}
};

#endif // AXI_SLAVE_HPP_
