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

  inline const std::string & get_postal_code(void)const;
  inline void set_postal_code(const std::string & p_code_postal);

  /**
     Return type of object. This is used to name the corresponding table in database
  */
  inline const std::string get_class_type(void)const;
  inline const std::string get_type(void)const;
 private:
  std::string m_postal_code;
  
};

inline std::ostream& operator<<(std::ostream& s, const ville & p_ville);

//------------------------------------------------------------------------------
const std::string ville::get_class_type(void)const
{
  return "Ville";
}

//------------------------------------------------------------------------------
const std::string ville::get_type(void)const
{
  return "Ville";
}

//------------------------------------------------------------------------------
ville::ville(void):
  named_item(),
  m_postal_code("")
{
}

//------------------------------------------------------------------------------
ville::ville(const std::string &p_name, const std::string &p_code_postal):
  named_item(p_name),
  m_postal_code(p_code_postal)
{
}

//------------------------------------------------------------------------------
ville::ville(uint32_t p_id, const std::string &p_name, const std::string &p_code_postal):
  named_item(p_id,p_name),
  m_postal_code(p_code_postal)
{
}

//------------------------------------------------------------------------------
const std::string & ville::get_postal_code(void)const
{
  return m_postal_code;
}


//------------------------------------------------------------------------------
void ville::set_postal_code(const std::string & p_code_postal)
{
  m_postal_code = p_code_postal;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const ville & p_ville)
{
  s << p_ville.get_class_type() << "{Id=" << p_ville.get_id() << ",Name=\"" << p_ville.get_name() << "\",Code_postal=\"" << p_ville.m_postal_code << "\"}" ;
  return s;
}


#endif
