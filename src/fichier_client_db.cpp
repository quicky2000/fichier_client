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
  cout << "Closing db" << endl ;
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
uint32_t fichier_client_db::get_livre_facture(uint32_t p_id,livre_facture & p_data)
{
  return m_table_livre_facture.get(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_all_livre_facture(std::vector<livre_facture> & p_list)
{
  m_table_livre_facture.get_all(p_list);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_livre_facture_containing_date(const std::string & p_date,std::vector<livre_facture> & p_result)
{
  m_table_livre_facture.containing_date(p_date,p_result);
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
uint32_t fichier_client_db::get_ville(uint32_t p_id,ville &p_data)
{
  return m_table_ville.get(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_all_ville(std::vector<ville> & p_list)
{
  m_table_ville.get_all(p_list);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_ville_by_name(const std::string & p_date,std::vector<ville> & p_result)
{
  m_table_ville.get_by_name(p_date,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_ville_by_code_postal(const std::string & p_date,std::vector<ville> & p_result)
{
  m_table_ville.get_by_code_postal(p_date,p_result);
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
uint32_t fichier_client_db::get_marque(uint32_t p_id,marque & p_data)
{
  return m_table_marque.get(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_all_marque(std::vector<marque> & p_list)
{
  m_table_marque.get_all(p_list);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_marque_by_name(const std::string & p_name,std::vector<marque> & p_result)
{
  m_table_marque.get_by_name(p_name,p_result);
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
uint32_t fichier_client_db::get_type_achat(uint32_t p_id,type_achat & p_data)
{
  return m_table_type_achat.get(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_all_type_achat(std::vector<type_achat> & p_list)
{
  m_table_type_achat.get_all(p_list);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_type_achat_by_name(const std::string & p_name,std::vector<type_achat> & p_result)
{
  m_table_type_achat.get_by_name(p_name,p_result);
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
uint32_t fichier_client_db::get_achat(uint32_t p_id,achat & p_data)
{
  return m_table_achat.get(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_all_achat(std::vector<achat> & p_list)
{
  m_table_achat.get_all(p_list);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_achat_by_date(const std::string & p_date,std::vector<achat> & p_result)
{
  m_table_achat.get_by_date(p_date,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_achat_by_client_id(uint32_t p_client_id,std::vector<achat> & p_result)
{
  m_table_achat.get_by_client_id(p_client_id,p_result);
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
uint32_t fichier_client_db::get_client(uint32_t p_id,client & p_data)
{
  return m_table_client.get(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_all_client(std::vector<client> & p_list)
{
  m_table_client.get_all(p_list);
}

//------------------------------------------------------------------------------
void fichier_client_db::check_db_coherency(void)
{
  cout << "Starting database coherency checking" << endl ;
  
  //Check that all id referenced in achat table really existe in the various table.
  vector<achat> l_achats;
  this->get_all_achat(l_achats);
  vector<achat>::const_iterator l_iter = l_achats.begin();
  vector<achat>::const_iterator l_iter_end = l_achats.end();
  while(l_iter != l_iter_end)
    {
      // Check client id
      uint32_t l_client_id = l_iter->get_client_id();
      client l_client;
      
      if(!m_table_client.get(l_client_id,l_client))
	{
	      cout << "ERROR : no client corresponding to id " << l_client_id << " referenced by " << *l_iter << endl ; 
	}

      // Check marque id
      uint32_t l_marque_id = l_iter->get_marque_id();
      marque l_marque;
      if(!m_table_marque.get(l_marque_id,l_marque))
	{
	      cout << "ERROR : no marque corresponding to id " << l_marque_id << " referenced by " << *l_iter << endl ; 
	}

      // Check type_achat id
      uint32_t l_type_achat_id = l_iter->get_type_id();
      type_achat l_type_achat;
      if(!m_table_type_achat.get(l_type_achat_id,l_type_achat))
	{
	      cout << "ERROR : no type_achat corresponding to id " << l_type_achat_id << " referenced by " << *l_iter << endl ; 
	}

      // Check livre facture id
      uint32_t l_livre_facture_id = l_iter->get_livre_facture_id();
      if(l_livre_facture_id)
	{
	  livre_facture l_livre;
	  if(!m_table_livre_facture.get(l_livre_facture_id,l_livre))
	    {
	      cout << "ERROR : no livre facture corresponding to id " << l_livre_facture_id << " referenced by " << *l_iter << endl ; 
	    }	 
	}
      else
	{
	  cout << "WARNING : no livre facture assosicated with " << *l_iter << endl;
	}
      ++l_iter;
    }


  //Check that all id referenced in client table really existe in the various table.
  vector<client> l_clients;
  this->get_all_client(l_clients);
  vector<client>::const_iterator l_iter_client = l_clients.begin();
  vector<client>::const_iterator l_iter_client_end = l_clients.end();
  while(l_iter_client != l_iter_client_end)
    {
      // Check vile id
      uint32_t l_ville_id = l_iter_client->get_ville_id();
      ville l_ville;
      
      if(!m_table_ville.get(l_ville_id,l_ville))
	{
	      cout << "ERROR : no ville corresponding to id " << l_ville_id << " referenced by " << *l_iter_client << endl ; 
	}

      // Check if client has associated achats
      uint32_t l_client_id = l_iter_client->getId();
      vector<achat> l_achats;
      m_table_achat.get_by_client_id(l_client_id,l_achats);
      if(!l_achats.size())
	{
	  cout << "ERROR : no achat associated to client " << *l_iter_client << endl ;
	}

      ++l_iter_client;
    }


  cout << "End of database coherency checking" << endl ;
}

//EOF
