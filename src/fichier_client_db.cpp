#include "fichier_client_db.h"
#include "compatibility_db.h"
#include <sqlite3.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

//------------------------------------------------------------------------------
fichier_client_db::fichier_client_db(const std::string &p_name):
  m_modified(false),
  m_db(NULL),
  m_get_complete_client_stmt(NULL),
  m_search_client_stmt(NULL),
  m_search_achat_stmt(NULL),
  m_search_purchase_by_bill_id_stmt(NULL),
  m_search_facture_by_client_id_stmt(NULL),
  m_search_facture_by_livre_facture_id_stmt(NULL)
{
  // Opening the database in compatibility mode
  compatibility_db * l_compatibility_db = new compatibility_db(p_name,m_schema_version);
  delete l_compatibility_db;

  // Opening the database
  int l_status = sqlite3_open(p_name.c_str(), &m_db);
  if(l_status == SQLITE_OK)
    {
      m_table_livre_facture.set_db(m_db);
      m_table_facture.set_db(m_db);
      m_table_ville.set_db(m_db);
      m_table_marque.set_db(m_db);
      m_table_type_achat.set_db(m_db);
      m_table_facture_reason.set_db(m_db);
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

  // Preparing get_complete_client statement
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,
				("SELECT " +
				 description<client>::get_class_type() + ".Id, " +
				 description<client>::get_class_type() + ".Name, " + 
				 description<client>::get_class_type() + ".FirstName, " +
				 description<client>::get_class_type() + ".Address, " +
				 description<ville>::get_class_type() + ".Name " +
				 " FROM " +
				 description<client>::get_class_type() + ", " +
				 description<ville>::get_class_type() + 
				 " WHERE "+
				 description<client>::get_class_type() + ".CityId" +
				 " == " +
				 description<ville>::get_class_type() + ".Id" +
				 " AND " +
				 description<client>::get_class_type() + ".Id" +
				 " == " +
				 "$client_id"
				 ).c_str(),
				-1,
				&m_get_complete_client_stmt,
				NULL);

  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement get_complete_client : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing search client statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("SELECT " +
				      description<client>::get_class_type() + ".Id, " +
				      description<client>::get_class_type() + ".Name, "+
				      "FirstName, " +
				      "Address, " +
				      description<ville>::get_class_type() + ".Name " +
				      "FROM " + 
				      description<client>::get_class_type() + ", " + 
				      description<ville>::get_class_type() +
				      " WHERE " +
				      description<client>::get_class_type() + ".CityId = " + description<ville>::get_class_type() + ".Id "+ 
				      " AND " +
				      description<client>::get_class_type() + ".Name LIKE @client_name" +
				      " AND " +
				      "FirstName LIKE @client_first_name" +
				      " AND " +
				      "Address LIKE @client_address"+
				      " AND "+
				      description<ville>::get_class_type() + ".Name LIKE @ville_name"
				      ).c_str(),
				-1,
				&m_search_client_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get search_client_item: " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing search achat statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("SELECT " + 
				      description<achat>::get_class_type() + ".Id, " + 
				      description<facture>::get_class_type() +".Date, " + 
				      description<marque>::get_class_type() + ".Name, " + 
				      description<type_achat>::get_class_type()+".Name, " +
				      "Reference, " + 
				      "PriceEuro, " +
				      "PriceFranc, " +
				      "Warranty, " +
				      description<facture>::get_class_type()+".BillBookId " +
				      " FROM " + 
				      description<achat>::get_class_type() + 
				      "," + 
				      description<marque>::get_class_type() + 
				      ","+ 
				      description<type_achat>::get_class_type() + 
				      "," + 
				      description<facture>::get_class_type() + 
				      " WHERE " + 
				      description<marque>::get_class_type() + ".Id"
				      " = " + 
				      description<achat>::get_class_type() + ".BrandId" +
				      " AND " + 
				      description<type_achat>::get_class_type()+".Id" +
				      " = " + 
				      description<achat>::get_class_type() + ".TypeId" +
				      " AND " + 
				      "CustomerId = $client_id" +
				      " AND " +
				      "BillId" + 
				      " = " +
				      description<facture>::get_class_type() + ".Id"
				      ).c_str(),
				-1,
				&m_search_achat_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get search_client_achat : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing search purchase by bill id statements
  //------------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("SELECT " + 
				      description<achat>::get_class_type() + ".Id, " + 
				      description<facture>::get_class_type() +".Date, " + 
				      description<marque>::get_class_type() + ".Name, " + 
				      description<type_achat>::get_class_type()+".Name, " +
				      "Reference, " + 
				      "PriceEuro, " +
				      "PriceFranc, " +
				      "Warranty, " +
				      description<facture>::get_class_type()+".BillBookId " +
				      " FROM " + 
				      description<achat>::get_class_type() + 
				      "," + 
				      description<marque>::get_class_type() + 
				      ","+ 
				      description<type_achat>::get_class_type() + 
				      "," + 
				      description<facture>::get_class_type() + 
				      " WHERE " + 
				      description<marque>::get_class_type() + ".Id"
				      " = " + 
				      description<achat>::get_class_type() + ".BrandId" +
				      " AND " + 
				      description<type_achat>::get_class_type()+".Id" +
				      " = " + 
				      description<achat>::get_class_type() + ".TypeId" +
				      " AND " +
				      "BillId" + 
				      " = " +
				      description<facture>::get_class_type() + ".Id" +
				      " AND " +
				      "BillId" + 
				      " == " + 
				      "$bill_id"
				      ).c_str(),
				-1,
				&m_search_purchase_by_bill_id_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get search purchase by bill id statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing search_facture_by_client_id statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,
				("SELECT " + 
				 description<facture>::get_class_type() + ".Id, " + 
				 description<facture>::get_class_type() +".BillRef," + 
				 description<facture>::get_class_type() + ".Date," + 
				 description<facture>::get_class_type()+".BillBookId, " + 
				 description<facture_status>::get_class_type()+".Name" + 
				 " FROM " + 
				 description<facture>::get_class_type() + "," + 
				 description<facture_status>::get_class_type() + 
				 " WHERE " + 
				 description<facture>::get_class_type() + ".StatusId" + 
				 " == " + 
				 description<facture_status>::get_class_type() + ".Id" + 
				 " AND " + 
				 description<facture>::get_class_type()+".CustomerId" + 
				 " == " + 
				 "$client_id"
				 ).c_str(),
				-1,
				&m_search_facture_by_client_id_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement get_facture_by_client_id : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing search_facture_by_livre_id statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,
				("SELECT " + 
				 description<facture>::get_class_type() + ".Id, " +
				 description<facture>::get_class_type() + ".BillRef, " +
				 description<facture>::get_class_type() + ".Date, " +
				 description<facture>::get_class_type() + ".BillBookId, " +
				 description<facture_status>::get_class_type() + ".Name, " +
				 description<facture>::get_class_type() + ".CustomerId, " +
				 description<facture>::get_class_type() + ".ReasonId " +
				 " FROM " + 
				 description<facture>::get_class_type() + ", " + 
				 description<facture_status>::get_class_type() + " " +
				 " WHERE " + 
				 description<facture>::get_class_type() + ".StatusId" + 
				 " == " + 
				 description<facture_status>::get_class_type() + ".Id"  +
				 " AND " + 
				 description<facture>::get_class_type()+".BillBookId" + 
				 " == " + 
				 "$livre_facture_id" +
				 " ORDER BY " +
				 description<facture>::get_class_type() + ".BillRef"
				 ).c_str()
				,-1,
				&m_search_facture_by_livre_facture_id_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement get_facture_by_livre_facture_id : " << sqlite3_errmsg(m_db) << std::endl ;     
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
  sqlite3_finalize(m_search_facture_by_livre_facture_id_stmt);
  sqlite3_finalize(m_search_facture_by_client_id_stmt);
  sqlite3_finalize(m_search_purchase_by_bill_id_stmt);
  sqlite3_finalize(m_search_achat_stmt);
  sqlite3_finalize(m_search_client_stmt);
  sqlite3_finalize(m_get_complete_client_stmt);
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
void fichier_client_db::get_by_status(uint32_t p_facture_status_id,std::vector<facture> & p_result)
{
  m_table_facture.get_by_status(p_facture_status_id,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_by_reason(uint32_t p_facture_reason_id,std::vector<facture> & p_result)
{
  m_table_facture.get_by_reason(p_facture_reason_id,p_result);
}

//------------------------------------------------------------------------------
std::pair<std::string,std::string> fichier_client_db::get_min_max_date(uint32_t p_livre_facture_id)
{
  std::string l_min_date("9999-99-99");
  std::string l_max_date("0000-00-00");
  std::vector<facture> l_facture_list;
  get_by_livre_facture(p_livre_facture_id,l_facture_list);
  std::vector<facture>::const_iterator l_iter = l_facture_list.begin();
  std::vector<facture>::const_iterator l_iter_end = l_facture_list.end();
  while(l_iter != l_iter_end)
    {
      if(l_iter->get_date() != "")
	{
	  if(l_iter->get_date() < l_min_date)
	    {
	      l_min_date = l_iter->get_date();
	    }
	  if(l_iter->get_date() > l_max_date)
	    {
	      l_max_date = l_iter->get_date();
	    }
	}
      ++l_iter;
    }
  return std::pair<std::string,std::string>(l_min_date,l_max_date);
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
void fichier_client_db::get_ville_by_name(const std::string & p_date,std::vector<ville> & p_result, bool p_exact)
{
  m_table_ville.get_by_name(p_date,p_result,p_exact);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_ville_by_code_postal(const std::string & p_date,std::vector<ville> & p_result)
{
  m_table_ville.get_by_code_postal(p_date,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_city_by_name_and_postal_code(const std::string & p_name,const std::string & p_code_postal,std::vector<ville> & p_result)
{
  m_table_ville.get_by_name_and_postal_code(p_name,p_code_postal,p_result);
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
void fichier_client_db::get_marque_by_name(const std::string & p_name,std::vector<marque> & p_result, bool p_exact)
{
  m_table_marque.get_by_name(p_name,p_result,p_exact);
}

//------------------------------------------------------------------------------
void fichier_client_db::create( facture_reason & p_facture_reason)
{
  m_modified = true;
  m_table_facture_reason.create(p_facture_reason);
}

//------------------------------------------------------------------------------
void fichier_client_db::update(const facture_reason & p_facture_reason)
{
  m_modified = true;
  m_table_facture_reason.update(p_facture_reason);
}

//------------------------------------------------------------------------------
void fichier_client_db::remove(const facture_reason & p_facture_reason)
{
  m_modified = true;
  m_table_facture_reason.remove(p_facture_reason);
}

//------------------------------------------------------------------------------
uint32_t fichier_client_db::get_facture_reason(uint32_t p_id,facture_reason & p_data)
{
  return m_table_facture_reason.get(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_all_facture_reason(std::vector<facture_reason> & p_list)
{
  m_table_facture_reason.get_all(p_list);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_facture_reason_by_name(const std::string & p_name,std::vector<facture_reason> & p_result, bool p_exact)
{
  m_table_facture_reason.get_by_name(p_name,p_result,p_exact);
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
void fichier_client_db::get_type_achat_by_name(const std::string & p_name,std::vector<type_achat> & p_result,bool p_exact)
{
  m_table_type_achat.get_by_name(p_name,p_result,p_exact);
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
void fichier_client_db::get_facture_status_by_name(const std::string & p_name,std::vector<facture_status> & p_result,bool p_exact)
{
  m_table_facture_status.get_by_name(p_name,p_result,p_exact);
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
void fichier_client_db::get_by_facture_id(uint32_t p_id,std::vector<achat> & p_result)
{
  m_table_achat.get_by_facture_id(p_id,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_by_type_id(uint32_t p_id,std::vector<achat> & p_result)
{
  m_table_achat.get_by_type_id(p_id,p_result);
}

//------------------------------------------------------------------------------
void fichier_client_db::get_by_brand_id(uint32_t p_id,std::vector<achat> & p_result)
{
  m_table_achat.get_by_brand_id(p_id,p_result);
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
void fichier_client_db::get_customer_by_city(uint32_t p_city_id,std::vector<client> & p_list)
{
  m_table_client.get_by_city(p_city_id,p_list);
}

//------------------------------------------------------------------------------
void fichier_client_db::search_client(const std::string & p_name, const std::string & p_first_name,const std::string & p_address, const std::string & p_city, vector<search_client_item> & p_result, bool p_exact)
{
  //Preparing search criteria
  string l_client_name_criteria = (p_exact ? p_name : std::string("%") + p_name + "%");

  //Preparing search criteria
  string l_client_first_name_criteria = (p_exact ? p_first_name  : std::string("%") + p_first_name + "%");

  //Preparing search criteria
  string l_client_address_criteria = (p_exact ? p_address : std::string("%") + p_address + "%");

  //Preparing search criteria
  string l_city_criteria = (p_exact ? p_city  : std::string("%") + p_city + "%");

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
  
  l_status = sqlite3_bind_text(m_search_client_stmt,sqlite3_bind_parameter_index(m_search_client_stmt,"@client_address"),l_client_address_criteria.c_str(),-1,SQLITE_STATIC);
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
void fichier_client_db::get_purchase_by_bill_id(uint32_t p_bill_id,std::vector<search_achat_item> & p_result)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_search_purchase_by_bill_id_stmt,sqlite3_bind_parameter_index(m_search_purchase_by_bill_id_stmt,"$bill_id"),p_bill_id);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of bill_id parameter for search purchase by bill id statement jointure : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_search_purchase_by_bill_id_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(search_achat_item(m_search_purchase_by_bill_id_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of search purchase result : " << m_db << endl ;
      exit(-1);
    }

  cout << "Purchase for bill_id " << p_bill_id << " successfully listed" << endl ;

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_search_purchase_by_bill_id_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of search purchase by bill id statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_search_purchase_by_bill_id_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of search purchase by bill id statement : " << sqlite3_errmsg(m_db) << endl ;     
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
void fichier_client_db::get_facture_by_livre_facture_id(uint32_t p_livre_facture_id,std::vector<search_facture_item> & p_result)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_search_facture_by_livre_facture_id_stmt,sqlite3_bind_parameter_index(m_search_facture_by_livre_facture_id_stmt,"$livre_facture_id"),p_livre_facture_id);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of livre_facture_id parameter for search_facture_by_livre_facture_id statement jointure : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_search_facture_by_livre_facture_id_stmt)) == SQLITE_ROW)
    {
      cout << "result siwe = " << p_result.size() << endl ;
      p_result.push_back(search_facture_item(m_search_facture_by_livre_facture_id_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of search_facture_by_livre_facture_id result : " << m_db << endl ;
      exit(-1);
    }

  cout << "Facture for livre_facture_id " << p_livre_facture_id << " successfully listed" << endl ;

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_search_facture_by_livre_facture_id_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of search_facture_by_livre_facture_id statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_search_facture_by_livre_facture_id_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of search_facture_by_livre_facture_id statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
}

//------------------------------------------------------------------------------
void fichier_client_db::get_complete_client(uint32_t p_client_id,search_client_item & p_result)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_get_complete_client_stmt,sqlite3_bind_parameter_index(m_get_complete_client_stmt,"$client_id"),p_client_id);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of client_id parameter for get_complete_client statement jointure : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_get_complete_client_stmt);
  if(l_status == SQLITE_ROW)
    {
      p_result = search_client_item(m_get_complete_client_stmt);
    }

  // Ensure that ID is unique
  l_status = sqlite3_step(m_get_complete_client_stmt);
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of get_complete_client result : " << m_db << endl ;
      exit(-1);
    }

  cout << "Complete client for client_id " << p_client_id << " successfully get" << endl ;

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_complete_client_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of get_complete_client statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_complete_client_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of get_complete_client statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
}


//------------------------------------------------------------------------------
void fichier_client_db::check_db_coherency(uint32_t & p_nb_error,uint32_t & p_nb_warning,std::vector<coherency_report_item> & p_error_list,std::vector<coherency_report_item> & p_warning_list)
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

      if(l_facture_id)
	{
	  facture l_facture;
	  
	  if(!m_table_facture.get(l_facture_id,l_facture))
	    { 
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "La facture d ID " << l_facture_id << " r�f�renc�e par " << *l_iter_achat << " n existe pas";
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_achat->get_id(),"Achat",l_message.str()));
	    }
	}
      else
	{
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "L'achat " << *l_iter_achat << " n est associ� a aucune facture";
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_achat->get_id(),"Achat",l_message.str()));	  
	}

      // Check marque id
      uint32_t l_marque_id = l_iter_achat->get_marque_id();
      if(l_marque_id)
	{
	  marque l_marque;
	  if(!m_table_marque.get(l_marque_id,l_marque))
	    {
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "La marque d'ID " << l_marque_id << " referenc�e par " << *l_iter_achat << " n existe pas"; 
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_achat->get_id(),"Achat",l_message.str()));
	    }
	}
      else
	{
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "L'achat " << *l_iter_achat << " n est associ� a aucune marque";
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_achat->get_id(),"Achat",l_message.str()));	  	  
	}

      // Check type_achat id
      uint32_t l_type_achat_id = l_iter_achat->get_type_id();
      if(l_type_achat_id)
	{
	  type_achat l_type_achat;
	  if(!m_table_type_achat.get(l_type_achat_id,l_type_achat))
	    {
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "Le type d'achat d'ID " << l_type_achat_id << " r�f�renc� par " << *l_iter_achat << " n'existe pas";
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_achat->get_id(),"Achat",l_message.str()));
	    }
	}
      else
	{
	  ++p_nb_error;
	  stringstream l_message;
	  l_message << "L'achat " << *l_iter_achat << " n est associ� a aucun type";
	  p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_achat->get_id(),"Achat",l_message.str()));	  	  
	}

      // Check that franc price is not null
      if(l_iter_achat->get_prix_franc()==0)
	{
	  ++p_nb_warning;
	  stringstream l_message;
	  p_warning_list.push_back(coherency_report_item(coherency_report_item::WARNING,l_iter_achat->get_id(),"Achat","L achat a un prix en franc a z�ro"));
	}

      // Check that euro price is not null
      if(l_iter_achat->get_prix_euro()==0)
	{
	  ++p_nb_warning;
	  p_warning_list.push_back(coherency_report_item(coherency_report_item::WARNING,l_iter_achat->get_id(),"Achat","L achat a un prix en euro a z�ro"));
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
      
      // Check facture ref
      if(!l_iter_facture->get_facture_ref())
	{
	  ++p_nb_warning;
	  stringstream l_message;
	  l_message << "R�f�rence a z�ro pour la facture " <<  *l_iter_facture;
	  p_warning_list.push_back(coherency_report_item(coherency_report_item::WARNING,l_iter_facture->get_id(),"Facture",l_message.str()));
	}

      // Check status id
      uint32_t l_facture_status_id = l_iter_facture->get_status();
      if(l_facture_status_id)
	{
	  facture_status l_facture_status;      
	  if(!m_table_facture_status.get(l_facture_status_id,l_facture_status))
	    {
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "Le facture_status d'ID " << l_facture_status_id << " r�f�renc� par " << *l_iter_facture << " n existe pas" ;
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));
	    }
	}
      else
	{
	  ++p_nb_error;
	  stringstream l_message;
	  l_message << "La facture " << *l_iter_facture << " n est associ�e a aucun status";
	  p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_achat->get_id(),"Facture",l_message.str()));	  	  
	  
	}
      
      // Check client id
      uint32_t l_client_id = l_iter_facture->get_client_id();
      if(l_client_id)
	{
	  client l_client;      
	  if(!m_table_client.get(l_client_id,l_client))
	    {
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "Le client d'ID " << l_client_id << " r�f�renc� par " << *l_iter_facture << " n existe pas" ;
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));
	    }
	  if(l_iter_facture->get_reason_id())
	    {
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "La facture " << *l_iter_facture << " poss�de une raison de non attribution alors qu elle est affect�e a un client";
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));
	    }
	  // Check if there are some purchases related to this bill
	  std::vector<achat> l_achats;
	  m_table_achat.get_by_facture_id(l_iter_facture->get_id(),l_achats);
	  if(!l_achats.size())
	    {
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "Pas d achats associ�s a la facture " << *l_iter_facture;
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));
	    }
	}
      else 
	{
	  uint32_t l_reason_id = l_iter_facture->get_reason_id();
	  if(l_reason_id)
	    {
	      facture_reason l_facture_reason;
	      if(!m_table_facture_reason.get(l_reason_id,l_facture_reason))
		{
		  ++p_nb_error;
		  stringstream l_message;
		  l_message << "La raison d'ID " << l_reason_id << " r�f�renc� par " << *l_iter_facture << " n existe pas" ;
		  p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));		  
		}
	    }
	  else
	    {
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "La facture " << *l_iter_facture << " n est associ�e a aucun client et ne poss�de pas de raison de non attribution";
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));
	    }
	}
      
      // Check livre_facture_id 
      uint32_t l_livre_facture_id = l_iter_facture->get_livre_facture_id();
      if(l_livre_facture_id)
	{
	  livre_facture l_livre;
	  if(!m_table_livre_facture.get(l_livre_facture_id,l_livre))
	    {
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "Le livre de facture d'ID " << l_livre_facture_id << " r�f�renc� par " << *l_iter_facture << " n existe pas" ;
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));
	    }
	  else
	    {
	      if(l_livre.get_nb_max_facture() < l_iter_facture->get_facture_ref())
		{
		  ++p_nb_error;
		  stringstream l_message;
		  l_message << "La facture " << *l_iter_facture << " a une r�f�rence sup�rieure au nombe de facture du libre d'ID " << l_livre_facture_id << " : " << l_iter_facture->get_facture_ref() << " > " << l_livre.get_nb_max_facture();
		  p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));
		}
	      if(l_iter_facture->get_date() < l_livre.getStartDate())
		{
		  ++p_nb_error;
		  stringstream l_message;
		  l_message << "La facture " << *l_iter_facture << " a une date " << l_iter_facture->get_date() << " ant�rieure � la date de d�but du livre de facture associ� " << l_livre.getStartDate();
		  p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));
		  
		}
	      if(l_iter_facture->get_date() > l_livre.getEndDate())
		{
		  ++p_nb_error;
		  stringstream l_message;
		  l_message << "La facture " << *l_iter_facture << " a une date " << l_iter_facture->get_date() << " psot�rieure � la date de fin du livre de facture associ� " << l_livre.getEndDate();
		  p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));
		}
	    }
	}
      else
	{
	  ++p_nb_warning;
	  stringstream l_message;
	  l_message << "Pas de livre de facture associ� � la facture " << *l_iter_facture ;
	  p_warning_list.push_back(coherency_report_item(coherency_report_item::WARNING,l_iter_facture->get_id(),"Facture",l_message.str()));

	  std::vector<livre_facture> l_potential_book;
	  m_table_livre_facture.containing_date(l_iter_facture->get_date(),l_potential_book);
	  if(!l_potential_book.size())
	    {
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "Aucun livre de facture ne correspond a la date \"" << l_iter_facture->get_date() << "\" de la facture " << *l_iter_facture ;
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));
	    }
	}

      if(l_iter_facture->get_date() == "")
	{
	  ++p_nb_error;
	  stringstream l_message;
	  l_message << "La facture " << *l_iter_facture << " n'a pas de date";
	  p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_facture->get_id(),"Facture",l_message.str()));	  
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
      // Check phone number length
      std::string l_phone = l_iter_client->get_phone();
      if(l_phone.size() != 10 && l_phone.size() !=0)
	{
	  stringstream l_phone_message ;
	  l_phone_message << "Phone number \"" << l_phone << "\" has a length differnt of 10 or 0" ;
	  p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_client->get_id(),"Client",l_phone_message.str()));
	}

      // Check vile id
      uint32_t l_ville_id = l_iter_client->get_city_id();
      if(l_ville_id)
	{
	  ville l_ville;
	  
	  if(!m_table_ville.get(l_ville_id,l_ville))
	    {
	      ++p_nb_error;
	      stringstream l_message;
	      l_message << "La ville d'ID " << l_ville_id << " r�f�renc�e par le client " << *l_iter_client << " n existe pas";
	      p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_client->get_id(),"Client",l_message.str()));
	    }
	}
      else
	{
	  ++p_nb_error;
	  stringstream l_message;
	  l_message << "Pas de ville associ�e au client " << *l_iter_client;
	  p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_client->get_id(),"Client",l_message.str()));
	}

      // Check if client has associated achats
      uint32_t l_client_id = l_iter_client->get_id();
      std::vector<search_facture_item> l_factures;
      get_facture_by_client_id(l_client_id,l_factures);
      if(!l_factures.size())
	{
	  ++p_nb_error;
	  stringstream l_message;
	  l_message << "Pas de factures associ�es au client " << *l_iter_client;
	  p_error_list.push_back(coherency_report_item(coherency_report_item::ERROR,l_iter_client->get_id(),"Client",l_message.str()));
	}

      ++l_iter_client;
    }


  cout << "End of database coherency checking" << endl ;
}

const std::string fichier_client_db::m_schema_version = "1.1";

//EOF
