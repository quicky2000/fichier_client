#ifndef _FICHIER_CLIENT_UI_IF_
#define _FICHIER_CLIENT_UI_IF_

#include <vector>
#include "facture_status.h"
class facture;

class fichier_client_UI_if
{
 public:
  // Comunication with user
  virtual void display_warning_message(const std::string & p_title,const std::string & p_text)=0;
  virtual void display_information_message(const std::string & p_title,const std::string & p_text)=0;

  // Interactions with livre facture information
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
  virtual void set_non_attributed_allowed_facture_references(const std::vector<uint32_t> & p_remaining_refs)=0;
  virtual void set_non_attributed_facture_allowed_livre_ids(const std::vector<uint32_t> & p_livre_ids)=0;
  virtual void set_non_attributed_facture_status_list(const std::vector<facture_status> & p_status_list)=0;
  virtual void set_non_attributed_facture_reason_list(const std::vector<facture_reason> & p_reason_list)=0;
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

  // Interactions with non attributed facture buttons
  virtual void set_facture_creation_for_selected_livre_enabled( bool p_enabled)=0;

  // Interactions with facture status information
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
};

#endif
