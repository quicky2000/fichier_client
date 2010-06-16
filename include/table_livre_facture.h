#ifndef TABLE_LIVRE_FACTURE_H
#define TABLE_LIVRE_FACTURE_H

#include <stdint.h>
#include <vector>
#include <string>

class sqlite3;
class sqlite3_stmt;
class livre_facture;

class table_livre_facture
{
 public:
  table_livre_facture(void);
  ~table_livre_facture(void);
  void set_db(sqlite3 *p_db);

  void create(const livre_facture & p_livre_facture);
  void update(const livre_facture & p_livre_facture);
  void remove(const livre_facture & p_livre_facture);
  livre_facture* get(uint32_t p_id);
  const std::vector<livre_facture>* get_all(void);
  const std::vector<livre_facture>* containing_date(const std::string & p_date);

 private:
  sqlite3 *m_db;
  sqlite3_stmt *m_update_livre_facture_stmt;
  sqlite3_stmt *m_delete_livre_facture_stmt;
  sqlite3_stmt *m_get_by_id_livre_facture_stmt;
  sqlite3_stmt *m_get_all_livre_facture_stmt;
  sqlite3_stmt *m_containing_date_stmt;

};

#endif
