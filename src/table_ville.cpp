#include "table_ville.h"
#include "ville.h"

#include <sqlite3.h>
#include <assert.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
table_ville::table_ville(void):
  m_db(NULL),
  m_update_stmt(NULL),
  m_delete_stmt(NULL),
  m_get_by_id_stmt(NULL),
  m_get_all_stmt(NULL),
  m_get_by_name_stmt(NULL),
  m_get_by_code_postal_stmt(NULL)
{
}

//------------------------------------------------------------------------------
table_ville::~table_ville(void)
{
  sqlite3_finalize(m_get_by_code_postal_stmt);
  sqlite3_finalize(m_get_by_name_stmt);
  sqlite3_finalize(m_get_all_stmt);
  sqlite3_finalize(m_get_by_id_stmt);
  sqlite3_finalize(m_delete_stmt);  
  sqlite3_finalize(m_update_stmt);  
  cout << "Table ville end of destruction" << endl ;
}

//------------------------------------------------------------------------------
void table_ville::set_db(sqlite3 *p_db)
{
  assert(p_db);
  m_db = p_db;

  sqlite3_stmt *l_stmt = NULL;

  // Creation to table
  //--------------------
  int l_status = sqlite3_prepare_v2(m_db,"CREATE TABLE IF NOT EXISTS ville ( Id INTEGER PRIMARY KEY AUTOINCREMENT ,Name TEXT, CodePostal TEXT);",-1,&l_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of staement to create ville table : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }
  
  l_status = sqlite3_step(l_stmt);
  if(!l_status == SQLITE_DONE)
    {
      cout << "ERROR during creation of ville table : " << sqlite3_errmsg(m_db) << endl ;
    }
  sqlite3_finalize(l_stmt);  

   // Preparing ville update statement
  //-------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"UPDATE ville SET CodePostal = $code_postal , Name = $name WHERE Id == $id ;",-1,&m_update_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of staement to update ville item : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Preparing ville delete statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"DELETE FROM ville WHERE Id == $id",-1,&m_delete_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to delete ville item : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Preparing ville get_by_id statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"SELECT Id, Name, CodePostal FROM ville WHERE Id == $id",-1,&m_get_by_id_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get ville item by id: " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Preparing get_all statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"SELECT Id, Name, CodePostal FROM ville",-1,&m_get_all_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get all ville items : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Preparing ville get_by_name statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"SELECT Id, Name, CodePostal FROM ville WHERE Name LIKE @name",-1,&m_get_by_name_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get ville item by name : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Preparing ville get_by_code_postal statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,"SELECT Id, Name, CodePostal FROM ville WHERE CodePostal LIKE $code_postal",-1,&m_get_by_code_postal_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of statement to get ville item by name : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  cout << "Statements OK" << endl ;

  cout << "Table ville successfully created" << endl ;
  
}

