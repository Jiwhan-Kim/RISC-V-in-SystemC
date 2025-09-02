# Multiple PEs with one memory

This project includes two PEs (processing elements) and connected them in one memory.

## Build

```shell
mkdir build

cd build

cmake ..

make

# to start the simulation
./test
```

## Components


## AXI Responses

- Memory returns non-OKAY responses for invalid requests:
  - `DECERR (0x3)`: address outside memory range (`addr/4 >= MEM_SIZE`).
  - `SLVERR (0x2)`: unaligned access (address not 4-byte aligned).
- On errors, reads return `rdata = 0` with `rvalid=1` and `rresp` set as above; writes do not modify memory and return `bvalid=1` with `bresp` set.
- Current arbiter does not forward `bresp/rresp` to masters, so masters effectively assume OKAY. The signals are driven for future extension and waveform inspection.
