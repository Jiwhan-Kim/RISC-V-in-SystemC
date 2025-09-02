#include "memory.hpp"
#include <cstring>

void mem::mem_main() {
  // Initialize memory and outputs
  for (unsigned i = 0; i < MEM_SIZE; ++i)
    memory[i] = 0;

  memory[0x100 >> 2] = 0x1000; // A
  memory[0x104 >> 2] = 0x2000; // B
  memory[0x108 >> 2] = 0x2;    // M
  memory[0x10c >> 2] = 0x3;    // N
  memory[0x110 >> 2] = 0x2;    // K
  memory[0x114 >> 2] = 0x3000; // C

  memory[0x200 >> 2] = 0x5000; // First PC to run
  memory[0x5000 >> 2] = 0x000032B7;
  memory[0x5004 >> 2] = 0x0002A483;
  memory[0x5008 >> 2] = 0x42480337;
  memory[0x500c >> 2] = 0x00649363;
  memory[0x5010 >> 2] = 0x00100413;
  memory[0x5014 >> 2] = 0x0080006F;
  memory[0x5018 >> 2] = 0x00000413;
  memory[0x501c >> 2] = 0x000043B7;
  memory[0x5020 >> 2] = 0x0083A023;
  memory[0x5024 >> 2] = 0x00000073;

  float sample_A[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  float sample_B[6] = {7.0, 10.0, 8.0, 11.0, 9.0, 12.0};
  memcpy(&memory[0x1000 >> 2], &sample_A, sizeof(sample_A));
  memcpy(&memory[0x2000 >> 2], &sample_B, sizeof(sample_B));

  axi->awready.write(false);
  axi->wready.write(false);
  axi->bvalid.write(false);
  axi->bresp.write(0);
  axi->arready.write(false);
  axi->rdata.write(0);
  axi->rvalid.write(false);
  axi->rresp.write(0);
  wait();

  unsigned int waddr = 0;
  unsigned int raddr = 0;
  unsigned int waddr_raw = 0;
  unsigned int raddr_raw = 0;
  sc_uint<2> bresp_code = 0;
  sc_uint<2> rresp_code = 0;
  unsigned int rdata_latched = 0;
  bool bresp_pending = false;
  bool read_pending = false;

  while (true) {
    // Defaults each cycle
    axi->awready.write(false);
    axi->wready.write(false);
    axi->arready.write(false);

    // Handle write address
    if (axi->awvalid.read()) {
      waddr_raw = axi->awaddr.read();
      waddr = waddr_raw / 4;
      axi->awready.write(true);
    }

    // Handle write data
    if (axi->wvalid.read()) {
      // Validate alignment and range
      bool aligned = ((waddr_raw & 0x3u) == 0u);
      bool in_range = (waddr < MEM_SIZE);

      if (aligned && in_range) {
        memory[waddr] = axi->wdata.read();
        bresp_code = 0; // OKAY
      } else {
        // Do not modify memory on error
        bresp_code =
            in_range ? (sc_uint<2>)2 : (sc_uint<2>)3; // SLVERR or DECERR
      }

      axi->wready.write(true);
      bresp_pending = true;
    }

    // Handle write response
    if (bresp_pending) {
      axi->bvalid.write(true);
      axi->bresp.write(bresp_code);
      if (axi->bready.read()) {
        axi->bvalid.write(false);
        bresp_pending = false;
      }
    } else {
      axi->bvalid.write(false);
    }

    // Handle read address
    if (axi->arvalid.read()) {
      raddr_raw = axi->araddr.read();
      raddr = raddr_raw / 4;
      // Validate immediately and latch data/resp for this request
      bool aligned = ((raddr_raw & 0x3u) == 0u);
      bool in_range = (raddr < MEM_SIZE);
      if (aligned && in_range) {
        rdata_latched = memory[raddr];
        rresp_code = 0; // OKAY
      } else {
        rdata_latched = 0; // return zero data on error
        rresp_code =
            in_range ? (sc_uint<2>)2 : (sc_uint<2>)3; // SLVERR or DECERR
      }
      axi->arready.write(true);
      read_pending = true;
    }

    // Handle read data
    if (read_pending) {
      axi->rdata.write(rdata_latched);
      axi->rvalid.write(true);
      axi->rresp.write(rresp_code);
      if (axi->rready.read()) {
        axi->rvalid.write(false);
        read_pending = false;
      }
    } else {
      axi->rvalid.write(false);
    }

    wait();
  }
}
