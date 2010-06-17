#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <string>
#include <stdint.h>
#include <assert.h>

class item
{

  friend std::ostream& operator<<(std::ostream& s, const item & p_item);

 public:
  inline item(void);
  inline item(uint32_t p_id);
  inline uint32_t getId(void)const;
  virtual const std::string getType(void)const=0;
  
 private:
  uint32_t m_id;
  
};

inline std::ostream& operator<<(std::ostream& s, const item & p_item);

//------------------------------------------------------------------------------
item::item():
  m_id(0)
{
}

//------------------------------------------------------------------------------
item::item(uint32_t p_id):
  m_id(p_id)
{
  assert(m_id);
}

//------------------------------------------------------------------------------
uint32_t item::getId(void)const
{
  return m_id;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const item & p_item)
{
  s << p_item.getType() << "{Id=" << p_item.m_id << "}" ;
  return s;
}


#endif
