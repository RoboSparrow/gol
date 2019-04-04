#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "test.h"
#include "utils.h"

typedef struct Item {
    char title[50];
    int value;
} Item;


//// Tests for RLE parser

void test_genlist_init() {
    DESCRIBE("genlist_init");
    GenList list;
    genlist_init(&list);

    assert(list.size == GENLIST_INIT_SIZE);
    assert(list.length == 0);
    assert(list.items != NULL);

    DONE();
}

void test_genlist_push() {
    DESCRIBE("genlist_push two items");
    GenList list;
    Item item1 = { "item1", 1 };
    Item item2 = { "item2", 2 };

    genlist_init(&list);
    genlist_push(&list, &item1);
    genlist_push(&list, &item2);

    // Item *t = (Item *) genlist_get(&list, 0);
    // printf("--- %s\n", t->title);
    assert(list.size == GENLIST_INIT_SIZE);
    assert(list.length == 2);
    assert(list.items != NULL);

    assert(strcmp(((Item *)list.items[0])->title, "item1") == 0);
    assert(strcmp(((Item *)list.items[1])->title, "item2") == 0);

    assert(((Item *)list.items[0])->value == 1);
    assert(((Item *)list.items[1])->value == 2);

    DONE();
}

void test_genlist_push_realloc() {
    DESCRIBE("genlist_push: grow over initial size limit");
    GenList list;

    int exceed = GENLIST_INIT_SIZE + 1;
    int values[exceed];
    for (int i = 0; i < exceed; i++) {
        values[i] = i * 5;
    }

    genlist_init(&list);
    for (int k = 0; k < exceed; k++) {
        genlist_push(&list, &values[k]);
    }

    // Item *t = (Item *) genlist_get(&list, 0);
    // printf("--- %s\n", t->title);
    assert(list.size == GENLIST_INIT_SIZE * 2);
    assert(list.length == exceed);
    assert(list.items != NULL);

    for (int n = 0; n < list.length; n++) {
        assert(*((int*)list.items[n]) == values[n]);
    }

    DONE();
}

void test_genlist_set() {
    DESCRIBE("genlist_push two items and set first");
    GenList list;
    Item item1 = { "item1", 1 };
    Item item2 = { "item2", 2 };
    Item item3 = { "item3", 3 };

    // prepare
    genlist_init(&list);
    genlist_push(&list, &item1);
    genlist_push(&list, &item2);

    // set
    genlist_set(&list, 0, &item3);

    assert(strcmp(((Item *)list.items[0])->title, "item3") == 0);
    assert(strcmp(((Item *)list.items[1])->title, "item2") == 0);

    assert(((Item *)list.items[0])->value == 3);
    assert(((Item *)list.items[1])->value == 2);

    DONE();
}

void test_genlist_get() {
    DESCRIBE("genlist_get");
    GenList list;
    Item item1 = { "item1", 1 };
    Item item2 = { "item2", 2 };

    genlist_init(&list);
    genlist_push(&list, &item1);
    genlist_push(&list, &item2);

    Item *gets;

    gets = (Item *) genlist_get(&list, 99);
    assert(gets == NULL);

    gets = (Item *) genlist_get(&list, -99);
    assert(gets == NULL);

    gets = (Item *) genlist_get(&list, 1);
    assert(strcmp(gets->title, "item2") == 0);
    assert(gets->value == 2);

    gets = (Item *) genlist_get(&list, 0);
    assert(strcmp(gets->title, "item1") == 0);
    assert(gets->value == 1);

    DONE();
}

void test_genlist_delete() {
    DESCRIBE("genlist_get");
    GenList list;
    Item item1 = { "item1", 1 };
    Item item2 = { "item2", 2 };

    genlist_init(&list);
    genlist_push(&list, &item1);
    genlist_push(&list, &item2);

    Item *gets;

    gets = (Item *) genlist_get(&list, 0);
    assert(strcmp(gets->title, "item1") == 0);
    assert(gets->value == 1);

    genlist_delete(&list, 0);
    gets = (Item *) genlist_get(&list, 0);
    assert(strcmp(gets->title, "item2") == 0);
    assert(gets->value == 2);

    genlist_delete(&list, 0);
    gets = (Item *) genlist_get(&list, 0);
    assert(gets == NULL);

    genlist_delete(&list, 0);
    gets = (Item *) genlist_get(&list, 0);
    assert(gets == NULL);

    DONE();
}

void test_genlist_flush() {
    DESCRIBE("genlist_flush");

    GenList list;
    Item item1 = { "item1", 1 };
    Item item2 = { "item2", 2 };

    genlist_init(&list);
    genlist_push(&list, &item1);
    genlist_push(&list, &item2);

    assert(list.size == GENLIST_INIT_SIZE);
    assert(list.length == 2);
    assert(list.items != NULL);

    genlist_flush(&list);
    assert(list.size == GENLIST_INIT_SIZE);
    assert(list.length == 0);
    assert(list.items != NULL);

    DONE();
}

void test_genlist_free() {
    DESCRIBE("genlist_free");

    GenList empty;

    genlist_init(&empty);
    genlist_free(&empty);

    GenList list;
    Item item1 = { "item1", 1 };
    Item item2 = { "item2", 2 };

    genlist_init(&list);
    genlist_push(&list, &item1);
    genlist_push(&list, &item2);

    assert(list.size == GENLIST_INIT_SIZE);
    assert(list.length == 2);
    assert(list.items != NULL);

    genlist_free(&list);


    DONE();
}
