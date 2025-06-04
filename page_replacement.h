#ifndef PAGE_REPLACEMENT_H
#define PAGE_REPLACEMENT_H

#include "page_table.h"
#include "disk.h"

#include <iostream>
#include <queue>

using namespace std;

class Page_Replacement
{
public:
    /*
     * This this the method called when a page fault occurs. Your work begins here!
     */
    static void page_fault_handler(Page_Table *pt, int page);
    static void init_page_fault_algorithm(const std::string &alg);

private:
    static int page_faults;
    static int disk_reads;
    static int disk_writes;
    static std::vector<int> frame_table;
    static std::string algorithm;
    static std::queue<int> fifo_queue;

    static int find_free_frame();
    static int find_frame_for_page(int page);
    static int select_frame_to_be_removed();
};

#endif