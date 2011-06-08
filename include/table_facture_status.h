#ifndef TABLE_FACTURE_STATUS_H
#define TABLE_FACTURE_STATUS_H

#include "named_table.h"
#include "description_facture_status.h"

class table_facture_status: public named_table<facture_status>
{
 public:
  table_facture_status(void);
  ~table_facture_status(void);
  void set_db(sqlite3 *p_db);
 private:
  void check_mandatory_status(const std::string & p_status);
};

#endif
