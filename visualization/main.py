import data_management as dm
from plot import (
    plot_linux_systems,
    plot_gpu_comparison_linux,
    plot_cpu_linux_windows,
    plot_cpu_linux,
    plot_avg_values,
)

if __name__ == "__main__":
    multiple_systems, benchmark_results = dm.init()

    plot_linux_systems(benchmark_results, multiple_systems)
    if multiple_systems:
        plot_gpu_comparison_linux(benchmark_results)
        plot_cpu_linux_windows(benchmark_results)
        plot_cpu_linux(benchmark_results)
        plot_avg_values(benchmark_results)
