/*
Main program for the virtual memory project.
Make all of your modifications to this file.
You may add or rearrange any code or data as you need.
The header files page_table.h and disk.h explain
how to use the page table and disk interfaces.
*/

#include "page_table.h"
#include "disk.h"
#include "program.h"
// #include "page_table.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define UNUSED_FRAME -1

int disk_read_count = 0;
int disk_write_count = 0;
int page_faults_count = 0;
int page_count = 0;

struct frame_table
{
    int *frames;
    int nframes;
    int *frame_mapping;
    int *frame_queue;
    int queue_front;
    int queue_back;
    int *ages;
} *ft;

struct frame_table *frame_table_create(int nframes)
{
    struct frame_table *ft = malloc(sizeof(struct frame_table));
    if (!ft)
    {
        return NULL;
    }

    ft->frames = malloc(sizeof(int) * nframes);
    ft->frame_mapping = malloc(sizeof(int) * nframes);
    ft->frame_queue = malloc(sizeof(int) * nframes);
    ft->ages = malloc(sizeof(int) * nframes);
    ft->queue_front = 0;
    ft->queue_back = 0;
    if (!ft->frames)
    {
        free(ft);
        return NULL;
    }

    ft->nframes = nframes;

    for (int i = 0; i < nframes; i++)
    {
        ft->frames[i] = UNUSED_FRAME;
        ft->frame_mapping[i] = UNUSED_FRAME;
        ft->ages[i] = UNUSED_FRAME;
        ft->frame_queue[i] = 0;
    }

    return ft;
}

void frame_table_delete(struct frame_table *ft)
{
    free(ft->frames);
    free(ft->frame_mapping);
    free(ft);
}

void frame_table_assign_frame(struct frame_table *ft, int frame, int page)
{
    ft->frames[frame] = 1;
    ft->frame_mapping[frame] = page;
}

void frame_table_free_frame(struct frame_table *ft, int frame)
{
    ft->frames[frame] = UNUSED_FRAME;
    ft->frame_mapping[frame] = UNUSED_FRAME;
}

