#include "compatibility_db.h"
#include "table_achat.h"
#include "ville.h"
#include <iostream>
#include <assert.h>

//------------------------------------------------------------------------------
compatibility_db::compatibility_db(const std::string &p_name,const std::string & p_current_version):
  m_db(NULL)
{
  // Opening the database
  int l_status = sqlite3_open(p_name.c_str(), &m_db);
  if(l_status == SQLITE_OK)
    {
      m_table_facture.set_db(m_db);
      m_table_livre_facture.set_db(m_db);
      m_information_table.set_db(m_db);
      m_table_ville.set_db(m_db);
      m_table_facture_status.set_db(m_db);
      m_table_type_achat.set_db(m_db);
      m_table_marque.set_db(m_db);
      m_table_client.set_db(m_db);
    }
  else
    {
      std::cerr << "Can't open database \"" << p_name << "\" : " << sqlite3_errmsg(m_db) << std::endl ;
    }
  bool l_updated = false;
  do
    {
      std::pair<std::string,std::string> l_info;
      bool l_success = m_information_table.get("db_schema_version",l_info);
      if(l_success)
	{
	  if(l_info.second != p_current_version)
	    {
	      upgrade(l_info.second);
	    }
	  else
	    {
	      l_updated = true;
	    }
	}
      else
	{
	  l_updated = true;
	}
    } while(!l_updated);
  
}

//------------------------------------------------------------------------------
void compatibility_db::upgrade(const std::string & p_from_version)
{
  std::cout << "Upgrading from version \"" << p_from_version << "\"" << std::endl ;
  if(p_from_version == "1.0")
    {
      upgrade_from_1_0();
    }
}

