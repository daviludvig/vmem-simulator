#include "page_replacement.h"
#include <climits>
#include <cstdlib>

// Ponteiro da instancia. Nulo ate receber o obj criado
Page_Replacement *Page_Replacement::current_instance = nullptr;

Page_Replacement::Page_Replacement(const std::string &alg, Disk *d,
                                   Page_Table *pt) {
  this->page_faults = 0;
  this->disk_reads = 0;
  this->disk_writes = 0;
  this->algorithm = alg;
  this->disk = d;
  this->page_table = pt; // Pontiero da page table
  this->tempo_atual = 0;

  // Inicializa as tabelas com base nos dados da Page_Table
  int num_frames = pt->page_table_get_nframes();
  this->frame_table.resize(num_frames, -1);
  if (this->algorithm == "custom") {
    this->tempo_acesso.resize(num_frames, 0);
  }

  std::srand(0); // Garante a determinismo para o 'rand'

  // Define a instancia static pro wrapper chamar o metodo correto. Isso aqui
  // precisa existir pq Page_Table (que nao deve ser modificado) solicita um
  // ponteiro de funcao simples. Funcionaria pra metodos static já que eles não
  // pertencem a nenhum objeto especifico
  current_instance = this;
}

// Isso aqui é um wrapper pro Page_Table. Ele e responsavel por delegar a
// chamada do metodo pra instancia correta
void Page_Replacement::page_fault_handler_wrapper(Page_Table *pt, int page) {
  if (current_instance) {
    current_instance->page_fault_handler(page);
  } else {
    // Isso aqui nao deve ocorrer, e cenario do handler ser chamado antes da
    // init do obj. Se ocorrer matar execucao
    cerr << "FATAL: Page fault handler called without an active "
            "Page_Replacement instance."
         << endl;
    abort();
  }
}

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
    int page_to_remove = fifo_queue.front();
    fifo_queue.pop();
    return find_frame_for_page(page_to_remove);
  } else if (algorithm == "custom") { // LRU
    int tempo_minimo = INT_MAX;
    int frame_a_remover = -1;
    for (int i = 0; i < (int)tempo_acesso.size(); ++i) {
      if (tempo_acesso[i] < tempo_minimo) {
        tempo_minimo = tempo_acesso[i];
        frame_a_remover = i;
      }
    }
    return frame_a_remover;
  }
  return 0; // Fallback
}

// O handler de falta de pagina real
void Page_Replacement::page_fault_handler(int page) {
  page_faults++;
  if (algorithm == "custom") {
    ++tempo_atual;
  }

  int frame;
  int bits;
  page_table->page_table_get_entry(page, &frame, &bits);

  if (bits == 0) { // Pagina não presente (falta de pagina real)
    int free_frame = find_free_frame();

    if (free_frame == -1) { // Nenhum frame livre, precisa substituir
      int frame_to_remove = select_frame_to_be_removed();
      int victim_page = frame_table[frame_to_remove];
      int victim_bits;
      int dummy_frame;

      page_table->page_table_get_entry(victim_page, &dummy_frame, &victim_bits);

      // Se a pagina vitima foi modificada (dirty bit) escrever no disco
      if (victim_bits & PROT_WRITE) {
        char *physmem_ptr = (char *)page_table->page_table_get_physmem();
        char *data_to_write =
            physmem_ptr + frame_to_remove * Page_Table::PAGE_SIZE;
        disk->write(victim_page, data_to_write);
        disk_writes++;
      }

      // Invalida a entrada da tabela de pagina para a pagina vitima
      page_table->page_table_set_entry(victim_page, 0, 0);
      frame_table[frame_to_remove] = -1; // Libera o frame
      free_frame = frame_to_remove;
    }

    // Carrega a nova pagina do disco para o frame livre
    char *physmem_ptr = (char *)page_table->page_table_get_physmem();
    char *data_to_read = physmem_ptr + free_frame * Page_Table::PAGE_SIZE;
    disk->read(page, data_to_read);
    disk_reads++;

    // Atualiza a tabela de paginas para a nova pagina
    page_table->page_table_set_entry(page, free_frame, PROT_READ);
    frame_table[free_frame] = page;

    // Atualiza ED do algoritmo de substituicao
    if (algorithm == "fifo") {
      fifo_queue.push(page);
    }
    if (algorithm == "custom") { // LRU
      tempo_acesso[free_frame] = tempo_atual;
    }

  } else { // Falta de permissao (tentativa de escrita em pagina RO)
    // O handler concede permissao de escrita.
    page_table->page_table_get_entry(page, &frame, &bits);
    page_table->page_table_set_entry(page, frame, bits | PROT_WRITE);

    // Atualiza o tempo de acesso para LRU, pq a pagina foi referenciada
    if (algorithm == "custom") {
      tempo_acesso[frame] = tempo_atual;
    }
  }
}

void Page_Replacement::print_stats() {
  std::cout << "Page faults: " << page_faults << std::endl;
  std::cout << "Disk reads: " << disk_reads << std::endl;
  std::cout << "Disk writes: " << disk_writes << std::endl;
}