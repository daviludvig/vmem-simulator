#!/bin/bash

# Arquivo de saída para os resultados em formato CSV
OUTPUT_FILE="results.csv"

# Parâmetros
NPAGES=100
PROGRAMS=("alpha" "beta" "gamma" "delta")
ALGORITHMS=("rand" "fifo" "custom")

#Isso aqui tá dando trashing pra caso beta com numero de frames mto pequeno. To lidando ainda c isso

echo "programa,algoritmo,frames,page_faults,disk_reads,disk_writes" > $OUTPUT_FILE

# Loop principal para executar todos os testes
for prog in "${PROGRAMS[@]}"; do
  for alg in "${ALGORITHMS[@]}"; do
    # Varia o número de frames de 3 a 100
    for frames in $(seq 3 100); do
      echo "Executando: $prog, $alg, frames=$frames..."

      result=$(./build/virtmem $NPAGES $frames $alg $prog)
      
      # Extrai os valores da saída
      faults=$(echo "$result" | grep "Page faults:" | awk '{print $3}')
      reads=$(echo "$result" | grep "Disk reads:" | awk '{print $3}')
      writes=$(echo "$result" | grep "Disk writes:" | awk '{print $3}')
      
      # Salva a linha no arquivo CSV
      echo "$prog,$alg,$frames,$faults,$reads,$writes" >> $OUTPUT_FILE
    done
  done
done

echo "Testes finalizados. Resultados em $OUTPUT_FILE"