//------------------------------------------------------------------------------
void compatibility_db::upgrade_from_1_0(void)
{
  sqlite3_stmt * l_upgrade_stmt = NULL;
  
  //--------------------------------------------
  // Adding new column to facture table
  //--------------------------------------------
  int l_status = sqlite3_prepare_v2(m_db,
				    "ALTER TABLE Facture ADD COLUMN ReasonId INTEGER DEFAULT 0",
				    -1,
				    &l_upgrade_stmt,
				    NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of upgrade from 1.0 statement for facture : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  l_status = sqlite3_step(l_upgrade_stmt);
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during upgrade from 1.0" << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_upgrade_stmt);  

  //-----------------------------------------
  // Conversion of ville table to city table
  //-----------------------------------------
  std::cout << "Converting \"Ville\" table to \"City\" table" << std::endl ;
  // Preparing get_all statements
  l_status = sqlite3_prepare_v2(m_db,
				"SELECT * FROM Ville",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get_all_from_ville" << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

   // Executing statement
  while( (l_status = sqlite3_step(l_upgrade_stmt)) == SQLITE_ROW)
    {
      ville l_city = description<ville>::get_item_from_row(l_upgrade_stmt);
      m_table_ville.create(l_city);
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of table Ville upgrade : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  l_status = sqlite3_reset(l_upgrade_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of table Ville upgrade statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    } 
  sqlite3_finalize(l_upgrade_stmt);  

  // Preparing drop table statements
  l_status = sqlite3_prepare_v2(m_db,
				"DROP TABLE Ville",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to drop table Ville" << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  l_status = sqlite3_step(l_upgrade_stmt);
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of drop Ville table : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_upgrade_stmt);  


  //-----------------------------------------
  // Conversion of type_achat table to Purchase_type table
  //-----------------------------------------
  std::cout << "Converting \"Type_achat\" table to \"Purchase_type\" table" << std::endl ;
  // Preparing get_all statements
  l_status = sqlite3_prepare_v2(m_db,
				"SELECT * FROM Type_achat",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get_all_from_type_achat " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

   // Executing statement
  while( (l_status = sqlite3_step(l_upgrade_stmt)) == SQLITE_ROW)
    {
      type_achat l_type_achat = description<type_achat>::get_item_from_row(l_upgrade_stmt);
      m_table_type_achat.create(l_type_achat);
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of table type_achat upgrade : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  l_status = sqlite3_reset(l_upgrade_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of table type_achat upgrade statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    } 
  sqlite3_finalize(l_upgrade_stmt);  

  // Preparing drop table statements
  l_status = sqlite3_prepare_v2(m_db,
				"DROP TABLE type_achat",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to drop table type_achat" << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  l_status = sqlite3_step(l_upgrade_stmt);
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of drop type_achat table : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_upgrade_stmt);  

  //-----------------------------------------
  // Conversion of marque table to Purchase_brand table
  //-----------------------------------------
  std::cout << "Converting \"Marque\" table to \"Purchase_brand\" table" << std::endl ;
  // Preparing get_all statements
  l_status = sqlite3_prepare_v2(m_db,
				"SELECT * FROM Marque",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get_all_from_marque " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

   // Executing statement
  while( (l_status = sqlite3_step(l_upgrade_stmt)) == SQLITE_ROW)
    {
      marque l_marque = description<marque>::get_item_from_row(l_upgrade_stmt);
      m_table_marque.create(l_marque);
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of table marque upgrade : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  l_status = sqlite3_reset(l_upgrade_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of table marque upgrade statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    } 
  sqlite3_finalize(l_upgrade_stmt);  

  // Preparing drop table statements
  l_status = sqlite3_prepare_v2(m_db,
				"DROP TABLE Marque",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to drop table marque" << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  l_status = sqlite3_step(l_upgrade_stmt);
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of drop marque table : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_upgrade_stmt);  


  //-----------------------------------------
  // Conversion of livre_facture table to Bill_book table
  //-----------------------------------------
  std::cout << "Converting \"LivreFacture\" table to \"Bill_book\" table" << std::endl ;
  // Preparing get_all statements
  l_status = sqlite3_prepare_v2(m_db,
				"SELECT * FROM LivreFacture",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get_all_from_livre_facture " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

   // Executing statement
  while( (l_status = sqlite3_step(l_upgrade_stmt)) == SQLITE_ROW)
    {
      livre_facture l_livre_facture = description<livre_facture>::get_item_from_row(l_upgrade_stmt);
      m_table_livre_facture.create(l_livre_facture);
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of table livre_facture upgrade : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  l_status = sqlite3_reset(l_upgrade_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of table livre_facture upgrade statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    } 
  sqlite3_finalize(l_upgrade_stmt);  

  // Preparing drop table statements
  l_status = sqlite3_prepare_v2(m_db,
				"DROP TABLE LivreFacture",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to drop table livre_facture" << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  l_status = sqlite3_step(l_upgrade_stmt);
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of drop livre_facture table : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_upgrade_stmt);  


  //-----------------------------------------
  // Conversion of facture_status table to Bill_status table
  //-----------------------------------------
  std::cout << "Converting \"Facture_status\" table to \"Bill_status\" table" << std::endl ;
  // Preparing get_all statements
  l_status = sqlite3_prepare_v2(m_db,
				"SELECT * FROM Facture_status",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get_all_from_facture_status " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

   // Executing statement
  while( (l_status = sqlite3_step(l_upgrade_stmt)) == SQLITE_ROW)
    {
      facture_status l_facture_status = description<facture_status>::get_item_from_row(l_upgrade_stmt);
      if(l_facture_status.get_name() != facture_status::get_ok_status() && l_facture_status.get_name() != facture_status::get_non_checked_status())
	{
	  m_table_facture_status.create(l_facture_status);
	}
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of table facture_status upgrade : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  l_status = sqlite3_reset(l_upgrade_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of table facture_status upgrade statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    } 
  sqlite3_finalize(l_upgrade_stmt);  

  // Preparing drop table statements
  l_status = sqlite3_prepare_v2(m_db,
				"DROP TABLE Facture_status",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to drop table facture_status" << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  l_status = sqlite3_step(l_upgrade_stmt);
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of drop facture_status table : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_upgrade_stmt);  


  //-----------------------------------------
  // Conversion of facture table to Bill table
  //-----------------------------------------
  std::cout << "Converting \"Facture\" table to \"Bill\" table" << std::endl ;
  // Preparing get_all statements
  l_status = sqlite3_prepare_v2(m_db,
				"SELECT * FROM Facture",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get_all_from_facture " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

   // Executing statement
  while( (l_status = sqlite3_step(l_upgrade_stmt)) == SQLITE_ROW)
    {
      facture l_facture = description<facture>::get_item_from_row(l_upgrade_stmt);
      m_table_facture.create(l_facture);
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of table facture upgrade : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  l_status = sqlite3_reset(l_upgrade_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of table facture upgrade statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    } 
  sqlite3_finalize(l_upgrade_stmt);  

  // Preparing drop table statements
  l_status = sqlite3_prepare_v2(m_db,
				"DROP TABLE Facture",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to drop table facture" << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  l_status = sqlite3_step(l_upgrade_stmt);
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of drop facture table : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_upgrade_stmt);  


  //-----------------------------------------
  // Conversion of client table to Customer table
  //-----------------------------------------
  std::cout << "Converting \"Client\" table to \"Customer\" table" << std::endl ;
  // Preparing get_all statements
  l_status = sqlite3_prepare_v2(m_db,
				"SELECT * FROM Client",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get_all_from_client " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

   // Executing statement
  while( (l_status = sqlite3_step(l_upgrade_stmt)) == SQLITE_ROW)
    {
      client l_client = description<client>::get_item_from_row(l_upgrade_stmt);
      m_table_client.create(l_client);
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of table client upgrade : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  l_status = sqlite3_reset(l_upgrade_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of table client upgrade statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    } 
  sqlite3_finalize(l_upgrade_stmt);  

  // Preparing drop table statements
  l_status = sqlite3_prepare_v2(m_db,
				"DROP TABLE Client",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to drop table client" << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  l_status = sqlite3_step(l_upgrade_stmt);
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of drop client table : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_upgrade_stmt);  

  //-----------------------------------------
  // Conversion of achat table to Purchase table
  //-----------------------------------------
  std::cout << "Converting \"Achat\" table to \"Purchase\" table" << std::endl ;
  table_achat l_table_achat;
  l_table_achat.set_db(m_db);
  // Preparing get_all statements
  l_status = sqlite3_prepare_v2(m_db,
				"SELECT * FROM Achat",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get_all_from_achat " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

   // Executing statement
  while( (l_status = sqlite3_step(l_upgrade_stmt)) == SQLITE_ROW)
    {
      achat l_achat = description<achat>::get_item_from_row(l_upgrade_stmt);
      l_table_achat.create(l_achat);
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of table achat upgrade : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  l_status = sqlite3_reset(l_upgrade_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of table achat upgrade statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    } 
  sqlite3_finalize(l_upgrade_stmt);  

  // Preparing drop table statements
  l_status = sqlite3_prepare_v2(m_db,
				"DROP TABLE Achat",
				-1,
				&l_upgrade_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to drop table achat" << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  l_status = sqlite3_step(l_upgrade_stmt);
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during execution of drop achat table : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_upgrade_stmt);  

  //-------------------------
  // Updating version number
  //-------------------------
  m_information_table.update("db_schema_version","1.1");
}

//------------------------------------------------------------------------------
compatibility_db::~compatibility_db(void)
{
  sqlite3_close(m_db);
}

//EOF
