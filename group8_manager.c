#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILE "addresses.txt"
#define BACKING_STORE "BACKING_STORE.bin"

typedef struct{
    int frame;
    int valid;
}PageTableEntry;

//this method returns a page table of size 256 with no loaded page//
PageTableEntry *initialize_page_table(void){
    int table_size = 256;
    PageTableEntry *page_table = malloc(sizeof(PageTableEntry) * table_size);

    for(int i = 0 ; i < table_size; i++){
        page_table[i].frame = -1;
        page_table[i].valid = 0;
    }

    return page_table;
}

int main(void){
    PageTableEntry *page_table;
    signed char physical_memory[256][256]; //256 frames and 256 bytes per frame//
    int next_free_frame = 0;

    //stats//
    int page_fault_count = 0;
    // Sam: add TLB state here (16 entries, next insert index, tlb hit counter). //

    //initialize page table//
    page_table = initialize_page_table();

    if(page_table == NULL){
        printf("Failed to allocate page table\n");
        return 1;
    }

    //open address file//
    FILE *input_file = fopen(INPUT_FILE, "r");
    if (input_file == NULL){
        fprintf(stderr, "Error opening input file\n");
        exit(EXIT_FAILURE);
    }

    //open backing store//
    FILE *backing_store = fopen(BACKING_STORE, "rb");
    if(backing_store == NULL){
        printf("Failed to open BACKING_STORE.bin\n");
        return 1;
    }

    //read each line of input//
    int address;
    while (fscanf(input_file, "%d", &address ) == 1){
        address &= 0xFFFF; //mask address to 16 bits//

        //{ page: 8 bits }{ offset: 8 bits } 
        int page = (address >> 8) & 0xFF;
        int offset = address & 0xFF;
        // Sam: do TLB lookup here before checking the page table. //

        //check if page is in table already//
        if(page_table[page].valid == 0){
            //page fault logic//
            //on page fault: load page to next free physical memory//
            fseek(backing_store, page * 256, SEEK_SET); //moves file from BACKING_STORE.bin to start of page//
            fread(physical_memory[next_free_frame], sizeof(signed char), 256, backing_store);//read one full page from BACKING_STORE.bin into a frame in physical memory//

            //record mapping in page table//
            page_table[page].frame = next_free_frame;
            page_table[page].valid = 1; //the entry is now loaded in physical memory//
            next_free_frame++;
            
            //stats//
            page_fault_count++;
        }
        // todo: if TLB missed but frame is now known, insert (page, frame) into the TLB here. //

        //convert virtual address to physical address//
        int frame = page_table[page].frame;
        int physical_address = frame * 256 + offset;
        signed char value = physical_memory[frame][offset];
        printf("Virtual address: %d Physical address: %d Value: %d\n", address, physical_address, value);
    }

    // todo: print Page Fault Rate and TLB Hit Rate here after the loop. //

    fclose(input_file);
    fclose(backing_store);
    free(page_table);
    return 0;
}
