import subprocess
import re
import matplotlib.pyplot as plt
import time
import statistics
import os

npages = 100
programs = ["alpha", "beta", "gamma", "delta"]  # delta incluso se quiser os 4
algorithms = ["rand", "fifo", "custom"]
estilos = {
    "rand": {"marker": "o", "linestyle": "-"},
    "fifo": {"marker": "s", "linestyle": "--"},
    "custom": {"marker": "x", "linestyle": "-."}
}
valor_esperado = {
    "delta": 52019200,
    "gamma": 52019200,
    "beta": 52019200,
    "alpha": 52019200
}

media_exec_por_programa = {}

for program in programs:
    frame_range = list(range(3, 100))
    print(f"Testando com {npages} páginas e até {frame_range[-1]} frames, programa: {program}, algoritmos: {algorithms}")
    results = {
        alg: {
            "page_faults": [],
            "disk_reads": [],
            "disk_writes": [],
            "result": [],
            "exec_times": []
        } for alg in algorithms
    }

    for alg in algorithms:
        print(f"Executando testes para algoritmo: {alg}")
        for nframes in frame_range:
            cmd = ["./virtmem", str(npages), str(nframes), alg, program]
            cmd2 = ["./opsys-sp22/project5/src/virtmem", str(npages), str(nframes), alg, program]
            try:
                print(f"Executando comando: {' '.join(cmd)}")
                start = time.perf_counter()
                output = subprocess.check_output(cmd, text=True)
                end = time.perf_counter()
                exec_time = end - start
                print(f"Tempo de execução: {exec_time} segundos")
                # output2 = subprocess.check_output(cmd2, text=True)
            except subprocess.CalledProcessError as e:
                print(f"Erro na execução com {nframes} frames: {e}")
                continue

            pf = int(re.search(r"Page faults: (\d+)", output).group(1))
            rd = int(re.search(r"Disk reads: (\d+)", output).group(1))
            wr = int(re.search(r"Disk writes: (\d+)", output).group(1))
            result = int(re.search(r"result is (\d+)", output).group(1))
            # pf2 = int(re.search(r"Page faults: (\d+)", output2).group(1))
            # rd2 = int(re.search(r"Disk reads: (\d+)", output2).group(1))
            # wr2 = int(re.search(r"Disk writes: (\d+)", output2).group(1))
            # result2 = int(re.search(r"result is (\d+)", output2).group(1))

            results[alg]["page_faults"].append(pf)
            results[alg]["disk_reads"].append(rd)
            results[alg]["disk_writes"].append(wr)
            results[alg]["result"].append(result)
            results[alg]["exec_times"].append(exec_time)

    # Plotagens por métrica
    for metric in ["page_faults", "disk_reads", "disk_writes"]:
        plt.figure(figsize=(10, 6))
        for alg in algorithms:
            plt.plot(frame_range, results[alg][metric], label=alg.upper(), **estilos[alg])
        plt.title(f"{metric.replace('_', ' ').capitalize()} vs. Número de Frames ({program})")
        plt.xlabel("Número de Frames")
        plt.ylabel(metric.replace('_', ' ').capitalize())
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        os.makedirs(f"python/outputs/{program}", exist_ok=True)
        plt.savefig(f"python/outputs/{program}/{metric}_{program}.png")
        plt.close()

    # Cálculo da média de todos os tempos de execução desse programa (soma dos 3 algoritmos)
    all_times = []
    for alg in algorithms:
        all_times.extend(results[alg]["exec_times"])
    media_exec_por_programa[program] = statistics.mean(all_times)

# Plot único com a média de execução por programa
plt.figure(figsize=(10, 6))
plt.bar(
    [prog.upper() for prog in media_exec_por_programa.keys()],
    list(media_exec_por_programa.values())
)
plt.title("Tempo médio de execução por programa (média de todos os algoritmos)")
plt.ylabel("Tempo médio (s)")
plt.grid(axis="y")
plt.tight_layout()
os.makedirs("python/outputs", exist_ok=True)
plt.savefig("python/outputs/exec_time_avg_por_programa.png")
plt.close()
