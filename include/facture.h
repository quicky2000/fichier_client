#ifndef FACTURE_H
#define FACTURE_H

#include "item.h"

class facture:public item
{

  friend std::ostream& operator<<(std::ostream& s, const facture & p_facture);

 public:
  inline facture(void);

  inline facture(uint32_t p_facture_ref,
		 uint32_t p_client_id,
		 const std::string &p_date,
		 uint32_t p_livre_facture_id,
		 uint32_t p_status);

  inline facture(uint32_t p_id,
		 uint32_t p_facture_ref,
		 uint32_t p_client_id,
		 const std::string &p_date,
		 uint32_t p_livre_facture_id,
		 uint32_t p_status);

  inline uint32_t get_client_id(void)const;
  inline uint32_t get_facture_ref(void)const;
  inline void set_facture_ref(uint32_t p_facture_ref);
  inline const std::string & get_date(void)const;
  inline void set_date(const std::string & p_date);
  inline uint32_t get_livre_facture_id(void)const;
  inline void set_livre_facture_id(uint32_t p_livre_facture_id);
  inline uint32_t get_status(void)const;
  inline void set_status(uint32_t p_status);

  /**
     Return type of object. This is used to name the corresponding table in database
  */
  inline const std::string getType(void)const;
 private:
  uint32_t m_facture_ref;
  uint32_t m_client_id;
  std::string m_date;
  uint32_t m_livre_facture_id;
  uint32_t m_status;
};

inline std::ostream& operator<<(std::ostream& s, const facture & p_item);

//------------------------------------------------------------------------------
facture::facture(void):
  item(),
  m_facture_ref(0),
  m_client_id(0),
  m_date(""),
  m_livre_facture_id(0),
  m_status(0)
{
}

//------------------------------------------------------------------------------
facture::facture(uint32_t p_facture_ref,
		 uint32_t p_client_id,
		 const std::string &p_date,
		 uint32_t p_livre_facture_id,
		 uint32_t p_status
		 ):
  item(),
  m_facture_ref(p_facture_ref),
  m_client_id(p_client_id),
  m_date(p_date),
  m_livre_facture_id(p_livre_facture_id),
  m_status(p_status)
{
}

//------------------------------------------------------------------------------
facture::facture(uint32_t p_id,
		 uint32_t p_facture_ref,
		 uint32_t p_client_id,
		 const std::string &p_date,
		 uint32_t p_livre_facture_id,
		 uint32_t p_status):
  item(p_id),
  m_facture_ref(p_facture_ref),
  m_client_id(p_client_id),
  m_date(p_date),
  m_livre_facture_id(p_livre_facture_id),
  m_status(p_status)
{
}

//------------------------------------------------------------------------------
uint32_t facture::get_client_id(void)const
{
  return m_client_id;
}

//------------------------------------------------------------------------------
uint32_t facture::get_facture_ref(void)const
{
  return m_facture_ref;
}

//------------------------------------------------------------------------------
void facture::set_facture_ref(uint32_t p_facture_ref)
{
  m_facture_ref = p_facture_ref;
}


//------------------------------------------------------------------------------
const std::string facture::getType(void)const
{
  return "Facture";
}

//------------------------------------------------------------------------------
const std::string & facture::get_date(void)const
{
  return m_date;
}

//------------------------------------------------------------------------------
void facture::set_date(const std::string & p_date)
{
  m_date = p_date;
}

//------------------------------------------------------------------------------
uint32_t facture::get_status(void)const
{
  return m_status;
}

//------------------------------------------------------------------------------
void facture::set_status(uint32_t p_status)
{
  m_status = p_status;
}

//------------------------------------------------------------------------------
uint32_t facture::get_livre_facture_id(void)const
{
  return m_livre_facture_id;
}

//------------------------------------------------------------------------------
void facture::set_livre_facture_id(uint32_t p_livre_facture_id)
{
  m_livre_facture_id = p_livre_facture_id;
}


//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const facture & p_facture)
{
  s << p_facture.getType() << "{Id=\"" << p_facture.get_id() << "\",Facture_ref=\"" << p_facture.get_facture_ref() << "\",ClientId=\"" << p_facture.m_client_id << "\",Date=\"" << p_facture.m_date << "\",LivreFactureId=\"" << p_facture.m_livre_facture_id << "\",Status=\"" << p_facture.m_status << "\"}" ;
  return s;
}



#endif
