#ifndef NAMED_TABLE_H
#define NAMED_TABLE_H

#include "base_table.h"

class named_item;

template <class T> class named_table:public base_table<T>
{
 public:
  named_table(void);
  ~named_table(void);
  void set_db(sqlite3 *p_db);

  const std::vector<T*>* get_by_name(const std::string & p_name);

 private:
  sqlite3_stmt *m_get_by_name_stmt;
};

//------------------------------------------------------------------------------
template <class T> named_table<T>::named_table(void):
  base_table<T>(),
  m_get_by_name_stmt(NULL)
{
}

//------------------------------------------------------------------------------
template <class T> named_table<T>::~named_table(void)
{
  sqlite3_finalize(m_get_by_name_stmt);
  std::cout << "Table " << description<T>::getClassType() << " end of destruction" << std::endl ;
}

//------------------------------------------------------------------------------
template <class T> void named_table<T>::set_db(sqlite3 *p_db)
{
  base_table<T>::set_db(p_db);
  
  // Preparing get_by_name statements
  //--------------------------------------------
  int l_status = sqlite3_prepare_v2(base_table<T>::get_db(),("SELECT Id,"+description<T>::getTableFields()+" FROM " + description<T>::getClassType() + " WHERE Name LIKE @name").c_str(),-1,&m_get_by_name_stmt,NULL);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during preparation of statement to get "+description<T>::getTableFields()+" item by name : " << sqlite3_errmsg(base_table<T>::get_db()) << std::endl ;     
      exit(-1);
    }

}

//------------------------------------------------------------------------------
template <class T> const std::vector<T*>* named_table<T>::get_by_name(const std::string & p_name)
{
  std::vector<T*> *l_result = new std::vector<T*>();
  std::string l_param_value("%");
  l_param_value += p_name + "%";

  // Binding values to statement
  //----------------------------

  int l_status = sqlite3_bind_text(m_get_by_name_stmt,sqlite3_bind_parameter_index(m_get_by_name_stmt,"@name"),l_param_value.c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of name parameter for get_by_name statement of " << description<T>::getClassType() << " : " << sqlite3_errmsg(base_table<T>::get_db()) << std::endl ;     
      exit(-1);
    }
    
  // Executing statement
  //---------------------
  while( (l_status = sqlite3_step(m_get_by_name_stmt)) == SQLITE_ROW)
    {
      l_result->push_back(description<T>::getItemFromRow(m_get_by_name_stmt));
    }
  if(l_status != SQLITE_DONE)
    {
      std::cout << "ERROR during selection of " << description<T>::getClassType() << " : " << sqlite3_errmsg(base_table<T>::get_db()) << std::endl ;
      exit(-1);
    }

  std::cout << "" << description<T>::getClassType() << " get_by_name successfully listed" << std::endl ;


  // Reset the statement for the next use
  //--------------------------------------
  l_status = sqlite3_reset(m_get_by_name_stmt);  
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of " << description<T>::getClassType() << " get_by_name statement : " << sqlite3_errmsg(base_table<T>::get_db()) << std::endl ;     
      exit(-1);
    }

  // Reset bindings because they are now useless
  //--------------------------------------------
  l_status = sqlite3_clear_bindings(m_get_by_name_stmt);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during reset of bindings of " << description<T>::getClassType() << " get_by_name statement : " << sqlite3_errmsg(base_table<T>::get_db()) << std::endl ;     
      exit(-1);
    }

  return l_result;
}


#endif
