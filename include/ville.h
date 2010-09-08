#ifndef VILLE_H
#define VILLE_H

#include "item.h"

class ville: public item
{

  friend std::ostream& operator<<(std::ostream& s, const ville & p_ville);

 public:
  inline ville(const std::string &p_name, const std::string &p_code_postal);
  inline ville(uint32_t p_id, const std::string &p_name, const std::string &p_code_postal);
  inline const std::string & getName(void)const;
  inline const std::string & getCodePostal(void)const;

  inline void setName(const std::string & p_name);
  inline void setCodePostal(const std::string & p_code_postal);

  /**
     Return type of object. This is used to name the corresponding table in database
  */
  inline const std::string getType(void)const;
 private:
  std::string m_name;
  std::string m_code_postal;
  
};

inline std::ostream& operator<<(std::ostream& s, const ville & p_ville);

//------------------------------------------------------------------------------
const std::string ville::getType(void)const
{
  return "Ville";
}

//------------------------------------------------------------------------------
ville::ville(const std::string &p_name, const std::string &p_code_postal):
  item(),
  m_name(p_name),
  m_code_postal(p_code_postal)
{
}

//------------------------------------------------------------------------------
ville::ville(uint32_t p_id, const std::string &p_name, const std::string &p_code_postal):
  item(p_id),
  m_name(p_name),
  m_code_postal(p_code_postal)
{
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
  s << p_ville.getType() << "{Id=" << p_ville.getId() << ",Name=\"" << p_ville.m_name << "\",Code_postal=\"" << p_ville.m_code_postal << "\"}" ;
  return s;
}


#endif
