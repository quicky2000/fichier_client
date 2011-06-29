#ifndef _FICHIER_CLIENT_UI_IF_
#define _FICHIER_CLIENT_UI_IF_

#include <vector>
#include "facture_status.h"
class facture;

class fichier_client_UI_if
{
 public:
  virtual void display_warning_message(const std::string & p_title,const std::string & p_text)=0;
  virtual void display_information_message(const std::string & p_title,const std::string & p_text)=0;

  virtual void set_facture_creation_for_selected_livre_enabled( bool p_enabled)=0;
  virtual void set_delete_livre_facture_enabled(bool p_enabled)=0;
  virtual void set_modify_livre_facture_enabled(bool p_enabled)=0;
  virtual void refresh_list_facture_of_livre_facture(void)=0;
  virtual void set_non_attributed_allowed_facture_references(const std::vector<uint32_t> & p_remaining_refs)=0;
  virtual void set_non_attributed_facture_allowed_livre_ids(const std::vector<uint32_t> & p_livre_ids)=0;
  virtual void set_non_attributed_facture_status_list(const std::vector<facture_status> & p_status_list)=0;
  virtual const std::string get_non_attributed_facture_date(void)const=0;
  virtual uint32_t get_non_attributed_facture_livre_facture_id(void)const=0;
  virtual uint32_t get_non_attributed_facture_reference(void)const=0;
  virtual const facture_status * get_non_attributed_facture_status(void)const=0;
  virtual void clear_non_attributed_facture_date(void)=0;
  virtual void enable_non_attributed_facture_fields(bool p_enable)=0;
  virtual bool is_non_attributed_facture_date_complete(void)=0;
  virtual bool is_non_attributed_facture_date_empty(void)=0;
};

#endif
