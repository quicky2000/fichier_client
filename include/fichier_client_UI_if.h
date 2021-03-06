#ifndef _FICHIER_CLIENT_UI_IF_
#define _FICHIER_CLIENT_UI_IF_

#include <vector>
#include "facture_status.h"
#include "type_achat.h"
#include "marque.h"
#include "ville.h"
#include "coherency_report_item.h"
class facture;

class fichier_client_UI_if
{
 public:
  // Comunication with user
  virtual void display_warning_message(const std::string & p_title,const std::string & p_text)=0;
  virtual void display_information_message(const std::string & p_title,const std::string & p_text)=0;
  virtual void display_status_message(const std::string & p_text)=0;
  virtual const std::string get_readable_date(const std::string & p_date)const=0;
  virtual void set_application_title(const std::string & p_name)=0;
  virtual bool ask_yes_no_qestion(const std::string & p_title, const std::string & p_question)=0;

  // Interactions with main actions
  virtual void set_import_file_action_enabled(bool p_enabled)=0;
  virtual void set_open_file_action_enabled(bool p_enabled)=0;
  virtual void set_save_file_action_enabled(bool p_enabled)=0;
  virtual void set_save_as_file_action_enabled(bool p_enabled)=0;
  virtual void set_close_file_action_enabled(bool p_enabled)=0;
  virtual void set_exit_file_action_enabled(bool p_enabled)=0;

  // Interactions with customer search information
  virtual void set_customer_search_enabled(bool p_enabled)=0;
  virtual void set_focus_on_customer_search(void)=0;
  virtual void clear_customer_search_criteria(void)=0;
  virtual const std::string get_search_customer_name(void)const=0;
  virtual const std::string get_search_customer_first_name(void)const=0;
  virtual const std::string get_search_customer_address(void)const=0;
  virtual const std::string get_search_customer_city(void)const=0;
  virtual uint32_t get_selected_customer(void)const=0;
  virtual void update_search_customer_list(const std::vector<search_client_item> & p_list)=0;
  virtual void update_search_customer_list_achat(const std::vector<search_achat_item> & p_list)=0;
  virtual void update_search_customer_list_facture(const std::vector<search_facture_item> & p_list)=0;
  virtual void set_customer_search_add_customer_enabled(bool p_enabled)=0;
  virtual void set_customer_search_modify_customer_enabled(bool p_enabled)=0;
  virtual void set_customer_search_delete_customer_enabled(bool p_enabled)=0;

  // Interactions with Customer identity information
  virtual void set_focus_on_customer_data(void)=0;
  virtual void set_customer_name(const std::string & p_name)=0;
  virtual const std::string get_customer_name(void)const=0;
  virtual void set_customer_first_name(const std::string & p_first_name)=0;
  virtual const std::string get_customer_first_name(void)const=0;
  virtual void set_customer_phone(const std::string & p_phone)=0;
  virtual const std::string get_customer_phone(void)const=0;
  virtual bool is_customer_phone_empty(void)const=0;
  virtual bool is_customer_phone_complete(void)const=0;
  virtual void set_customer_address(const std::string & p_address)=0;
  virtual const std::string get_customer_address(void)const=0;
  virtual void set_customer_postal_code(const std::string & p_postal_code)=0;
  virtual const std::string get_customer_postal_code(void)const=0;
  virtual void set_customer_city_list(const std::vector<ville> & p_list)=0;
  virtual const ville * get_customer_city(void)const=0;
  virtual void set_customer_data_identity_fields_enabled(bool p_enabled)=0;
  virtual void set_customer_data_create_customer_enabled(bool p_enabled)=0;
  virtual void set_customer_data_modify_customer_enabled(bool p_enabled)=0;
  virtual void set_customer_data_delete_customer_enabled(bool p_enabled)=0;

