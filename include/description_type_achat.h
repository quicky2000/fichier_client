#ifndef DESCRIPTION_TYPE_ACHAT_H
#define DESCRIPTION_TYPE_ACHAT_H

#include "description.h"
#include "type_achat.h"

template <> class description<type_achat>
{

 public:
  inline static const std::string getClassType(void);
  inline static const std::string getTableFieldsDeclaration(void);
  inline static const std::string getTableFields(void);
  inline static const std::string getUpdateFields(void);
  inline static const std::string getFieldValues(const type_achat & p_type_achat);
  inline static void bind_update_values(const type_achat & p_type_achat,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static type_achat* getItemFromRow(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<type_achat>::getClassType(void)
{
  return "Type_achat";
}

//------------------------------------------------------------------------------
const std::string description<type_achat>::getTableFieldsDeclaration(void)
{
  return "Name TEXT";
}

//------------------------------------------------------------------------------
const std::string description<type_achat>::getTableFields(void)
{
  return "Name";
}

//------------------------------------------------------------------------------
const std::string description<type_achat>::getUpdateFields(void)
{
  return "Name = $name";
}

//------------------------------------------------------------------------------
const std::string description<type_achat>::getFieldValues(const type_achat & p_type_achat)
{
  return "\""+p_type_achat.getName()+"\"";
}

//------------------------------------------------------------------------------
void description<type_achat>::bind_update_values(const type_achat & p_type_achat,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$name"),p_type_achat.getName().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of name parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
type_achat* description<type_achat>::getItemFromRow(sqlite3_stmt* p_stmt)
{
  return new type_achat(sqlite3_column_int(p_stmt,0),(const char*)sqlite3_column_text(p_stmt,1));
}

#endif
