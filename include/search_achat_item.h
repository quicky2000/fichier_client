#ifndef SEARCH_ACHAT_ITEM_H
#define SEARCH_ACHAT_ITEM_H
#include <iostream>
#include <stdint.h>
#include <sstream>

class sqlite3_stmt;

class search_achat_item
{
  friend std::ostream& operator<<(std::ostream& s, const search_achat_item & p_client);
 public:
  inline search_achat_item(sqlite3_stmt* p_stmt);
  inline uint32_t get_id(void)const;
  inline const std::string & get_date(void)const;
  inline const std::string & get_marque(void)const;
  inline const std::string & get_type(void)const;
  inline const std::string & get_reference(void)const;
  inline const std::string & get_prix_euro(void)const;
  inline const std::string & get_prix_franc(void)const;
  inline const std::string & get_garantie(void)const;
  inline uint32_t get_livre_facture_id(void)const;
 private:
  uint32_t m_id;
  std::string m_date;
  std::string m_marque;
  std::string m_type;
  std::string m_reference;
  std::string m_prix_euro;
  std::string m_prix_franc;
  std::string m_garantie;
  uint32_t m_livre_facture_id;
};

inline std::ostream& operator<<(std::ostream& s, const search_achat_item & p_item);

#include "sqlite3.h"

search_achat_item::search_achat_item(sqlite3_stmt* p_stmt):
  m_id(sqlite3_column_int(p_stmt,0)),
  m_date((const char*)sqlite3_column_text(p_stmt,1)),
  m_marque((const char*)sqlite3_column_text(p_stmt,2)),
  m_type((const char*)sqlite3_column_text(p_stmt,3)),
  m_reference((const char*)sqlite3_column_text(p_stmt,4)),
  m_prix_euro(""),
  m_prix_franc(""),
  m_garantie((sqlite3_column_int(p_stmt,7)? "oui":"non")),
  m_livre_facture_id(sqlite3_column_int(p_stmt,8))
{
  std::stringstream l_stream_euro;
  l_stream_euro << (const char*)sqlite3_column_text(p_stmt,5) << " Euros";
  m_prix_euro = l_stream_euro.str();
  std::stringstream l_stream_franc;
  l_stream_franc << (const char*)sqlite3_column_text(p_stmt,6) << " Francs";
  m_prix_franc = l_stream_franc.str();
 }

std::ostream& operator<<(std::ostream& s, const search_achat_item & p_item)
{
  s << "{" << p_item.m_id << "," << p_item.m_date << "," << p_item.m_marque << "," << p_item.m_type << "," << p_item.m_reference<< "," << p_item.m_prix_euro << "," << p_item.m_prix_franc << "," << p_item.m_garantie << "," << p_item.m_livre_facture_id << "}";
  return s;
}

uint32_t search_achat_item::get_id(void)const
{
  return m_id;
}

const std::string & search_achat_item::get_date(void)const
{
  return m_date;
}

const std::string & search_achat_item::get_marque(void)const
{
  return m_marque;
}

const std::string & search_achat_item::get_type(void)const
{
  return m_type;
}

const std::string & search_achat_item::get_reference(void)const
{
  return m_reference;
}

const std::string & search_achat_item::get_prix_euro(void)const
{
  return m_prix_euro;
}

const std::string & search_achat_item::get_prix_franc(void)const
{
  return m_prix_franc;
}

const std::string & search_achat_item::get_garantie(void)const
{
  return m_garantie;
}

uint32_t search_achat_item::get_livre_facture_id(void)const
{
  return m_livre_facture_id;
}

#endif
