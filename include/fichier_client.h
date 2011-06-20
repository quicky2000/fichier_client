#ifndef FICHIER_CLIENT_H
#define FICHIER_CLIENT_H
#include "search_client_item.h"
#include "search_achat_item.h"
#include "search_facture_item.h"
#include "search_facture_client_item.h"
#include "livre_facture.h"
#include "facture_status.h"
#include "achat.h"
#include <string>
#include <vector>

class fichier_client_db;
class fichier_client_UI_if;

class fichier_client
{
 public:
  fichier_client(void);
  ~fichier_client(void);

  void set_user_interface(fichier_client_UI_if * p_user_interface);

  // Database file manipulation
  void import_external_sql(const std::string & p_name);
  void open_db(const std::string & p_name);
  void close_db(void);
  void save(void);
  void save_as(const std::string & p_name);
  bool need_save(void)const;

  void create_non_attributed_facture(uint32_t p_livre_facture_id);
  void livre_facture_selected(uint32_t p_id);
  void livre_facture_selection_changed(bool p_empty_selection);

  // Operations on database
  void check_db_coherency(void);

  // Client related operqtions
  void search_client(const std::string & p_name,
		     const std::string & p_first_name,
		     const std::string & p_city,
		     std::vector<search_client_item> & p_result);

  // Achat related operations
  void get_achat_by_client_id(uint32_t p_client_id,
			      std::vector<search_achat_item> & p_result);

  // Facture related operations
  void get_facture_by_client_id(uint32_t p_client_id,
				std::vector<search_facture_item> & p_result);

  void get_facture_by_livre_facture_id(uint32_t p_livre_facture_id,std::vector<search_facture_client_item> & p_result);

  // Livre facture related operations
  void create( livre_facture & p_livre_facture);
  void remove(const livre_facture & p_livre_facture);
  void update(const livre_facture & p_livre_facture);
  uint32_t get_livre_facture(uint32_t p_id,livre_facture & p_data);
  void get_all_livre_facture(std::vector<livre_facture> & p_list);

  // Facture status related operations
  void create( facture_status & p_facture_status);
  void update(const facture_status & p_facture_status);
  void remove(const facture_status & p_facture_status);
  uint32_t get_facture_status(uint32_t p_id,facture_status & p_data);
  void get_all_facture_status(std::vector<facture_status> & p_list);
  void get_facture_status_by_name(const std::string & p_name,std::vector<facture_status> & p_result);


 

 private:
  void remove_tmp_db(void);
  void open_tmp_db(void);
  void close_tmp_db(void);
  void copy(const std::string & p_src,const std::string & p_dest);

  fichier_client_UI_if * m_user_interface;
  fichier_client_db *m_db;
  std::string m_db_name;
  static const std::string m_tmp_db_name;
  static const std::string m_version;
};

#endif
//EOF
