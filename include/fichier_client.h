#ifndef FICHIER_CLIENT_H
#define FICHIER_CLIENT_H
#include "search_client_item.h"
#include "search_achat_item.h"
#include "search_facture_item.h"
#include "search_facture_client_item.h"
#include "livre_facture.h"
#include "facture.h"
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
  void treat_open_database_event(const std::string & p_name);
  void treat_close_database_event(void);
  void treat_save_database_event(void);
  void treat_save_as_database_event(const std::string & p_name);
  void treat_import_external_sql_event(const std::string & p_name);
  bool need_save(void)const;

  // Search client related events
  void treat_search_customer_criteria_modification_event(void);
  void treat_search_customer_customer_selected_event(void);
  void treat_search_customer_no_more_customer_selected_event(void);
  void treat_search_customer_add_customer_event(void);
  void treat_search_customer_modify_customer_event(void);
  void treat_search_customer_delete_customer_event(void);

  void treat_leave_customer_data_for_search_event(void);

  // Customer data identity information related events
  void treat_customer_postal_code_modification_event(void);
  void treat_customer_city_selection_event(void);
  void treat_identity_content_modification_event(void);

  // Customer data identity actions related events
  void treat_customer_data_create_customer_event(void);
  void treat_customer_data_modify_customer_event(void);
  void treat_customer_data_delete_customer_event(void);

  // Customer data bill information related events
  void treat_customer_data_bill_date_modification_event(void);
  void treat_customer_data_bill_book_selection_event(void);
  void treat_customer_data_bill_ref_selection_event(void);
  void treat_customer_data_bill_status_selection_event(void);

  // Customer data bill list related events
  void treat_customer_data_bill_selected_event(void);
  void treat_customer_data_no_more_bill_selected_event(void);

  // Customer data bill actions related events
  void treat_customer_data_bill_creation_event(void);
  void treat_customer_data_bill_modification_event(void);
  void treat_customer_data_bill_deletion_event(void);

  // Customer data purchase information related events
  void treat_customer_data_purchase_brand_selection_event(void);
  void treat_customer_data_purchase_type_selection_event(void);
  void treat_customer_data_purchase_reference_modification_event(void);
  void treat_customer_data_purchase_euro_price_modification_event(void);
  void treat_customer_data_purchase_franc_price_modification_event(void);
  void treat_customer_data_purchase_warranty_modification_event(void);

  // Customer data purchase list related events
  void treat_customer_data_purchase_selected_event(void);
  void treat_customer_data_no_more_purchase_selected_event(void);

  // Customer data purchase actions related events
  void treat_customer_data_purchase_creation_event(void);
  void treat_customer_data_purchase_modification_event(void);
  void treat_customer_data_purchase_deletion_event(void);

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
  void treat_facture_selected_event(void);
  void treat_no_more_facture_selected_event(void);
  void treat_delete_non_attributed_facture_event(void);
  void treat_modify_non_attributed_facture_event(void);

  // Facture status information related events
  void treat_facture_status_name_modif_event(void);

  // Facture status list related events
  void treat_facture_status_selected_event(void);
  void treat_no_more_facture_status_selected_event(void);

  // Facture status actions related events
  void treat_delete_facture_status_event(void);
  void treat_modify_facture_status_event(void);
  void treat_create_facture_status_event(void);

  // Facture reason information related events
  void treat_facture_reason_name_modif_event(void);

  // Facture reason list related events
  void treat_no_more_facture_reason_selected_event(void);
  void treat_facture_reason_selected_event(void);

  // Facture reason actions related events
  void treat_delete_facture_reason_event(void);
  void treat_modify_facture_reason_event(void);
  void treat_create_facture_reason_event(void);

  // Brand information related events
  void treat_purchase_configuration_brand_name_modif_event(void);
  // Brand list related events
  void treat_purchase_configuration_brand_selected_event(void);
  void treat_purchase_configuration_no_more_brand_selected_event(void);
  // Brand actions related events
  void treat_purchase_configuration_create_brand_event(void);
  void treat_purchase_configuration_delete_brand_event(void);
  void treat_purchase_configuration_modify_brand_event(void);
  // Type information related events
  void treat_purchase_configuration_type_name_modif_event(void);
  // type list related events
  void treat_purchase_configuration_type_selected_event(void);
  void treat_purchase_configuration_no_more_type_selected_event(void);
  // Type actions related events
  void treat_purchase_configuration_create_type_event(void);
  void treat_purchase_configuration_delete_type_event(void);
  void treat_purchase_configuration_modify_type_event(void);

  // City information related events
  void treat_city_criteria_modification_event(void);

  // City list related events
  void treat_city_selection_event(void);
  void treat_city_no_more_selected_event(void);

  // City action related events
  void treat_city_create_event(void);
  void treat_city_modify_event(void);
  void treat_city_delete_event(void);


  // Operations on database
  void treat_check_db_coherency_event(void);

 private:
  void remove_tmp_db(void);
  void open_tmp_db(void);
  void close_tmp_db(void);
  void copy(const std::string & p_src,const std::string & p_dest);
  void save(void);
  void save_as(const std::string & p_name);

  void manage_features(bool p_enabled);

  // Customer search

  // Customer data
  void set_customer_information_enabled(bool p_enabled);
  void set_customer_identity_enabled(bool p_enabled);
  void set_customer_data_bill_enabled(bool p_enabled);
  void set_customer_data_purchase_enabled(bool p_enabled);

  void refresh_customer_data_bill_list(void);
  void refresh_customer_data_purchase_list(void);
  void refresh_customer_bill_actions(void);
  void refresh_customer_data_facture_status_list(void);
  void refresh_customer_data_purchase_actions(void);
  void check_customer_identity(void);

  void clear_purchase_informations(void);

  // Bill book widget
  void refresh_livre_facture_list(void);
  void check_non_attributed_facture(void);
  void check_livre_facture_information(void);
  void refresh_non_attributed_facture_list(void);
  void refresh_non_attributed_facture_status_list(void);
  void refresh_non_attributed_facture_reason_list(void);

  // Facture status
  void refresh_facture_status_list(void);

  // Facture reason
  void refresh_facture_reason_list(void);

  // City
  void refresh_city_list(void);
  void refresh_city_actions(void);

  // General
  void refresh_brand_list(void);
  void refresh_purchase_type_list(void);
  

  void get_facture_by_livre_facture_id(uint32_t p_livre_facture_id,std::vector<search_facture_client_item> & p_result);
  void get_remaining_refs(const livre_facture & p_livre,std::vector<facture> & l_facture_list,std::vector<uint32_t> & p_remaining_refs);

  uint32_t m_current_customer_id;
  bool m_facture_status_pending_modif;
  bool m_facture_reason_pending_modif;
  bool m_purchase_brand_pending_modif;
  bool m_purchase_type_pending_modif;
  bool m_city_pending_modif;
  fichier_client_UI_if * m_user_interface;
  fichier_client_db *m_db;
  std::string m_db_name;
  static const std::string m_tmp_db_name;
  static const std::string m_version;
};

#endif
//EOF
