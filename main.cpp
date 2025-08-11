#include "arbiter.hpp"
#include "matmul.hpp"
#include "memory.hpp"
#include "rv32i.hpp"
#include "sysc/tracing/sc_trace.h"
#include "tb.hpp"
#include <systemc.h>

SC_MODULE(SYSTEM) {
  tb *tb0;
  arbiter *arbiter0;
  mem *mem0;
  matmul *matmul0; // 0
  rv32i *rv32i0;   // 1

  // Master interface signals
  // Write address channel
  sc_signal<sc_uint<32>> m_awaddr[MASTER];
  sc_signal<bool> m_awvalid[MASTER];
  sc_signal<bool> m_awready[MASTER];

  // Write data channel
  sc_signal<sc_uint<32>> m_wdata[MASTER];
  sc_signal<bool> m_wvalid[MASTER];
  sc_signal<bool> m_wready[MASTER];

  // Write response channel
  sc_signal<bool> m_bvalid[MASTER];
  sc_signal<bool> m_bready[MASTER];

  // Read address channel
  sc_signal<sc_uint<32>> m_araddr[MASTER];
  sc_signal<bool> m_arvalid[MASTER];
  sc_signal<bool> m_arready[MASTER];

  // Read data channel
  sc_signal<sc_uint<32>> m_rdata[MASTER];
  sc_signal<bool> m_rvalid[MASTER];
  sc_signal<bool> m_rready[MASTER];

  // Slave signalterface signals (memory)
  // Write Address Channel
  sc_signal<sc_uint<32>> s_awaddr;
  sc_signal<bool> s_awvalid;
  sc_signal<bool> s_awready;

  // Write Data Channel
  sc_signal<sc_uint<32>> s_wdata;
  sc_signal<bool> s_wvalid;
  sc_signal<bool> s_wready;

  // Write Response Channel
  sc_signal<bool> s_bvalid;
  sc_signal<bool> s_bready;

  // Read Address Channel
  sc_signal<sc_uint<32>> s_araddr;
  sc_signal<bool> s_arvalid;
  sc_signal<bool> s_arready;

  // Read Data Channel
  sc_signal<sc_uint<32>> s_rdata;
  sc_signal<bool> s_rvalid;
  sc_signal<bool> s_rready;

  sc_clock clk_sig;
  sc_signal<bool> rst_sig;
  sc_signal<bool> en_sig[2];
  sc_signal<sc_uint<2>> status_sig[2];

  // follows C++ syntax of constructor
  SC_CTOR(SYSTEM) : clk_sig("clk_sig", 10, SC_NS) {
    sc_trace_file *fp;
    fp = sc_create_vcd_trace_file("top_vcd");
    fp->set_time_unit(100, SC_PS);
    sc_trace(fp, clk_sig, "CLK");
    sc_trace(fp, rst_sig, "reset");
    sc_trace(fp, en_sig[0], "enable0");
    sc_trace(fp, status_sig[0], "status0");
    sc_trace(fp, en_sig[1], "enable1");
    sc_trace(fp, status_sig[1], "status1");

    tb0 = new tb("tb0");
    tb0->clk(clk_sig);
    tb0->rst(rst_sig);
    tb0->en[0](en_sig[0]);
    tb0->status[0](status_sig[0]);
    tb0->en[1](en_sig[1]);
    tb0->status[1](status_sig[1]);

    arbiter0 = new arbiter("arbiter0");
    arbiter0->clk(clk_sig);
    arbiter0->rst(rst_sig);

    for (unsigned int i = 0; i < MASTER; i++) {
      arbiter0->m_awaddr[i](m_awaddr[i]);
      arbiter0->m_awvalid[i](m_awvalid[i]);
      arbiter0->m_awready[i](m_awready[i]);

      arbiter0->m_wdata[i](m_wdata[i]);
      arbiter0->m_wvalid[i](m_wvalid[i]);
      arbiter0->m_wready[i](m_wready[i]);

      arbiter0->m_bvalid[i](m_bvalid[i]);
      arbiter0->m_bready[i](m_bready[i]);

      arbiter0->m_araddr[i](m_araddr[i]);
      arbiter0->m_arvalid[i](m_arvalid[i]);
      arbiter0->m_arready[i](m_arready[i]);

      arbiter0->m_rdata[i](m_rdata[i]);
      arbiter0->m_rvalid[i](m_rvalid[i]);
      arbiter0->m_rready[i](m_rready[i]);
    }

    arbiter0->s_awaddr(s_awaddr);
    arbiter0->s_awvalid(s_awvalid);
    arbiter0->s_awready(s_awready);

    arbiter0->s_wdata(s_wdata);
    arbiter0->s_wvalid(s_wvalid);
    arbiter0->s_wready(s_wready);

    arbiter0->s_bvalid(s_bvalid);
    arbiter0->s_bready(s_bready);

    arbiter0->s_araddr(s_araddr);
    arbiter0->s_arvalid(s_arvalid);
    arbiter0->s_arready(s_arready);

    arbiter0->s_rdata(s_rdata);
    arbiter0->s_rvalid(s_rvalid);
    arbiter0->s_rready(s_rready);

    mem0 = new mem("mem0");
    mem0->clk(clk_sig);
    mem0->rst(rst_sig);

    mem0->awaddr(s_awaddr);
    mem0->awvalid(s_awvalid);
    mem0->awready(s_awready);

    mem0->wdata(s_wdata);
    mem0->wvalid(s_wvalid);
    mem0->wready(s_wready);

    mem0->bvalid(s_bvalid);
    mem0->bready(s_bready);

    mem0->araddr(s_araddr);
    mem0->arvalid(s_arvalid);
    mem0->arready(s_arready);

    mem0->rdata(s_rdata);
    mem0->rvalid(s_rvalid);
    mem0->rready(s_rready);

    matmul0 = new matmul("matmul");
    matmul0->clk(clk_sig);
    matmul0->rst(rst_sig);
    matmul0->en(en_sig[0]);
    matmul0->status(status_sig[0]);

    matmul0->awaddr(m_awaddr[0]);
    matmul0->awvalid(m_awvalid[0]);
    matmul0->awready(m_awready[0]);

    matmul0->wdata(m_wdata[0]);
    matmul0->wvalid(m_wvalid[0]);
    matmul0->wready(m_wready[0]);

    matmul0->bvalid(m_bvalid[0]);
    matmul0->bready(m_bready[0]);

    matmul0->araddr(m_araddr[0]);
    matmul0->arvalid(m_arvalid[0]);
    matmul0->arready(m_arready[0]);

    matmul0->rdata(m_rdata[0]);
    matmul0->rvalid(m_rvalid[0]);
    matmul0->rready(m_rready[0]);

    rv32i0 = new rv32i("rv32i");
    rv32i0->clk(clk_sig);
    rv32i0->rst(rst_sig);
    rv32i0->en(en_sig[1]);
    rv32i0->status(status_sig[1]);

    rv32i0->awaddr(m_awaddr[1]);
    rv32i0->awvalid(m_awvalid[1]);
    rv32i0->awready(m_awready[1]);

    rv32i0->wdata(m_wdata[1]);
    rv32i0->wvalid(m_wvalid[1]);
    rv32i0->wready(m_wready[1]);

    rv32i0->bvalid(m_bvalid[1]);
    rv32i0->bready(m_bready[1]);

    rv32i0->araddr(m_araddr[1]);
    rv32i0->arvalid(m_arvalid[1]);
    rv32i0->arready(m_arready[1]);

    rv32i0->rdata(m_rdata[1]);
    rv32i0->rvalid(m_rvalid[1]);
    rv32i0->rready(m_rready[1]);
  }

  ~SYSTEM() {
    delete tb0;
    delete arbiter0;
    delete mem0;
    delete matmul0;
    delete rv32i0;
  }
};

SYSTEM *top = NULL;

int sc_main(int argc, char *argv[]) {
  top = new SYSTEM("top");
  sc_start(100000000, SC_NS); // Run simulation for 2000 ns
  return 0;
}
