#ifndef NAMED_ITEM_H
#define NAMED_ITEM_H

#include "item.h"
#include <iostream>
#include <string>
#include <stdint.h>
#include <assert.h>

class named_item: public item
{

  friend std::ostream& operator<<(std::ostream& s, const named_item & p_named_item);

 public:
  inline named_item(void);
  inline named_item(const std::string &p_name);
  inline named_item(uint32_t p_id, const std::string &p_name);
  inline const std::string & get_name(void)const;
  inline void set_name(const std::string & p_name);
  
 private:
  std::string m_name;
  
};

inline std::ostream& operator<<(std::ostream& s, const named_item & p_named_item);

//------------------------------------------------------------------------------
named_item::named_item(void):
  item(),
  m_name("")
{
}

//------------------------------------------------------------------------------
named_item::named_item(const std::string &p_name):
  item(),
  m_name(p_name)
{
}

//------------------------------------------------------------------------------
named_item::named_item(uint32_t p_id, const std::string &p_name):
  item(p_id),
  m_name(p_name)
{
}

//------------------------------------------------------------------------------
const std::string & named_item::get_name(void)const
{
  return m_name;
}

//------------------------------------------------------------------------------
void named_item::set_name(const std::string & p_name)
{
  m_name = p_name;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const named_item & p_named_item)
{
  s << p_named_item.get_type() << "{Id=" << p_named_item.get_id() << ",Name=\"" << p_named_item.m_name << "\"}" ;
  return s;
}



#endif
