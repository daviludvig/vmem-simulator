import subprocess
import re
import matplotlib.pyplot as plt

npages = 100
program = "beta"
algorithms = ["rand", "fifo", "custom"]
frame_range = list(range(3, 100))  # para garantir que seja uma lista
print(f"Testando com {npages} páginas e até {frame_range} frames, programa: {program}, algoritmos: {algorithms}, frames: {frame_range}")
results = {
    alg: {
        "page_faults": [],
        "disk_reads": [],
        "disk_writes": []
    } for alg in algorithms
}

# Execução
for alg in algorithms:
    print(f"Executando testes para algoritmo: {alg}")
    for nframes in frame_range:
        cmd = ["./opsys-sp22/project5/src/virtmem", str(npages), str(nframes), alg, program]
        cmd = ["./build/virtmem", str(npages), str(nframes), alg, program]
        try:
            output = subprocess.check_output(cmd, text=True)
        except subprocess.CalledProcessError as e:
            print(f"Erro na execução com {nframes} frames: {e}")
            continue

        pf = int(re.search(r"Page faults: (\d+)", output).group(1))
        rd = int(re.search(r"Disk reads: (\d+)", output).group(1))
        wr = int(re.search(r"Disk writes: (\d+)", output).group(1))

        results[alg]["page_faults"].append(pf)
        results[alg]["disk_reads"].append(rd)
        results[alg]["disk_writes"].append(wr)

# Plotagem
for metric in ["page_faults", "disk_reads", "disk_writes"]:
    plt.figure(figsize=(10, 6))
    for alg in algorithms:
        y = results[alg][metric]
        x = frame_range
        print(f"{alg.upper()} - {metric.replace('_', ' ').capitalize()}: {y}")
        plt.plot(x, y,  label=f"{alg.upper()}")  # <- adiciona marcadores

    print(f"Plotando gráfico para {metric} com {program}")

    plt.title(f"{metric.replace('_', ' ').capitalize()} vs. Número de Frames ({program})")
    plt.xlabel("Número de Frames")
    plt.ylabel(metric.replace('_', ' ').capitalize())
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    import os
    os.makedirs(f"python/outputs/{program}", exist_ok=True)
    plt.savefig(f"python/outputs/{program}/{metric}_{program}.png")
    plt.close()
