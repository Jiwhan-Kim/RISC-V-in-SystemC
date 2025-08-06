#include "matmul.hpp"
#include "sysc/tracing/sc_trace.h"
#include "tb.hpp"
#include <systemc.h>

SC_MODULE(SYSTEM) {
  tb *tb0;
  matmul *matmul0;

  sc_clock clk_sig;
  sc_signal<bool> rst_sig;
  sc_signal<bool> en_sig;
  sc_signal<sc_uint<2>> status_sig;

  sc_signal<sc_uint<32>> awaddr;
  sc_signal<bool> awvalid;
  sc_signal<bool> awready;

  sc_signal<sc_uint<32>> wdata;
  sc_signal<bool> wvalid;
  sc_signal<bool> wready;

  sc_signal<bool> bvalid;
  sc_signal<bool> bready;

  sc_signal<sc_uint<32>> araddr;
  sc_signal<bool> arvalid;
  sc_signal<bool> arready;

  sc_signal<sc_uint<32>> rdata;
  sc_signal<bool> rvalid;
  sc_signal<bool> rready;

  // follows C++ syntax of constructor
  SC_CTOR(SYSTEM) : clk_sig("clk_sig", 10, SC_NS) {
    sc_trace_file *fp;
    fp = sc_create_vcd_trace_file("top_vcd");
    fp->set_time_unit(100, SC_PS);
    sc_trace(fp, clk_sig, "CLK");
    sc_trace(fp, rst_sig, "reset");
    sc_trace(fp, en_sig, "enable");
    sc_trace(fp, status_sig, "status");

    sc_trace(fp, awaddr, "awaddr");
    sc_trace(fp, awvalid, "awvalid");
    sc_trace(fp, awready, "awready");

    sc_trace(fp, wdata, "wdata");
    sc_trace(fp, wvalid, "wvalid");
    sc_trace(fp, wready, "wready");

    sc_trace(fp, bvalid, "bvalid");
    sc_trace(fp, bready, "bready");

    sc_trace(fp, araddr, "araddr");
    sc_trace(fp, arvalid, "arvalid");
    sc_trace(fp, arready, "arready");

    sc_trace(fp, rdata, "rdata");
    sc_trace(fp, rvalid, "rvalid");
    sc_trace(fp, rready, "rready");

    tb0 = new tb("tb0");
    tb0->clk(clk_sig);
    tb0->rst(rst_sig);
    tb0->en(en_sig);
    tb0->status(status_sig);

    tb0->awaddr(awaddr);
    tb0->awvalid(awvalid);
    tb0->awready(awready);

    tb0->wdata(wdata);
    tb0->wvalid(wvalid);
    tb0->wready(wready);

    tb0->bvalid(bvalid);
    tb0->bready(bready);

    tb0->araddr(araddr);
    tb0->arvalid(arvalid);
    tb0->arready(arready);

    tb0->rdata(rdata);
    tb0->rvalid(rvalid);
    tb0->rready(rready);

    matmul0 = new matmul("matmul");
    matmul0->clk(clk_sig);
    matmul0->rst(rst_sig);
    matmul0->en(en_sig);
    matmul0->status(status_sig);

    matmul0->awaddr(awaddr);
    matmul0->awvalid(awvalid);
    matmul0->awready(awready);

    matmul0->wdata(wdata);
    matmul0->wvalid(wvalid);
    matmul0->wready(wready);

    matmul0->bvalid(bvalid);
    matmul0->bready(bready);

    matmul0->araddr(araddr);
    matmul0->arvalid(arvalid);
    matmul0->arready(arready);

    matmul0->rdata(rdata);
    matmul0->rvalid(rvalid);
    matmul0->rready(rready);
  }

  ~SYSTEM() {
    delete tb0;
    delete matmul0;
  }
};

SYSTEM *top = NULL;

int sc_main(int argc, char *argv[]) {
  top = new SYSTEM("top");
  sc_start(100000000, SC_NS); // Run simulation for 2000 ns
  return 0;
}
