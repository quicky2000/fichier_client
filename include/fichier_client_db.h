#ifndef FICHIER_CLIENT_DB_H
#define FICHIER_CLIENT_DB_H

#include "table_livre_facture.h"
#include "table_ville.h"
#include "table_named_item.h"
#include "marque.h"
#include "named_table.h"
#include "description_type_achat.h"
#include "table_achat.h"
#include "table_client.h"
#include <string>
#include <vector>
#include <stdint.h>

class sqlite3;
class livre_facture;

class fichier_client_db
{
 public:
  fichier_client_db(const std::string &p_name);
  ~fichier_client_db(void);

  //Management of livre facture
  void create(const livre_facture & p_livre_facture);
  void update(const livre_facture & p_livre_facture);
  void remove(const livre_facture & p_livre_facture);
  livre_facture* get_livre_facture(uint32_t p_id);
  const std::vector<livre_facture> * get_all_livre_facture(void);
  const std::vector<livre_facture> * get_livre_facture_containing_date(const std::string & p_date);
  
  //Management of ville
  void create(const ville & p_ville);
  void update(const ville & p_ville);
  void remove(const ville & p_ville);
  ville* get_ville(uint32_t p_id);
  const std::vector<ville> * get_all_ville(void);
  const std::vector<ville> * get_ville_by_name(const std::string & p_name);
  const std::vector<ville> * get_ville_by_code_postal(const std::string & p_code_postal);
  
  //Management of marque
  void create(const marque & p_marque);
  void update(const marque & p_marque);
  void remove(const marque & p_marque);
  marque* get_marque(uint32_t p_id);
  const std::vector<marque> * get_all_marque(void);
  const std::vector<marque> * get_marque_by_name(const std::string & p_name);
  
  //Management of type_achat
  void create(const type_achat & p_type_achat);
  void update(const type_achat & p_type_achat);
  void remove(const type_achat & p_type_achat);
  type_achat* get_type_achat(uint32_t p_id);
  const std::vector<type_achat*> * get_all_type_achat(void);
  const std::vector<type_achat*> * get_type_achat_by_name(const std::string & p_name);
  
  //Management of achat
  void create(const achat & p_achat);
  void update(const achat & p_achat);
  void remove(const achat & p_achat);
  achat* get_achat(uint32_t p_id);
  const std::vector<achat*> * get_all_achat(void);
  const std::vector<achat*> * get_achat_by_date(const std::string & p_date);
  
  //Management of client
  void create(const client & p_client);
  void update(const client & p_client);
  void remove(const client & p_client);
  client* get_client(uint32_t p_id);
  const std::vector<client*> * get_all_client(void);
  
 private:

  table_livre_facture m_table_livre_facture;
  table_ville m_table_ville;
  table_named_item<marque> m_table_marque;
  named_table<type_achat> m_table_type_achat;
  table_achat m_table_achat;
  table_client m_table_client;
  sqlite3 *m_db;
};

#endif // QUICKY_REPOSITORY
