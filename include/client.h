#ifndef CLIENT_H
#define CLIENT_H

#include "item.h"
#include <string>

class client: public item
{

  friend std::ostream& operator<<(std::ostream& s, const client & p_client);

 public:
  inline client(void);
  inline client(uint32_t p_id,const std::string & p_surname, const std::string & p_first_name,const std::string & p_address,const std::string & p_tel, uint32_t p_ville_id);
  inline client(const std::string & p_surname, const std::string & p_first_name,const std::string & p_address,const std::string & p_tel, uint32_t p_ville_id);

  inline const std::string & get_name(void)const;
  inline void set_name(const std::string & p_name);
  inline const std::string & get_first_name(void)const;
  inline void set_first_name(const std::string & p_first_name);
  inline const std::string & get_address(void)const;
  inline void set_address(const std::string & p_address);
  inline const std::string & get_tel(void)const;
  inline void set_tel(const std::string & p_tel);
  inline uint32_t get_ville_id(void)const;
  inline void set_ville_id(uint32_t p_ville_id);

  /**
     Return type of object. This is used to name the corresponding table in database
  */
  inline const std::string getType(void)const;
 private:
  std::string m_surname;
  std::string m_first_name;
  std::string m_address;
  std::string m_tel;
  uint32_t m_ville_id;
};

inline std::ostream& operator<<(std::ostream& s, const client & p_item);

//------------------------------------------------------------------------------
const std::string client::getType(void)const
{
  return "Client";
}

//------------------------------------------------------------------------------
client::client(void):
  item(),
  m_surname(""),
  m_first_name(""),
  m_address(""),
  m_tel(""),
  m_ville_id(0)
{
}

//------------------------------------------------------------------------------
client::client(uint32_t p_id,const std::string & p_surname, const std::string & p_first_name,const std::string & p_address,const std::string & p_tel, uint32_t p_ville_id):
  item(p_id),
  m_surname(p_surname),
  m_first_name(p_first_name),
  m_address(p_address),
  m_tel(p_tel),
  m_ville_id(p_ville_id)
{
}

//------------------------------------------------------------------------------
client::client(const std::string & p_surname, const std::string & p_first_name,const std::string & p_address,const std::string & p_tel, uint32_t p_ville_id):
  item(),
  m_surname(p_surname),
  m_first_name(p_first_name),
  m_address(p_address),
  m_tel(p_tel),
  m_ville_id(p_ville_id)
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
const std::string & client::get_tel(void)const
{
  return m_tel;
}

//------------------------------------------------------------------------------
void client::set_tel(const std::string & p_tel)
{
  m_tel = p_tel;
}

//------------------------------------------------------------------------------
uint32_t client::get_ville_id(void)const
{
  return m_ville_id;
}

//------------------------------------------------------------------------------
void client::set_ville_id(uint32_t p_ville_id)
{
  m_ville_id = p_ville_id;
}


//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const client & p_client)
{
  s << p_client.getType() << "{Id=" << p_client.get_id() << "\",Name=\"" << p_client.m_surname << "\",First Name=\"" << p_client.m_first_name << "\",Tel=\"" << p_client.m_tel << "\",VilleId=\"" << p_client.m_ville_id << "\"" ;
  return s;
}

#endif
