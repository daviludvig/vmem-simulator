#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "disk.h"
#include "page_table.h"

#include <iostream>
#include <queue>
#include <vector>
#include <string>

using namespace std;

class Page_Replacement {
public:
  //Construtor
  Page_Replacement(const std::string &alg, Disk *d, Page_Table *pt);

  // Wrapper static que vai ser passado pro page_table. Explicação na classe
  static void page_fault_handler_wrapper(Page_Table *pt, int page);

  // Imprime as metricas finais
  void print_stats();

private:
  int page_faults;
  int disk_reads;
  int disk_writes;
  std::vector<int> frame_table; // Mapeia frame -> page
  std::string algorithm;
  std::queue<int> fifo_queue;
  Disk *disk;
  Page_Table *page_table;

  // Para o algoritmo LRU (custom)
  std::vector<int> tempo_acesso;
  int tempo_atual;

  void page_fault_handler(int page);

  // Métodos auxiliares
  int find_free_frame();
  int find_frame_for_page(int page);
  int select_frame_to_be_removed();

  // Ponteiro para a unica instância ativa, para que o wrapper possa encontrar
  static Page_Replacement *current_instance;
};

#endif