  // Interactions with customer bill information
  virtual void set_customer_bill_fields_enabled(bool p_enabled)=0;
  virtual void set_customer_facture_date(const std::string & p_date)=0;
  virtual void set_customer_facture_allowed_livre_ids(const std::vector<uint32_t> & p_livre_ids)=0;
  virtual void set_customer_facture_livre_id(uint32_t p_id)=0;
  virtual void set_customer_allowed_facture_references(const std::vector<uint32_t> & p_remaining_refs)=0;
  virtual void set_customer_facture_reference(uint32_t p_ref)=0;
  virtual void set_customer_facture_status_list(const std::vector<facture_status> & p_status_list)=0;
  virtual void set_customer_facture_status(uint32_t p_id)=0;
  virtual bool is_customer_bill_date_complete(void)const=0;
  virtual bool is_customer_bill_date_empty(void)const=0;
  virtual const std::string get_customer_bill_date(void)const=0;
  virtual uint32_t get_customer_bill_book_id(void)const=0;
  virtual uint32_t get_customer_bill_reference(void)const=0;
  virtual const facture_status * get_customer_bill_status(void)const=0;

  // Interactions with customer bill list
  virtual bool is_customer_data_bill_selection_empty(void)const=0;
  virtual uint32_t get_customer_data_selected_bill_id(void)const=0;
  virtual void update_customer_data_bill_list(const std::vector<search_facture_item> & p_list)=0;
  virtual void set_customer_bill_list_enabled(bool p_enabled)=0;

  // Interactions with customer bill actions  
  virtual void set_customer_bill_creation_enabled(bool p_enabled)=0;
  virtual void set_customer_bill_modification_enabled(bool p_enabled)=0;
  virtual void set_customer_bill_deletion_enabled(bool p_enabled)=0;

  // Interactions with customer purchase information
  virtual void set_customer_purchase_fields_enabled(bool p_enabled)=0;
  virtual void set_customer_purchase_brand_list(const std::vector<marque> & p_list)=0;
  virtual void set_customer_purchase_brand(uint32_t p_id)=0;
  virtual void set_customer_purchase_type_list(const std::vector<type_achat> & p_list)=0;
  virtual void set_customer_purchase_type(uint32_t p_id)=0;
  virtual void set_customer_purchase_reference(const std::string & p_ref)=0;
  virtual void set_customer_purchase_euro_price(const std::string & p_euro_price)=0;
  virtual void set_customer_purchase_franc_price(const std::string & p_franc_price)=0;
  virtual void set_customer_purchase_warranty(bool p_warranty)=0;
  virtual const marque * get_customer_purchase_brand(void)const=0;
  virtual const type_achat * get_customer_purchase_type(void)const=0;
  virtual const std::string get_customer_purchase_reference(void)const=0; 
  virtual const std::string get_customer_purchase_euro_price(void)const=0; 
  virtual const std::string get_customer_purchase_franc_price(void)const=0; 
  virtual bool is_customer_purchase_warranty_selected(void)const=0;

  // Interactions with customer purchase list
  virtual bool is_customer_data_purchase_selection_empty(void)const=0;
  virtual uint32_t get_customer_data_selected_purchase_id(void)const=0;
  virtual void update_customer_data_purchase_list(const std::vector<search_achat_item> & p_list)=0;
  virtual void set_customer_purchase_list_enabled(bool p_enabled)=0;

  // Interactions customer purchase actions
  virtual void set_customer_purchase_creation_enabled(bool p_enabled)=0;
  virtual void set_customer_purchase_modification_enabled(bool p_enabled)=0;
  virtual void set_customer_purchase_deletion_enabled(bool p_enabled)=0;

  // Interactions with livre facture information
  virtual void set_livre_facture_information_enabled(bool p_enabled)=0;
  virtual void clear_livre_facture_information(void)=0;
  virtual void set_livre_facture_start_date(const std::string & p_date)=0;
  virtual void set_livre_facture_end_date(const std::string & p_date)=0;
  virtual void set_livre_facture_id(const std::string & p_id)=0;
  virtual const std::string get_livre_facture_start_date(void)const=0;
  virtual bool is_livre_facture_start_date_complete(void)const=0;
  virtual const std::string get_livre_facture_end_date(void)const=0;
  virtual bool is_livre_facture_end_date_complete(void)const=0;
  virtual const std::string get_livre_facture_id(void)const=0;

