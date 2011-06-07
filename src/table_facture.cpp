#include "table_facture.h"

using namespace std;

//------------------------------------------------------------------------------
table_facture::table_facture(void):
  base_table<facture>(),
  m_get_by_date_stmt(NULL),
  m_get_by_date_and_client_id_stmt(NULL),
  m_get_by_livre_facture_stmt(NULL),
  m_get_by_livre_facture_and_ref_stmt(NULL)
{
}

//------------------------------------------------------------------------------
table_facture::~table_facture(void)
{
  sqlite3_finalize(m_get_by_livre_facture_and_ref_stmt);
  sqlite3_finalize(m_get_by_livre_facture_stmt);
  sqlite3_finalize(m_get_by_date_and_client_id_stmt);
  sqlite3_finalize(m_get_by_date_stmt);
  cout << "Table " << description<facture>::getClassType() << " end of destruction" << endl ;
}

//------------------------------------------------------------------------------
void table_facture::set_db(sqlite3 *p_db)
{
  base_table<facture>::set_db(p_db);

  // Preparing get_by_date statements
  //--------------------------------------------
  int l_status = sqlite3_prepare_v2(base_table<facture>::get_db(),("SELECT Id,"+description<facture>::getTableFields()+" FROM " + description<facture>::getClassType() + " WHERE Date LIKE @date").c_str(),-1,&m_get_by_date_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get "+description<facture>::getTableFields()+" item by date : " << sqlite3_errmsg(base_table<facture>::get_db()) << std::endl ;     
      exit(-1);
    }

  // Preparing livre facture get by date and client_id statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(base_table<facture>::get_db(),("SELECT Id,"+description<facture>::getTableFields()+" FROM " + description<facture>::getClassType() + " WHERE Date LIKE @date AND ClientId = $client_id").c_str(),-1,&m_get_by_date_and_client_id_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get " << description<facture>::getClassType() << " item by date and client id : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }

  // Preparing livre facture get by livre facture id statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(base_table<facture>::get_db(),("SELECT Id,"+description<facture>::getTableFields()+" FROM " + description<facture>::getClassType() + " WHERE LivreFactureId = $livre_facture_id").c_str(),-1,&m_get_by_livre_facture_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get " << description<facture>::getClassType() << " item by livre id : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }

  // Preparing livre facture get by ref and livre facture id statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(base_table<facture>::get_db(),("SELECT Id,"+description<facture>::getTableFields()+" FROM " + description<facture>::getClassType() + " WHERE FactureRef = $facture_ref AND LivreFactureId = $livre_facture_id").c_str(),-1,&m_get_by_livre_facture_and_ref_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get " << description<facture>::getClassType() << " item by livre id and facture ref : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }
}

