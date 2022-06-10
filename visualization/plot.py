from typing import List, Dict
from pathlib import Path
import collections
from tabulate import tabulate
import pandas as pd
import matplotlib.pyplot as plt
import data_management as dm


def plot_values(
    _benchmark_results: Dict,
    column: str,
    title: str,
    ylabel: str,
    ylog: bool,
    name: str,
    h_lines: List[float],
    font_size: int = 12,
):
    plt.figure(figsize=(16, 8), dpi=300)
    plt.rcParams.update({"font.size": font_size})

    for meta, df in _benchmark_results.items():
        plt.plot(df["nbody"], df[column], label=meta)

    plt.xlabel("Number of Bodies", fontsize=20)
    plt.ylabel(ylabel, fontsize=20)
    plt.title(title, fontsize=20)
    # for line in h_lines:
    #    plt.axhline(line, color="grey", linestyle="--")
    plt.grid(True, which="major", linestyle="--", axis="y")
    plt.xscale("log")
    if ylog:
        plt.yscale("log")
    plt.legend()
    plt.savefig(f"{dm.visual_folder}/{name}.png")
    plt.close()


def plot_speed_up(_benchmark_results: Dict, name: str):
    plt.figure(figsize=(10, 6), dpi=300)
    plt.rcParams.update({"font.size": 16})

    # Speed-Up with Multi-Thread CPU
    df_gpu = dm.get(_benchmark_results, r".*Kernel: nbody.cl$")
    df_cpu = dm.get(_benchmark_results, r".*- with SIMD - with OpenMP$")
    speed_up_multi = []
    for c0, c1 in zip(df_gpu["calc_avg"], df_cpu["calc_avg"]):
        speed_up_multi.append(c1 / c0)
    plt.plot(df_gpu["nbody"][:6], speed_up_multi, label="GPU vs. Multi-Thread CPU")

    # Speed-Up with Single-Thread CPU
    df_gpu = dm.get(_benchmark_results, r".*Kernel: nbody.cl$")
    df_cpu = dm.get(_benchmark_results, r".*- with SIMD$")
    speed_up_single = []
    for c0, c1 in zip(df_gpu["calc_avg"], df_cpu["calc_avg"]):
        speed_up_single.append(c1 / c0)
    plt.plot(df_gpu["nbody"][:6], speed_up_single, label="GPU vs. Single-Thread CPU")

    # Print speed-up as table
    print_values = {
        "nbody": df_cpu["nbody"],
        "Single-Thread CPU": speed_up_single,
        "Multi-Thread CPU": speed_up_multi,
    }
    print(tabulate(print_values, headers=["# Bodies", "Single-Thread Speed-Up", "Multi-Thread Speed-Up"],
            tablefmt="grid"))

    plt.xlabel("Number of Bodies", fontsize=16)
    plt.ylabel("Speed-Up", fontsize=16)
    plt.title("Speed-Up between GPU and CPU", fontsize=16)
    plt.xscale("log")
    plt.yscale("log")
    plt.grid(True, which="major", linestyle="--", axis="y")
    plt.legend()
    plt.savefig(f"{dm.visual_folder}/{name}.png")
    plt.close()


def plot_linux_systems(_benchmark_results: Dict, _filter: bool = True):
    """
    Plot calc, fps and speed up for the Linux benchmarks
    :param _benchmark_results: Raw benchmark data as panda data frames
    :param _filter: Whether to filter the given data or not
    """
    for index, party in enumerate(dm.parties):
        # filter the correct data sets per party
        if _filter:
            benchmark_results_set = dm.filter_or(
                dm.filter_or(_benchmark_results, party), [r"^Linux.*"]
            )
        else:
            benchmark_results_set = _benchmark_results

        # sort the data sets to display them nicely
        tmp = sorted(benchmark_results_set.items())
        tmp.reverse()
        benchmark_results_set = collections.OrderedDict(tmp)

        # render plots
        plot_speed_up(benchmark_results_set, f"{index}_speedup")
        h_lines = [10000, 2000, 30, 1]
        plot_values(
            benchmark_results_set,
            "fps_avg",
            "Avg FPS Comparison",
            "Frames per Second",
            True,
            f"{index}_fps_avg",
            h_lines,
        )
        h_lines = [1, 30]
        plot_values(
            benchmark_results_set,
            "calc_avg",
            "Avg Calc Time Comparison",
            "Time in s",
            True,
            f"{index}_calc_avg",
            h_lines,
        )

        if not _filter:
            break


