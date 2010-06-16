#ifndef VILLE_H
#define VILLE_H

#include <iostream>
#include <string>
#include <stdint.h>
#include <assert.h>

class ville
{

  friend std::ostream& operator<<(std::ostream& s, const ville & p_ville);

 public:
  inline ville(const std::string &p_name, const std::string &p_code_postal);
  inline ville(uint32_t p_id, const std::string &p_name, const std::string &p_code_postal);
  inline uint32_t getId(void)const;
  inline const std::string & getName(void)const;
  inline const std::string & getCodePostal(void)const;

  inline void setName(const std::string & p_name);
  inline void setCodePostal(const std::string & p_code_postal);

 private:
  uint32_t m_id;
  std::string m_name;
  std::string m_code_postal;
  
};

inline std::ostream& operator<<(std::ostream& s, const ville & p_ville);

//------------------------------------------------------------------------------
ville::ville(const std::string &p_name, const std::string &p_code_postal):
  m_id(0),
  m_name(p_name),
  m_code_postal(p_code_postal)
{
}

//------------------------------------------------------------------------------
ville::ville(uint32_t p_id, const std::string &p_name, const std::string &p_code_postal):
  m_id(p_id),
  m_name(p_name),
  m_code_postal(p_code_postal)
{
  assert(m_id);
}

//------------------------------------------------------------------------------
uint32_t ville::getId(void)const
{
  return m_id;
}

//------------------------------------------------------------------------------
const std::string & ville::getName(void)const
{
  return m_name;
}

//------------------------------------------------------------------------------
const std::string & ville::getCodePostal(void)const
{
  return m_code_postal;
}


//------------------------------------------------------------------------------
void ville::setName(const std::string & p_name)
{
  m_name = p_name;
}

//------------------------------------------------------------------------------
void ville::setCodePostal(const std::string & p_code_postal)
{
  m_code_postal = p_code_postal;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const ville & p_ville)
{
  s << "Ville{Id=" << p_ville.m_id << ",Name=\"" << p_ville.m_name << "\",Code_postal=\"" << p_ville.m_code_postal << "\"}" ;
  return s;
}


#endif
