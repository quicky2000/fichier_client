#ifndef LIVRE_FACTURE_H
#define LIVRE_FACTURE_H

#include "item.h"

class livre_facture: public item
{

  friend std::ostream& operator<<(std::ostream& s, const livre_facture & p_livre_facture);

 public:
  inline livre_facture(uint32_t p_livre_id, const std::string &p_start_date, const std::string &p_end_date);
  inline const std::string & getStartDate(void)const;
  inline const std::string & getEndDate(void)const;
  inline bool containsDate(const std::string & p_date)const;

  inline void setStartDate(const std::string & p_start_date);
  inline void setEndDate(const std::string & p_end_date);


  /**
     Return type of object. This is used to name the corresponding table in database
  */
  inline const std::string getType(void)const;
 private:
  uint32_t m_livre_id;
  std::string m_start_date;
  std::string m_end_date;
  
};

inline std::ostream& operator<<(std::ostream& s, const livre_facture & p_livre_facture);

//------------------------------------------------------------------------------
const std::string livre_facture::getType(void)const
{
  return "LivreFacture";
}

//------------------------------------------------------------------------------
livre_facture::livre_facture(uint32_t p_livre_id, const std::string &p_start_date, const std::string &p_end_date):
  item(p_livre_id),
  m_start_date(p_start_date),
  m_end_date(p_end_date)
{
  assert(m_start_date < m_end_date);
}

//------------------------------------------------------------------------------
bool livre_facture::containsDate(const std::string & p_date)const
{
  return p_date >= m_start_date && p_date <= m_end_date;
}

//------------------------------------------------------------------------------
const std::string & livre_facture::getStartDate(void)const
{
  return m_start_date;
}

//------------------------------------------------------------------------------
const std::string & livre_facture::getEndDate(void)const
{
  return m_end_date;
}

//------------------------------------------------------------------------------
void livre_facture::setStartDate(const std::string & p_start_date)
{
  m_start_date = p_start_date;
}

//------------------------------------------------------------------------------
void livre_facture::setEndDate(const std::string & p_end_date)
{
  m_end_date = p_end_date;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const livre_facture & p_livre_facture)
{
  s << p_livre_facture.getType() << "{Id=" << p_livre_facture.getId() << ",start_date=\"" << p_livre_facture.m_start_date << "\",end_date=\"" << p_livre_facture.m_end_date << "\"}" ;
  return s;
}

#endif
