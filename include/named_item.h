#ifndef NAMED_ITEM_H
#define NAMED_ITEM_H

#include <iostream>
#include <string>
#include <stdint.h>
#include <assert.h>

class named_item
{

  friend std::ostream& operator<<(std::ostream& s, const named_item & p_named_item);

 public:
  inline named_item(const std::string &p_name);
  inline named_item(uint32_t p_id, const std::string &p_name);
  inline uint32_t getId(void)const;
  inline const std::string & getName(void)const;
  virtual const std::string getType(void)const=0;
  inline void setName(const std::string & p_name);
  
 private:
  uint32_t m_id;
  std::string m_name;
  
};

inline std::ostream& operator<<(std::ostream& s, const named_item & p_named_item);

//------------------------------------------------------------------------------
named_item::named_item(const std::string &p_name):
  m_id(0),
  m_name(p_name)
{
}

//------------------------------------------------------------------------------
named_item::named_item(uint32_t p_id, const std::string &p_name):
  m_id(p_id),
  m_name(p_name)
{
  assert(m_id);
}

//------------------------------------------------------------------------------
uint32_t named_item::getId(void)const
{
  return m_id;
}

//------------------------------------------------------------------------------
const std::string & named_item::getName(void)const
{
  return m_name;
}

//------------------------------------------------------------------------------
void named_item::setName(const std::string & p_name)
{
  m_name = p_name;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const named_item & p_named_item)
{
  s << p_named_item.getType() << "{Id=" << p_named_item.m_id << ",Name=\"" << p_named_item.m_name << "\"}" ;
  return s;
}


#endif
