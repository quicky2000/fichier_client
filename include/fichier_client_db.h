#ifndef FICHIER_CLIENT_DB_H
#define FICHIER_CLIENT_DB_H

#include "table_livre_facture.h"
#include "table_ville.h"
#include "named_table.h"
#include "description_marque.h"
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

  //Management of livre facture table
  void create(const livre_facture & p_livre_facture);
  void update(const livre_facture & p_livre_facture);
  void remove(const livre_facture & p_livre_facture);
  uint32_t get_livre_facture(uint32_t p_id,livre_facture & p_data);
  void get_all_livre_facture(std::vector<livre_facture> & p_list);
  void get_livre_facture_containing_date(const std::string & p_date,std::vector<livre_facture> & p_result);
  
  //Management of ville table
  void create(const ville & p_ville);
  void update(const ville & p_ville);
  void remove(const ville & p_ville);
  uint32_t get_ville(uint32_t p_id,ville & p_data);
  void get_all_ville(std::vector<ville> & p_list);
  void get_ville_by_name(const std::string & p_name,std::vector<ville> & p_result);
  void get_ville_by_code_postal(const std::string & p_code_postal,std::vector<ville> & p_result);
  
  //Management of marque table
  void create(const marque & p_marque);
  void update(const marque & p_marque);
  void remove(const marque & p_marque);
  uint32_t get_marque(uint32_t p_id,marque & p_data);
  void get_all_marque(std::vector<marque> & p_list);
  void get_marque_by_name(const std::string & p_name,std::vector<marque> & p_result);
  
  //Management of type_achat table
  void create(const type_achat & p_type_achat);
  void update(const type_achat & p_type_achat);
  void remove(const type_achat & p_type_achat);
  uint32_t get_type_achat(uint32_t p_id,type_achat & p_data);
  void get_all_type_achat(std::vector<type_achat> & p_list);
  void get_type_achat_by_name(const std::string & p_name,std::vector<type_achat> & p_result);
  
  //Management of achat table
  void create(const achat & p_achat);
  void update(const achat & p_achat);
  void remove(const achat & p_achat);
  uint32_t get_achat(uint32_t p_id,achat & p_data);
  void get_all_achat(std::vector<achat> & p_list);
  void get_achat_by_date(const std::string & p_date,std::vector<achat> & p_result);
  void get_achat_by_client_id(uint32_t p_client_id,std::vector<achat> & p_result);
  
  //Management of client table
  void create(const client & p_client);
  void update(const client & p_client);
  void remove(const client & p_client);
  uint32_t get_client(uint32_t p_id,client & p_data);
  void get_all_client(std::vector<client> & p_list );

  void check_db_coherency(void);
  
 private:

  table_livre_facture m_table_livre_facture;
  table_ville m_table_ville;
  named_table<marque> m_table_marque;
  named_table<type_achat> m_table_type_achat;
  table_achat m_table_achat;
  table_client m_table_client;
  sqlite3 *m_db;
};

#endif // FICHIER_CLIENT_DB_H
