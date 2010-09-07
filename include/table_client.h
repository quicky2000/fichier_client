#ifndef TABLE_CLIENT_H
#define TABLE_CLIENT_H

#include "base_table.h"
#include "description_client.h"

class table_client:public base_table<client>
{
 public:
  table_client(void);
  ~table_client(void);
  void set_db(sqlite3 *p_db);

 private:
  sqlite3_stmt *m_get_by_date_stmt;
};


#endif
