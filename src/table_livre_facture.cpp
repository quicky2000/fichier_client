#include "table_livre_facture.h"

using namespace std;

//------------------------------------------------------------------------------
table_livre_facture::table_livre_facture(void):
  base_table<livre_facture>(),
  m_containing_date_stmt(NULL)
{
}

//------------------------------------------------------------------------------
table_livre_facture::~table_livre_facture(void)
{
  sqlite3_finalize(m_containing_date_stmt);
  cout << "Table " << description<livre_facture>::getClassType() << " end of destruction" << endl ;
}

//------------------------------------------------------------------------------
void table_livre_facture::set_db(sqlite3 *p_db)
{
  base_table<livre_facture>::set_db(p_db);

  // Preparing livre facture containing_date statements
  //--------------------------------------------
  uint32_t l_status = sqlite3_prepare_v2(base_table<livre_facture>::get_db(),("SELECT Id,"+description<livre_facture>::getTableFields()+" FROM " + description<livre_facture>::getClassType() + " WHERE StartDate <= $date AND EndDate >= $date").c_str(),-1,&m_containing_date_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get " << description<livre_facture>::getClassType() << " item containing date : " << sqlite3_errmsg(base_table<livre_facture>::get_db()) << endl ;     
      exit(-1);
    }
}

//------------------------------------------------------------------------------
void table_livre_facture::containing_date(const std::string & p_date,std::vector<livre_facture> & p_result)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_text(m_containing_date_stmt,sqlite3_bind_parameter_index(m_containing_date_stmt,"$date"),p_date.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of date parameter for containing date statement of " << description<livre_facture>::getClassType() << " : " << sqlite3_errmsg(base_table<livre_facture>::get_db()) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_containing_date_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(description<livre_facture>::getItemFromRow(m_containing_date_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of " << description<livre_facture>::getClassType() << " : " << sqlite3_errmsg(base_table<livre_facture>::get_db()) << endl ;
      exit(-1);
    }

  cout << description<livre_facture>::getClassType() << " containing date successfully listed" << endl ;


  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_containing_date_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of " << description<livre_facture>::getClassType() << " containing_date statement : " << sqlite3_errmsg(base_table<livre_facture>::get_db()) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_containing_date_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of " << description<livre_facture>::getClassType() << " containing_date statement : " << sqlite3_errmsg(base_table<livre_facture>::get_db()) << endl ;     
      exit(-1);
    }

}


//EOF

