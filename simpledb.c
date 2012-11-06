#include "simpledb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct db_node {
    struct db_node *next;
    char data[1];
};

struct db_hdr {
    int itemsz;
    int num;
};

struct db {
    struct db_hdr hdr;
    struct db_node *head;
    struct db_node *tail;
};

static struct db_node*
_new_node(int len) {
    struct db_node *node = (struct db_node*)malloc(sizeof(*node) + sizeof(char) * len);
    assert( node );
    node->next = NULL;
    return node;
}

static void
_free_node(struct db_node **n) {
    if ( n ) {
        free( *n );
        *n = 0;
    }        
}

static void
_add_node(struct db *dbase, struct db_node *node ) {
    assert( dbase && node );
    if ( dbase->head == NULL ) {
        dbase->head = node;
        dbase->tail = node;
        dbase->tail->next = NULL;
    }
    else {
        dbase->tail->next = node;
        dbase->tail = node;
        dbase->tail->next = NULL;
    }

    dbase->hdr.num ++;
}

static void
_free_db(struct db **dbase) {
    if ( dbase ) {
        struct db *d = *dbase;
        struct db_node *curr = d->head;
        struct db_node *next;
        if (curr != NULL) {
            do {
                next = curr->next;
                _free_node( &curr );
            } while ( curr != NULL );
        }
        
        free( *dbase );
        *dbase = 0;
    }
}

struct db*
db_create(const int itemsz) {
    struct db *dbase = (struct db*)malloc(sizeof(*dbase));
    assert( dbase );
    dbase->hdr.itemsz = itemsz;
    dbase->hdr.num = 0;
    dbase->head = dbase->tail = NULL;
    return dbase;
}

void
db_write(struct db **pdbase, const char *name) {
    assert( pdbase && name );
    struct db *dbase = *pdbase;
    FILE *f = fopen(name, "wb");
    assert(f);
    fwrite(&(dbase->hdr), sizeof(struct db_hdr), 1, f);

    const int num = dbase->hdr.num;
    const int itemsz = dbase->hdr.itemsz;
    struct db_node *curr = dbase->head;

    int i;
    for (i = 0; i < num; ++i) {
        assert( curr );
        fwrite(curr->data, sizeof(char) * itemsz, 1, f);
        curr = curr->next;
    }

    if ( i != num ) {
        fprintf(stderr, "unexpexted error happened!\n");
    }
    fclose(f);
    _free_db( pdbase );
}

void
db_append(struct db *dbase, char *data) {
    assert( dbase && data );
    struct db_node *node = _new_node(dbase->hdr.itemsz);
    memcpy(node->data, data, dbase->hdr.itemsz);
    _add_node(dbase, node);
}

void
db_close(struct db **pdbase, const char *newname) {
    if (newname != NULL) {
        db_write(pdbase, newname);
    }
    else
        _free_db( pdbase );
}

int
db_itemsz(const struct db *dbase) {
    assert( dbase );
    return dbase->hdr.itemsz;
}

int 
db_itemnum(const struct db *dbase) {
    assert( dbase );
    return dbase->hdr.num;
}

const char*
db_index(const struct db *dbase, const int idx) {
    assert( dbase && (idx >= 0));

    if ( dbase->hdr.num <= idx ) {
        fprintf(stderr, "indexing out of range!\n");
        return NULL;
    }

    struct db_node *curr = dbase->head;
    int i = idx;
    while (i) {
        curr = curr->next;
        --i;
    }
    assert( curr );
    
    return curr->data;
}

struct db*
db_open(const char *name) {
    struct db *dbase = (struct db*)malloc(sizeof( *dbase) );
    dbase->head = dbase->tail = NULL;

    assert(dbase);
    FILE *f = fopen( name, "rb" );
    assert( f );
    fread(&(dbase->hdr), sizeof(struct db_hdr), 1, f);
    /* @TODO check the hdr */
    const int itemsz = dbase->hdr.itemsz;
    const int itemnum = dbase->hdr.num;

    for (int i = 0; i < itemnum; ++i) {
        struct db_node *node = _new_node(sizeof(char) * itemsz);
        assert( node );
        fread(node->data, sizeof(char) * itemsz, 1, f);
        _add_node(dbase, node);
    }

    dbase->hdr.num = itemnum;
    return dbase;
}

