#include "table_ville.h"

using namespace std;

//------------------------------------------------------------------------------
table_ville::table_ville(void):
  named_table<ville>(),
  m_get_by_code_postal_stmt(NULL),
  m_get_by_name_and_postal_code_stmt(NULL)
{
}

//------------------------------------------------------------------------------
table_ville::~table_ville(void)
{
  sqlite3_finalize(m_get_by_name_and_postal_code_stmt);
  sqlite3_finalize(m_get_by_code_postal_stmt);
  cout << "Table " << description<ville>::get_class_type() << " end of destruction" << endl ;
}

//------------------------------------------------------------------------------
void table_ville::set_db(sqlite3 *p_db)
{
  named_table<ville>::set_db(p_db);

  // Preparing ville get_by_code_postal statements
  //--------------------------------------------
  uint32_t l_status = sqlite3_prepare_v2(named_table<ville>::get_db(),
					 (std::string("SELECT ") + 
					  "Id," + 
					  description<ville>::get_table_fields() + 
					  " FROM " + 
					  description<ville>::get_class_type() + 
					  " WHERE " +
					  "PostalCode LIKE @postal_code" +
					  " ORDER BY Name"
					  ).c_str(),
					 -1,
					 &m_get_by_code_postal_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get "+description<ville>::get_class_type()+" item by code postal : " << sqlite3_errmsg(base_table<ville>::get_db()) << std::endl ;     
      exit(-1);
    }

  // Preparing ville get_by_name_and_postal_code statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(named_table<ville>::get_db(),
				(std::string("SELECT ") + 
				 "Id," + 
				 description<ville>::get_table_fields() + 
				 " FROM " + 
				 description<ville>::get_class_type() + 
				 " WHERE " +
				 "PostalCode LIKE @postal_code" + 
				 " AND " +
				 "Name LIKE @name" +
				 " ORDER BY Name"
				 ).c_str(),
				-1,
				&m_get_by_name_and_postal_code_stmt,
				NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get "+description<ville>::get_class_type()+" item by name and postal code: " << sqlite3_errmsg(base_table<ville>::get_db()) << std::endl ;     
      exit(-1);
    }
}

//------------------------------------------------------------------------------
void table_ville::get_by_code_postal(const std::string & p_code_postal,std::vector<ville> & p_result)
{
  string l_param_value("%");
  l_param_value += p_code_postal + "%";

  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_text(m_get_by_code_postal_stmt,sqlite3_bind_parameter_index(m_get_by_code_postal_stmt,"@postal_code"),l_param_value.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of postal_code parameter for get_by_code_postal statement of " << description<ville>::get_class_type() << " : " << sqlite3_errmsg(named_table<ville>::get_db()) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while((l_status = sqlite3_step(m_get_by_code_postal_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(description<ville>::get_item_from_row(m_get_by_code_postal_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of " << description<ville>::get_class_type() << " : " << sqlite3_errmsg(named_table<ville>::get_db()) << endl ;
      exit(-1);
    }

  cout << description<ville>::get_class_type() << " containing get_by_code_postal successfully listed" << endl ;


  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_code_postal_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of " << description<ville>::get_class_type() << " get_by_code_postal statement : " << sqlite3_errmsg(named_table<ville>::get_db()) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_code_postal_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of " << description<ville>::get_class_type() << " get_by_code_postal statement : " << sqlite3_errmsg(named_table<ville>::get_db()) << endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
void table_ville::get_by_name_and_postal_code(const std::string & p_name,const std::string & p_postal_code,std::vector<ville> & p_result)
{
  std::string l_postal_code_param_value("%");
  l_postal_code_param_value += p_postal_code + "%";

  std::string l_name_param_value("%");
  l_name_param_value += p_name + "%";

  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_text(m_get_by_name_and_postal_code_stmt,sqlite3_bind_parameter_index(m_get_by_name_and_postal_code_stmt,"@postal_code"),l_postal_code_param_value.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of postal_code parameter for get_by_name_and_postal_code statement of " << description<ville>::get_class_type() << " : " << sqlite3_errmsg(named_table<ville>::get_db()) << endl ;     
      exit(-1);
    }
    
  sqlite3_bind_text(m_get_by_name_and_postal_code_stmt,sqlite3_bind_parameter_index(m_get_by_name_and_postal_code_stmt,"@name"),l_name_param_value.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of name parameter for get_by_name_and_postal_code statement of " << description<ville>::get_class_type() << " : " << sqlite3_errmsg(named_table<ville>::get_db()) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while((l_status = sqlite3_step(m_get_by_name_and_postal_code_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(description<ville>::get_item_from_row(m_get_by_name_and_postal_code_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of " << description<ville>::get_class_type() << " : " << sqlite3_errmsg(named_table<ville>::get_db()) << endl ;
      exit(-1);
    }

  cout << description<ville>::get_class_type() << " containing get_by_name_and_postal_code successfully listed" << endl ;


  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_name_and_postal_code_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of " << description<ville>::get_class_type() << " get_by_name_and_postal_code statement : " << sqlite3_errmsg(named_table<ville>::get_db()) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_name_and_postal_code_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of " << description<ville>::get_class_type() << " get_by_name_and_postal_code statement : " << sqlite3_errmsg(named_table<ville>::get_db()) << endl ;     
      exit(-1);
    }

}


//EOF

