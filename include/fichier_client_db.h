#ifndef FICHIER_CLIENT_DB_H
#define FICHIER_CLIENT_DB_H

#include "table_livre_facture.h"
#include "table_facture.h"
#include "table_facture_status.h"
#include "table_ville.h"
#include "named_table.h"
#include "description_marque.h"
#include "description_type_achat.h"
#include "description_facture_status.h"
#include "description_facture_reason.h"
#include "table_achat.h"
#include "table_client.h"
#include "information_table.h"
#include "search_client_item.h"
#include "search_achat_item.h"
#include "search_facture_item.h"
#include "search_facture_client_item.h"

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

  bool is_modified(void)const;
  void save(void);

  //Management of information table
  void create_information(const std::string & p_key, const std::string &p_value);
  void update_information(const std::string & p_key, const std::string &p_value);
  void remove_information(const std::string & p_key);
  uint32_t get_information(const std::string & p_key, std::pair<std::string,std::string> & p_data);
  void get_all_information(std::vector<std::pair<std::string,std::string> > & p_list);

  //Management of livre facture table
  void create( livre_facture & p_livre_facture);
  void update(const livre_facture & p_livre_facture);
  void remove(const livre_facture & p_livre_facture);
  uint32_t get_livre_facture(uint32_t p_id,livre_facture & p_data);
  void get_all_livre_facture(std::vector<livre_facture> & p_list);
  void get_livre_facture_containing_date(const std::string & p_date,std::vector<livre_facture> & p_result);
  
  //Management of facture table
  void create( facture & p_facture);
  void update(const facture & p_facture);
  void remove(const facture & p_facture);
  uint32_t get_facture(uint32_t p_id,facture & p_data);
  void get_all_facture(std::vector<facture> & p_list);
  void get_by_date(const std::string & p_date,std::vector<facture> & p_result);
  void get_by_date_and_client_id(const std::string & p_date,uint32_t p_client_id,std::vector<facture> & p_result);
  void get_by_livre_facture(uint32_t p_livre_facture_id,std::vector<facture> & p_result);
  void get_by_livre_facture_and_ref(uint32_t p_facture_ref, uint32_t p_livre_facture_id,std::vector<facture> & p_result);
  void get_by_status(uint32_t p_facture_status_id,std::vector<facture> & p_result);
  void get_by_reason(uint32_t p_facture_reason_id,std::vector<facture> & p_result);
  std::pair<std::string,std::string> get_min_max_date(uint32_t p_livre_facture_id);

  //Management of ville table
  void create( ville & p_ville);
  void update(const ville & p_ville);
  void remove(const ville & p_ville);
  uint32_t get_ville(uint32_t p_id,ville & p_data);
  void get_all_ville(std::vector<ville> & p_list);
  void get_ville_by_name(const std::string & p_name,std::vector<ville> & p_result, bool p_exact=false);
  void get_ville_by_code_postal(const std::string & p_code_postal,std::vector<ville> & p_result);
  void get_city_by_name_and_postal_code(const std::string & p_name,const std::string & p_code_postal,std::vector<ville> & p_result);
  
  //Management of marque table
  void create( marque & p_marque);
  void update(const marque & p_marque);
  void remove(const marque & p_marque);
  uint32_t get_marque(uint32_t p_id,marque & p_data);
  void get_all_marque(std::vector<marque> & p_list);
  void get_marque_by_name(const std::string & p_name,std::vector<marque> & p_result, bool p_exact);
  
  //Management of facture_reason table
  void create( facture_reason & p_facture_reason);
  void update(const facture_reason & p_facture_reason);
  void remove(const facture_reason & p_facture_reason);
  uint32_t get_facture_reason(uint32_t p_id,facture_reason & p_data);
  void get_all_facture_reason(std::vector<facture_reason> & p_list);
  void get_facture_reason_by_name(const std::string & p_name,std::vector<facture_reason> & p_result,bool p_exact);
  
  //Management of type_achat table
  void create( type_achat & p_type_achat);
  void update(const type_achat & p_type_achat);
  void remove(const type_achat & p_type_achat);
  uint32_t get_type_achat(uint32_t p_id,type_achat & p_data);
  void get_all_type_achat(std::vector<type_achat> & p_list);
  void get_type_achat_by_name(const std::string & p_name,std::vector<type_achat> & p_result, bool p_exact);
  
  //Management of facture_status table
  void create( facture_status & p_facture_status);
  void update(const facture_status & p_facture_status);
  void remove(const facture_status & p_facture_status);
  uint32_t get_facture_status(uint32_t p_id,facture_status & p_data);
  void get_all_facture_status(std::vector<facture_status> & p_list);
  void get_facture_status_by_name(const std::string & p_name,std::vector<facture_status> & p_result,bool p_exact);
  
  //Management of achat table
  void create( achat & p_achat);
  void update(const achat & p_achat);
  void remove(const achat & p_achat);
  uint32_t get_achat(uint32_t p_id,achat & p_data);
  void get_all_achat(std::vector<achat> & p_list);
  void get_by_facture_id(uint32_t p_facture_id,std::vector<achat> & p_result);
  void get_by_type_id(uint32_t p_id,std::vector<achat> & p_result);
  void get_by_brand_id(uint32_t p_id,std::vector<achat> & p_result);
  
  //Management of client table
  void create( client & p_client);
  void update(const client & p_client);
  void remove(const client & p_client);
  uint32_t get_client(uint32_t p_id,client & p_data);
  void get_all_client(std::vector<client> & p_list );
  void get_customer_by_city(uint32_t p_city_id,std::vector<client> & p_list );

  //Mangement of jointures
  void get_complete_client(uint32_t p_id,search_client_item & p_result);
  void search_client(const std::string & p_name, const std::string & p_first_name,const std::string & p_address, const std::string & p_city, std::vector<search_client_item> & p_result);
  void get_achat_by_client_id(uint32_t p_client_id,std::vector<search_achat_item> & p_result);
  void get_purchase_by_bill_id(uint32_t p_facture_id,std::vector<search_achat_item> & p_result);
  void get_facture_by_client_id(uint32_t p_client_id,std::vector<search_facture_item> & p_result);
  void get_facture_by_livre_facture_id(uint32_t p_livre_facture_id,std::vector<search_facture_item> & p_result);

  void check_db_coherency(void);
  
 private:
  bool m_modified;
  table_livre_facture m_table_livre_facture;
  table_facture m_table_facture;
  table_ville m_table_ville;
  named_table<marque> m_table_marque;
  named_table<type_achat> m_table_type_achat;
  named_table<facture_reason> m_table_facture_reason;
  table_facture_status m_table_facture_status;
  table_achat m_table_achat;
  table_client m_table_client;
  information_table m_information_table;
  sqlite3 *m_db;
  sqlite3_stmt *m_get_complete_client_stmt;
  sqlite3_stmt *m_search_client_stmt;
  sqlite3_stmt *m_search_achat_stmt;
  sqlite3_stmt *m_search_purchase_by_bill_id_stmt;
  sqlite3_stmt *m_search_facture_by_client_id_stmt;
  sqlite3_stmt *m_search_facture_by_livre_facture_id_stmt;
  static const std::string m_schema_version;
};

#endif // FICHIER_CLIENT_DB_H
