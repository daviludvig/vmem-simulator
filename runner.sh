#!/bin/bash

# Arquivo de saída para os resultados em formato CSV
OUTPUT_FILE="results_with_timeout.csv"

# Parâmetros
NPAGES=100
PROGRAMS=("alpha" "beta" "gamma" "delta")
ALGORITHMS=("rand" "fifo" "custom")
TIMEOUT_DURATION="10s" # Define um timeout de 10 segundos

echo "programa,algoritmo,frames,page_faults,disk_reads,disk_writes" > $OUTPUT_FILE

# Loop principal para executar todos os testes
for prog in "${PROGRAMS[@]}"; do
  for alg in "${ALGORITHMS[@]}"; do
    for frames in $(seq 3 100); do
      echo "Executando: $prog, $alg, frames=$frames (Timeout: $TIMEOUT_DURATION)..."
      
      # Executa o programa com o comando timeout
      result=$(timeout $TIMEOUT_DURATION ./build/virtmem $NPAGES $frames $alg $prog)
      
      # Captura o código de saída IMEDIATAMENTE após a execução
      exit_code=$?
      
      # Verifica o código de saída para decidir o que fazer
      if [ $exit_code -eq 124 ]; then
        # CASO 1: Ocorreu Timeout (Thrashing)
        echo "Timeout!"
        # Grava "timeout" no CSV para indicar que o teste falhou por tempo
        echo "$prog,$alg,$frames,timeout,timeout,timeout" >> $OUTPUT_FILE

      elif [ $exit_code -eq 0 ]; then
        # CASO 2: Execução bem-sucedida
        # Extrai os valores da saída normalmente
        faults=$(echo "$result" | grep "Page faults:" | awk '{print $3}')
        reads=$(echo "$result" | grep "Disk reads:" | awk '{print $3}')
        writes=$(echo "$result" | grep "Disk writes:" | awk '{print $3}')
        
        # Salva a linha no arquivo CSV
        echo "$prog,$alg,$frames,$faults,$reads,$writes" >> $OUTPUT_FILE
      
      else
        # CASO 3: O programa travou por outro motivo (ex: falha de segmentação)
        echo "Crash/Error (Código de saída: $exit_code)!"
        # Grava "crash" no CSV
        echo "$prog,$alg,$frames,crash,crash,crash" >> $OUTPUT_FILE
      fi
    done
  done
done

echo "Testes finalizados. Resultados em $OUTPUT_FILE"
