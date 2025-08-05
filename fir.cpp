#include "fir.hpp"
#include <systemc.h>

const sc_uint<8> coef[5] = {18, 77, 107, 77, 18};

void fir::fir_main() {
  // FIR instances
  sc_int<16> taps[5];

  // Reset protocol
  for (int i = 0; i < 5; i++) {
    taps[i] = 0;
  }

  // Initialize handshake
  inp_rdy.write(0);
  outp_vld.write(0);
  outp.write(0);
  wait();

  // FIR filter
  while (true) {
    sc_int<16> in_val;
    sc_int<16> out_val;

    inp_rdy.write(1);
    do {
      wait();
    } while (!inp_vld.read());
    inp_rdy.write(0);

    // Read Input into shift register
    in_val = inp.read();
    for (int i = 4; i > 0; i--) {
      taps[i] = taps[i - 1];
    }
    taps[0] = in_val;

    for (int i = 0; i < 5; i++) {
      out_val += coef[i] * taps[i];
    }

    outp_vld.write(1);
    outp.write(out_val);

    do {
      wait();
    } while (!outp_rdy.read());
    outp_vld.write(0);
  }
}