//------------------------------------------------------------------------------
void table_facture::get_by_date(const std::string & p_date,std::vector<facture> & p_result)
{
  std::string l_param_value("%");
  l_param_value += p_date + "%";

  // Binding values to statement
  //----------------------------

  int l_status = sqlite3_bind_text(m_get_by_date_stmt,sqlite3_bind_parameter_index(m_get_by_date_stmt,"@date"),l_param_value.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of date parameter for get_by_date statement of " << description<facture>::getClassType() << " : " << sqlite3_errmsg(base_table<facture>::get_db()) << std::endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_by_date_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(description<facture>::getItemFromRow(m_get_by_date_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during selection of " << description<facture>::getClassType() << " : " << sqlite3_errmsg(base_table<facture>::get_db()) << std::endl ;
      exit(-1);
    }

#ifdef ENABLE_SUCCESS_STATUS_DISPLAY
  std::cout << "" << description<facture>::getClassType() << " get_by_date successfully listed" << std::endl ;
#endif

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_date_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << description<facture>::getClassType() << " get_by_date statement : " << sqlite3_errmsg(base_table<facture>::get_db()) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_date_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << description<facture>::getClassType() << " get_by_date statement : " << sqlite3_errmsg(base_table<facture>::get_db()) << std::endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
void table_facture::get_by_date_and_client_id(const std::string & p_date,uint32_t p_client_id,std::vector<facture> & p_result)
{
  // Binding values to statement
  //----------------------------

  int l_status = sqlite3_bind_text(m_get_by_date_and_client_id_stmt,sqlite3_bind_parameter_index(m_get_by_date_and_client_id_stmt,"@date"),p_date.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of date parameter for get_by_date_and_client_id statement of " << description<facture>::getClassType() << " : " << sqlite3_errmsg(base_table<facture>::get_db()) << std::endl ;     
      exit(-1);
    }
    
  l_status = sqlite3_bind_int(m_get_by_date_and_client_id_stmt,sqlite3_bind_parameter_index(m_get_by_date_and_client_id_stmt,"$client_id"),p_client_id);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of client_id parameter for get_by_date_abd_client_id of " << description<facture>::getClassType() << " : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_by_date_and_client_id_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(description<facture>::getItemFromRow(m_get_by_date_and_client_id_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during selection of " << description<facture>::getClassType() << " : " << sqlite3_errmsg(base_table<facture>::get_db()) << std::endl ;
      exit(-1);
    }

#ifdef ENABLE_SUCCESS_STATUS_DISPLAY
  std::cout << "" << description<facture>::getClassType() << " get_by_date_and_client_id successfully listed "<< p_date << "\t" << p_client_id << std::endl ;
#endif

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_date_and_client_id_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << description<facture>::getClassType() << " get_by_date statement : " << sqlite3_errmsg(base_table<facture>::get_db()) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_date_and_client_id_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << description<facture>::getClassType() << " get_by_date statement : " << sqlite3_errmsg(base_table<facture>::get_db()) << std::endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
void table_facture::get_by_livre_facture(uint32_t p_livre_facture_id,std::vector<facture> & p_result)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_get_by_livre_facture_stmt,sqlite3_bind_parameter_index(m_get_by_livre_facture_stmt,"$livre_facture_id"),p_livre_facture_id);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of livre_facture_id parameter for get_by_livre_facture of " << description<facture>::getClassType() << " : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_by_livre_facture_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(description<facture>::getItemFromRow(m_get_by_livre_facture_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of " << description<facture>::getClassType() << " : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;
      exit(-1);
    }

  cout << description<facture>::getClassType() << " get_by_livre_facture successfully listed" << endl ;


  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_livre_facture_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of " << description<facture>::getClassType() << " containing_date statement : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_livre_facture_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of " << description<facture>::getClassType() << " containing_date statement : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }

}


//------------------------------------------------------------------------------
void table_facture::get_by_livre_facture_and_ref(uint32_t p_facture_ref, uint32_t p_livre_facture_id,std::vector<facture> & p_result)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_get_by_livre_facture_and_ref_stmt,sqlite3_bind_parameter_index(m_get_by_livre_facture_and_ref_stmt,"$facture_ref"),p_facture_ref);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of facture_ref parameter for get_by_livre_facture_and_ref of " << description<facture>::getClassType() << " : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }
    
  l_status = sqlite3_bind_int(m_get_by_livre_facture_and_ref_stmt,sqlite3_bind_parameter_index(m_get_by_livre_facture_and_ref_stmt,"$livre_facture_id"),p_livre_facture_id);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of livre_facture_id parameter for get_by_livre_facture_and_ref of " << description<facture>::getClassType() << " : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_by_livre_facture_and_ref_stmt)) == SQLITE_ROW)
    {
      p_result.push_back(description<facture>::getItemFromRow(m_get_by_livre_facture_and_ref_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of " << description<facture>::getClassType() << " : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;
      exit(-1);
    }

  cout << description<facture>::getClassType() << " get_by_livre_facture successfully listed" << endl ;


  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_livre_facture_and_ref_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of " << description<facture>::getClassType() << " get_by_livre_facture statement : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_livre_facture_and_ref_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of " << description<facture>::getClassType() << " get_by_livre_facture statement : " << sqlite3_errmsg(base_table<facture>::get_db()) << endl ;     
      exit(-1);
    }

}


//EOF

