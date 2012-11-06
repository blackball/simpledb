#ifndef SIMPLE_DB_H
#define SIMPLE_DB_H

struct db;

struct  db* db_create(const int itemsz);
void db_write(struct db **pdbase, const char *name);
struct db* db_open(const char *name);

/* provide new name to write another file, this is useful for appending */
void db_close(struct db **pdbase, const char *newname /*= NULL*/);
int db_itemsz(const struct db *dbase);
int db_itemnum(const struct db *dbase);
void db_append(struct db *dbase, char *data);
const char* db_index(const struct db *dbase, int idx);

#define db_retrival(dbase, type, idx) \
    (const type *) db_index(dbase, idx)

#endif 
