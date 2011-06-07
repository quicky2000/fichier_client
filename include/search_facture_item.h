#ifndef SEARCH_FACTURE_ITEM_H
#define SEARCH_FACTURE_ITEM_H
#include <iostream>
#include <stdint.h>

class sqlite3_stmt;

class search_facture_item
{
  friend std::ostream& operator<<(std::ostream& s, const search_facture_item & p_item);
 public:
  inline search_facture_item(sqlite3_stmt* p_stmt);

  // select Facture.Id, FactureRef, Date, LivreFactureId,Facture_status.name from Facture,Facture_status where Facture.Status == Facture_status.Id; 

  inline uint32_t get_id(void)const;
  inline uint32_t get_facture_ref(void)const;
  inline const std::string & get_date(void)const;
  inline uint32_t get_livre_facture_id(void)const;
  inline const std::string & get_facture_status(void)const;

 private:
  uint32_t m_id;
  uint32_t m_facture_ref;
  std::string m_date;
  uint32_t m_livre_facture_id;
  std::string m_facture_status;
};

inline std::ostream& operator<<(std::ostream& s, const search_facture_item & p_item);

#include "sqlite3.h"

//------------------------------------------------------------------------------
search_facture_item::search_facture_item(sqlite3_stmt* p_stmt):
  m_id(sqlite3_column_int(p_stmt,0)),
  m_facture_ref(sqlite3_column_int(p_stmt,1)),
  m_date((const char*)sqlite3_column_text(p_stmt,2)),
  m_livre_facture_id(sqlite3_column_int(p_stmt,3)),
  m_facture_status((const char*)sqlite3_column_text(p_stmt,4))
{
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const search_facture_item & p_item)
{
  s << "{" << p_item.m_id << "," << p_item.m_facture_ref << ", \"" << p_item.m_date << "\"," << p_item.m_livre_facture_id << ", \"" << p_item.m_facture_status << "\"}";
  return s;
}

//------------------------------------------------------------------------------
uint32_t search_facture_item::get_id(void)const
{
  return m_id;
}

//------------------------------------------------------------------------------
uint32_t search_facture_item::get_facture_ref(void)const
{
  return m_facture_ref;
}

//------------------------------------------------------------------------------
const std::string & search_facture_item::get_date(void)const
{
  return m_date;
}

//------------------------------------------------------------------------------
uint32_t search_facture_item::get_livre_facture_id(void)const
{
  return m_livre_facture_id;
}


//------------------------------------------------------------------------------
const std::string & search_facture_item::get_facture_status(void)const
{
  return m_facture_status;
}

#endif
