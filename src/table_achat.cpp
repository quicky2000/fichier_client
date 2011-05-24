#include "table_achat.h"


//------------------------------------------------------------------------------
table_achat::table_achat(void):
  base_table<achat>(),
  m_get_by_facture_id_stmt(NULL)
{
}

//------------------------------------------------------------------------------
table_achat::~table_achat(void)
{
  sqlite3_finalize(m_get_by_facture_id_stmt);
  std::cout << "Table " << description<achat>::getClassType() << " end of destruction" << std::endl ;
}

//------------------------------------------------------------------------------
void table_achat::set_db(sqlite3 *p_db)
{
  base_table<achat>::set_db(p_db);
  
  // Preparing get_by_facture_id statements
  //--------------------------------------------
  int l_status = sqlite3_prepare_v2(base_table<achat>::get_db(),("SELECT Id,"+description<achat>::getTableFields()+" FROM " + description<achat>::getClassType() + " WHERE Achat.Factureid == $facture_id;").c_str(),-1,&m_get_by_facture_id_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get "+description<achat>::getTableFields()+" item by facture_id : " << sqlite3_errmsg(base_table<achat>::get_db()) << std::endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
void table_achat::get_by_facture_id(uint32_t p_facture_id,std::vector<achat> & p_result)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_get_by_facture_id_stmt,sqlite3_bind_parameter_index(m_get_by_facture_id_stmt,"$facture_id"),p_facture_id);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of Id parameter for get_by_facture_id statement of " << description<achat>::getClassType() << " : " << sqlite3_errmsg(base_table<achat>::get_db()) << std::endl ;     
      exit(-1);
    }

  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_by_facture_id_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(description<achat>::getItemFromRow(m_get_by_facture_id_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during selection of " << description<achat>::getClassType() << " : " << sqlite3_errmsg(base_table<achat>::get_db()) << std::endl ;
      exit(-1);
    }



  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_facture_id_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << description<achat>::getClassType() << " get_by_facture_id statement : " << sqlite3_errmsg(base_table<achat>::get_db()) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_facture_id_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << description<achat>::getClassType() << " get_by_facture_id statement : " << sqlite3_errmsg(base_table<achat>::get_db()) << std::endl ;     
      exit(-1);
    }

}

//EOF
