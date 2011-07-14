#ifndef CLIENT_H
#define CLIENT_H

#include "item.h"
#include <string>

class client: public item
{

  friend std::ostream& operator<<(std::ostream& s, const client & p_client);

 public:
  inline client(void);
  inline client(uint32_t p_id,const std::string & p_surname, const std::string & p_first_name,const std::string & p_address,const std::string & p_phone, uint32_t p_city_id);
  inline client(const std::string & p_surname, const std::string & p_first_name,const std::string & p_address,const std::string & p_phone, uint32_t p_city_id);

  inline const std::string & get_name(void)const;
  inline void set_name(const std::string & p_name);
  inline const std::string & get_first_name(void)const;
  inline void set_first_name(const std::string & p_first_name);
  inline const std::string & get_address(void)const;
  inline void set_address(const std::string & p_address);
  inline const std::string & get_phone(void)const;
  inline void set_phone(const std::string & p_phone);
  inline uint32_t get_city_id(void)const;
  inline void set_city_id(uint32_t p_city_id);

  /**
     Return type of object. This is used to name the corresponding table in database
  */
  inline const std::string get_type(void)const;
 private:
  std::string m_surname;
  std::string m_first_name;
  std::string m_address;
  std::string m_phone;
  uint32_t m_city_id;
};

inline std::ostream& operator<<(std::ostream& s, const client & p_item);

//------------------------------------------------------------------------------
const std::string client::get_type(void)const
{
  return "Client";
}

//------------------------------------------------------------------------------
client::client(void):
  item(),
  m_surname(""),
  m_first_name(""),
  m_address(""),
  m_phone(""),
  m_city_id(0)
{
}

//------------------------------------------------------------------------------
client::client(uint32_t p_id,const std::string & p_surname, const std::string & p_first_name,const std::string & p_address,const std::string & p_phone, uint32_t p_city_id):
  item(p_id),
  m_surname(p_surname),
  m_first_name(p_first_name),
  m_address(p_address),
  m_phone(p_phone),
  m_city_id(p_city_id)
{
}

//------------------------------------------------------------------------------
client::client(const std::string & p_surname, const std::string & p_first_name,const std::string & p_address,const std::string & p_phone, uint32_t p_city_id):
  item(),
  m_surname(p_surname),
  m_first_name(p_first_name),
  m_address(p_address),
  m_phone(p_phone),
  m_city_id(p_city_id)
{
}

//------------------------------------------------------------------------------
const std::string & client::get_name(void)const
{
  return m_surname;
}

//------------------------------------------------------------------------------
void client::set_name(const std::string & p_name)
{
  m_surname = p_name;
}

//------------------------------------------------------------------------------
const std::string & client::get_first_name(void)const
{
  return m_first_name;
}

//------------------------------------------------------------------------------
void client::set_first_name(const std::string & p_first_name)
{
  m_first_name = p_first_name;
}

//------------------------------------------------------------------------------
const std::string & client::get_address(void)const
{
  return m_address;
}

//------------------------------------------------------------------------------
void client::set_address(const std::string & p_address)
{
  m_address = p_address;
}

//------------------------------------------------------------------------------
const std::string & client::get_phone(void)const
{
  return m_phone;
}

//------------------------------------------------------------------------------
void client::set_phone(const std::string & p_phone)
{
  m_phone = p_phone;
}

//------------------------------------------------------------------------------
uint32_t client::get_city_id(void)const
{
  return m_city_id;
}

//------------------------------------------------------------------------------
void client::set_city_id(uint32_t p_city_id)
{
  m_city_id = p_city_id;
}


//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const client & p_client)
{
  s << p_client.get_type() << "{Id=" << p_client.get_id() << "\",Name=\"" << p_client.m_surname << "\",First Name=\"" << p_client.m_first_name << "\",Phone=\"" << p_client.m_phone << "\",CityId=\"" << p_client.m_city_id << "\"" ;
  return s;
}

#endif
