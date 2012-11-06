#include "simpledb.h"
#include <stdio.h>
#include <stdlib.h>

static void 
test_write() {
    struct db *dbase = db_create(sizeof(double) * 4);
    double arr[4] = {1,2,3,4};
    db_append(dbase, (char *)arr);
    arr[2] = 8;
    db_append(dbase, (char *)arr);
    db_write(&dbase, "gallery.db");
}

static void 
test_open() {
    struct db *dbase = db_open("gallery_new.db");
    const int num = db_itemnum(dbase);
    const int sz  = db_itemsz(dbase) / sizeof(double);
    for (int i = 0; i < num; ++i) {
        const double *data = db_retrival(dbase, double, i);
        for (int j = 0; j < sz; ++j) {
            printf("%lf, ", data[j]);
        }
        printf("\n");
    }
    db_close(&dbase, NULL);
}

static void 
test_append() {
    struct db *dbase = db_open("gallery.db");
    double arr[4] = {6,7,8,9};
    db_append(dbase, (char *)arr);
    db_close(&dbase, "gallery_new.db");
}

int main(int argc, char *argv[]) {
    test_append();
    test_open();

    getchar();
    return 0;
}
