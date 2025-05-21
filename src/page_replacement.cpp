#include "page_replacement.h"

/*
 * This this the method called when a page fault occurs. Your work begins here!
 */
void Page_Replacement::page_fault_handler(Page_Table *pt, int page )
{
    cout << "page fault on page #" << page << endl;
    
    pt->page_table_set_entry(page, page, PROT_READ | PROT_WRITE);
    
}
