#include "page_replacement.h"

int Page_Replacement::page_faults = 0;
int Page_Replacement::disk_reads = 0;
int Page_Replacement::disk_writes = 0;
std::vector<int> Page_Replacement::frame_table;
std::string Page_Replacement::algorithm;
std::queue<int> Page_Replacement::fifo_queue;
Disk *Page_Replacement::disk = nullptr;
std::vector<int> Page_Replacement::tempo_acesso; // Para LRU
int Page_Replacement::tempo_atual = 0;           // Contador de tempo para LRU

void Page_Replacement::init_page_fault_algorithm(const std::string &alg) {
  algorithm = alg;
  std::srand(0); // Determinístico para testes
}

void Page_Replacement::set_disk(Disk *d) { disk = d; }

int Page_Replacement::find_frame_for_page(int page) {
  for (size_t i = 0; i < frame_table.size(); ++i) {
    if (frame_table[i] == page)
      return i;
  }
  return -1;
}

int Page_Replacement::find_free_frame() {
  for (int i = 0; i < (int)frame_table.size(); ++i) {
    if (frame_table[i] == -1)
      return i;
  }
  return -1;
}

int Page_Replacement::select_frame_to_be_removed() {
  if (algorithm == "rand") {
    return std::rand() % frame_table.size();
  } else if (algorithm == "fifo") {
    int page = fifo_queue.front();
    fifo_queue.pop();
    return find_frame_for_page(page);
  } else if (algorithm == "custom") {
    /*
     * LRU
     *
     */

    int min_time = INT_MAX;
    int frame_to_remove = -1;
    for (int i = 0; i < (int)tempo_acesso.size(); ++i) {
      if (tempo_acesso[i] < min_time) {
        min_time = tempo_acesso[i];
        frame_to_remove = i;
      }
    }
    std::cout << "[LRU] Removendo frame " << frame_to_remove
              << " com último acesso em t=" << min_time << std::endl;
    return frame_to_remove;
  }

  return -1; // Caso nenhum algoritmo seja reconhecido
}

void Page_Replacement::page_fault_handler(Page_Table *pt, int page) {
  std::cout << "===== INICIO =====" << std::endl;
  page_faults++;
  ++tempo_atual; // Incrementa o tempo para LRU

  int frame;
  int bits;
  pt->page_table_get_entry(page, &frame, &bits);

  std::cout << "Pagina: " << page << std::endl;

  // Inicializa estruturas na primeira falta
  if (frame_table.empty()) {
    int nframes = pt->page_table_get_nframes();
    frame_table.resize(nframes, -1);
    tempo_acesso.resize(nframes, 0); // Inicializa o vetor de tempos de acesso
    std::cout << "[DEBUG] Inicializando frame_table com " << nframes
              << " frames." << std::endl;
  }

  std::cout << "[DEBUG] frame_table: ";
  for (size_t i = 0; i < frame_table.size(); ++i) {
    std::cout << frame_table[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "bits: " << bits << std::endl;

  if (bits == 0) { // Página não presente
    int free_frame = find_free_frame();
    std::cout << free_frame << std::endl;

    if (free_frame == -1) { // Não há frame livre
      int frame_to_remove = select_frame_to_be_removed();
      int victim_page = frame_table[frame_to_remove];
      int victim_bits;
      int dummy_frame;

      pt->page_table_get_entry(victim_page, &dummy_frame, &victim_bits);

      // Escreve no disco se modificada
      if (victim_bits & PROT_WRITE) {
        char *physmem = (char *)pt->page_table_get_physmem();
        char *data = physmem + frame_to_remove * Page_Table::PAGE_SIZE;
        std::cout << "victim_page: " << victim_page << std::endl;
        std::cout << "data: " << victim_page << std::endl;
        disk->write(victim_page, data);
        disk_writes++;
      }

      pt->page_table_set_entry(victim_page, 0, 0);
      frame_table[frame_to_remove] = -1;
      free_frame = frame_to_remove;
    }

    // Lê página do disco
    char *physmem = (char *)pt->page_table_get_physmem();
    char *data = physmem + free_frame * Page_Table::PAGE_SIZE;
    disk->read(page, data);
    disk_reads++;

    std::cout << "FREE FRAME: " << free_frame << std::endl;
    pt->page_table_set_entry(
        page, free_frame,
        PROT_READ); // Define a página como presente e com permissão de leitura
    frame_table[free_frame] = page;
    tempo_acesso[free_frame] =
        tempo_atual; // Atualiza o tempo de acesso para LRU

    if (algorithm == "fifo") {
      fifo_queue.push(page);
    }

  } else if (bits &
             PROT_READ) { // Página presente, mas sem permissão de escrita
    pt->page_table_set_entry(page, frame, bits | PROT_WRITE);
    if (algorithm == "custom") {
      tempo_acesso[frame] = tempo_atual;
    }
  }

  std::cout << "===== FIM =====" << std::endl;
  std::cout << std::endl;
}

void Page_Replacement::print_stats() {
  std::cout << "Page faults: " << page_faults << std::endl;
  std::cout << "Disk reads: " << disk_reads << std::endl;
  std::cout << "Disk writes: " << disk_writes << std::endl;

  if (algorithm == "custom") {
    std::cout << "[DEBUG] Tempo de acesso (LRU):\n";
    for (size_t i = 0; i < frame_table.size(); ++i) {
      std::cout << "Frame " << i << ": página " << frame_table[i]
                << " | tempo=" << tempo_acesso[i] << std::endl;
    }
  }
}
