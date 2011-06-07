#include "fichier_client_db.h"
#include <sqlite3.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

//------------------------------------------------------------------------------
fichier_client_db::fichier_client_db(const std::string &p_name):
  m_modified(false),
  m_db(NULL),
  m_search_client_stmt(NULL),
  m_search_achat_stmt(NULL),
  m_search_facture_by_client_id_stmt(NULL)
{
  // Opening the database
  int l_status = sqlite3_open(p_name.c_str(), &m_db);
  if(l_status == SQLITE_OK)
    {
      m_table_livre_facture.set_db(m_db);
      m_table_facture.set_db(m_db);
      m_table_ville.set_db(m_db);
      m_table_marque.set_db(m_db);
      m_table_type_achat.set_db(m_db);
      m_table_facture_status.set_db(m_db);
      m_table_achat.set_db(m_db);
      m_table_client.set_db(m_db);
      m_information_table.set_db(m_db);
    }
  else
    {
      cerr << "Can't open database \"" << p_name << "\" : " << sqlite3_errmsg(m_db) << endl ;
    }
  cout << "Database successfully opened" << endl ; 

  pair<string,string> l_info;
  
  if(get_information("db_schema_version",l_info))
    {
      cout << "Database schema version is \"" << l_info.second << endl ;
      assert(l_info.second == m_schema_version);
    }
  else
    {
      cout << "Create db_schema_version" << endl ;
      create_information("db_schema_version",m_schema_version);
    }

  // Preparing search client statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"SELECT Client.Id,Client.Name,FirstName,Address,Ville.Name FROM Client,Ville WHERE Client.VilleId = Ville.Id AND Client.Name LIKE @client_name AND FirstName LIKE @client_first_name AND Ville.Name LIKE @ville_name",-1,&m_search_client_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get search_client_item: " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing search achat statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("SELECT " + description<achat>::getClassType() + ".Id, "+ description<facture>::getClassType() +".Date,"+ description<marque>::getClassType() + ".Name,"+description<type_achat>::getClassType()+".Name,Reference,PrixEuro, PrixFranc,Garantie, "+description<facture>::getClassType()+".LivreFactureId FROM " + description<achat>::getClassType() + ","+ description<marque>::getClassType() + ","+ description<type_achat>::getClassType() + ","+ description<facture>::getClassType()+" WHERE "+ description<marque>::getClassType() + ".Id = " + description<achat>::getClassType() + ".MarqueId AND " + description<type_achat>::getClassType()+".Id = " + description<achat>::getClassType() + ".TypeId AND ClientId = $client_id AND FactureId = Facture.Id").c_str(),-1,&m_search_achat_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get search_client_achat : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing search_facture_by_client_id statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("SELECT " + description<facture>::getClassType() + ".Id, "+ description<facture>::getClassType() +".FactureRef,"+ description<facture>::getClassType() + ".Date,"+description<facture>::getClassType()+".LivreFactureId, "+description<facture_status>::getClassType()+".Name FROM " + description<facture>::getClassType() + ","+ description<facture_status>::getClassType() + " WHERE "+ description<facture>::getClassType() + ".Status = " + description<facture_status>::getClassType() + ".Id AND "+description<facture>::getClassType()+".ClientId == $client_id").c_str(),-1,&m_search_facture_by_client_id_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement get_facture_by_client_id : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
void fichier_client_db::save(void)
{
  m_modified = false;
}

//------------------------------------------------------------------------------
bool fichier_client_db::is_modified(void)const
{
  return m_modified;
}

//------------------------------------------------------------------------------
fichier_client_db::~fichier_client_db(void)
{
  cout << "Closing db" << endl ;
  sqlite3_finalize(m_search_facture_by_client_id_stmt);
  sqlite3_finalize(m_search_achat_stmt);
  sqlite3_finalize(m_search_client_stmt);
  sqlite3_close(m_db);
}

//------------------------------------------------------------------------------
void fichier_client_db::create_information(const std::string & p_key, const std::string &p_value)
{
  m_modified = true;
  m_information_table.create(p_key,p_value);
}

//------------------------------------------------------------------------------
void fichier_client_db::update_information(const std::string & p_key, const std::string &p_value)
{
  m_modified = true;
  m_information_table.update(p_key,p_value);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove_information(const std::string & p_key)
{
  m_modified = true;
  m_information_table.remove(p_key);
}

//------------------------------------------------------------------------------
uint32_t fichier_client_db::get_information(const std::string & p_key, std::pair<std::string,std::string> & p_data)
{
  return m_information_table.get(p_key,p_data);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_all_information(std::vector<std::pair<std::string,std::string> > & p_list)
{
  m_information_table.get_all(p_list);
}


//------------------------------------------------------------------------------
void fichier_client_db::create( livre_facture & p_livre_facture)
{
  m_modified = true;
  m_table_livre_facture.create(p_livre_facture);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const livre_facture & p_livre_facture)
{
  m_modified = true;
  m_table_livre_facture.update(p_livre_facture);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const livre_facture & p_livre_facture)
{
  m_modified = true;
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
void fichier_client_db::create( facture & p_facture)
{
  m_modified = true;
  m_table_facture.create(p_facture);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const facture & p_facture)
{
  m_modified = true;
  m_table_facture.update(p_facture);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const facture & p_facture)
{
  m_modified = true;
  m_table_facture.remove(p_facture);
}

//------------------------------------------------------------------------------
uint32_t fichier_client_db::get_facture(uint32_t p_id,facture & p_data)
{
  return m_table_facture.get(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_all_facture(std::vector<facture> & p_list)
{
  m_table_facture.get_all(p_list);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_by_date(const std::string & p_date,std::vector<facture> & p_result)
{
  m_table_facture.get_by_date(p_date,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_by_date_and_client_id(const std::string & p_date,uint32_t p_client_id,std::vector<facture> & p_result)
{
  m_table_facture.get_by_date_and_client_id(p_date,p_client_id,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_by_livre_facture_and_ref(uint32_t p_facture_ref, uint32_t p_livre_facture_id,std::vector<facture> & p_result)
{
  m_table_facture.get_by_livre_facture_and_ref(p_facture_ref,p_livre_facture_id,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_by_livre_facture(uint32_t p_livre_facture_id,std::vector<facture> & p_result)
{
  m_table_facture.get_by_livre_facture(p_livre_facture_id,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::create( ville & p_ville)
{
  m_modified = true;
  m_table_ville.create(p_ville);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const ville & p_ville)
{
  m_modified = true;
  m_table_ville.update(p_ville);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const ville & p_ville)
{
  m_modified = true;
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
void fichier_client_db::create( marque & p_marque)
{
  m_modified = true;
  m_table_marque.create(p_marque);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const marque & p_marque)
{
  m_modified = true;
  m_table_marque.update(p_marque);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const marque & p_marque)
{
  m_modified = true;
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
void fichier_client_db::create( type_achat & p_type_achat)
{
  m_modified = true;
  m_table_type_achat.create(p_type_achat);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const type_achat & p_type_achat)
{
  m_modified = true;
  m_table_type_achat.update(p_type_achat);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const type_achat & p_type_achat)
{
  m_modified = true;
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
void fichier_client_db::create( facture_status & p_facture_status)
{
  m_modified = true;
  m_table_facture_status.create(p_facture_status);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const facture_status & p_facture_status)
{
  m_modified = true;
  m_table_facture_status.update(p_facture_status);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const facture_status & p_facture_status)
{
  m_modified = true;
  m_table_facture_status.remove(p_facture_status);
}

//------------------------------------------------------------------------------
uint32_t fichier_client_db::get_facture_status(uint32_t p_id,facture_status & p_data)
{
  return m_table_facture_status.get(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_all_facture_status(std::vector<facture_status> & p_list)
{
  m_table_facture_status.get_all(p_list);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_facture_status_by_name(const std::string & p_name,std::vector<facture_status> & p_result)
{
  m_table_facture_status.get_by_name(p_name,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::create( achat & p_achat)
{
  m_modified = true;
  m_table_achat.create(p_achat);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const achat & p_achat)
{
  m_modified = true;
  m_table_achat.update(p_achat);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const achat & p_achat)
{
  m_modified = true;
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
void fichier_client_db::get_by_facture_id(uint32_t p_facture_id,std::vector<achat> & p_result)
{
  m_table_achat.get_by_facture_id(p_facture_id,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::create( client & p_client)
{
  m_modified = true;
  m_table_client.create(p_client);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const client & p_client)
{
  m_modified = true;
  m_table_client.update(p_client);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const client & p_client)
{
  m_modified = true;
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
void fichier_client_db::search_client(const std::string & p_name, const std::string & p_first_name, const std::string & p_city, vector<search_client_item> & p_result)
{
  //Preparing search criteria
  string l_client_name_criteria("%");
  l_client_name_criteria += p_name + "%";

  //Preparing search criteria
  string l_client_first_name_criteria("%");
  l_client_first_name_criteria += p_first_name + "%";

  //Preparing search criteria
  string l_city_criteria("%");
  l_city_criteria += p_city + "%";

  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_text(m_search_client_stmt,sqlite3_bind_parameter_index(m_search_client_stmt,"@client_name"),l_client_name_criteria.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of Client name for search client statement jointure : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(m_search_client_stmt,sqlite3_bind_parameter_index(m_search_client_stmt,"@client_first_name"),l_client_first_name_criteria.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of Client first name for search client statement jointure : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(m_search_client_stmt,sqlite3_bind_parameter_index(m_search_client_stmt,"@ville_name"),l_city_criteria.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of Client first name for search client statement jointure : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_search_client_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(search_client_item(m_search_client_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of search client result : " << m_db << endl ;
      exit(-1);
    }

  cout << "Client containing \"" << p_name << "\" in their name and \"" << p_first_name << "\" in their first name and \"" << p_city << "\" in their city successfully listed" << endl ;

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_search_client_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of search_client_statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_search_client_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of search_client statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }


}

//------------------------------------------------------------------------------
void fichier_client_db::get_achat_by_client_id(uint32_t p_client_id,std::vector<search_achat_item> & p_result)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_search_achat_stmt,sqlite3_bind_parameter_index(m_search_achat_stmt,"$client_id"),p_client_id);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of client_id parameter for search client statement jointure : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_search_achat_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(search_achat_item(m_search_achat_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of search achat result : " << m_db << endl ;
      exit(-1);
    }

  cout << "Achat for client_id " << p_client_id << " successfully listed" << endl ;

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_search_achat_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of search_achat statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_search_achat_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of search achat statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
}

//------------------------------------------------------------------------------
void fichier_client_db::get_facture_by_client_id(uint32_t p_client_id,std::vector<search_facture_item> & p_result)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_search_facture_by_client_id_stmt,sqlite3_bind_parameter_index(m_search_facture_by_client_id_stmt,"$client_id"),p_client_id);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of client_id parameter for search_facture_by_client_id statement jointure : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_search_facture_by_client_id_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(search_facture_item(m_search_facture_by_client_id_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of search_facture_by_client_id result : " << m_db << endl ;
      exit(-1);
    }

  cout << "Facture for client_id " << p_client_id << " successfully listed" << endl ;

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_search_facture_by_client_id_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of search_facture_by_client_id statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_search_facture_by_client_id_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of search_facture_by_client_id statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
}

//------------------------------------------------------------------------------
void fichier_client_db::check_db_coherency(void)
{
  cout << "Starting database coherency checking" << endl ;
  
  //Check that all id referenced in achat table really existe in the various table.
  vector<achat> l_achats;
  this->get_all_achat(l_achats);
  vector<achat>::const_iterator l_iter_achat = l_achats.begin();
  vector<achat>::const_iterator l_iter_achat_end = l_achats.end();
  while(l_iter_achat != l_iter_achat_end)
    {
      // Check facture id
      uint32_t l_facture_id = l_iter_achat->get_facture_id();
      facture l_facture;
      
      if(!m_table_facture.get(l_facture_id,l_facture))
	{
	      cout << "ERROR : no facture corresponding to id " << l_facture_id << " referenced by " << *l_iter_achat << endl ; 
	}

      // Check marque id
      uint32_t l_marque_id = l_iter_achat->get_marque_id();
      marque l_marque;
      if(!m_table_marque.get(l_marque_id,l_marque))
	{
	      cout << "ERROR : no marque corresponding to id " << l_marque_id << " referenced by " << *l_iter_achat << endl ; 
	}

      // Check type_achat id
      uint32_t l_type_achat_id = l_iter_achat->get_type_id();
      type_achat l_type_achat;
      if(!m_table_type_achat.get(l_type_achat_id,l_type_achat))
	{
	      cout << "ERROR : no type_achat corresponding to id " << l_type_achat_id << " referenced by " << *l_iter_achat << endl ; 
	}

      ++l_iter_achat;
    }

  //Check that all id referenced in facture table really existe in the various table.
  vector<facture> l_factures;
  this->get_all_facture(l_factures);
  vector<facture>::const_iterator l_iter_facture = l_factures.begin();
  vector<facture>::const_iterator l_iter_facture_end = l_factures.end();
  while(l_iter_facture != l_iter_facture_end)
    {
      
      // Check client id
      uint32_t l_client_id = l_iter_facture->get_client_id();
      cout << "Client id is " << l_client_id << endl ;
       client l_client;
      
      if(!m_table_client.get(l_client_id,l_client))
	{
	      cout << "ERROR : no client corresponding to id " << l_client_id << " referenced by " << *l_iter_facture << endl ; 
	}
      
      // Check facture ref
      if(!l_iter_facture->get_facture_ref())
	{
	  cout << "WARNING : no facture refence for facture " << *l_iter_facture << endl ;
	}

      // Check livre_facture_id 
      uint32_t l_livre_facture_id = l_iter_facture->get_livre_facture_id();
      if(l_livre_facture_id)
	{
	  livre_facture l_livre;
	  if(!m_table_livre_facture.get(l_livre_facture_id,l_livre))
	    {
	      cout << "ERROR : no livre facture corresponding to id " << l_livre_facture_id << " referenced by " << *l_iter_facture << endl ; 
	    }	 
	}
      else
	{
	  cout << "WARNING : no livre facture id for facture " << *l_iter_facture << endl ;
	}

      ++l_iter_facture;
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
      uint32_t l_client_id = l_iter_client->get_id();
      vector<search_achat_item> l_achats;
      get_achat_by_client_id(l_client_id,l_achats);
      if(!l_achats.size())
	{
	  cout << "ERROR : no achat associated to client " << *l_iter_client << endl ;
	}

      ++l_iter_client;
    }


  cout << "End of database coherency checking" << endl ;
}

const std::string fichier_client_db::m_schema_version = "1.0";

//EOF
