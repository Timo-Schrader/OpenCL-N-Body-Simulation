# Benchmarks

## Run Benchmarks

Execute ``benchmarks/run.sh`` from the root directory of this repository.
This can take multiple hours. It should run the following benchmarks:

- GPU with Kernel nbody.cl
- GPU with Kernel nbody_local.cl
- GPU with Kernel nbody_async.cl
- CPU with OpenMP and SIMD
- CPU with SIMD

The benchmark results will be saved in this folder.
