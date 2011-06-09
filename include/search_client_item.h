#ifndef SEARCH_CLIENT_ITEM_H
#define SEARCH_CLIENT_ITEM_H
#include <iostream>
#include <stdint.h>

class sqlite3_stmt;

class search_client_item
{
  friend std::ostream& operator<<(std::ostream& s, const search_client_item & p_client);
 public:
  inline search_client_item(sqlite3_stmt* p_stmt,uint32_t p_offset=0);
  inline uint32_t get_id(void)const;
  inline const std::string & get_name(void)const;
  inline const std::string & get_first_name(void)const;
  inline const std::string & get_address(void)const;
  inline const std::string & get_city(void)const;

  inline const std::string getType(void)const;

 private:
  uint32_t m_id;
  std::string m_name;
  std::string m_first_name;
  std::string m_address;
  std::string m_city;
};

inline std::ostream& operator<<(std::ostream& s, const search_client_item & p_item);

#include "sqlite3.h"

search_client_item::search_client_item(sqlite3_stmt* p_stmt,uint32_t p_offset):
  m_id(sqlite3_column_int(p_stmt,p_offset + 0)),
  m_name((const char*)sqlite3_column_text(p_stmt,p_offset + 1)),
  m_first_name((const char*)sqlite3_column_text(p_stmt,p_offset + 2)),
  m_address((const char*)sqlite3_column_text(p_stmt,p_offset + 3)),
  m_city((const char*)sqlite3_column_text(p_stmt,p_offset + 4))
{
}

std::ostream& operator<<(std::ostream& s, const search_client_item & p_client)
{
  s << "{" << p_client.m_id << "," << p_client.m_name << "," << p_client.m_first_name << "," << p_client.m_address << "," << p_client.m_city << "}";
  return s;
}

uint32_t search_client_item::get_id(void)const
{
  return m_id;
}

const std::string & search_client_item::get_name(void)const
{
  return m_name;
}

const std::string & search_client_item::get_first_name(void)const
{
  return m_first_name;
}

const std::string & search_client_item::get_address(void)const
{
  return m_address;
}


const std::string & search_client_item::get_city(void)const
{
  return m_city;
}

#endif
