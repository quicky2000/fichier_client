#ifndef VILLE_H
#define VILLE_H

#include "named_item.h"

class ville: public named_item
{

  friend std::ostream& operator<<(std::ostream& s, const ville & p_ville);

 public:
  inline ville(void);
  inline ville(const std::string &p_name, const std::string &p_code_postal);
  inline ville(uint32_t p_id, const std::string &p_name, const std::string &p_code_postal);

  inline const std::string & getCodePostal(void)const;
  inline void setCodePostal(const std::string & p_code_postal);

  /**
     Return type of object. This is used to name the corresponding table in database
  */
  inline const std::string getClassType(void)const;
  inline const std::string getType(void)const;
 private:
  std::string m_code_postal;
  
};

inline std::ostream& operator<<(std::ostream& s, const ville & p_ville);

//------------------------------------------------------------------------------
const std::string ville::getClassType(void)const
{
  return "Ville";
}

//------------------------------------------------------------------------------
const std::string ville::getType(void)const
{
  return "Ville";
}

//------------------------------------------------------------------------------
ville::ville(void):
  named_item(),
  m_code_postal("")
{
}

//------------------------------------------------------------------------------
ville::ville(const std::string &p_name, const std::string &p_code_postal):
  named_item(p_name),
  m_code_postal(p_code_postal)
{
}

//------------------------------------------------------------------------------
ville::ville(uint32_t p_id, const std::string &p_name, const std::string &p_code_postal):
  named_item(p_id,p_name),
  m_code_postal(p_code_postal)
{
}

//------------------------------------------------------------------------------
const std::string & ville::getCodePostal(void)const
{
  return m_code_postal;
}


//------------------------------------------------------------------------------
void ville::setCodePostal(const std::string & p_code_postal)
{
  m_code_postal = p_code_postal;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const ville & p_ville)
{
  s << p_ville.getClassType() << "{Id=" << p_ville.get_id() << ",Name=\"" << p_ville.getName() << "\",Code_postal=\"" << p_ville.m_code_postal << "\"}" ;
  return s;
}


#endif
