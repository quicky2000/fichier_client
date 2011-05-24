#ifndef INFORMATION_TABLE_H
#define INFORMATION_TABLE_H

#include "sqlite3.h"
#include <vector>
#include <string>
#include <stdint.h>


class information_table
{
 public:
  void create(const std::string & p_key, const std::string & p_value);
  void update(const std::string & p_key, const std::string & p_value);
  void remove(const std::string & p_key);
  uint32_t get(const std::string & p_key, std::pair<std::string,std::string> & p_data);
  void get_all(std::vector<std::pair<std::string,std::string> > & p_list);

  information_table(void);
  ~information_table(void);
  void set_db(sqlite3 *p_db);
  sqlite3 * get_db(void);

 private:
  const std::string m_table_name;
  sqlite3 *m_db;
  sqlite3_stmt *m_create_stmt;
  sqlite3_stmt *m_update_stmt;
  sqlite3_stmt *m_delete_stmt;
  sqlite3_stmt *m_get_stmt;
  sqlite3_stmt *m_get_all_stmt;
};

#endif
//EOF
