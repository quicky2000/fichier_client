#ifndef COMPATIBILITY_DB_H
#define COMPATIBILITY_DB_H

#include "information_table.h"

#include <string>

class sqlite3;

class compatibility_db
{
 public:
  compatibility_db(const std::string &p_name,const std::string & p_current_version);
  ~compatibility_db(void);

 private:
  void upgrade(const std::string & p_from_version);
  void upgrade_from_1_0(void);

  information_table m_information_table;
  sqlite3 *m_db;
};

#endif // COMPATIBILITY_DB_H
