#ifndef FICHIER_CLIENT_H
#define FICHIER_CLIENT_H
#include "search_client_item.h"
#include "search_achat_item.h"
#include "search_facture_item.h"
#include "search_facture_client_item.h"
#include "livre_facture.h"
#include "facture_status.h"
#include "facture_reason.h"
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

  // Livre facture related events
  bool check_new_facture_id(uint32_t p_facture_id, uint32_t p_livre_facture_id);
  void treat_livre_facture_selected_event(void);
  void treat_no_more_livre_facture_selected_event(void);
  void treat_livre_facture_id_modif_event(void);
  void treat_livre_facture_content_modif_event(void);
  void treat_delete_livre_facture_event(void);
  void treat_modify_livre_facture_event(void);
  void treat_create_livre_facture_event(void);

  // Non attributed facture related events
  void treat_create_non_attributed_facture_event(void);
  void non_attributed_facture_date_entered(void);
  void non_attributed_facture_reference_selected(void);
  void non_attributed_facture_status_selected(void);
  void non_attributed_facture_reason_selected(void);
  void non_attributed_facture_livre_facture_selected(void);

  // Facture status related events
  void treat_no_more_facture_status_selected_event(void);
  void treat_delete_facture_status_event(void);
  void treat_modify_facture_status_event(void);
  void treat_facture_status_name_modif_event(void);
  void treat_create_facture_status_event(void);
  void treat_facture_status_selected_event(void);

  // Facture reason related events
  void treat_no_more_facture_reason_selected_event(void);
  void treat_delete_facture_reason_event(void);
  void treat_modify_facture_reason_event(void);
  void treat_facture_reason_name_modif_event(void);
  void treat_create_facture_reason_event(void);
  void treat_facture_reason_selected_event(void);

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

  void refresh_non_attributed_facture_status_list(void);
  void refresh_non_attributed_facture_reason_list(void);

 private:
  void remove_tmp_db(void);
  void open_tmp_db(void);
  void close_tmp_db(void);
  void copy(const std::string & p_src,const std::string & p_dest);

  void check_non_attributed_facture(void);
  void check_livre_facture_information(void);
  void refresh_non_attributed_facture_list(void);
  void refresh_livre_facture_list(void);
  void refresh_facture_status_list(void);
  void refresh_facture_reason_list(void);

  void get_facture_by_livre_facture_id(uint32_t p_livre_facture_id,std::vector<search_facture_client_item> & p_result);

  bool m_facture_status_pending_modif;
  bool m_facture_reason_pending_modif;
  fichier_client_UI_if * m_user_interface;
  fichier_client_db *m_db;
  std::string m_db_name;
  static const std::string m_tmp_db_name;
  static const std::string m_version;
};

#endif
//EOF
