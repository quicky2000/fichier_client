#ifndef DESCRIPTION_FACTURE_STATUS_H
#define DESCRIPTION_FACTURE_STATUS_H

#include "description.h"
#include "facture_status.h"

template <> class description<facture_status>
{

 public:
  inline static const std::string getClassType(void);
  inline static const std::string getTableFieldsDeclaration(void);
  inline static const std::string getTableFields(void);
  inline static const std::string getUpdateFields(void);
  inline static const std::string getFieldValues(const facture_status & p_facture_status);
  inline static void bind_update_values(const facture_status & p_facture_status,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static facture_status getItemFromRow(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<facture_status>::getClassType(void)
{
  return "Facture_status";
}

//------------------------------------------------------------------------------
const std::string description<facture_status>::getTableFieldsDeclaration(void)
{
  return "Name TEXT";
}

//------------------------------------------------------------------------------
const std::string description<facture_status>::getTableFields(void)
{
  return "Name";
}

//------------------------------------------------------------------------------
const std::string description<facture_status>::getUpdateFields(void)
{
  return "Name = $name";
}

//------------------------------------------------------------------------------
const std::string description<facture_status>::getFieldValues(const facture_status & p_facture_status)
{
  return "\"" + p_facture_status.getName() + "\"";
}

//------------------------------------------------------------------------------
void description<facture_status>::bind_update_values(const facture_status & p_facture_status,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$name"),p_facture_status.getName().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of name parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
facture_status description<facture_status>::getItemFromRow(sqlite3_stmt* p_stmt)
{
  return facture_status(sqlite3_column_int(p_stmt,0),(const char*)sqlite3_column_text(p_stmt,1));
}

#endif
