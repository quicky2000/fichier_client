#ifndef TABLE_ACHAT_H
#define TABLE_ACHAT_H

#include "base_table.h"
#include "description_achat.h"

class table_achat:public base_table<achat>
{
 public:
  table_achat(void);
  ~table_achat(void);
  void set_db(sqlite3 *p_db);

  const std::vector<achat*>* get_by_date(const std::string & p_date);

 private:
  sqlite3_stmt *m_get_by_date_stmt;
};


#endif
