#ifndef MARQUE_H
#define MARQUE_H

#include "named_item.h"

class marque: public named_item
{

 public:
  inline marque(void);
  inline marque(const std::string &p_name);
  inline marque(uint32_t p_id, const std::string &p_name);
  inline const std::string get_type(void)const;
  inline static const std::string get_class_type(void);

 private:
  
};

//------------------------------------------------------------------------------
marque::marque():
  named_item()
{
}

//------------------------------------------------------------------------------
marque::marque(const std::string &p_name):
  named_item(p_name)
{
}

//------------------------------------------------------------------------------
marque::marque(uint32_t p_id, const std::string &p_name):
  named_item(p_id,p_name)
{
}

//------------------------------------------------------------------------------
const std::string marque::get_type(void)const
{
  return "Marque";
}

//------------------------------------------------------------------------------
const std::string marque::get_class_type(void)
{
  return "Marque";
}

#endif
