from typing import List, Dict, Tuple
from pathlib import Path
import collections
import re
import pandas as pd

visual_folder = Path("results/")

parties = [
    [r".*NVIDIA GeForce RTX 2080.*", r".*AMD Ryzen 9 5900X 12-Core Processor.*"],
    [r".*GeForce RTX 3070.*", r".*Intel\(R\) Core\(TM\) i5-6600K CPU @ 3.50GHz.*"],
    [r".*GeForce RTX 2070 SUPER.*", r".*AMD Ryzen 7 1800X Eight-Core Processor.*"],
]


def init() -> Tuple[bool, Dict[str, pd.DataFrame]]:
    if not visual_folder.exists():
        visual_folder.mkdir()

    data_folder = Path("data/")
    if data_folder.exists():
        return True, load_files_from_data()

    return False, load_files_from_benchmark()


def load_files_from_data() -> Dict[str, pd.DataFrame]:
    _benchmark_results = {}
    folder = Path("data/")
    for fol in folder.iterdir():
        if fol.is_dir():
            for fileName in fol.iterdir():
                if str(fileName).endswith(".csv"):
                    file = Path(fileName)
                    meta_data = file.open("r").readline()
                    df = pd.read_csv(file, skiprows=1)
                    _benchmark_results[meta_data] = df

    return _benchmark_results


def load_files_from_benchmark() -> Dict[str, pd.DataFrame]:
    _benchmark_results = {}
    folder = Path("../benchmarks")
    for fileName in folder.iterdir():
        if str(fileName).endswith(".csv"):
            file = Path(fileName)
            meta_data = file.open("r").readline()
            df = pd.read_csv(file, skiprows=1)
            _benchmark_results[meta_data] = df

    return _benchmark_results


def filter_or(
    _benchmark_results: Dict, filter_list: List[str]
) -> collections.OrderedDict:
    result = collections.OrderedDict()
    for meta, df in _benchmark_results.items():
        for f in filter_list:
            if re.match(f, meta):
                result[meta] = df
    return result


def filter_and(
    _benchmark_results: Dict, filter_list: List[str]
) -> collections.OrderedDict:
    result = collections.OrderedDict()
    for meta, df in _benchmark_results.items():
        c = True
        for f in filter_list:
            if not re.match(f, meta):
                c = False
                break
        if c:
            result[meta] = df

    return result


def get(_benchmark_results: Dict, search: str) -> pd.DataFrame:
    for meta, df in _benchmark_results.items():
        if re.match(search, meta):
            return df
    return None