  // Interactions with livre facture list
  virtual bool is_livre_facture_selection_empty(void)const=0;
  virtual uint32_t get_selected_livre_facture_id(void)const=0;
  virtual void refresh_livre_facture_list(std::vector<livre_facture> & p_list)=0;

  // Interactions with livre facture actions
  virtual void set_delete_livre_facture_enabled(bool p_enabled)=0;
  virtual void set_modify_livre_facture_enabled(bool p_enabled)=0;
  virtual void set_create_livre_facture_enabled(bool p_enabled)=0;

  // Interactions with non attributed facture information
  virtual void set_non_attributed_facture_date(const std::string & p_date)=0;
  virtual void set_non_attributed_facture_allowed_livre_ids(const std::vector<uint32_t> & p_livre_ids)=0;
  virtual void set_non_attributed_allowed_facture_references(const std::vector<uint32_t> & p_remaining_refs)=0;
  virtual void set_non_attributed_facture_reference(uint32_t p_ref)=0;
  virtual void set_non_attributed_facture_status_list(const std::vector<facture_status> & p_status_list)=0;
  virtual void set_non_attributed_facture_status(uint32_t p_id)=0;
  virtual void set_non_attributed_facture_reason_list(const std::vector<facture_reason> & p_reason_list)=0;
  virtual void set_non_attributed_facture_reason(uint32_t p_id)=0;
  virtual const std::string get_non_attributed_facture_date(void)const=0;
  virtual uint32_t get_non_attributed_facture_livre_facture_id(void)const=0;
  virtual uint32_t get_non_attributed_facture_reference(void)const=0;
  virtual const facture_status * get_non_attributed_facture_status(void)const=0;
  virtual const facture_reason * get_non_attributed_facture_reason(void)const=0;
  virtual void clear_non_attributed_facture_date(void)=0;
  virtual void enable_non_attributed_facture_fields(bool p_enable)=0;
  virtual bool is_non_attributed_facture_date_complete(void)const=0;
  virtual bool is_non_attributed_facture_date_empty(void)const=0;

  // Interactions with non attributed facture list
  virtual void refresh_list_facture_of_livre_facture(std::vector<search_facture_client_item> & p_list)=0;
  virtual bool is_list_facture_of_livre_facture_selection_empty(void)const=0;
  virtual uint32_t get_list_facture_of_livre_facture_selected_id(void)const=0;

  // Interactions with non attributed facture buttons
  virtual void set_facture_creation_for_selected_livre_enabled( bool p_enabled)=0;
  virtual void set_facture_deletion_for_selected_livre_enabled( bool p_enabled)=0;
  virtual void set_facture_modification_for_selected_livre_enabled( bool p_enabled)=0;

  // Interactions with facture status information
  virtual void set_facture_status_information_enabled(bool p_enabled)=0;
  virtual void clear_facture_status_information(void)=0;
  virtual void set_facture_status_name(const std::string & p_name)=0;  
  virtual const std::string get_facture_status_name(void)const=0;

  // Interactions with facture status list
  virtual bool is_facture_status_selection_empty(void)const=0;
  virtual uint32_t get_selected_facture_status_id(void)const=0;
  virtual void refresh_facture_status_list(std::vector<facture_status> & p_list)=0;
  virtual void set_facture_status_list_enabled(bool p_enable)=0;

  // Interactions with facture status actions
  virtual void set_create_facture_status_enabled(bool p_enable)=0;
  virtual void set_delete_facture_status_enabled(bool p_enable)=0;
  virtual void set_modify_facture_status_enabled(bool p_enable)=0;
  virtual void set_modify_facture_status_action_name(const std::string & p_name)=0;

  // Interactions with facture reason information
  virtual void set_facture_reason_information_enabled(bool p_enabled)=0;
  virtual void clear_facture_reason_information(void)=0;
  virtual void set_facture_reason_name(const std::string & p_name)=0;  
  virtual const std::string get_facture_reason_name(void)const=0;

  // Interactions with facture reason list
  virtual bool is_facture_reason_selection_empty(void)const=0;
  virtual uint32_t get_selected_facture_reason_id(void)const=0;
  virtual void refresh_facture_reason_list(std::vector<facture_reason> & p_list)=0;
  virtual void set_facture_reason_list_enabled(bool p_enable)=0;

