#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>
#include <string>

class client
{
 public:
  client(uint32_t p_client_id,const std::string & p_surname, const std::string & p_first_name,const std::string & p_address,const std::string & p_tel, const std::string & p_postal_code, const std::string & p_town);
 private:
  uint32_t m_client_id;
  std::string m_surname;
  std::string m_first_name;
  std::string m_address;
  std::string m_tel;
  std::string m_postal_code;
  std::string m_town;
};

#endif
