#ifndef TYPE_ACHAT_H
#define TYPE_ACHAT_H

#include "named_item.h"

class type_achat: public named_item
{

 public:
  inline type_achat(const std::string &p_name);
  inline type_achat(uint32_t p_id, const std::string &p_name);
  inline const std::string getType(void)const;

  // Static methods needed for table management in database
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
type_achat::type_achat(const std::string &p_name):
  named_item(p_name)
{
}

//------------------------------------------------------------------------------
type_achat::type_achat(uint32_t p_id, const std::string &p_name):
  named_item(p_id,p_name)
{
}

//------------------------------------------------------------------------------
const std::string type_achat::getType(void)const
{
  return "Type_achat";
}

//------------------------------------------------------------------------------
const std::string type_achat::getClassType(void)
{
  return "Type_achat";
}

//------------------------------------------------------------------------------
const std::string type_achat::getTableFieldsDeclaration(void)
{
  return "Name TEXT";
}

//------------------------------------------------------------------------------
const std::string type_achat::getTableFields(void)
{
  return "Name";
}

//------------------------------------------------------------------------------
const std::string type_achat::getUpdateFields(void)
{
  return "Name = $name";
}

//------------------------------------------------------------------------------
const std::string type_achat::getFieldValues(const type_achat & p_type_achat)
{
  return "\""+p_type_achat.getName()+"\"";
}

//------------------------------------------------------------------------------
void type_achat::bind_update_values(const type_achat & p_type_achat,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$name"),p_type_achat.getName().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of name parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
type_achat* type_achat::getItemFromRow(sqlite3_stmt* p_stmt)
{
  return new type_achat(sqlite3_column_int(p_stmt,0),(const char*)sqlite3_column_text(p_stmt,1));
}

#endif