int is_frame_used(struct frame_table *ft, int frame)
{
    if (ft->frames[frame] != UNUSED_FRAME)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int all_frames_used(struct frame_table *ft, int nframes)
{
    for (int i = 0; i < nframes; i++)
    {
        // printf("\n%d %d\n", i, is_frame_used(ft, i));
        if (!is_frame_used(ft, i))
        {

            return 0;
        }
    }
    return 1;
}

int get_dirtypage_frame(struct page_table *pt)
{
    int priority = -1;
    int no_priority = -1;
    int nframes = page_table_get_nframes(pt);
    for (int i = 0; i < nframes; i++)
    {
        int frame, bits;
        int page = ft->frame_mapping[i];
        page_table_get_entry(pt, page, &frame, &bits);
        if (bits == 3 || bits == 7)
        { // RW Permissions
            no_priority = i;
        }
        else if (bits == 1 || bits == 5)
        {
            priority = i;
        }
    }
    if (priority == -1)
    {
        priority = no_priority;
    }
    return priority;
}
void assign_new_frame(struct page_table *pt, int page, struct disk *disk)
{
    int nframes = page_table_get_nframes(pt);
    char *physmem = page_table_get_physmem(pt);
    int new_frame = -1;
    for (int i = 0; i < nframes; i++)
    {
        if (!is_frame_used(ft, i))
        {
            new_frame = i;
            break;
        }
    }
    page_table_set_entry(pt, page, new_frame, PROT_READ);
    disk_read(disk, page, &physmem[new_frame * PAGE_SIZE]);
    disk_read_count++;
    frame_table_assign_frame(ft, new_frame, page);
    ft->frame_queue[ft->queue_back] = new_frame;
    ft->queue_back = (ft->queue_back + 1) % ft->nframes;

    printf("Page fault on page #%d, assigned frame #%d\n", page, new_frame);
    page_faults_count++;
}

void assign_rand_frame(struct page_table *pt, int page, struct disk *disk)
{
    int nframes = page_table_get_nframes(pt);
    char *physmem = page_table_get_physmem(pt);
    int rand_frame = lrand48() % nframes;
    int used_page = ft->frame_mapping[rand_frame];
    int frame, bits;
    page_table_get_entry(pt, used_page, &frame, &bits);
    if (bits == 2 || bits == 3 || bits == 6 || bits == 7)
    {
        disk_write(disk, used_page, &physmem[rand_frame * PAGE_SIZE]);
        disk_write_count++;
        page_table_set_entry(pt, page, rand_frame, PROT_READ);
        disk_read(disk, page, &physmem[frame * PAGE_SIZE]);
        disk_read_count++;
        page_table_set_entry(pt, used_page, 0, 0);
        frame_table_assign_frame(ft, frame, page);
        page_faults_count++;
        printf("Page fault on page #%d, assigned frame #%d\n", page, rand_frame);
    }
    else
    {
        page_table_set_entry(pt, page, rand_frame, PROT_READ);
        disk_read(disk, page, &physmem[frame * PAGE_SIZE]);
        disk_read_count++;

        page_table_set_entry(pt, used_page, 0, 0);
        frame_table_assign_frame(ft, frame, page);
        page_faults_count++;
        printf("Page fault on page #%d, assigned frame #%d\n", page, rand_frame);
    }
}

void assign_queue_frame(struct page_table *pt, int page, struct disk *disk)
{
    char *physmem = page_table_get_physmem(pt);
    int queue_frame = ft->frame_queue[ft->queue_front];
    ft->queue_front = (ft->queue_front + 1) % ft->nframes;
    int used_page = ft->frame_mapping[queue_frame];
    int frame, bits;
    page_table_get_entry(pt, used_page, &frame, &bits);
    if (bits == 2 || bits == 3 || bits == 6 || bits == 7)
    {
        disk_write(disk, used_page, &physmem[queue_frame * PAGE_SIZE]);
        disk_write_count++;
        page_table_set_entry(pt, page, queue_frame, PROT_READ);
        disk_read(disk, page, &physmem[frame * PAGE_SIZE]);
        disk_read_count++;
        page_table_set_entry(pt, used_page, 0, 0);
        frame_table_assign_frame(ft, frame, page);
        page_faults_count++;
        printf("Page fault on page #%d, assigned frame #%d\n", page, queue_frame);
    }
    else
    {
        page_table_set_entry(pt, page, queue_frame, PROT_READ);
        disk_read(disk, page, &physmem[frame * PAGE_SIZE]);
        disk_read_count++;
        page_table_set_entry(pt, used_page, 0, 0);
        frame_table_assign_frame(ft, frame, page);
        page_faults_count++;
        printf("Page fault on page #%d, assigned frame #%d\n", page, queue_frame);
    }
}

void assign_dirty_frame(struct page_table *pt, int page, struct disk *disk)
{
    char *physmem = page_table_get_physmem(pt);

    int rand_frame = get_dirtypage_frame(pt);
    int used_page = ft->frame_mapping[rand_frame];
    int frame, bits;
    page_table_get_entry(pt, used_page, &frame, &bits);
    if (bits == 2 || bits == 3 || bits == 6 || bits == 7)
    {
        disk_write(disk, used_page, &physmem[rand_frame * PAGE_SIZE]);
        disk_write_count++;
        page_table_set_entry(pt, page, rand_frame, PROT_READ);
        disk_read(disk, page, &physmem[frame * PAGE_SIZE]);
        disk_read_count++;
        page_table_set_entry(pt, used_page, 0, 0);
        frame_table_assign_frame(ft, frame, page);
        page_faults_count++;
        printf("Page fault on page #%d, assigned frame #%d\n", page, rand_frame);
    }
    else
    {
        page_table_set_entry(pt, page, rand_frame, PROT_READ);
        disk_read(disk, page, &physmem[frame * PAGE_SIZE]);
        disk_read_count++;
        page_table_set_entry(pt, used_page, 0, 0);
        frame_table_assign_frame(ft, frame, page);
        page_faults_count++;
        printf("Page fault on page #%d, assigned frame #%d\n", page, rand_frame);
    }
}

void page_fault_handler(struct page_table *pt, int page)
{
    page_table_set_entry(pt, page, page, PROT_READ | PROT_WRITE);
    page_faults_count++;
    printf("page fault on page #%d\n", page);
    // exit(1);
}

void page_fault_handler_random(struct page_table *pt, int page)
{
    int nframes = page_table_get_nframes(pt);
    int npages = page_table_get_npages(pt);
    struct disk *disk = disk_open("myvirtualdisk", npages);

    int frame, bits;
    page_table_get_entry(pt, page, &frame, &bits);
    if (bits == 0 || bits == 4) // - || X
    {
        if (!all_frames_used(ft, nframes))
        {
            assign_new_frame(pt, page, disk);
        }
        else
        {
            assign_rand_frame(pt, page, disk);
        }
    }
    else if (bits == 1 || bits == 5) // R || RX
    {
        page_table_set_entry(pt, page, frame, PROT_READ | PROT_WRITE);
        page_faults_count++;
        printf("Page fault on page #%d, modified frame #%d\n", page, frame);
    }
    else // 2, 3, 6, 7 (W || RW || WX || RWX)
    {
        printf("ye toh aa hi nahi sakta aur agar aa raha hai toh iske bits hain %d", bits);
    }

    page_table_print(pt);
}

void page_fault_handler_fifo(struct page_table *pt, int page)
{
    int nframes = page_table_get_nframes(pt);
    int npages = page_table_get_npages(pt);
    struct disk *disk = disk_open("myvirtualdisk", npages);

    int frame, bits;
    page_table_get_entry(pt, page, &frame, &bits);
    if (bits == 0 || bits == 4) // - || X
    {
        if (!all_frames_used(ft, nframes))
        {
            assign_new_frame(pt, page, disk);
        }
        else
        {
            assign_queue_frame(pt, page, disk);
        }
    }
    else if (bits == 1 || bits == 5) // R || RX
    {
        page_table_set_entry(pt, page, frame, PROT_READ | PROT_WRITE);
        page_faults_count++;
        printf("Page fault on page #%d, modified frame #%d\n", page, frame);
    }
    else // 2, 3, 6, 7 (W || RW || WX || RWX)
    {
        printf("ye toh aa hi nahi sakta aur agar aa raha hai toh iske bits hain %d", bits);
    }

    page_table_print(pt);
}

void page_fault_handler_dirty(struct page_table *pt, int page)
{
    int nframes = page_table_get_nframes(pt);
    int npages = page_table_get_npages(pt);
    struct disk *disk = disk_open("myvirtualdisk", npages);

    int frame, bits;
    page_table_get_entry(pt, page, &frame, &bits);
    if (bits == 0 || bits == 4) // - || X
    {
        if (!all_frames_used(ft, nframes))
        {
            assign_new_frame(pt, page, disk);
        }
        else
        {
            assign_dirty_frame(pt, page, disk);
        }
    }
    else if (bits == 1 || bits == 5) // R || RX
    {
        page_table_set_entry(pt, page, frame, PROT_READ | PROT_WRITE);
        page_faults_count++;
        printf("Page fault on page #%d, modified frame #%d\n", page, frame);
    }
    else // 2, 3, 6, 7 (W || RW || WX || RWX)
    {
        printf("ye toh aa hi nahi sakta aur agar aa raha hai toh iske bits hain %d", bits);
    }

    page_table_print(pt);
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("use: virtmem <npages> <nframes> <rand|fifo|custom> <sort|scan|focus>\n");
        return 1;
    }

    int npages = atoi(argv[1]);
    int nframes = atoi(argv[2]);
    const char *algo = argv[3];
    const char *program = argv[4];

    struct disk *disk = disk_open("myvirtualdisk", npages);
    if (!disk)
    {
        fprintf(stderr, "couldn't create virtual disk: %s\n", strerror(errno));
        return 1;
    }

    ft = frame_table_create(nframes);
    void *choose;
    if (!strcmp(algo, "rand"))
    {
        choose = page_fault_handler_random;
    }
    else if (!strcmp(algo, "fifo"))
    {
        choose = page_fault_handler_fifo;
    }
    else if (!strcmp(algo, "custom"))
    {
        choose = page_fault_handler_dirty;
    }
    else
    {
        fprintf(stderr, "unknown program: %s\n", argv[3]);
    }

    struct page_table *pt = page_table_create(npages, nframes, choose);
    if (!pt)
    {
        fprintf(stderr, "couldn't create page table: %s\n", strerror(errno));
        return 1;
    }

    char *virtmem = page_table_get_virtmem(pt);

    char *physmem = page_table_get_physmem(pt);

    if (!strcmp(program, "sort") && (strcmp(algo, "custom")))
    {
        sort_program(virtmem, npages * PAGE_SIZE);
    }
    else if (!strcmp(program, "scan"))
    {
        scan_program(virtmem, npages * PAGE_SIZE);
    }
    else if (!strcmp(program, "focus"))
    {
        focus_program(virtmem, npages * PAGE_SIZE);
    }
    else if((!strcmp(algo, "custom")) && !(strcmp(program, "sort"))){
        scan_program(virtmem, npages * PAGE_SIZE);
    }
    else
    {
        fprintf(stderr, "unknown program: %s\n", argv[3]);
    }

    printf("disk read: %d\n", disk_read_count);
    printf("disk write: %d\n", disk_write_count);
    printf("fault count: %d\n", page_faults_count);

    page_table_delete(pt);
    disk_close(disk);

    return 0;
}
