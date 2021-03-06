#ifndef ACHAT_H
#define ACHAT_H

#include "item.h"

class achat:public item
{

  friend std::ostream& operator<<(std::ostream& s, const achat & p_achat);

 public:
  inline achat(void);

  inline achat(uint32_t p_facture_id,
	       uint32_t p_marque_id,
	       uint32_t p_type_id,
	       const std::string & p_reference ,
	       float p_prix_franc,
	       float p_prix_euro,
	       bool p_garantie);

  inline achat(uint32_t p_id,
	       uint32_t p_facture_id,
	       uint32_t p_marque_id,
	       uint32_t p_type_id,
	       const std::string & p_reference ,
	       float p_prix_franc,
	       float p_prix_euro,
	       bool p_garantie);

  inline uint32_t get_facture_id(void)const;
  inline void set_facture_id(uint32_t p_facture_id);
  inline uint32_t get_marque_id(void)const;
  inline void set_marque_id(uint32_t p_marque_id);
  inline uint32_t get_type_id(void)const;
  inline void set_type_id(uint32_t p_type_id);
  inline const std::string & get_reference(void)const;
  inline void set_reference(const std::string & p_reference);
  inline float get_prix_franc(void)const;
  inline void set_prix_franc(float p_prix_franc);
  inline float get_prix_euro(void)const;
  inline void set_prix_euro(float p_prix_euro);
  inline bool get_garantie(void)const;
  inline void set_garantie(bool p_garantie);

  /**
     Return type of object. This is used to name the corresponding table in database
  */
  inline const std::string get_type(void)const;
 private:
  uint32_t m_facture_id;
  uint32_t m_marque_id;
  uint32_t m_type_id;
  std::string m_reference;
  float m_prix_franc;
  float m_prix_euro;
  bool m_garantie;
};

inline std::ostream& operator<<(std::ostream& s, const achat & p_item);

//------------------------------------------------------------------------------
achat::achat(void):
  item(),
  m_facture_id(0),
  m_marque_id(0),
  m_type_id(0),
  m_reference(""),
  m_prix_franc(0.0),
  m_prix_euro(0.0),
  m_garantie(false)
{
}

//------------------------------------------------------------------------------
achat::achat(uint32_t p_facture_id,
	     uint32_t p_marque_id,
	     uint32_t p_type_id,
	     const std::string & p_reference ,
	     float p_prix_franc,
	     float p_prix_euro,
	     bool p_garantie
	     ):
  item(),
  m_facture_id(p_facture_id),
  m_marque_id(p_marque_id),
  m_type_id(p_type_id),
  m_reference(p_reference),
  m_prix_franc(p_prix_franc),
  m_prix_euro(p_prix_euro),
  m_garantie(p_garantie)
{
}

//------------------------------------------------------------------------------
achat::achat(uint32_t p_id,
	     uint32_t p_facture_id,
	     uint32_t p_marque_id,
	     uint32_t p_type_id,
	     const std::string & p_reference ,
	     float p_prix_franc,
	     float p_prix_euro,
	     bool p_garantie):
  item(p_id),
  m_facture_id(p_facture_id),
  m_marque_id(p_marque_id),
  m_type_id(p_type_id),
  m_reference(p_reference),
  m_prix_franc(p_prix_franc),
  m_prix_euro(p_prix_euro),
  m_garantie(p_garantie)
{
}

//------------------------------------------------------------------------------
const std::string achat::get_type(void)const
{
  return "Achat";
}

//------------------------------------------------------------------------------
uint32_t achat::get_marque_id(void)const
{
  return m_marque_id;
}

//------------------------------------------------------------------------------
void achat::set_marque_id(uint32_t p_marque_id)
{
  m_marque_id = p_marque_id;
}

//------------------------------------------------------------------------------
uint32_t achat::get_facture_id(void)const
{
  return m_facture_id;
}

//------------------------------------------------------------------------------
void achat::set_facture_id(uint32_t p_facture_id)
{
  m_facture_id = p_facture_id;
}

//------------------------------------------------------------------------------
uint32_t achat::get_type_id(void)const
{
  return m_type_id;
}

//------------------------------------------------------------------------------
void achat::set_type_id(uint32_t p_type_id)
{
  m_type_id = p_type_id;
}

//------------------------------------------------------------------------------
const std::string & achat::get_reference(void)const
{
  return m_reference;
}

//------------------------------------------------------------------------------
void achat::set_reference(const std::string & p_reference)
{
  m_reference = p_reference;
}

//------------------------------------------------------------------------------
float achat::get_prix_franc(void)const
{
  return m_prix_franc;
}

//------------------------------------------------------------------------------
void achat::set_prix_franc(float p_prix_franc)
{
  m_prix_franc = p_prix_franc;
}

//------------------------------------------------------------------------------
float achat::get_prix_euro(void)const
{
  return m_prix_euro;
}

//------------------------------------------------------------------------------
void achat::set_prix_euro(float p_prix_euro)
{
  m_prix_euro = p_prix_euro;
}

//------------------------------------------------------------------------------
bool achat::get_garantie(void)const
{
  return m_garantie;
}

//------------------------------------------------------------------------------
void achat::set_garantie(bool p_garantie)
{
  m_garantie = p_garantie;
}


//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const achat & p_achat)
{
  s << p_achat.get_type() << "{Id=" << p_achat.get_id() << "\",FactureId=\"" << p_achat.m_facture_id << "\", MarqueId=\"" << p_achat.m_marque_id << "\",TypeId=\"" << p_achat.m_type_id << "\",Reference=\"" << p_achat.m_reference << "\",PrixFranc=\"" << p_achat.m_prix_franc << "\",PrixEuro=\"" << p_achat.m_prix_euro << "\",Garantie=\"" << (p_achat.m_garantie ? "oui" : "non" )<< "\"}" ;
  return s;
}



#endif
