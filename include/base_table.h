#ifndef BASE_TABLE_H
#define BASE_TABLE_H

#include "description.h"
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
  void create(const T & p_named_item);
  void update(const T & p_named_item);
  void remove(const T & p_named_item);
  uint32_t get(uint32_t p_id, T & p_data);
  void get_all(std::vector<T> & p_list);

 protected:
  base_table(void);
  ~base_table(void);
  void set_db(sqlite3 *p_db);
  sqlite3 * get_db(void);

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
  std::cout << "Table " << description<T>::getClassType() << " end of destruction" << std::endl ;
}

//------------------------------------------------------------------------------
template <class T> sqlite3 * base_table<T>::get_db(void)
{
  return m_db;
}

//------------------------------------------------------------------------------
template <class T> void base_table<T>::set_db(sqlite3 *p_db)
{
  assert(p_db);
  m_db = p_db;

  sqlite3_stmt *l_stmt = NULL;

  // Creation to table
  //--------------------
  int l_status = sqlite3_prepare_v2(m_db,("CREATE TABLE IF NOT EXISTS " + description<T>::getClassType() + " ( Id INTEGER PRIMARY KEY AUTOINCREMENT, "+ description<T>::getTableFieldsDeclaration()+");").c_str(),-1,&l_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of staement to create " << description<T>::getClassType() << " table : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }
  
  l_status = sqlite3_step(l_stmt);
  if(!l_status == SQLITE_DONE)
    {
      std::cout << "ERROR during creation of " << description<T>::getClassType() << " table : " << sqlite3_errmsg(m_db) << std::endl ;
    }
  sqlite3_finalize(l_stmt);  

  // Preparing named_item update statement
  //-------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("UPDATE " + description<T>::getClassType() + " SET " + description<T>::getUpdateFields() + " WHERE Id == $id ;").c_str(),-1,&m_update_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of staement to update " << description<T>::getClassType() << " item : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing named_item delete statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("DELETE FROM " + description<T>::getClassType() + " WHERE Id == $id").c_str(),-1,&m_delete_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to delete " << description<T>::getClassType() << " item : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing named_item get_by_id statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("SELECT Id,"+description<T>::getTableFields()+" FROM " + description<T>::getClassType() + " WHERE Id == $id").c_str(),-1,&m_get_by_id_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get " << description<T>::getClassType() << " item by id: " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Preparing get_all statements
  //--------------------------------------------
  l_status = sqlite3_prepare_v2(m_db,("SELECT Id,"+description<T>::getTableFields()+" FROM " + description<T>::getClassType()).c_str(),-1,&m_get_all_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get all " << description<T>::getClassType() << " items : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  std::cout << "Statements OK" << std::endl ;

  std::cout << "Table " << description<T>::getClassType() << " successfully created" << std::endl ;
  
}

//------------------------------------------------------------------------------
template <class T> void base_table<T>::create(const T & p_named_item)
{
  sqlite3_stmt *l_stmt = NULL;
  std::string l_stmt_text("INSERT INTO " + description<T>::getClassType() + " (");
  std::string l_stmt_text_values(description<T>::getTableFields()+") VALUES (");
  if(p_named_item.getId())
    {
      l_stmt_text += "Id,";
      std::stringstream l_id_str;
      l_id_str << p_named_item.getId();
      l_stmt_text_values += l_id_str.str() +",";
    }
  l_stmt_text += l_stmt_text_values+description<T>::getFieldValues(p_named_item)+")";
  
  int l_status = sqlite3_prepare_v2(m_db,l_stmt_text.c_str(),-1,&l_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of staement to create " << description<T>::getClassType() << " \"" <<  l_stmt_text << "\" : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_step(l_stmt);
  if( l_status == SQLITE_DONE)
    {
      std::cout << description<T>::getClassType() << " successfully created" << std::endl ;
    }
  else
    {
      std::cout << "ERROR during creation of " << description<T>::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
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
      std::cout << "ERROR during binding of Id parameter for update statement of " << description<T>::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
  
  description<T>::bind_update_values(p_named_item,m_update_stmt,m_db);

  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_update_stmt);
  if( l_status == SQLITE_DONE)
    {
      std::cout << description<T>::getClassType() << " successfully updated" << std::endl ;
    }
  else
    {
      std::cout << "ERROR during update of " << description<T>::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_update_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << description<T>::getClassType() << " update statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_update_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << description<T>::getClassType() << " update statement : " << sqlite3_errmsg(m_db) << std::endl ;     
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
      std::cout << "ERROR during binding of Id parameter for delete statement of " << description<T>::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_delete_stmt);
  if( l_status == SQLITE_DONE)
    {
      std::cout << description<T>::getClassType() << " successfully deleted" << std::endl ;
    }
  else
    {
      std::cout << "ERROR during delete of " << description<T>::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_delete_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << description<T>::getClassType() << " delete statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_delete_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << description<T>::getClassType() << " delete statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
template <class T> uint32_t base_table<T>::get(uint32_t p_id, T & p_data)
{
  
  uint32_t l_result = 0;

  // Binding values to statement
  //----------------------------
  int l_status = sqlite3_bind_int(m_get_by_id_stmt,sqlite3_bind_parameter_index(m_get_by_id_stmt,"$id"),p_id);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of Id parameter for get_by_id statement of " << description<T>::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  l_status = sqlite3_step(m_get_by_id_stmt);
  if( l_status == SQLITE_ROW)
    {
      std::cout << description<T>::getClassType() << " successfully selected" << std::endl ;
      p_data = description<T>::getItemFromRow(m_get_by_id_stmt);

      // Ensure that ID is unique
      l_status = sqlite3_step(m_get_by_id_stmt);
      if( l_status == SQLITE_DONE)
	{
	  std::cout << description<T>::getClassType() << " successfully selected done" << std::endl ;
	  l_result = 1;
	}
      else
	{
	  std::cout << "ERROR during selection of " << description<T>::getClassType() << " : Id " << p_id << " is not unique " << sqlite3_errmsg(m_db) << std::endl ;
	  exit(-1);
	}
    }
  else if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during selection of " << description<T>::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }



  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_id_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << description<T>::getClassType() << " get_by_id statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_id_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << description<T>::getClassType() << " get_by_id statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

  return l_result;
}

//------------------------------------------------------------------------------
template <class T> void base_table<T>::get_all(std::vector<T> & p_list)
{

  int l_status = 0;
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_all_stmt)) == SQLITE_ROW)
    {
      p_list.push_back(description<T>::getItemFromRow(m_get_all_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during selection of " << description<T>::getClassType() << " : " << sqlite3_errmsg(m_db) << std::endl ;
      exit(-1);
    }

  std::cout <<  description<T>::getClassType() << " successfully listed" << std::endl ;

  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_all_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << description<T>::getClassType() << " get_all statement : " << sqlite3_errmsg(m_db) << std::endl ;     
      exit(-1);
    }

}

#endif
