# Benchmark Visualization

Run the ``main.py`` script to generate plots and calculate 
the speed-up between the GPU and the CPU. This script
expects the benchmark results in the ``../benchmarks``
folder as CSV files. To generate the results you can run
the ``../benchmarks/run.sh`` script.

## Installation

1. Create Python Virtual Environment: ``python3 -m venv venv/``
2. Activate venv: ``source venv/bin/activate``
3. Install pip packages: ``pip install -r requirements.txt``

## Execution

Run the main script with ``python main.py``. This should generate a ``results`` 
folder where the generated plots are saved and the speed-up values
are printed on the command line.
