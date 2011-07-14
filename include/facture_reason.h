#ifndef FACTURE_REASON_H
#define FACTURE_REASON_H

#include "named_item.h"

class facture_reason: public named_item
{

 public:
  inline facture_reason(void);
  inline facture_reason(const std::string &p_name);
  inline facture_reason(uint32_t p_id, const std::string &p_name);
  inline const std::string get_type(void)const;
  inline static const std::string get_class_type(void);

 private:
  
};

//------------------------------------------------------------------------------
facture_reason::facture_reason():
  named_item()
{
}

//------------------------------------------------------------------------------
facture_reason::facture_reason(const std::string &p_name):
  named_item(p_name)
{
}

//------------------------------------------------------------------------------
facture_reason::facture_reason(uint32_t p_id, const std::string &p_name):
  named_item(p_id,p_name)
{
}

//------------------------------------------------------------------------------
const std::string facture_reason::get_type(void)const
{
  return "Facture_Reason";
}

//------------------------------------------------------------------------------
const std::string facture_reason::get_class_type(void)
{
  return "Facture_Reason";
}

#endif
