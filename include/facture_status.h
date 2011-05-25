#ifndef FACTURE_STATUS_H
#define FACTURE_STATUS_H

#include "named_item.h"

class facture_status: public named_item
{

 public:
  inline facture_status(void);
  inline facture_status(const std::string &p_name);
  inline facture_status(uint32_t p_id, const std::string &p_name);
  inline const std::string getType(void)const;
  inline static const std::string getClassType(void);

 private:
  
};

//------------------------------------------------------------------------------
facture_status::facture_status():
  named_item()
{
}

//------------------------------------------------------------------------------
facture_status::facture_status(const std::string &p_name):
  named_item(p_name)
{
}

//------------------------------------------------------------------------------
facture_status::facture_status(uint32_t p_id, const std::string &p_name):
  named_item(p_id,p_name)
{
}

//------------------------------------------------------------------------------
const std::string facture_status::getType(void)const
{
  return "Facture_status";
}

//------------------------------------------------------------------------------
const std::string facture_status::getClassType(void)
{
  return "Facture_status";
}

#endif
