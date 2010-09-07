#ifndef TYPE_ACHAT_H
#define TYPE_ACHAT_H

#include "named_item.h"

class type_achat: public named_item
{

 public:
  inline type_achat(const std::string &p_name);
  inline type_achat(uint32_t p_id, const std::string &p_name);
  inline const std::string getType(void)const;
 private:
  
};

//------------------------------------------------------------------------------
type_achat::type_achat(const std::string &p_name):
  named_item(p_name)
{
}

//------------------------------------------------------------------------------
type_achat::type_achat(uint32_t p_id, const std::string &p_name):
  named_item(p_id,p_name)
{
}

//------------------------------------------------------------------------------
const std::string type_achat::getType(void)const
{
  return "Type_achat";
}

#endif
