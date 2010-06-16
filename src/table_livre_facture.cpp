#include "table_livre_facture.h"
#include "livre_facture.h"

#include <sqlite3.h>
#include <assert.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
table_livre_facture::table_livre_facture(void):
  m_db(NULL),
  m_update_livre_facture_stmt(NULL),
  m_delete_livre_facture_stmt(NULL),
  m_get_by_id_livre_facture_stmt(NULL),
  m_get_all_livre_facture_stmt(NULL),
  m_containing_date_stmt(NULL)
{
}

//------------------------------------------------------------------------------
table_livre_facture::~table_livre_facture(void)
{
  sqlite3_finalize(m_containing_date_stmt);
  sqlite3_finalize(m_get_all_livre_facture_stmt);
  sqlite3_finalize(m_get_by_id_livre_facture_stmt);
  sqlite3_finalize(m_delete_livre_facture_stmt);  
  sqlite3_finalize(m_update_livre_facture_stmt);  
  cout << "Table livre facture end of destruction" << endl ;
}

//------------------------------------------------------------------------------
void table_livre_facture::set_db(sqlite3 *p_db)
{
  assert(p_db);
  m_db = p_db;

  sqlite3_stmt *l_stmt = NULL;

  // Creation to table
  //--------------------
  int l_status = sqlite3_prepare_v2(m_db,"CREATE TABLE IF NOT EXISTS livre_facture ( LivreId INTEGER PRIMARY KEY AUTOINCREMENT ,StartDate TEXT, EndDate TEXT);",-1,&l_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of staement to create livre_facture table : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }
  
  l_status = sqlite3_step(l_stmt);
  if(!l_status == SQLITE_DONE)
    {
      cout << "ERROR during creation of livre_facture table : " << sqlite3_errmsg(m_db) << endl ;
    }
  sqlite3_finalize(l_stmt);  

   // Preparing livre_facture update statement
  //-------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"UPDATE livre_facture SET StartDate = $start_date , EndDate = $end_date WHERE LivreId == $id ;",-1,&m_update_livre_facture_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of staement to update livre_facture item : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Preparing livre facture delete statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"DELETE FROM livre_facture WHERE LivreId == $id",-1,&m_delete_livre_facture_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to delete livre_facture item : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Preparing livre facture get_by_id statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"SELECT LivreId, StartDate, EndDate FROM livre_facture WHERE LivreId == $id",-1,&m_get_by_id_livre_facture_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get livre_facture item by id: " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Preparing get_all statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"SELECT LivreId, StartDate, EndDate FROM livre_facture",-1,&m_get_all_livre_facture_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get all livre_facture items : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Preparing livre facture containing_date statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"SELECT LivreId, StartDate, EndDate FROM livre_facture WHERE StartDate <= $date AND EndDate >= $date",-1,&m_containing_date_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get livre_facture item containing date : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  cout << "Statements OK" << endl ;

  cout << "Table livre_facture successfully created" << endl ;
  
}

