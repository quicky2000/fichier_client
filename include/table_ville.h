#ifndef TABLE_VILLE_H
#define TABLE_VILLE_H

#include "named_table.h"
#include "description_ville.h"

class table_ville: public named_table<ville>
{
 public:
  table_ville(void);
  ~table_ville(void);
  void set_db(sqlite3 *p_db);

  void get_by_code_postal(const std::string & p_name,std::vector<ville> & p_result);
  void get_by_name_and_postal_code(const std::string & p_name,const std::string & p_code_postal,std::vector<ville> & p_result);
 private:
  sqlite3_stmt *m_get_by_code_postal_stmt;
  sqlite3_stmt *m_get_by_name_and_postal_code_stmt;
};

#endif
