#include "compatibility_db.h"
#include "table_facture.h"
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
      m_information_table.set_db(m_db);
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
  
  // Preparing get_by_date statements
  //--------------------------------------------
  int l_status = sqlite3_prepare_v2(m_db,
				    ("ALTER TABLE " + 
				     description<facture>::get_class_type() + 
				     " ADD COLUMN ReasonId INTEGER DEFAULT 0" 
				     ).c_str(),
				    -1,
				    &l_upgrade_stmt,
				    NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of upgrade from 1.0 statement for facture : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  l_status = sqlite3_step(l_upgrade_stmt);
  if(!l_status == SQLITE_DONE)
    {
      std::cout << "ERROR during upgrade from 1.0" << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_upgrade_stmt);  


  m_information_table.update("db_schema_version","1.1");
}

//------------------------------------------------------------------------------
compatibility_db::~compatibility_db(void)
{
  sqlite3_close(m_db);
}

//EOF
