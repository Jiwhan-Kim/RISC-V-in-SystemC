#include "rv32i.hpp"

// AXI single 32-bit read (non-blocking wrapper around blocking sequence)
void rv32i::axi_read32(uint32_t addr, uint32_t &data) {
  data = axi_read32_blocking(addr);
}

// AXI single 32-bit write
void rv32i::axi_write32(uint32_t addr, uint32_t data) {
  axi_write32_blocking(addr, data);
}

uint32_t rv32i::axi_read32_blocking(uint32_t addr) {
  // Issue address
  araddr.write(addr);
  arvalid.write(true);
  do {
    wait();
  } while (!arready.read());
  arvalid.write(false);

  // Receive data
  rready.write(true);
  do {
    wait();
  } while (!rvalid.read());
  uint32_t v = rdata.read().to_uint();
  rready.write(false);
  return v;
}

void rv32i::axi_write32_blocking(uint32_t addr, uint32_t data) {
  // Send address and data
  awaddr.write(addr);
  awvalid.write(true);
  wdata.write(data);
  wvalid.write(true);
  bready.write(true);

  do {
    wait();
  } while (!(awready.read() && wready.read()));
  awvalid.write(false);
  wvalid.write(false);

  // Wait for response
  do {
    wait();
  } while (!bvalid.read());
  bready.write(false);
}