def plot_gpu_comparison_linux(_benchmark_results: Dict):
    """
    Plot GPU comparison for the Linux benchmarks
    :param _benchmark_results: Raw benchmark data as panda data frames
    """
    gpu_filter = [
        r".*NVIDIA GeForce RTX 2080.*",
        r".*GeForce RTX 3070.*",
        r".*GeForce RTX 2070 SUPER.*",
    ]
    benchmark_results_set = dm.filter_and(
        dm.filter_or(_benchmark_results, gpu_filter),
        [r".*Kernel: nbody.cl$", r"^Linux.*"],
    )
    h_lines = [1500, 30, 1]
    plot_values(
        benchmark_results_set,
        "fps_avg",
        "GPU Comparison (Frames per Second)",
        "Frames per Second",
        True,
        "gpu_fps_avg",
        h_lines,
    )
    h_lines = [1, 1e-3]
    plot_values(
        benchmark_results_set,
        "calc_avg",
        "GPU Comparison (Calculation Time)",
        "Time in s",
        True,
        "gpu_calc_avg",
        h_lines,
    )


def plot_cpu_linux_windows(_benchmark_results: Dict):
    """
    Plot CPU comparison between Windows and Linux benchmarks
    :param _benchmark_results: Raw benchmark data as panda data frames
    """
    cpu_filter = [
        r".*AMD Ryzen 7 1800X Eight-Core Processor.*",
        r".*- with SIMD - with OpenMP$",
    ]
    benchmark_results_set = dm.filter_and(_benchmark_results, cpu_filter)
    h_lines = [1e-2, 1, 10]
    plot_values(
        benchmark_results_set,
        "calc_avg",
        "Multi-Thread CPU Windows vs. Linux (Calculation Time)",
        "Time in s",
        True,
        "multi_linux_windows_calc_avg",
        h_lines,
    )

    cpu_filter = [r".*AMD Ryzen 7 1800X Eight-Core Processor.*", r".*- with SIMD$"]
    benchmark_results_set = dm.filter_and(_benchmark_results, cpu_filter)
    h_lines = [1e-2, 1, 10, 100]
    plot_values(
        benchmark_results_set,
        "calc_avg",
        "Single-Thread CPU Windows vs. Linux (Calculation Time)",
        "Time in s",
        True,
        "single_linux_windows_calc_avg",
        h_lines,
    )


def plot_cpu_linux(_benchmark_results: Dict):
    """
    Plot CPU comparisons of Linux benchmarks
    :param _benchmark_results: Raw benchmark data as panda data frames
    """
    cpu_filter = [
        r".*AMD Ryzen 7 1800X Eight-Core Processor.*",
        r".*AMD Ryzen 9 5900X 12-Core Processor.*",
        r".*Intel\(R\) Core\(TM\) i5-6600K CPU @ 3.50GHz.*",
    ]
    benchmark_results_set = dm.filter_and(
        dm.filter_or(_benchmark_results, cpu_filter),
        [r"^Linux.*", ".*- with SIMD - with OpenMP$"],
    )
    h_lines = [1, 5, 10]
    plot_values(
        benchmark_results_set,
        "calc_avg",
        "Multi-Thread CPU Comparison (Calculation Time)",
        "Time in s",
        True,
        "multi_cpu_linux_calc_avg",
        h_lines,
    )

    cpu_filter = [
        r".*AMD Ryzen 7 1800X Eight-Core Processor.*",
        r".*AMD Ryzen 9 5900X 12-Core Processor.*",
        r".*Intel\(R\) Core\(TM\) i5-6600K CPU @ 3.50GHz.*",
    ]
    benchmark_results_set = dm.filter_and(
        dm.filter_or(_benchmark_results, cpu_filter), [r"^Linux.*", ".*- with SIMD$"]
    )
    h_lines = [40, 60]
    plot_values(
        benchmark_results_set,
        "calc_avg",
        "Single-Thread CPU Comparison (Calculation Time)",
        "Time in s",
        True,
        "single_cpu_linux_calc_avg",
        h_lines,
    )


def device_average_calculation(devices: Dict[str, pd.DataFrame]) -> pd.DataFrame:
    """
    Method to compute the average values of 3 devices.
    :param devices:
    :return:
    """
    device1 = list(devices.values())[0]
    device2 = list(devices.values())[1]
    device3 = list(devices.values())[2]
    device = device1.join(device2, lsuffix="_device1", rsuffix="_device2")
    device.rename(columns={"nbody_device1": "nbody"}, inplace=True)
    device = device.join(device3, rsuffix="_device3")

    device_result = pd.DataFrame(columns=["nbody"], data=device["nbody"])

    device_result["fps_avg"] = device[
        ["fps_avg_device1", "fps_avg_device2", "fps_avg"]
    ].mean(axis=1)

    device_result["calc_avg"] = device[
        ["calc_avg_device1", "calc_avg_device2", "calc_avg"]
    ].mean(axis=1)

    return device_result


