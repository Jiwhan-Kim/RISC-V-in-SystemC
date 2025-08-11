#include <systemc.h>

#define MASTER 2

SC_MODULE(arbiter) {
  sc_in<bool> clk;
  sc_in<bool> rst;

  // Master interface signals
  // Write address channel
  sc_in<sc_uint<32>> m_awaddr[MASTER];
  sc_in<bool> m_awvalid[MASTER];
  sc_out<bool> m_awready[MASTER];

  // Write data channel
  sc_in<sc_uint<32>> m_wdata[MASTER];
  sc_in<bool> m_wvalid[MASTER];
  sc_out<bool> m_wready[MASTER];

  // Write response channel
  sc_out<bool> m_bvalid[MASTER];
  sc_in<bool> m_bready[MASTER];

  // Read address channel
  sc_in<sc_uint<32>> m_araddr[MASTER];
  sc_in<bool> m_arvalid[MASTER];
  sc_out<bool> m_arready[MASTER];

  // Read data channel
  sc_out<sc_uint<32>> m_rdata[MASTER];
  sc_out<bool> m_rvalid[MASTER];
  sc_in<bool> m_rready[MASTER];

  // Slave interface signals (memory)
  // Write Address Channel
  sc_out<sc_uint<32>> s_awaddr;
  sc_out<bool> s_awvalid;
  sc_in<bool> s_awready;

  // Write Data Channel
  sc_out<sc_uint<32>> s_wdata;
  sc_out<bool> s_wvalid;
  sc_in<bool> s_wready;

  // Write Response Channel
  sc_in<bool> s_bvalid;
  sc_out<bool> s_bready;

  // Read Address Channel
  sc_out<sc_uint<32>> s_araddr;
  sc_out<bool> s_arvalid;
  sc_in<bool> s_arready;

  // Read Data Channel
  sc_in<sc_uint<32>> s_rdata;
  sc_in<bool> s_rvalid;
  sc_out<bool> s_rready;

  void arbiter_main();

  SC_CTOR(arbiter) {
    SC_CTHREAD(arbiter_main, clk.pos());
    reset_signal_is(rst, true);
  }
};
