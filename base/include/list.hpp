#pragma once
struct list_entry_t
{
    list_entry_t *prev;
    list_entry_t *next;
    void *data;
};

list_entry_t *gen_list(void *data);
list_entry_t *add_entry(void *data, list_entry_t *list);
void pop_entry(list_entry_t *entry);

enum LIST_ENTRY_AVAIBLE_IN {
    NEXT,
    PREV
};

class ListIterator {
    private:
        list_entry_t *entry;
    public:
        ListIterator(list_entry_t *list);
        bool avaible(LIST_ENTRY_AVAIBLE_IN avaible_in=NEXT);
        void *next();
        void *prev();
};