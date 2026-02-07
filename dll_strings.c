#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ---------- Doubly linked list of heap-allocated strings ---------- */

typedef struct Node {
    char *s;              // heap-allocated string
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    size_t size;
} DList;

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static char *strdup_safe(const char *src) {
    if (!src) return NULL;
    size_t n = strlen(src) + 1;
    char *p = (char *)malloc(n);
    if (!p) die("malloc");
    memcpy(p, src, n);
    return p;
}

void dlist_init(DList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

Node *dlist_insert_tail(DList *list, const char *value) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) die("malloc");
    n->s = strdup_safe(value);
    n->prev = list->tail;
    n->next = NULL;

    if (list->tail) list->tail->next = n;
    else list->head = n;

    list->tail = n;
    list->size++;
    return n;
}

Node *dlist_find_first(const DList *list, const char *value) {
    for (Node *cur = list->head; cur; cur = cur->next) {
        if (cur->s && value && strcmp(cur->s, value) == 0) return cur;
        if (!cur->s && !value) return cur;
    }
    return NULL;
}

int dlist_delete_node(DList *list, Node *n) {
    if (!list || !n) return 0;

    if (n->prev) n->prev->next = n->next;
    else list->head = n->next;

    if (n->next) n->next->prev = n->prev;
    else list->tail = n->prev;

    free(n->s);
    free(n);
    list->size--;
    return 1;
}

int dlist_delete_first_match(DList *list, const char *value) {
    Node *n = dlist_find_first(list, value);
    return dlist_delete_node(list, n);
}

void dlist_print(const DList *list) {
    printf("DList(size=%zu): [", list->size);
    for (Node *cur = list->head; cur; cur = cur->next) {
        printf("\"%s\"", cur->s ? cur->s : "(null)");
        if (cur->next) printf(", ");
    }
    printf("]\n");
}

void dlist_clear(DList *list) {
    Node *cur = list->head;
    while (cur) {
        Node *next = cur->next;
        free(cur->s);
        free(cur);
        cur = next;
    }
    list->head = list->tail = NULL;
    list->size = 0;
}

/* ------------------------------ Tests ------------------------------ */

static void test_basic_ops(void) {
    DList list;
    dlist_init(&list);
    assert(list.size == 0);

    dlist_insert_tail(&list, "apple");
    dlist_insert_tail(&list, "banana");
    dlist_insert_tail(&list, "cherry");
    dlist_insert_tail(&list, "banana");
    assert(list.size == 4);

    assert(dlist_find_first(&list, "banana") != NULL);
    assert(dlist_find_first(&list, "durian") == NULL);

    // delete first "banana"
    assert(dlist_delete_first_match(&list, "banana") == 1);
    assert(list.size == 3);
    assert(dlist_find_first(&list, "banana") != NULL); // still one left

    // delete head
    assert(dlist_delete_first_match(&list, "apple") == 1);
    assert(list.size == 2);

    // delete tail (current tail should be "banana")
    assert(dlist_delete_first_match(&list, "banana") == 1);
    assert(list.size == 1);

    // delete remaining
    assert(dlist_delete_first_match(&list, "cherry") == 1);
    assert(list.size == 0);

    // deleting non-existent
    assert(dlist_delete_first_match(&list, "nope") == 0);

    dlist_clear(&list);
    assert(list.size == 0);
}

static void test_delete_by_node_pointer(void) {
    DList list;
    dlist_init(&list);

    Node *a = dlist_insert_tail(&list, "A");
    Node *b = dlist_insert_tail(&list, "B");
    Node *c = dlist_insert_tail(&list, "C");
    (void)a; (void)c;

    assert(list.size == 3);
    assert(dlist_delete_node(&list, b) == 1);
    assert(list.size == 2);
    assert(dlist_find_first(&list, "B") == NULL);

    dlist_clear(&list);
}

int main(void) {
    test_basic_ops();
    test_delete_by_node_pointer();

    // quick demo output
    DList list;
    dlist_init(&list);
    dlist_insert_tail(&list, "hello");
    dlist_insert_tail(&list, "world");
    dlist_print(&list);
    dlist_clear(&list);

    puts("All tests passed.");
    return 0;
}