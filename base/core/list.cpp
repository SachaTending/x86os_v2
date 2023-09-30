#include <list.hpp>
#include <libc.hpp>

list_entry_t *gen_list(void *data) {
    list_entry_t *list = new list_entry_t;
    memset((void *)list, 0, sizeof(list_entry_t));
    list->data = data;
    list->next = 0;
    list->prev = 0;
    return list;
}

list_entry_t *add_entry(void *data, list_entry_t *list) {
    list_entry_t *list2 = new list_entry_t;
    memset((void *)list2, 0, sizeof(list_entry_t));
    if (list->next != 0) {
        list2->next = list->next;
        list->next = list2;
        list2->next->prev = list2;
    }
    list2->prev = list;
    list2->data = data;
    return list2;
}

void pop_entry(list_entry_t *entry) {
    if (entry->prev != 0) {
        entry->prev->next = entry->next;
        entry->next->prev = entry->prev;
    }
    delete entry;
}

ListIterator::ListIterator(list_entry_t *list) {
    this->entry = list;
}

bool ListIterator::avaible(LIST_ENTRY_AVAIBLE_IN avaible_in=NEXT) {
    if (avaible_in == NEXT) return this->entry->next->data != 0;
    return this->entry->prev->data != 0;
}

void *ListIterator::next() {
    this->entry = this->entry->next;
    return this->entry->data;
}

void *ListIterator::prev() {
    this->entry = this->entry->prev;
    return this->entry->data;
}