#include "myalloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct block* head = NULL;

void print_data() {
    struct block* b = head;

    if (b == NULL) {
        printf("[empty]\n");
        return;
    }

    while (b != NULL) {
        // Uncomment the following line if you want to see the pointer values
        // printf("[%p:%d,%s]", b, b->size, b->in_use? "used": "free");
        printf("[%d,%s]", b->size, b->in_use ? "used" : "free");
        if (b->next != NULL) {
            printf(" -> ");
        }

        b = b->next;
    }

    printf("\n");
}

void myfree(void* p) {
    struct block* node = p - sizeof(struct block);
    node->in_use = 0;
}

void split_space(struct block* current, int size) {
    int available_space = current->size;
    int padded_requested_space = PADDED_SIZE(size);
    int padded_block_size = PADDED_SIZE(sizeof(struct block));
    int min_space = padded_requested_space + padded_block_size;
    if (available_space < min_space + 16) return;

    struct block* split_tail = PTR_OFFSET(current, min_space);
    split_tail->size = available_space - min_space;
    current->size = padded_requested_space;
    current->next = split_tail;
}

void* myalloc(int amount) {
    if (head == NULL) {
        head = sbrk(1024);
        head->next = NULL;
        head->size = 1024 - PADDED_SIZE(sizeof(struct block));
        head->in_use = 0;
    }
    struct block* pointer = head;
    int padded_amount = PADDED_SIZE(amount);
    while (pointer != NULL) {
        if (pointer->in_use == 0 && pointer->size >= padded_amount) {
            split_space(pointer, amount);

            pointer->in_use = 1;
            int padded_block_size = PADDED_SIZE(sizeof(struct block));

            return PTR_OFFSET(pointer, padded_block_size);
        }
        pointer = pointer->next;
    }
    return NULL;
}

int main() {
    void* p;

    p = myalloc(512);
    print_data();


    myfree(p);
    print_data();
    return 0;
}
