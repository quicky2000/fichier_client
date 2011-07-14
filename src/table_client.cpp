#include "table_client.h"


//------------------------------------------------------------------------------
table_client::table_client(void):
  base_table<client>(),
  m_get_by_date_stmt(NULL)
{
}

//------------------------------------------------------------------------------
table_client::~table_client(void)
{
  sqlite3_finalize(m_get_by_date_stmt);
  std::cout << "Table " << description<client>::get_class_type() << " end of destruction" << std::endl ;
}

//------------------------------------------------------------------------------
void table_client::set_db(sqlite3 *p_db)
{
  base_table<client>::set_db(p_db);
  
  // Preparing get_by_date statements
  //--------------------------------------------


}


//EOF
