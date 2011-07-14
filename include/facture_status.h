#ifndef FACTURE_STATUS_H
#define FACTURE_STATUS_H

#include "named_item.h"

class facture_status: public named_item
{

 public:
  inline facture_status(void);
  inline facture_status(const std::string &p_name);
  inline facture_status(uint32_t p_id, const std::string &p_name);
  inline const std::string get_type(void)const;
  inline static const std::string get_class_type(void);
  inline static const std::string & get_ok_status(void);
  inline static const std::string & get_non_checked_status(void);
 private:
  static const std::string m_class_type;
  static const std::string m_ok_status;
  static const std::string m_non_checked_status;
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
const std::string facture_status::get_type(void)const
{
  return m_class_type;
}

//------------------------------------------------------------------------------
const std::string facture_status::get_class_type(void)
{
  return m_class_type;
}

//------------------------------------------------------------------------------
const std::string & facture_status::get_ok_status(void)
{
  return m_ok_status;
}

//------------------------------------------------------------------------------
const std::string & facture_status::get_non_checked_status(void)
{
  return m_non_checked_status;
}
#endif
