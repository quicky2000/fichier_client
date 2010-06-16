#ifndef LIVRE_FACTURE_H
#define LIVRE_FACTURE_H

#include <iostream>
#include <string>
#include <stdint.h>

class livre_facture
{

  friend std::ostream& operator<<(std::ostream& s, const livre_facture & p_livre_facture);

 public:
  livre_facture(uint32_t p_livre_id, const std::string &p_start_date, const std::string &p_end_date);
  uint32_t getId(void)const;
  const std::string & getStartDate(void)const;
  const std::string & getEndDate(void)const;
  bool containsDate(const std::string & p_date)const;

  void setStartDate(const std::string & p_start_date);
  void setEndDate(const std::string & p_end_date);

 private:
  uint32_t m_livre_id;
  std::string m_start_date;
  std::string m_end_date;
  
};

std::ostream& operator<<(std::ostream& s, const livre_facture & p_livre_facture);

#endif
