#include "livre_facture.h"
#include "assert.h"

using namespace std;

//------------------------------------------------------------------------------
livre_facture::livre_facture(uint32_t p_livre_id, const string &p_start_date, const string &p_end_date):
  m_livre_id(p_livre_id),
  m_start_date(p_start_date),
  m_end_date(p_end_date)
{
  assert(p_livre_id);
  assert(m_start_date < m_end_date);
}

//------------------------------------------------------------------------------
bool livre_facture::containsDate(const std::string & p_date)const
{
  return p_date >= m_start_date && p_date <= m_end_date;
}

//------------------------------------------------------------------------------
uint32_t livre_facture::getId(void)const
{
  return m_livre_id;
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
  s << "livre_facture{Id=" << p_livre_facture.m_livre_id << ",start_date=\"" << p_livre_facture.m_start_date << "\",end_date=\"" << p_livre_facture.m_end_date << "\"}" ;
  return s;
}

//EOF
