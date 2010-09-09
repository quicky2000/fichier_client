#ifndef TABLE_LIVRE_FACTURE_H
#define TABLE_LIVRE_FACTURE_H

#include "base_table.h"
#include "description_livre_facture.h"

class table_livre_facture: public base_table<livre_facture>
{
 public:
  table_livre_facture(void);
  ~table_livre_facture(void);
  void set_db(sqlite3 *p_db);

  const std::vector<livre_facture*>* containing_date(const std::string & p_date);

 private:
  sqlite3_stmt *m_containing_date_stmt;

};

#endif