void rv32i::rv32i_main() {
  // Reset defaults
  awaddr.write(0);
  awvalid.write(false);
  wdata.write(0);
  wvalid.write(false);
  bready.write(false);
  araddr.write(0);
  arvalid.write(false);
  rready.write(false);
  status.write(0x0);
  pc = 0;
  for (int i = 0; i < 32; ++i)
    reg[i] = 0;
  wait();

  while (true) {
    if (en.read() == true) {
      status.write(0x01); // busy

      // Initialize state on start
      pc = 0u;
      for (int i = 0; i < 32; ++i)
        reg[i] = 0;

      const int max_steps =
          2048; // safety budget to avoid infinite loops in empty memory
      int steps = 0;
      bool done = false;

      while (!done && steps < max_steps) {
        // Fetch
        uint32_t instr = axi_read32_blocking(pc);

        // Decode fields
        uint32_t opcode = instr & 0x7fu;
        uint32_t rd = (instr >> 7) & 0x1fu;
        uint32_t funct3 = (instr >> 12) & 0x7u;
        uint32_t rs1 = (instr >> 15) & 0x1fu;
        uint32_t rs2 = (instr >> 20) & 0x1fu;
        uint32_t funct7 = (instr >> 25) & 0x7fu;

        auto get_reg = [&](uint32_t idx) -> uint32_t {
          return (idx == 0) ? 0u : (uint32_t)reg[idx];
        };
        auto set_reg = [&](uint32_t idx, uint32_t val) {
          if (idx != 0)
            reg[idx] = (int32_t)val;
        };

        // Immediates
        int32_t imm_i = (int32_t)sext(instr >> 20, 12);
        int32_t imm_s =
            (int32_t)sext(((instr >> 25) << 5) | ((instr >> 7) & 0x1f), 12);
        int32_t imm_b = (int32_t)sext(
            (((instr >> 31) & 0x1) << 12) | (((instr >> 7) & 0x1) << 11) |
                (((instr >> 25) & 0x3f) << 5) | (((instr >> 8) & 0xf) << 1),
            13);
        uint32_t imm_u = instr & 0xfffff000u;
        int32_t imm_j = (int32_t)sext(
            (((instr >> 31) & 0x1) << 20) | (((instr >> 12) & 0xff) << 12) |
                (((instr >> 20) & 0x1) << 11) | (((instr >> 21) & 0x3ff) << 1),
            21);

        uint32_t next_pc = pc + 4;

        switch (opcode) {
        case 0x37: // LUI
          set_reg(rd, imm_u);
          break;
        case 0x17: // AUIPC
          set_reg(rd, pc + imm_u);
          break;
        case 0x6f: // JAL
          set_reg(rd, next_pc);
          next_pc = pc + imm_j;
          break;
        case 0x67: { // JALR
          uint32_t t = (get_reg(rs1) + (uint32_t)imm_i) & ~1u;
          set_reg(rd, next_pc);
          next_pc = t;
          break;
        }
        case 0x63: { // BRANCH
          uint32_t a = get_reg(rs1);
          uint32_t b = get_reg(rs2);
          bool take = false;
          switch (funct3) {
          case 0x0:
            take = ((int32_t)a == (int32_t)b);
            break; // BEQ
          case 0x1:
            take = ((int32_t)a != (int32_t)b);
            break; // BNE
          case 0x4:
            take = ((int32_t)a < (int32_t)b);
            break; // BLT
          case 0x5:
            take = ((int32_t)a >= (int32_t)b);
            break; // BGE
          case 0x6:
            take = (a < b);
            break; // BLTU
          case 0x7:
            take = (a >= b);
            break; // BGEU
          default:
            break;
          }
          if (take)
            next_pc = pc + imm_b;
          break;
        }
        case 0x03: { // LOAD
          uint32_t addr = get_reg(rs1) + (uint32_t)imm_i;
          uint32_t base = addr & ~3u;
          uint32_t word = axi_read32_blocking(base);
          uint32_t off = (addr & 3u) * 8u;
          uint32_t res = 0;
          switch (funct3) {
          case 0x0: { // LB
            uint32_t byte = (word >> off) & 0xffu;
            res = (uint32_t)(int32_t)((int8_t)byte);
            break;
          }
          case 0x1: { // LH
            uint32_t half = (word >> (off & ~8u)) &
                            0xffffu; // off must be 0 or 16 for aligned
            res = (uint32_t)(int32_t)((int16_t)half);
            break;
          }
          case 0x2: { // LW
            res = word;
            break;
          }
          case 0x4: { // LBU
            uint32_t byte = (word >> off) & 0xffu;
            res = byte;
            break;
          }
          case 0x5: { // LHU
            uint32_t half = (word >> (off & ~8u)) & 0xffffu;
            res = half;
            break;
          }
          default:
            break;
          }
          set_reg(rd, res);
          break;
        }
        case 0x23: { // STORE
          uint32_t addr = get_reg(rs1) + (uint32_t)imm_s;
          uint32_t base = addr & ~3u;
          uint32_t off = (addr & 3u) * 8u;
          uint32_t rs2v = get_reg(rs2);
          switch (funct3) {
          case 0x0: { // SB (read-modify-write)
            uint32_t word = axi_read32_blocking(base);
            uint32_t mask = 0xffu << off;
            uint32_t val = ((rs2v & 0xffu) << off);
            word = (word & ~mask) | val;
            axi_write32_blocking(base, word);
            break;
          }
          case 0x1: { // SH (read-modify-write)
            uint32_t word = axi_read32_blocking(base);
            uint32_t aligned_off = (off & ~8u);
            uint32_t mask = 0xffffu << aligned_off;
            uint32_t val = ((rs2v & 0xffffu) << aligned_off);
            word = (word & ~mask) | val;
            axi_write32_blocking(base, word);
            break;
          }
          case 0x2: { // SW
            axi_write32_blocking(base, rs2v);
            break;
          }
          default:
            break;
          }
          break;
        }
        case 0x13: { // OP-IMM
          uint32_t a = get_reg(rs1);
          switch (funct3) {
          case 0x0:
            set_reg(rd, a + (uint32_t)imm_i);
            break; // ADDI
          case 0x2:
            set_reg(rd, (int32_t)a < (int32_t)imm_i ? 1u : 0u);
            break; // SLTI
          case 0x3:
            set_reg(rd, a < (uint32_t)imm_i ? 1u : 0u);
            break; // SLTIU
          case 0x4:
            set_reg(rd, a ^ (uint32_t)imm_i);
            break; // XORI
          case 0x6:
            set_reg(rd, a | (uint32_t)imm_i);
            break; // ORI
          case 0x7:
            set_reg(rd, a & (uint32_t)imm_i);
            break;    // ANDI
          case 0x1: { // SLLI
            uint32_t sh = ((uint32_t)imm_i) & 0x1fu;
            set_reg(rd, a << sh);
            break;
          }
          case 0x5: { // SRLI/SRAI
            uint32_t sh = ((uint32_t)imm_i) & 0x1fu;
            if ((instr >> 30) & 0x1) { // SRAI
              set_reg(rd, (uint32_t)((int32_t)a >> sh));
            } else {
              set_reg(rd, a >> sh);
            }
            break;
          }
          default:
            break;
          }
          break;
        }
        case 0x33: { // OP
          uint32_t a = get_reg(rs1);
          uint32_t b = get_reg(rs2);
          switch ((funct7 << 3) | funct3) {
          case (0x00 << 3) | 0x0:
            set_reg(rd, a + b);
            break; // ADD
          case (0x20 << 3) | 0x0:
            set_reg(rd, a - b);
            break; // SUB
          case (0x00 << 3) | 0x1:
            set_reg(rd, a << (b & 0x1f));
            break; // SLL
          case (0x00 << 3) | 0x2:
            set_reg(rd, (int32_t)a < (int32_t)b ? 1u : 0u);
            break; // SLT
          case (0x00 << 3) | 0x3:
            set_reg(rd, a < b ? 1u : 0u);
            break; // SLTU
          case (0x00 << 3) | 0x4:
            set_reg(rd, a ^ b);
            break; // XOR
          case (0x00 << 3) | 0x5:
            set_reg(rd, a >> (b & 0x1f));
            break; // SRL
          case (0x20 << 3) | 0x5:
            set_reg(rd, (uint32_t)((int32_t)a >> (b & 0x1f)));
            break; // SRA
          case (0x00 << 3) | 0x6:
            set_reg(rd, a | b);
            break; // OR
          case (0x00 << 3) | 0x7:
            set_reg(rd, a & b);
            break; // AND
          default:
            break;
          }
          break;
        }
        case 0x73: { // SYSTEM: ECALL/EBREAK/FENCE (treat as halt for
                     // ECALL/EBREAK)
          if (instr == 0x00000073 || instr == 0x00100073) { // ECALL or EBREAK
            done = true;
          }
          break;
        }
        default:
          // Unsupported/illegal: treat as NOP
          break;
        }

        pc = next_pc;
        steps++;
      }

      status.write(0x00); // idle when done
    }

    wait();
  }
}
