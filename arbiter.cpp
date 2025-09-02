#include "arbiter.hpp"

void arbiter::arbiter_main() {
  for (int i = 0; i < MASTER; i++) {
    m_awready[i].write(false);
    m_wready[i].write(false);
    m_bvalid[i].write(false);
    m_bresp[i].write(0);
    m_arready[i].write(false);
    m_rdata[i].write(0);
    m_rvalid[i].write(false);
    m_rresp[i].write(0);
  }

  s_awaddr.write(0);
  s_awvalid.write(false);
  s_wdata.write(0);
  s_wvalid.write(false);
  s_bready.write(false);
  // responses are inputs from slave
  s_araddr.write(0);
  s_arvalid.write(false);
  s_rready.write(false);
  wait();

  unsigned int w_owner = 0;
  bool w_busy = false;
  unsigned int r_owner = 0;
  bool r_busy = false;

  while (true) {
    for (int i = 0; i < MASTER; i++) {
      m_awready[i].write(false);
      m_wready[i].write(false);
      m_bvalid[i].write(false);
      m_arready[i].write(false);
      m_rvalid[i].write(false);
    }

    s_awvalid.write(false);
    s_wvalid.write(false);
    s_bready.write(false);
    s_arvalid.write(false);
    s_rready.write(false);

    // Write Channel Arbitration
    if (!w_busy) {
      for (int i = 0; i < MASTER; ++i) {
        if (m_awvalid[i].read() && m_wvalid[i].read()) {
          s_awaddr.write(m_awaddr[i].read());
          s_awvalid.write(true);
          s_wdata.write(m_wdata[i].read());
          s_wvalid.write(true);
          m_awready[i].write(s_awready.read());
          m_wready[i].write(s_wready.read());
          if (s_awready.read() && s_wready.read()) {
            w_owner = i;
            w_busy = true;
          }
          break;
        }
      }
    } else {
      m_bvalid[w_owner].write(s_bvalid.read());
      m_bresp[w_owner].write(s_bresp.read());
      s_bready.write(m_bready[w_owner].read());
      if (s_bvalid.read() && m_bready[w_owner].read()) {
        w_busy = false;
      }
    }

    // Read Channel Arbitration
    if (!r_busy) {
      for (int i = 0; i < MASTER; i++) {
        if (m_arvalid[i].read()) {
          s_araddr.write(m_araddr[i].read());
          s_arvalid.write(m_arvalid[i].read()); // which is always true
          m_arready[i].write(s_arready.read());

          if (s_arready.read()) {
            r_owner = i;
            r_busy = true;
          }

          break;
        }
      }
    } else {
      m_rdata[r_owner].write(s_rdata.read());
      m_rvalid[r_owner].write(s_rvalid.read());
      m_rresp[r_owner].write(s_rresp.read());
      s_rready.write(m_rready[r_owner].read());

      if (s_rvalid.read() && m_rready[r_owner].read()) {
        r_busy = false;
      }
    }

    wait();
  }
}
