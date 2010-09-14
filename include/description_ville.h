#ifndef DESCRIPTION_VILLE_H
#define DESCRIPTION_VILLE_H

#include "description.h"
#include "ville.h"

template <> class description<ville>
{

 public:
  inline static const std::string getClassType(void);
  inline static const std::string getTableFieldsDeclaration(void);
  inline static const std::string getTableFields(void);
  inline static const std::string getUpdateFields(void);
  inline static const std::string getFieldValues(const ville & p_ville);
  inline static void bind_update_values(const ville & p_ville,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static ville getItemFromRow(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<ville>::getClassType(void)
{
  return "Ville";
}

//------------------------------------------------------------------------------
const std::string description<ville>::getTableFieldsDeclaration(void)
{
  return "Name TEXT, CodePostal TEXT";
}

//------------------------------------------------------------------------------
const std::string description<ville>::getTableFields(void)
{
  return "Name, CodePostal";
}

//------------------------------------------------------------------------------
const std::string description<ville>::getUpdateFields(void)
{
  return "Name = $name, CodePostal = $code_postal";
}

//------------------------------------------------------------------------------
const std::string description<ville>::getFieldValues(const ville & p_ville)
{
  return "\"" + p_ville.getName() + "\", \"" + p_ville.getCodePostal() + "\"";
}

//------------------------------------------------------------------------------
void description<ville>::bind_update_values(const ville & p_ville,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$name"),p_ville.getName().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of name parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$code_postal"),p_ville.getCodePostal().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of code_postal parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
ville description<ville>::getItemFromRow(sqlite3_stmt* p_stmt)
{
  return ville(sqlite3_column_int(p_stmt,0),//Id
	       (const char*)sqlite3_column_text(p_stmt,1),//Name
	       (const char*)sqlite3_column_text(p_stmt,2) //CodePostal
	       );
}

#endif
