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

  void get_by_facture_id(uint32_t p_id,std::vector<achat> & p_result);
  void get_by_type_id(uint32_t p_id,std::vector<achat> & p_result);
  void get_by_brand_id(uint32_t p_id,std::vector<achat> & p_result);
 private:
  sqlite3_stmt *m_get_by_facture_id_stmt;
  sqlite3_stmt *m_get_by_type_id_stmt;
  sqlite3_stmt *m_get_by_brand_id_stmt;
};


#endif
