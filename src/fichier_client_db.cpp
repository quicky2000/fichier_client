#include "fichier_client_db.h"
#include <sqlite3.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

//------------------------------------------------------------------------------
fichier_client_db::fichier_client_db(const std::string &p_name):
  m_db(NULL)
{
  // Opening the database
  int l_status = sqlite3_open(p_name.c_str(), &m_db);
  if(l_status == SQLITE_OK)
    {
      m_table_livre_facture.set_db(m_db);
      m_table_ville.set_db(m_db);
      m_table_marque.set_db(m_db);
      m_table_type_achat.set_db(m_db);
    }
  else
    {
      cerr << "Can't open database \"" << p_name << "\" : " << sqlite3_errmsg(m_db) << endl ;
    }
  cout << "Database successfully opened" << endl ; 

}

//------------------------------------------------------------------------------
fichier_client_db::~fichier_client_db(void)
{
  sqlite3_close(m_db);
}

//------------------------------------------------------------------------------
void fichier_client_db::create(const livre_facture & p_livre_facture)
{
  m_table_livre_facture.create(p_livre_facture);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const livre_facture & p_livre_facture)
{
  m_table_livre_facture.update(p_livre_facture);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const livre_facture & p_livre_facture)
{
  m_table_livre_facture.remove(p_livre_facture);
}

//------------------------------------------------------------------------------
livre_facture * fichier_client_db::get_livre_facture(uint32_t p_id)
{
  return m_table_livre_facture.get(p_id);
}

//------------------------------------------------------------------------------
const std::vector<livre_facture> * fichier_client_db::get_all_livre_facture(void)
{
  return m_table_livre_facture.get_all();
}

//------------------------------------------------------------------------------
const std::vector<livre_facture> * fichier_client_db::get_livre_facture_containing_date(const std::string & p_date)
{
  return m_table_livre_facture.containing_date(p_date);
}

//------------------------------------------------------------------------------
void fichier_client_db::create(const ville & p_ville)
{
  m_table_ville.create(p_ville);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const ville & p_ville)
{
  m_table_ville.update(p_ville);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const ville & p_ville)
{
  m_table_ville.remove(p_ville);
}

//------------------------------------------------------------------------------
ville * fichier_client_db::get_ville(uint32_t p_id)
{
  return m_table_ville.get(p_id);
}

//------------------------------------------------------------------------------
const std::vector<ville> * fichier_client_db::get_all_ville(void)
{
  return m_table_ville.get_all();
}

//------------------------------------------------------------------------------
const std::vector<ville> * fichier_client_db::get_ville_by_name(const std::string & p_date)
{
  return m_table_ville.get_by_name(p_date);
}

//------------------------------------------------------------------------------
const std::vector<ville> * fichier_client_db::get_ville_by_code_postal(const std::string & p_date)
{
  return m_table_ville.get_by_code_postal(p_date);
}

//------------------------------------------------------------------------------
void fichier_client_db::create(const marque & p_marque)
{
  m_table_marque.create(p_marque);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const marque & p_marque)
{
  m_table_marque.update(p_marque);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const marque & p_marque)
{
  m_table_marque.remove(p_marque);
}

//------------------------------------------------------------------------------
marque * fichier_client_db::get_marque(uint32_t p_id)
{
  return m_table_marque.get(p_id);
}

//------------------------------------------------------------------------------
const std::vector<marque> * fichier_client_db::get_all_marque(void)
{
  return m_table_marque.get_all();
}

//------------------------------------------------------------------------------
const std::vector<marque> * fichier_client_db::get_marque_by_name(const std::string & p_name)
{
  return m_table_marque.get_by_name(p_name);
}

//------------------------------------------------------------------------------
void fichier_client_db::create(const type_achat & p_type_achat)
{
  m_table_type_achat.create(p_type_achat);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const type_achat & p_type_achat)
{
  m_table_type_achat.update(p_type_achat);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const type_achat & p_type_achat)
{
  m_table_type_achat.remove(p_type_achat);
}

//------------------------------------------------------------------------------
type_achat * fichier_client_db::get_type_achat(uint32_t p_id)
{
  return m_table_type_achat.get(p_id);
}

//------------------------------------------------------------------------------
const std::vector<type_achat*> * fichier_client_db::get_all_type_achat(void)
{
  return m_table_type_achat.get_all();
}

//------------------------------------------------------------------------------
const std::vector<type_achat*> * fichier_client_db::get_type_achat_by_name(const std::string & p_name)
{
  return m_table_type_achat.get_by_name(p_name);
}

//EOF