//------------------------------------------------------------------------------
void table_ville::create(const ville & p_ville)
{
  sqlite3_stmt *l_stmt = NULL;
  string l_stmt_text("INSERT INTO ville (");
  string l_stmt_text_values("Name,CodePostal) VALUES (");
  if(p_ville.getId())
    {
      l_stmt_text += "Id,";
      stringstream l_id_str;
      l_id_str << p_ville.getId();
      l_stmt_text_values += l_id_str.str() +",";
    }
  l_stmt_text += l_stmt_text_values+"\""+p_ville.getName()+"\",\""+p_ville.getCodePostal()+"\")";
  
  int l_status = sqlite3_prepare_v2(m_db,l_stmt_text.c_str(),-1,&l_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during preparation of staement to create ville \"" <<  l_stmt_text << "\" : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_step(l_stmt);
  if( l_status == SQLITE_DONE)
    {
      cout << "ville successfully created" << endl ;
    }
  else
    {
      cout << "ERROR during creation of ville : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }
  sqlite3_finalize(l_stmt);  
}

//------------------------------------------------------------------------------
void table_ville::update(const ville & p_ville)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_update_stmt,sqlite3_bind_parameter_index(m_update_stmt,"$id"),p_ville.getId());
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of Id parameter for update statement of ville : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(m_update_stmt,sqlite3_bind_parameter_index(m_update_stmt,"$code_postal"),p_ville.getCodePostal().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of code_postal parameter for update statement of ville : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(m_update_stmt,sqlite3_bind_parameter_index(m_update_stmt,"$name"),p_ville.getName().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of name parameter for update statement of ville : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
  
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_update_stmt);
  if( l_status == SQLITE_DONE)
    {
      cout << "ville successfully updated" << endl ;
    }
  else
    {
      cout << "ERROR during update of ville : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_update_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of ville update statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_update_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of ville update statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }


}

//------------------------------------------------------------------------------
void table_ville::remove(const ville & p_ville)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_delete_stmt,sqlite3_bind_parameter_index(m_delete_stmt,"$id"),p_ville.getId());
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of Id parameter for delete statement of ville : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_delete_stmt);
  if( l_status == SQLITE_DONE)
    {
      cout << "ville successfully deleted" << endl ;
    }
  else
    {
      cout << "ERROR during delete of ville : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_delete_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of ville delete statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_delete_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of ville delete statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
ville * table_ville::get(uint32_t p_id)
{
  
  ville * l_result = NULL;

  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_get_by_id_stmt,sqlite3_bind_parameter_index(m_get_by_id_stmt,"$id"),p_id);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of Id parameter for get_by_id statement of ville : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_get_by_id_stmt);
  if( l_status == SQLITE_ROW)
    {
      cout << "ville successfully selected" << endl ;
      l_result = new ville(sqlite3_column_int(m_get_by_id_stmt,0),(const char*)sqlite3_column_text(m_get_by_id_stmt,1),(const char*)sqlite3_column_text(m_get_by_id_stmt,2));
    }
  else
    {
      cout << "ERROR during selection of ville : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  l_status = sqlite3_step(m_get_by_id_stmt);
  if( l_status == SQLITE_DONE)
    {
      cout << "ville successfully selected done" << endl ;
    }
  else
    {
      cout << "ERROR during selection of ville : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_id_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of ville get_by_id statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_id_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of ville get_by_id statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  return l_result;
}

//------------------------------------------------------------------------------
const std::vector<ville>* table_ville::get_all(void)
{
  vector<ville> *l_result = new vector<ville>();

  int l_status = 0;
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_all_stmt)) == SQLITE_ROW)
    {
      l_result->push_back(ville(sqlite3_column_int(m_get_all_stmt,0),(const char*)sqlite3_column_text(m_get_all_stmt,1),(const char*)sqlite3_column_text(m_get_all_stmt,2)));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of ville : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  cout << "ville successfully listed" << endl ;

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_all_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of ville get_all statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  return l_result;
}

//------------------------------------------------------------------------------
const std::vector<ville>* table_ville::get_by_name(const std::string & p_name)
{
  vector<ville> *l_result = new vector<ville>();
  string l_param_value("%");
  l_param_value += p_name + "%";

  // Binding values to statement
  //----------------------------
  cout << "parameter : " << l_param_value << endl ;

  int l_status = sqlite3_bind_text(m_get_by_name_stmt,sqlite3_bind_parameter_index(m_get_by_name_stmt,"@name"),l_param_value.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of name parameter for get_by_name statement of ville : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_by_name_stmt)) == SQLITE_ROW)
    {
      l_result->push_back(ville(sqlite3_column_int(m_get_by_name_stmt,0),(const char*)sqlite3_column_text(m_get_by_name_stmt,1),(const char*)sqlite3_column_text(m_get_by_name_stmt,2)));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of ville : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  cout << "ville get_by_name successfully listed" << endl ;


  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_name_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of ville get_by_name statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_name_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of ville get_by_name statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  return l_result;
}


//------------------------------------------------------------------------------
const std::vector<ville>* table_ville::get_by_code_postal(const std::string & p_code_postal)
{
  vector<ville> *l_result = new vector<ville>();
  string l_param_value("%");
  l_param_value += p_code_postal + "%";

  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_text(m_get_by_code_postal_stmt,sqlite3_bind_parameter_index(m_get_by_code_postal_stmt,"$code_postal"),l_param_value.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during binding of code_postal parameter for get_by_code_postal statement of ville : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_by_code_postal_stmt)) == SQLITE_ROW)
    {
      l_result->push_back(ville(sqlite3_column_int(m_get_by_code_postal_stmt,0),(const char*)sqlite3_column_text(m_get_by_code_postal_stmt,1),(const char*)sqlite3_column_text(m_get_by_code_postal_stmt,2)));
    }
  if(l_status != SQLITE_DONE)
    {
      cout << "ERROR during selection of ville : " << sqlite3_errmsg(m_db) << endl ;
      exit(-1);
    }

  cout << "ville containing get_by_code_postal successfully listed" << endl ;


  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_code_postal_stmt);  
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of ville get_by_code_postal statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_code_postal_stmt);
  if(l_status != SQLITE_OK)
    {
      cout << "ERROR during reset of bindings of ville get_by_code_postal statement : " << sqlite3_errmsg(m_db) << endl ;     
      exit(-1);
    }

  return l_result;
}


//EOF

