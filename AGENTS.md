10_pe SystemC Project Guide

This document gives a fast, practical overview of the project layout, how to build/run the simulation, and how AXI-style components are wired together. It’s written to help future automation (and humans) quickly orient and modify the design.

Project Overview

- AXI-like interconnect arbiter allowing multiple masters to access a single memory-mapped slave.
- Two masters:
  - matmul: a simple matrix-multiply engine using AXI reads/writes
  - rv32i: a minimal RV32I core executing from memory via AXI
- One slave:
  - mem: a memory module that services AXI transactions

main.cpp instantiates the system, connects signals, and runs a short simulation while producing a VCD trace (top_vcd.vcd).

Build and Run

Two common ways to build:

1) From the repository root (already configured in CMakeLists):
   - cmake -S . -B build
   - cmake --build build -j
   - ./build/test

2) Using an existing build directory (per workflow):
   - cd build
   - cmake ..
   - make -j
   - ./test

The simulation writes a VCD file named top_vcd.vcd in the working directory configured by main.cpp.

Source Map

- CMakeLists.txt: Build config; expects SystemC at ${CMAKE_PREFIX_PATH}.
- axi_master.hpp: AXI-like master port shell (clk, rst, aw/ w/ b/ ar/ r).
- axi_slave.hpp: AXI-like slave port shell (complementary directions).
- arbiter.hpp/.cpp: Simple interconnect:
  - Master side arrays: m_awaddr/… for MASTER masters
  - Single slave side: s_awaddr/… toward memory
  - Forwards address/data/valid/ready; response OKAY is assumed
- memory.hpp/.cpp: AXI-mapped memory slave:
  - Exposes axi_slave* axi bundle (embedded)
  - Initializes sample program/data and responds to AW/W/B and AR/R
  - Drives bresp and rresp to OKAY (0)
- matmul.hpp/.cpp: AXI master bundle internally (axi_master* axi):
  - Uses blocking read/write helpers to fetch params/vectors and write results
- rv32i.hpp/.cpp: Simple RV32I core using AXI fetch/load/store helpers
- main.cpp: Top-level SYSTEM module:
  - Instantiates tb, arbiter, mem, matmul, rv32i
  - Declares master and slave channel signals
  - Wires masters to arbiter, arbiter to memory
  - Creates VCD traces and runs sc_start
- tb.hpp/.cpp: Testbench signals for enable/status and clock/reset handling
- README.md: Brief project notes

AXI Conventions

- Address/data widths are sc_uint<32> by default.
- Channels implemented: AW, W, B, AR, R (single-beat, no bursts).
- Responses:
  - bresp and rresp are present on master/slave shells
  - Current arbiter does not forward bresp/rresp (masters assume OKAY)
  - Memory sets both responses to OKAY (0)

Top-Level Wiring (main.cpp)

- Master arrays (size MASTER, set in arbiter.hpp):
  - m_awaddr[], m_awvalid[], m_awready[], m_wdata[], m_wvalid[], m_wready[], m_bvalid[], m_bready[], m_araddr[], m_arvalid[], m_arready[], m_rdata[], m_rvalid[], m_rready[]
- Single slave signals to memory:
  - s_awaddr, s_awvalid, s_awready, s_wdata, s_wvalid, s_wready, s_bvalid, s_bready, s_araddr, s_arvalid, s_arready, s_rdata, s_rvalid, s_rready
- Masters (matmul, rv32i) connect their internal axi_master bundle to m_*[i]
- Memory connects its axi_slave bundle to s_*

Recent Refactor (AXI Slave in Memory)

- Memory previously exposed raw AXI-like ports; it now embeds axi_slave to mirror how masters use axi_master.
- Changes:
  - memory.hpp: add include of axi_slave.hpp and axi_slave* axi
  - memory.cpp: drive axi->... ports instead of local signals; set bresp/rresp = 0 (OKAY)
  - main.cpp: bind arbiter’s slave signals to mem0->axi->... and also connect clk/rst to mem0->axi
- Arbiter remains unchanged; it does not route bresp/rresp fields.

Memory Map Hints (from memory.cpp)

- Boot/parameters:
  - 0x100: A base
  - 0x104: B base
  - 0x108: M
  - 0x10c: N
  - 0x110: K
  - 0x114: C base
  - 0x200: Initial PC
- Sample program is written starting at 0x5000.
- Sample matrices begin at 0x1000 (A) and 0x2000 (B).

Adding a New Master

1) Create a module that owns an axi_master bundle and provides read/write helpers.
2) Increase MASTER in arbiter.hpp if needed and update any fixed-size arrays/signals in main.cpp accordingly.
3) Instantiate the module in main.cpp, bind its axi-> ports to m_*[index] signals, and hook up clk/rst/en/status if applicable.
4) Ensure address map consistency with memory.

Known Limitations / Next Steps

- Arbiter does not forward bresp/rresp; all responses are treated as OKAY. If you need error propagation, extend arbiter and add per-master response routing.
- Single-beat transfers only; no burst/IDs/locks/QoS.
- No protection checks; memory accepts any aligned 32-bit accesses.

