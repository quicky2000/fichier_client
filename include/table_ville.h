#ifndef TABLE_VILLE_H
#define TABLE_VILLE_H

#include <stdint.h>
#include <vector>
#include <string>

class sqlite3;
class sqlite3_stmt;
class ville;

class table_ville
{
 public:
  table_ville(void);
  ~table_ville(void);
  void set_db(sqlite3 *p_db);

  void create(const ville & p_ville);
  void update(const ville & p_ville);
  void remove(const ville & p_ville);
  ville* get(uint32_t p_id);
  const std::vector<ville>* get_all(void);
  const std::vector<ville>* get_by_name(const std::string & p_name);
  const std::vector<ville>* get_by_code_postal(const std::string & p_name);

 private:
  sqlite3 *m_db;
  sqlite3_stmt *m_update_stmt;
  sqlite3_stmt *m_delete_stmt;
  sqlite3_stmt *m_get_by_id_stmt;
  sqlite3_stmt *m_get_all_stmt;
  sqlite3_stmt *m_get_by_name_stmt;
  sqlite3_stmt *m_get_by_code_postal_stmt;

};

#endif