//------------------------------------------------------------------------------
void table_livre_facture::create(const livre_facture & p_livre_facture)
{
  sqlite3_stmt *l_stmt = NULL;
  string l_stmt_text("INSERT INTO livre_facture (");
  string l_stmt_text_values("StartDate,EndDate) VALUES (");
  if(p_livre_facture.getId())
    {
      l_stmt_text += "LivreId,";
      stringstream l_id_str;
      l_id_str << p_livre_facture.getId();
      l_stmt_text_values += l_id_str.str() +",";
    }
  l_stmt_text += l_stmt_text_values+"\""+p_livre_facture.getStartDate()+"\",\""+p_livre_facture.getEndDate()+"\")";
  
  int l_status = sqlite3_prepare_v2(m_db,l_stmt_text.c_str(),-1,&l_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of staement to create livre_facture \"" <<  l_stmt_text << "\" : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_step(l_stmt);
  if( l_status == SQLITE_DONE)
    {
      cout << "Livre_facture successfully created" << endl ;
    }
  else
    {
      cout << "ERROR during creation of livre_facture : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }
  sqlite3_finalize(l_stmt);  
}

//------------------------------------------------------------------------------
void table_livre_facture::update(const livre_facture & p_livre_facture)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_update_livre_facture_stmt,sqlite3_bind_parameter_index(m_update_livre_facture_stmt,"$id"),p_livre_facture.getId());
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of Id parameter for update statement of livre facture : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(m_update_livre_facture_stmt,sqlite3_bind_parameter_index(m_update_livre_facture_stmt,"$start_date"),p_livre_facture.getStartDate().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of start_date parameter for update statement of livre facture : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(m_update_livre_facture_stmt,sqlite3_bind_parameter_index(m_update_livre_facture_stmt,"$end_date"),p_livre_facture.getEndDate().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of end_date parameter for update statement of livre facture : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_update_livre_facture_stmt);
  if( l_status == SQLITE_DONE)
    {
      cout << "Livre_facture successfully updated" << endl ;
    }
  else
    {
      cout << "ERROR during update of livre_facture : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_update_livre_facture_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of livre facture update statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_update_livre_facture_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of livre facture update statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }


}

//------------------------------------------------------------------------------
void table_livre_facture::remove(const livre_facture & p_livre_facture)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_delete_livre_facture_stmt,sqlite3_bind_parameter_index(m_delete_livre_facture_stmt,"$id"),p_livre_facture.getId());
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of Id parameter for delete statement of livre facture : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_delete_livre_facture_stmt);
  if( l_status == SQLITE_DONE)
    {
      cout << "Livre_facture successfully deleted" << endl ;
    }
  else
    {
      cout << "ERROR during delete of livre_facture : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_delete_livre_facture_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of livre facture delete statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_delete_livre_facture_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of livre facture delete statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
livre_facture * table_livre_facture::get(uint32_t p_id)
{
  
  livre_facture * l_result = NULL;

  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_get_by_id_livre_facture_stmt,sqlite3_bind_parameter_index(m_get_by_id_livre_facture_stmt,"$id"),p_id);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of Id parameter for get_by_id statement of livre facture : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_get_by_id_livre_facture_stmt);
  if( l_status == SQLITE_ROW)
    {
      cout << "Livre_facture successfully selected" << endl ;
      l_result = new livre_facture(sqlite3_column_int(m_get_by_id_livre_facture_stmt,0),(const char*)sqlite3_column_text(m_get_by_id_livre_facture_stmt,1),(const char*)sqlite3_column_text(m_get_by_id_livre_facture_stmt,2));
    }
  else
    {
      cout << "ERROR during selection of livre_facture : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  l_status = sqlite3_step(m_get_by_id_livre_facture_stmt);
  if( l_status == SQLITE_DONE)
    {
      cout << "Livre_facture successfully selected done" << endl ;
    }
  else
    {
      cout << "ERROR during selection of livre_facture : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_id_livre_facture_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of livre facture get_by_id statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_id_livre_facture_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of livre facture get_by_id statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  return l_result;
}

//------------------------------------------------------------------------------
const std::vector<livre_facture>* table_livre_facture::get_all(void)
{
  vector<livre_facture> *l_result = new vector<livre_facture>();

  int l_status = 0;
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_all_livre_facture_stmt)) == SQLITE_ROW)
    {
      l_result->push_back(livre_facture(sqlite3_column_int(m_get_all_livre_facture_stmt,0),(const char*)sqlite3_column_text(m_get_all_livre_facture_stmt,1),(const char*)sqlite3_column_text(m_get_all_livre_facture_stmt,2)));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of livre_facture : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  cout << "Livre_facture successfully listed" << endl ;

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_all_livre_facture_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of livre facture get_all statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  return l_result;
}

//------------------------------------------------------------------------------
const std::vector<livre_facture>* table_livre_facture::containing_date(const std::string & p_date)
{
  vector<livre_facture> *l_result = new vector<livre_facture>();

  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_text(m_containing_date_stmt,sqlite3_bind_parameter_index(m_containing_date_stmt,"$date"),p_date.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of date parameter for containing date statement of livre facture : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_containing_date_stmt)) == SQLITE_ROW)
    {
      l_result->push_back(livre_facture(sqlite3_column_int(m_containing_date_stmt,0),(const char*)sqlite3_column_text(m_containing_date_stmt,1),(const char*)sqlite3_column_text(m_containing_date_stmt,2)));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of livre_facture : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  cout << "Livre_facture containing date successfully listed" << endl ;


  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_containing_date_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of livre facture containing_date statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_containing_date_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of livre facture containing_date statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  return l_result;
}


//EOF

