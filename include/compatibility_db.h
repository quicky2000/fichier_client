#ifndef COMPATIBILITY_DB_H
#define COMPATIBILITY_DB_H

#include "information_table.h"
#include "table_ville.h"
#include "table_facture.h"
#include "table_livre_facture.h"
#include "description_type_achat.h"
#include "type_achat.h"
#include "description_marque.h"
#include "table_facture_status.h"
#include "marque.h"
#include "named_table.h"
#include "table_client.h"
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

  table_facture m_table_facture;
  table_livre_facture m_table_livre_facture;
  information_table m_information_table;
  table_ville m_table_ville;
  table_facture_status m_table_facture_status;
  named_table<marque> m_table_marque;
  named_table<type_achat> m_table_type_achat;
  table_client m_table_client;
  sqlite3 *m_db;
};

#endif // COMPATIBILITY_DB_H
