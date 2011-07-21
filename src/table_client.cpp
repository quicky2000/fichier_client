#include "table_client.h"


//------------------------------------------------------------------------------
table_client::table_client(void):
  base_table<client>(),
  m_get_by_city_stmt(NULL)
{
}

//------------------------------------------------------------------------------
table_client::~table_client(void)
{
  sqlite3_finalize(m_get_by_city_stmt);
  std::cout << "Table " << description<client>::get_class_type() << " end of destruction" << std::endl ;
}

//------------------------------------------------------------------------------
void table_client::set_db(sqlite3 *p_db)
{
  base_table<client>::set_db(p_db);
  
  // Preparing get_by_city statements
  //--------------------------------------------
  int l_status = sqlite3_prepare_v2(base_table<client>::get_db(),
				    (std::string("SELECT ") + 
				     "Id," + 
				     description<client>::get_table_fields() + 
				     " FROM " + 
				     description<client>::get_class_type() + 
				     " WHERE " + 
				     "CityId == $ville_id" +
				     " ORDER BY Name,FirstName"
				     ).c_str(),
				    -1,
				    &m_get_by_city_stmt,
				    NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get "+description<client>::get_class_type()+" item by city_id : " << sqlite3_errmsg(base_table<client>::get_db()) << std::endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
void table_client::get_by_city(uint32_t p_city_id,std::vector<client> & p_result)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_get_by_city_stmt,sqlite3_bind_parameter_index(m_get_by_city_stmt,"$ville_id"),p_city_id);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of Id parameter for get_by_city statement of " << description<client>::get_class_type() << " : " << sqlite3_errmsg(base_table<client>::get_db()) << std::endl ;     
      exit(-1);
    }

  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_by_city_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(description<client>::get_item_from_row(m_get_by_city_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during selection of " << description<client>::get_class_type() << " : " << sqlite3_errmsg(base_table<client>::get_db()) << std::endl ;
      exit(-1);
    }



  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_city_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << description<client>::get_class_type() << " get_by_city statement : " << sqlite3_errmsg(base_table<client>::get_db()) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_city_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << description<client>::get_class_type() << " get_by_city statement : " << sqlite3_errmsg(base_table<client>::get_db()) << std::endl ;     
      exit(-1);
    }
}

//EOF
