#include "matmul.hpp"
#include <cstring>

// AXI single 32-bit read (non-blocking wrapper around blocking sequence)
void matmul::axi_read32(uint32_t addr, uint32_t &d) {
  d = axi_read32_blocking(addr);
}

// AXI single 32-bit write
void matmul::axi_write32(uint32_t addr, uint32_t d) {
  axi_write32_blocking(addr, d);
}

uint32_t matmul::axi_read32_blocking(uint32_t addr) {
  araddr.write(addr);
  arvalid.write(true);
  do {
    wait();
  } while (!arready.read());
  arvalid.write(false);

  rready.write(true);
  do {
    wait();
  } while (!rvalid.read());
  uint32_t v = rdata.read().to_uint();
  rready.write(false);
  return v;
}

void matmul::axi_write32_blocking(uint32_t addr, uint32_t d) {
  awaddr.write(addr);
  awvalid.write(true);
  wdata.write(d);
  wvalid.write(true);
  bready.write(true);

  do {
    wait();
  } while (!(awready.read() && wready.read()));
  awvalid.write(false);
  wvalid.write(false);

  do {
    wait();
  } while (!bvalid.read());
  bready.write(false);
}

/**
 * Wait for an enable signal to be true
 *
 * When the signal becomes true (only for one clock cycle),
 *
 * it starts computing.
 *
 *
 * Suppose we multiply A (M * N) and B (N * K), then store it in C (M * K).
 *
 * It first reads six words, address of A, B, M, N, K, C.
 *
 * Then it reads vectors.
 *
 */
void matmul::matmul_main() {
  awaddr.write(0);
  awvalid.write(false);
  wdata.write(0);
  wvalid.write(false);
  bready.write(false);
  araddr.write(0);
  arvalid.write(false);
  rready.write(false);
  status.write(0x0);
  wait();

  while (true) {
    if (en.read() == true) {
      status.write(0x01);
      // read A, B, M, N, K, C base addresses/params via AXI helper
      for (unsigned int i = 0; i < 6; i++) {
        uint32_t v = axi_read32_blocking(0x100 + i * 4);
        data[i] = (int32_t)v;
        if (i >= 2 && i <= 4 && data[i] == 0 || (i == 3 && data[i] > 1024)) {
          status.write(0x02);
          continue;
        }
        cout << "data[" << i << "] = " << data[i] << endl;
      }
      cout << endl;

      // MAC operations
      for (unsigned int row = 0; row < data[2]; row++) {
        // Gets A[m] from 0 - (N-1)
        for (unsigned int m = 0; m < (unsigned)data[3]; m++) {
          uint32_t v = axi_read32_blocking(
              (uint32_t)(data[0] + (row * data[3] + m) * 4));
          A[m] = (int32_t)v;
        }

        // calculate C[row][col]
        for (unsigned int col = 0; col < data[4]; col++) {
          for (unsigned int k = 0; k < (unsigned)data[3]; k++) {
            uint32_t v = axi_read32_blocking(
                (uint32_t)(data[1] + (k * data[4] + col) * 4));
            B[k] = (int32_t)v;
          }

          // mult A * B
          acc = 0;
          for (unsigned int n = 0; n < data[3]; n++) {
            float a, b;
            std::memcpy(&a, &A[n], sizeof(a));
            std::memcpy(&b, &B[n], sizeof(b));

            float c = a * b;
            cout << a << "\t" << b << "\t" << c << endl;
            wait();

            acc += c;
            cout << a << "\t" << b << "\t" << c << "\t" << acc << endl;
            cout << endl;
            wait();
          }

          // write result to memory
          unsigned int acc_int;
          std::memcpy(&acc_int, &acc, sizeof(acc_int));

          // Write result via AXI helper
          axi_write32_blocking((uint32_t)(data[5] + (row * data[4] + col) * 4),
                               acc_int);

          cout << "C[" << row << "][" << col << "] = " << acc << endl;
        }
      }

      status.write(0x0);
    }

    wait();
  }
}