  // Interactions with facture reason actions
  virtual void set_create_facture_reason_enabled(bool p_enable)=0;
  virtual void set_delete_facture_reason_enabled(bool p_enable)=0;
  virtual void set_modify_facture_reason_enabled(bool p_enable)=0;
  virtual void set_modify_facture_reason_action_name(const std::string & p_name)=0;

  // Interactions with brand information
  virtual void set_purchase_configuration_enabled(bool p_enabled)=0;
  virtual void clear_purchase_configuration_brand_information(void)=0;
  virtual void set_purchase_configuration_brand_name(const std::string & p_name)=0;  
  virtual const std::string get_purchase_configuration_brand_name(void)const=0;

  // Interactions with brand list
  virtual bool is_purchase_configuration_brand_selection_empty(void)const=0;
  virtual uint32_t get_purchase_configuration_selected_brand_id(void)const=0;
  virtual void set_purchase_configuration_brand_list(std::vector<marque> & p_list)=0;
  virtual void set_purchase_configuration_brand_list_enabled(bool p_enable)=0;

  // Interactions with brand actions
  virtual void set_purchase_configuration_create_brand_enabled(bool p_enable)=0;
  virtual void set_purchase_configuration_delete_brand_enabled(bool p_enable)=0;
  virtual void set_purchase_configuration_modify_brand_enabled(bool p_enable)=0;
  virtual void set_purchase_configuration_modify_brand_action_name(const std::string & p_name)=0;

  // Interactions with type information
  virtual void clear_purchase_configuration_type_information(void)=0;
  virtual void set_purchase_configuration_type_name(const std::string & p_name)=0;  
  virtual const std::string get_purchase_configuration_type_name(void)const=0;

  // Interactions with type list
  virtual bool is_purchase_configuration_type_selection_empty(void)const=0;
  virtual uint32_t get_purchase_configuration_selected_type_id(void)const=0;
  virtual void set_purchase_configuration_type_list(std::vector<type_achat> & p_list)=0;
  virtual void set_purchase_configuration_type_list_enabled(bool p_enable)=0;

  // Interactions with type actions
  virtual void set_purchase_configuration_create_type_enabled(bool p_enable)=0;
  virtual void set_purchase_configuration_delete_type_enabled(bool p_enable)=0;
  virtual void set_purchase_configuration_modify_type_enabled(bool p_enable)=0;
  virtual void set_purchase_configuration_modify_type_action_name(const std::string & p_name)=0;

  // Interactions with city information
  virtual void set_city_information_enabled(bool p_enabled)=0;
  virtual void clear_city_information(void)=0;
  virtual void set_city_name(const std::string & p_name)=0;  
  virtual const std::string get_city_name(void)const=0;
  virtual void set_city_postal_code(const std::string & p_postal_code)=0;  
  virtual const std::string get_city_postal_code(void)const=0;
  virtual bool is_city_postal_code_complete(void)const=0;

  // Interactions with city list
  virtual bool is_city_selection_empty(void)const=0;
  virtual uint32_t get_selected_city_id(void)const=0;
  virtual void set_city_list(std::vector<ville> & p_list)=0;
  virtual void set_city_list_enabled(bool p_enable)=0;

  // Interactions with city actions
  virtual void set_create_city_enabled(bool p_enable)=0;
  virtual void set_delete_city_enabled(bool p_enable)=0;
  virtual void set_modify_city_enabled(bool p_enable)=0;
  virtual void set_modify_city_action_name(const std::string & p_name)=0;

  // Interactions with coherency actions
  virtual void set_coherency_information_enabled(bool p_enabled)=0;
  virtual void set_coherency_report_launch_check_enabled(bool p_enable)=0;

  // Interactions with coherency information
  virtual void set_coherency_report_error_number(uint32_t p_nb)=0;
  virtual void set_coherency_report_error_list(std::vector<coherency_report_item> p_list)=0;
  virtual void set_coherency_report_warning_number(uint32_t p_nb)=0;
  virtual void set_coherency_report_warning_list(std::vector<coherency_report_item> p_list)=0;
  
};

#endif
