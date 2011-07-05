#ifndef SEARCH_FACTURE_CLIENT_ITEM_H
#define SEARCH_FACTURE_CLIENT_ITEM_H
#include <iostream>
#include <stdint.h>
#include <assert.h>
class sqlite3_stmt;

#include "search_facture_item.h"
#include "search_client_item.h"

class search_facture_client_item: public search_facture_item, public search_client_item
{
 public:
  inline search_facture_client_item(const search_facture_item & p_item,const std::string & p_reason);
  inline search_facture_client_item(const search_facture_item & p_facture_item,const search_client_item & p_client_item);

  inline uint32_t get_id(void)const;
  inline uint32_t get_client_id(void)const;
  inline const std::string & get_client_name(void)const;
  inline const std::string & get_client_first_name(void)const;
  inline const std::string & get_client_address(void)const;
  inline const std::string & get_client_city(void)const;
  inline const std::string & get_reason(void)const;
 private:
  std::string m_reason;
};

inline std::ostream& operator<<(std::ostream& s, const search_facture_client_item & p_item);

#include "sqlite3.h"

//------------------------------------------------------------------------------
search_facture_client_item::search_facture_client_item(const search_facture_item & p_item,const std::string & p_reason):
  search_facture_item(p_item),
  m_reason(p_reason)
{
}

//------------------------------------------------------------------------------
search_facture_client_item::search_facture_client_item(const search_facture_item & p_facture_item,const search_client_item & p_client_item):
  search_facture_item(p_facture_item),
  search_client_item(p_client_item)
{
  assert(search_facture_item::get_client_id() == search_client_item::get_id());
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const search_facture_client_item & p_item)
{
  s << "{" 
    << p_item.get_id() << ", "
    << p_item.get_facture_ref() << ", "
    << "\"" << p_item.get_date() << "\", " 
    << p_item.get_livre_facture_id() << ", "
    << "\"" << p_item.get_facture_status() << "\", "
    << p_item.get_client_id() << ", " 
    << "\"" << p_item.get_client_name() << "\", "
    << "\"" << p_item.get_client_first_name() << "\","
    << "\"" << p_item.get_client_address() << "\", "
    << "\"" << p_item.get_client_city() << "\""
    << "}";
  return s;
}

//------------------------------------------------------------------------------
uint32_t search_facture_client_item::get_id(void)const
{
  return search_facture_item::get_id();
}

//------------------------------------------------------------------------------
uint32_t search_facture_client_item::get_client_id(void)const
{
  return search_client_item::get_id();
}

//------------------------------------------------------------------------------
const std::string & search_facture_client_item::get_client_name(void)const
{
  return get_name();
}

//------------------------------------------------------------------------------
const std::string & search_facture_client_item::get_client_first_name(void)const
{
  return get_first_name();
}


//------------------------------------------------------------------------------
const std::string & search_facture_client_item::get_client_address(void)const
{
  return get_address();
}

//------------------------------------------------------------------------------
const std::string & search_facture_client_item::get_client_city(void)const
{
  return get_city();
}

//------------------------------------------------------------------------------
const std::string & search_facture_client_item::get_reason(void)const
{
  return m_reason;
}


#endif
