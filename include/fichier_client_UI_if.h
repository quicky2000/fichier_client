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
  virtual bool create_non_attributed_facture(std::vector<facture_status> & p_status_list, facture & p_facture)=0;
  virtual void refresh_list_facture_of_livre_facture(void)=0;
};

#endif
