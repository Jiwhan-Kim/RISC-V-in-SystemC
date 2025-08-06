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

  float sample_A[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  float sample_B[6] = {7.0, 10.0, 8.0, 11.0, 9.0, 12.0};
  memcpy(&memory[0x1000 >> 2], &sample_A, sizeof(sample_A));
  memcpy(&memory[0x2000 >> 2], &sample_B, sizeof(sample_B));

  awready.write(false);
  wready.write(false);
  bvalid.write(false);
  arready.write(false);
  rdata.write(0);
  rvalid.write(false);
  wait();

  unsigned int waddr = 0;
  unsigned int raddr = 0;
  bool bresp_pending = false;
  bool read_pending = false;

  while (true) {
    // Defaults each cycle
    awready.write(false);
    wready.write(false);
    arready.write(false);

    // Handle write address
    if (awvalid.read()) {
      waddr = awaddr.read() / 4;
      awready.write(true);
    }

    // Handle write data
    if (wvalid.read()) {
      memory[waddr] = wdata.read();
      wready.write(true);
      bresp_pending = true;
    }

    // Handle write response
    if (bresp_pending) {
      bvalid.write(true);
      if (bready.read()) {
        bvalid.write(false);
        bresp_pending = false;
      }
    } else {
      bvalid.write(false);
    }

    // Handle read address
    if (arvalid.read()) {
      raddr = araddr.read() / 4;
      arready.write(true);
      read_pending = true;
    }

    // Handle read data
    if (read_pending) {
      rdata.write(memory[raddr]);
      rvalid.write(true);
      if (rready.read()) {
        rvalid.write(false);
        read_pending = false;
      }
    } else {
      rvalid.write(false);
    }

    wait();
  }
}
