#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "disk.h"
#include "page_table.h"

#include <iostream>
#include <queue>

using namespace std;

class Page_Replacement {
public:
  static void page_fault_handler(Page_Table *pt, int page);
  static void init_page_fault_algorithm(const std::string &alg);
  static void set_disk(Disk *d);
  static void print_stats();

private:
  static int page_faults;
  static int disk_reads;
  static int disk_writes;
  static std::vector<int> frame_table;
  static std::string algorithm;
  static std::queue<int> fifo_queue;
  static Disk *disk;

  static int find_free_frame();
  static int find_frame_for_page(int page);
  static int select_frame_to_be_removed();

  static std::vector<int> tempo_acesso; // Para LRU
  static int tempo_atual;               // Contador de tempo para LRU
};

#endif