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
      m_table_achat.set_db(m_db);
      m_table_client.set_db(m_db);
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
const std::vector<livre_facture*> * fichier_client_db::get_all_livre_facture(void)
{
  return m_table_livre_facture.get_all();
}

//------------------------------------------------------------------------------
const std::vector<livre_facture*> * fichier_client_db::get_livre_facture_containing_date(const std::string & p_date)
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
const std::vector<ville*> * fichier_client_db::get_all_ville(void)
{
  return m_table_ville.get_all();
}

//------------------------------------------------------------------------------
const std::vector<ville*> * fichier_client_db::get_ville_by_name(const std::string & p_date)
{
  return m_table_ville.get_by_name(p_date);
}

//------------------------------------------------------------------------------
const std::vector<ville*> * fichier_client_db::get_ville_by_code_postal(const std::string & p_date)
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
const std::vector<marque*> * fichier_client_db::get_all_marque(void)
{
  return m_table_marque.get_all();
}

//------------------------------------------------------------------------------
const std::vector<marque*> * fichier_client_db::get_marque_by_name(const std::string & p_name)
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

//------------------------------------------------------------------------------
void fichier_client_db::create(const achat & p_achat)
{
  m_table_achat.create(p_achat);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const achat & p_achat)
{
  m_table_achat.update(p_achat);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const achat & p_achat)
{
  m_table_achat.remove(p_achat);
}

//------------------------------------------------------------------------------
achat * fichier_client_db::get_achat(uint32_t p_id)
{
  return m_table_achat.get(p_id);
}

//------------------------------------------------------------------------------
const std::vector<achat*> * fichier_client_db::get_all_achat(void)
{
  return m_table_achat.get_all();
}

//------------------------------------------------------------------------------
const std::vector<achat*> * fichier_client_db::get_achat_by_date(const std::string & p_date)
{
  return m_table_achat.get_by_date(p_date);
}

//------------------------------------------------------------------------------
void fichier_client_db::create(const client & p_client)
{
  m_table_client.create(p_client);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const client & p_client)
{
  m_table_client.update(p_client);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const client & p_client)
{
  m_table_client.remove(p_client);
}

//------------------------------------------------------------------------------
client * fichier_client_db::get_client(uint32_t p_id)
{
  return m_table_client.get(p_id);
}

//------------------------------------------------------------------------------
const std::vector<client*> * fichier_client_db::get_all_client(void)
{
  return m_table_client.get_all();
}

//------------------------------------------------------------------------------
void fichier_client_db::check_db_coherency(void)
{
  cout << "Starting database coherency checking" << endl ;
  
  //Check that all id referenced in achat table really existe in the various table.
  const vector<achat*> * l_achats = this->get_all_achat();
  vector<achat*>::const_iterator l_iter = l_achats->begin();
  vector<achat*>::const_iterator l_iter_end = l_achats->end();
  while(l_iter != l_iter_end)
    {
      // Check client id
      uint32_t l_client_id = (*l_iter)->get_client_id();
      client * l_client = m_table_client.get(l_client_id);
      if(l_client == NULL)
	{
	      cout << "ERROR : no client corresponding to id " << l_client_id << " referenced by " << **l_iter << endl ; 
	}

      // Check type_achat id
      uint32_t l_type_achat_id = (*l_iter)->get_type_id();
      type_achat * l_type_achat = m_table_type_achat.get(l_type_achat_id);
      if(l_type_achat == NULL)
	{
	      cout << "ERROR : no type_achat corresponding to id " << l_type_achat_id << " referenced by " << **l_iter << endl ; 
	}

      // Check livre facture id
      uint32_t l_livre_facture_id = (*l_iter)->get_livre_facture_id();
      if(l_livre_facture_id)
	{
	  livre_facture * l_livre = m_table_livre_facture.get(l_livre_facture_id);
	  if(l_livre == NULL)
	    {
	      cout << "ERROR : no livre facture corresponding to id " << l_livre_facture_id << " referenced by " << **l_iter << endl ; 
	    }
	}
      else
	{
	  cout << "WARNING : no livre facture assosicated with " << *l_iter << endl;
	}
      ++l_iter;
    }

  cout << "End of database coherency checking" << endl ;
}

//EOF
