#ifndef DESCRIPTION_MARQUE_H
#define DESCRIPTION_MARQUE_H

#include "description.h"
#include "marque.h"

template <> class description<marque>
{

 public:
  inline static const std::string getClassType(void);
  inline static const std::string getTableFieldsDeclaration(void);
  inline static const std::string getTableFields(void);
  inline static const std::string getUpdateFields(void);
  inline static const std::string getFieldValues(const marque & p_marque);
  inline static void bind_update_values(const marque & p_marque,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static marque* getItemFromRow(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<marque>::getClassType(void)
{
  return "Marque";
}

//------------------------------------------------------------------------------
const std::string description<marque>::getTableFieldsDeclaration(void)
{
  return "Name TEXT";
}

//------------------------------------------------------------------------------
const std::string description<marque>::getTableFields(void)
{
  return "Name";
}

//------------------------------------------------------------------------------
const std::string description<marque>::getUpdateFields(void)
{
  return "Name = $name";
}

//------------------------------------------------------------------------------
const std::string description<marque>::getFieldValues(const marque & p_marque)
{
  return "\"" + p_marque.getName() + "\"";
}

//------------------------------------------------------------------------------
void description<marque>::bind_update_values(const marque & p_marque,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$name"),p_marque.getName().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of name parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
marque* description<marque>::getItemFromRow(sqlite3_stmt* p_stmt)
{
  return new marque(sqlite3_column_int(p_stmt,0),(const char*)sqlite3_column_text(p_stmt,1));
}

#endif
