import csv
import os
import regex as re
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def get_files(dir: str, pre: str) -> list[str]:
    files = [os.path.join(os.getcwd(), dir, f) for f in os.listdir(dir) if re.match(pre, f)]
    return files

def extract_numbers(filename: str) -> int:
    bs_name = os.path.basename(filename)
    match = re.search(r'[0-9]+', bs_name)
    return int(match.group())

def graph_time(datafile: str) -> None:
    df = pd.read_csv(datafile)
    df['Size'] = df['Rows'] * df['Cols']
    df.sort_values(by=["NWorkers", "Size"], inplace=True)
    
    unique_nworkers = sorted(df['NWorkers'].unique())
    fig, ax = plt.subplots(figsize=(10, 6))
    
    for nworkers in unique_nworkers:
        data = df[df['NWorkers'] == nworkers]
        ax.plot(data['Size'], data['Times_NS'] / 1e6, 'o-', label=f"NWorkers = {nworkers}")

    ax.set_xlabel("Size (Total Elements)")
    ax.set_ylabel("Time (ms)")
    ax.set_title("Execution Time Comparison")
    ax.legend()
    ax.grid(True, alpha=0.3)
    fig.savefig("time.jpg")
    plt.show()
    plt.close()

def graph_scalability(datafile: str) -> None:
    df = pd.read_csv(datafile)
    df['Size'] = df['Rows'] * df['Cols']
    
    unique_sizes = sorted(df['Size'].unique())
    unique_workers = sorted(df['NWorkers'].unique())
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    for size in unique_sizes:
        subset = df[df['Size'] == size].sort_values("NWorkers")
        
        t1_row = subset[subset['NWorkers'] == 1]
        if not t1_row.empty:
            t1 = t1_row['Times_NS'].iloc[0]
        else:
            t1 = subset['Times_NS'].iloc[0] * subset['NWorkers'].iloc[0]
            
        speedup = t1 / subset['Times_NS']
        ax.plot(subset['NWorkers'], speedup, 'o-', label=f"Size: {size}")

    ax.plot(unique_workers, unique_workers, '--', color='red', linewidth=2, label="Ideal speedup")
    
    ax.set_title("Scalability (Speedup) for all sizes")
    ax.set_xlabel("Number of workers")
    ax.set_ylabel("Speedup (Times)")
    ax.set_xticks(unique_workers)
    ax.legend(title="Problem Size", bbox_to_anchor=(1.05, 1), loc='upper left')
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    fig.savefig("scalability.jpg")
    plt.show()
    plt.close()

def graph_efficiency(datafile: str) -> None:
    df = pd.read_csv(datafile)
    df['Size'] = df['Rows'] * df['Cols']
    
    unique_sizes = sorted(df['Size'].unique())
    unique_workers = sorted(df['NWorkers'].unique())
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    for size in unique_sizes:
        subset = df[df['Size'] == size].sort_values("NWorkers")
        
        t1_row = subset[subset['NWorkers'] == 1]
        t1 = t1_row['Times_NS'].iloc[0] if not t1_row.empty else subset['Times_NS'].iloc[0] * subset['NWorkers'].iloc[0]
        
        efficiency = (t1 / subset['Times_NS']) / subset['NWorkers']
        
        ax.plot(subset['NWorkers'], efficiency, 'o-', label=f"Size: {size}")

    ax.axhline(y=1, color='red', linestyle='--', label="Ideal efficiency (1.0)")
    
    ax.set_title("Parallel Efficiency")
    ax.set_xlabel("Number of workers")
    ax.set_ylabel("Efficiency (0.0 - 1.0)")
    ax.set_ylim(0, 1.1)
    ax.set_xticks(unique_workers)
    ax.legend(title="Problem Size", bbox_to_anchor=(1.05, 1), loc='upper left')
    ax.grid(True, alpha=0.3)
    
    plt.tight_layout()
    fig.savefig("efficiency.jpg")
    plt.show()
    plt.close()

def merge_csv(file_list: list[str], out_file: str) -> None:
    with open(out_file, "w") as outf:
        writer = csv.writer(outf)
        writer.writerow(["NWorkers", "Rows", "Cols", "Times_NS"])
        for file in file_list:
            n_workers = extract_numbers(file)
            with open(file, "r") as rf:
                reader = csv.reader(rf)
                for row in reader:
                    if not row: continue
                    writer.writerow([n_workers] + row)

if __name__ == "__main__":
    res_files = get_files("../res", "^res[0-9]+.csv$")
    merge_csv(res_files, "all.csv")
    
    # Atliekame pilną analizę
    graph_time("all.csv")
    graph_scalability("all.csv")
    graph_efficiency("all.csv")