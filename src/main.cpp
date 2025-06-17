#include "disk.h"
#include "page_replacement.h"
#include "page_table.h"
#include "program.h"
#include <cstring>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 5) {
    std::cout << "use: virtmem <npages> <nframes> <rand|fifo|custom> "
                 "<alpha|beta|gamma|delta>\n";
    return 1;
  }

  int npages = atoi(argv[1]);
  int nframes = atoi(argv[2]);
  const char *algorithm = argv[3];
  const char *program_name = argv[4];

  // Inicializa simulador do disco
  Disk *disk = new Disk("Disco simulado 1", npages);

  // Tabela de paginas. Precisa passar um wrapper aqui como handler
  // A Page_Table ainda nao sabe sobre a instância do Page_Replacement, e nao
  // precisa
  Page_Table pt(npages, nframes, Page_Replacement::page_fault_handler_wrapper);

  // Agora cria instnacia do Page_Replacement
  // O construtor ira se registrar como a 'current_instance' estatica
  Page_Replacement page_replacer(algorithm, disk, &pt);

  // Pega a memoria virtual e executa o programa
  Program my_program;
  unsigned char *virtmem = (unsigned char *)pt.page_table_get_virtmem();

  if (!strcmp(program_name, "alpha")) {
    my_program.alpha(virtmem, npages * Page_Table::PAGE_SIZE);
  } else if (!strcmp(program_name, "beta")) {
    my_program.beta(virtmem, npages * Page_Table::PAGE_SIZE);
  } else if (!strcmp(program_name, "gamma")) {
    my_program.gamma(virtmem, npages * Page_Table::PAGE_SIZE);
  } else if (!strcmp(program_name, "delta")) {
    my_program.delta(virtmem, npages * Page_Table::PAGE_SIZE);
  } else {
    std::cout << "unknown program: " << program_name << std::endl;
    delete disk; // Limpeza em caso de erro
    return 1;
  }

  // Cleanup e exibicao de estatisticas
  // print_stats() e chamado na instancia
  page_replacer.print_stats();

  pt.page_table_delete();
  disk->close_disk();
  delete disk;

  return 0;
}