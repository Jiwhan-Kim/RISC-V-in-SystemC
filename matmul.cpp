#include "matmul.hpp"
#include <cstring>

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
      // read A, B, M, N, K
      for (unsigned int i = 0; i < 6; i++) {
        araddr.write(0x100 + i * 4);
        arvalid.write(true);

        do {
          wait();
        } while (!arready.read());
        arvalid.write(false);

        rready.write(true);
        do {
          wait();
        } while (!rvalid.read());

        data[i] = (sc_int<32>)rdata.read();
        if (i >= 2 && i <= 4 && data[i] == 0 || i == 3 && data[i] > 1024) {
          status.write(0x02);
          continue;
        }
        cout << "data[" << i << "] = " << data[i] << endl;
        rready.write(false);
      }
      cout << endl;

      // MAC operations
      for (unsigned int row = 0; row < data[2]; row++) {
        // Gets A[m] from 0 - (N-1)
        for (unsigned int m = 0; m < data[3]; m++) {
          araddr.write(data[0] + (row * data[3] + m) * 4);
          arvalid.write(true);
          do {
            wait();
          } while (!arready.read());
          arvalid.write(false);

          rready.write(true);
          do {
            wait();
          } while (!rvalid.read());
          A[m] = rdata.read();
          rready.write(false);
        }

        // calculate C[row][col]
        for (unsigned int col = 0; col < data[4]; col++) {
          for (unsigned int k = 0; k < data[3]; k++) {
            araddr.write(data[1] + (k * data[4] + col) * 4);
            arvalid.write(true);
            do {
              wait();
            } while (!arready.read());
            arvalid.write(true);

            rready.write(true);
            do {
              wait();
            } while (!rvalid.read());
            B[k] = rdata.read();
            rready.write(false);
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
          // Write address
          awaddr.write(data[5] + (row * data[4] + col) * 4);
          awvalid.write(true);
          do {
            wait();
          } while (!awready.read());
          awvalid.write(false);

          unsigned int acc_int;
          std::memcpy(&acc_int, &acc, sizeof(acc_int));
          // Write data
          wdata.write(acc_int);
          wvalid.write(true);
          do {
            wait();
          } while (!wready.read());
          wvalid.write(false);

          // Wait for write response
          bready.write(true);
          do {
            wait();
          } while (!bvalid.read());
          bready.write(false);

          cout << "C[" << row << "][" << col << "] = " << acc << endl;
        }
      }

      status.write(0x0);
    }

    wait();
  }
}
