#ifndef DESCRIPTION_FACTURE_REASON_H
#define DESCRIPTION_FACTURE_REASON_H

#include "description.h"
#include "facture_reason.h"

template <> class description<facture_reason>
{

 public:
  inline static const std::string getClassType(void);
  inline static const std::string getTableFieldsDeclaration(void);
  inline static const std::string getTableFields(void);
  inline static const std::string getUpdateFields(void);
  inline static const std::string getFieldValues(void);
  inline static void bind_item_values(const facture_reason & p_facture_reason,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static facture_reason getItemFromRow(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<facture_reason>::getClassType(void)
{
  return "facture_reason";
}

//------------------------------------------------------------------------------
const std::string description<facture_reason>::getTableFieldsDeclaration(void)
{
  return "Name TEXT";
}

//------------------------------------------------------------------------------
const std::string description<facture_reason>::getTableFields(void)
{
  return "Name";
}

//------------------------------------------------------------------------------
const std::string description<facture_reason>::getUpdateFields(void)
{
  return "Name = $name";
}

//------------------------------------------------------------------------------
const std::string description<facture_reason>::getFieldValues(void)
{
  return "$name";
}

//------------------------------------------------------------------------------
void description<facture_reason>::bind_item_values(const facture_reason & p_facture_reason,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$name"),p_facture_reason.getName().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of name parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
facture_reason description<facture_reason>::getItemFromRow(sqlite3_stmt* p_stmt)
{
  return facture_reason(sqlite3_column_int(p_stmt,0),(const char*)sqlite3_column_text(p_stmt,1));
}

#endif
