#ifndef _FICHIER_CLIENT_UI_IF_
#define _FICHIER_CLIENT_UI_IF_

class fichier_client_UI_if
{
 public:
  virtual void display_warning_message(const std::string & p_title,const std::string & p_text)=0;
  virtual void display_information_message(const std::string & p_title,const std::string & p_text)=0;
};

#endif
