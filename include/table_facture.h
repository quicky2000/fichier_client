#ifndef TABLE_FACTURE_H
#define TABLE_FACTURE_H

#include "base_table.h"
#include "description_facture.h"

class table_facture: public base_table<facture>
{
 public:
  table_facture(void);
  ~table_facture(void);
  void set_db(sqlite3 *p_db);

  void get_by_date(const std::string & p_date,std::vector<facture> & p_result);
  void get_by_date_and_client_id(const std::string & p_date,uint32_t p_client_id,std::vector<facture> & p_result);
  void get_by_livre_facture(uint32_t p_livre_id,std::vector<facture> & p_result);
  void get_by_livre_facture_and_ref(uint32_t p_facture_ref, uint32_t p_livre_facture_id,std::vector<facture> & p_result);
  void get_by_status(uint32_t p_facture_status_id,std::vector<facture> & p_result);
  void get_by_reason(uint32_t p_facture_reason_id,std::vector<facture> & p_result);
 
 private:
  sqlite3_stmt *m_get_by_date_stmt;
  sqlite3_stmt *m_get_by_date_and_client_id_stmt;
  sqlite3_stmt *m_get_by_livre_facture_stmt;
  sqlite3_stmt *m_get_by_livre_facture_and_ref_stmt;
  sqlite3_stmt *m_get_by_status_stmt;
  sqlite3_stmt *m_get_by_reason_stmt;

};

#endif
