#ifndef BASE_TABLE_H
#define BASE_TABLE_H

#include "sqlite3.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include "assert.h"
#include <stdlib.h>
#include <sstream>
#include <vector>

class named_item;

template <class T> class base_table
{
 public:
  base_table(void);
  ~base_table(void);
  void set_db(sqlite3 *p_db);

  void create(const T & p_named_item);
  void update(const T & p_named_item);
  void remove(const T & p_named_item);
  T* get(uint32_t p_id);
  const std::vector<T*>* get_all(void);

 private:
  sqlite3 *m_db;
  sqlite3_stmt *m_update_stmt;
  sqlite3_stmt *m_delete_stmt;
  sqlite3_stmt *m_get_by_id_stmt;
  sqlite3_stmt *m_get_all_stmt;
};

//------------------------------------------------------------------------------
template <class T> base_table<T>::base_table(void):
  m_db(NULL),
  m_update_stmt(NULL),
  m_delete_stmt(NULL),
  m_get_by_id_stmt(NULL),
  m_get_all_stmt(NULL)
{
}

//------------------------------------------------------------------------------
template <class T> base_table<T>::~base_table(void)
{
  sqlite3_finalize(m_get_all_stmt);
  sqlite3_finalize(m_get_by_id_stmt);
  sqlite3_finalize(m_delete_stmt);  
  sqlite3_finalize(m_update_stmt);  
  std::cout << "Table " << T::getClassType() << " end of destruction" << std::endl ;
}

//------------------------------------------------------------------------------
template <class T> void base_table<T>::set_db(sqlite3 *p_db)
{
  assert(p_db);
  m_db = p_db;

  sqlite3_stmt *l_stmt = NULL;

  // Creation to table
  //--------------------
  int l_status = sqlite3_prepare_v2(m_db,("CREATE TABLE IF NOT EXISTS " + T::getClassType() + " ( Id INTEGER PRIMARY KEY AUTOINCREMENT, "+ T::getTableFieldsDeclaration()+");").c_str(),-1,&l_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of staement to create " << T::getClassType() << " table : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  
  l_status = sqlite3_step(l_stmt);
  if(!l_status == SQLITE_DONE)
    {
      std::cout << "ERROR during creation of " << T::getClassType() << " table : " << sqlite3_errmsg(m_db) << std::endl ;
    }
  sqlite3_finalize(l_stmt);  

  // Preparing named_item update statement
  //-------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("UPDATE " + T::getClassType() + " SET " + T::getUpdateFields() + " WHERE Id == $id ;").c_str(),-1,&m_update_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of staement to update " << T::getClassType() << " item : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing named_item delete statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("DELETE FROM " + T::getClassType() + " WHERE Id == $id").c_str(),-1,&m_delete_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to delete " << T::getClassType() << " item : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing named_item get_by_id statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("SELECT Id,"+T::getTableFields()+" FROM " + T::getClassType() + " WHERE Id == $id").c_str(),-1,&m_get_by_id_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get " << T::getClassType() << " item by id: " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing get_all statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("SELECT Id,"+T::getTableFields()+" FROM " + T::getClassType()).c_str(),-1,&m_get_all_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get all " << T::getClassType() << " items : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  std::cout << "Statements OK" << std::endl ;

  std::cout << "Table " << T::getClassType() << " successfully created" << std::endl ;
  
}

//------------------------------------------------------------------------------
template <class T> void base_table<T>::create(const T & p_named_item)
{
  sqlite3_stmt *l_stmt = NULL;
  std::string l_stmt_text("INSERT INTO " + T::getClassType() + " (");
  std::string l_stmt_text_values(T::getTableFields()+") VALUES (");
  if(p_named_item.getId())
    {
      l_stmt_text += "Id,";
      std::stringstream l_id_str;
      l_id_str << p_named_item.getId();
      l_stmt_text_values += l_id_str.str() +",";
    }
  l_stmt_text += l_stmt_text_values+T::getFieldValues(p_named_item)+")";
  
  int l_status = sqlite3_prepare_v2(m_db,l_stmt_text.c_str(),-1,&l_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of staement to create " << T::getClassType() << " \"" <<  l_stmt_text << "\" : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_step(l_stmt);
  if( l_status == SQLITE_DONE)
    {
      std::cout << T::getClassType() << " successfully created" << std::endl ;
    }
  else
    {
      std::cout << "ERROR during creation of " << T::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  sqlite3_finalize(l_stmt);  
}

//------------------------------------------------------------------------------
template <class T> void base_table<T>::update(const T & p_named_item)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_update_stmt,sqlite3_bind_parameter_index(m_update_stmt,"$id"),p_named_item.getId());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of Id parameter for update statement of " << T::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  
  T::bind_update_values(p_named_item,m_update_stmt,m_db);

  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_update_stmt);
  if( l_status == SQLITE_DONE)
    {
      std::cout << T::getClassType() << " successfully updated" << std::endl ;
    }
  else
    {
      std::cout << "ERROR during update of " << T::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_update_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << T::getClassType() << " update statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_update_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << T::getClassType() << " update statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }


}

//------------------------------------------------------------------------------
template <class T> void base_table<T>::remove(const T & p_named_item)
{
  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_delete_stmt,sqlite3_bind_parameter_index(m_delete_stmt,"$id"),p_named_item.getId());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of Id parameter for delete statement of " << T::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_delete_stmt);
  if( l_status == SQLITE_DONE)
    {
      std::cout << T::getClassType() << " successfully deleted" << std::endl ;
    }
  else
    {
      std::cout << "ERROR during delete of " << T::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_delete_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << T::getClassType() << " delete statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_delete_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << T::getClassType() << " delete statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
template <class T> T * base_table<T>::get(uint32_t p_id)
{
  
  T * l_result = NULL;

  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_get_by_id_stmt,sqlite3_bind_parameter_index(m_get_by_id_stmt,"$id"),p_id);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of Id parameter for get_by_id statement of " << T::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_get_by_id_stmt);
  if( l_status == SQLITE_ROW)
    {
      std::cout << T::getClassType() << " successfully selected" << std::endl ;
      l_result = T::getItemFromRow(m_get_by_id_stmt);
    }
  else
    {
      std::cout << "ERROR during selection of " << T::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  l_status = sqlite3_step(m_get_by_id_stmt);
  if( l_status == SQLITE_DONE)
    {
      std::cout << T::getClassType() << " successfully selected done" << std::endl ;
    }
  else
    {
      std::cout << "ERROR during selection of " << T::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_id_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << T::getClassType() << " get_by_id statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_id_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << T::getClassType() << " get_by_id statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  return l_result;
}

//------------------------------------------------------------------------------
template <class T> const std::vector<T*>* base_table<T>::get_all(void)
{
  std::vector<T*> *l_result = new std::vector<T*>();

  int l_status = 0;
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_all_stmt)) == SQLITE_ROW)
    {
      l_result->push_back(T::getItemFromRow(m_get_all_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during selection of " << T::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  std::cout <<  T::getClassType() << " successfully listed" << std::endl ;

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_all_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << T::getClassType() << " get_all statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  return l_result;
}

#endif
