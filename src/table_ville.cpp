#include "table_ville.h"

using namespace std;

//------------------------------------------------------------------------------
table_ville::table_ville(void):
  named_table<ville>(),
  m_get_by_code_postal_stmt(NULL)
{
}

//------------------------------------------------------------------------------
table_ville::~table_ville(void)
{
  sqlite3_finalize(m_get_by_code_postal_stmt);
  cout << "Table " << description<ville>::get_class_type() << " end of destruction" << endl ;
}

//------------------------------------------------------------------------------
void table_ville::set_db(sqlite3 *p_db)
{
  named_table<ville>::set_db(p_db);

  // Preparing ville get_by_code_postal statements
  //--------------------------------------------
  uint32_t l_status = sqlite3_prepare_v2(named_table<ville>::get_db(),("SELECT Id,"+description<ville>::get_table_fields()+" FROM " + description<ville>::get_class_type() + " WHERE CodePostal LIKE @code_postal").c_str(),-1,&m_get_by_code_postal_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get "+description<ville>::get_table_fields()+" item by code postal : " << sqlite3_errmsg(base_table<ville>::get_db()) << std::endl ;     
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
  int l_status = sqlite3_bind_text(m_get_by_code_postal_stmt,sqlite3_bind_parameter_index(m_get_by_code_postal_stmt,"@code_postal"),l_param_value.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of code_postal parameter for get_by_code_postal statement of " << description<ville>::get_class_type() << " : " << sqlite3_errmsg(named_table<ville>::get_db()) << endl ;     
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


//EOF