def average_speedup_calculation(
    benchmarks: Dict[str, pd.DataFrame]
) -> Dict[str, pd.DataFrame]:
    gpu = dm.get(benchmarks, r"GPU Kernel: nbody.cl")
    cpu_single = dm.get(benchmarks, r"Single-Thread CPU")
    cpu_multi = dm.get(benchmarks, r"Multi-Thread CPU")

    speedup_single = []
    speedup_multi = []
    for g, c_s, c_m in zip(
        gpu["calc_avg"], cpu_single["calc_avg"], cpu_multi["calc_avg"]
    ):
        speedup_single.append(c_s / g)
        speedup_multi.append(c_m / g)

    benchmarks["Single-Core CPU vs. GPU"] = pd.DataFrame(
        data={"nbody": cpu_single["nbody"].tolist(), "speedup": speedup_single}
    )
    benchmarks["Multi-Core CPU vs. GPU"] = pd.DataFrame(
        data={"nbody": cpu_single["nbody"].tolist(), "speedup": speedup_multi}
    )

    return benchmarks


def plot_avg_values(_benchmark_results: Dict):
    """

    :param _benchmark_results: Raw benchmark data as panda data frames
    """
    font_size = 20
    average_results = {}

    # CPU multi thread average
    devices = dm.filter_or(
        _benchmark_results,
        [r"^Linux \(gcc\) CPU.*Release Mode - with SIMD - with OpenMP$"],
    )

    average_results["Multi-Thread CPU"] = device_average_calculation(devices)

    # CPU single thread average
    devices = dm.filter_or(
        _benchmark_results,
        [r"^Linux \(gcc\) CPU.*Release Mode - with SIMD$"],
    )

    average_results["Single-Thread CPU"] = device_average_calculation(devices)

    h_lines = []
    plot_values(
        average_results,
        "calc_avg",
        "Single-Thread vs. Multi-Thread CPU Comparison (Calculation Time)",
        "Time in s",
        True,
        "avg_multi_single_cpu_linux",
        h_lines,
        font_size=font_size,
    )

    # GPU nbody average
    devices = dm.filter_or(
        _benchmark_results,
        [r"^Linux \(gcc\) GPU.*Kernel: nbody\.cl$"],
    )

    average_results["GPU Kernel: nbody.cl"] = device_average_calculation(devices)

    benchmark_results_set = dm.filter_or(
        average_results, [r"Multi-Thread CPU", r"GPU Kernel: nbody\.cl"]
    )
    h_lines = []
    plot_values(
        benchmark_results_set,
        "calc_avg",
        "GPU vs. CPU (Calculation Time)",
        "Time in s",
        True,
        "avg_calc_gpu_cpu_linux",
        h_lines,
        font_size=font_size,
    )

    h_lines = []
    plot_values(
        benchmark_results_set,
        "fps_avg",
        "GPU vs. CPU (Frames per Second)",
        "Frames per Second",
        True,
        "avg_fps_gpu_cpu_linux",
        h_lines,
        font_size=font_size,
    )

    # GPU Kernel comparison
    devices = dm.filter_or(
        _benchmark_results,
        [r"^Linux \(gcc\) GPU.*Kernel: nbody_local\.cl$"],
    )
    average_results["GPU Kernel: nbody_local.cl"] = device_average_calculation(devices)

    devices = dm.filter_or(
        _benchmark_results,
        [r"^Linux \(gcc\) GPU.*Kernel: nbody_async\.cl$"],
    )
    average_results["GPU Kernel: nbody_async.cl"] = device_average_calculation(devices)

    benchmark_results_set = dm.filter_or(
        average_results,
        [
            r"GPU Kernel: nbody\.cl",
            r"GPU Kernel: nbody_local\.cl",
            r"GPU Kernel: nbody_async\.cl",
        ],
    )
    h_lines = []
    plot_values(
        benchmark_results_set,
        "calc_avg",
        "GPU Kernel Comparison (Calculation Time)",
        "Time in s",
        True,
        "avg_calc_gpu_kernel_linux",
        h_lines,
        font_size=font_size,
    )

    h_lines = []
    plot_values(
        benchmark_results_set,
        "fps_avg",
        "GPU Kernel Comparison (Frames per Second)",
        "Frames per Second",
        True,
        "avg_fps_gpu_kernel_linux",
        h_lines,
        font_size=font_size,
    )

    # Speed-Up between GPU and CPU (single- and multi-core)
    average_results = average_speedup_calculation(average_results)
    benchmark_results_set = dm.filter_or(
        average_results,
        [r"Single-Core CPU vs\. GPU", r"Multi-Core CPU vs\. GPU"],
    )
    h_lines = []
    plot_values(
        benchmark_results_set,
        "speedup",
        "Speed-Up between GPU and CPU",
        "Speed-Up",
        True,
        "avg_speedup_linux",
        h_lines,
        font_size=font_size,
    )

    # print calculated values on the command line
    print(average_results)
    print("\nSpeed-Up results as Latex table:\n")
    speedup = average_results["Single-Core CPU vs. GPU"]
    speedup["multi_speedup"] = average_results["Multi-Core CPU vs. GPU"]["speedup"]
    print(
        tabulate(
            speedup,
            headers=["# Bodies", "Single-Thread Speed-Up", "Multi-Thread Speed-Up"],
            tablefmt="latex",
        )
    )
