#ifndef DESCRIPTION_LIVRE_FACTURE_H
#define DESCRIPTION_LIVRE_FACTURE_H

#include "description.h"
#include "livre_facture.h"

template <> class description<livre_facture>
{

 public:
  inline static const std::string getClassType(void);
  inline static const std::string getTableFieldsDeclaration(void);
  inline static const std::string getTableFields(void);
  inline static const std::string getUpdateFields(void);
  inline static const std::string getFieldValues(const livre_facture & p_livre_facture);
  inline static void bind_update_values(const livre_facture & p_livre_facture,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static livre_facture getItemFromRow(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<livre_facture>::getClassType(void)
{
  return "LivreFacture";
}

//------------------------------------------------------------------------------
const std::string description<livre_facture>::getTableFieldsDeclaration(void)
{
  return "StartDate TEXT, EndDate TEXT";
}

//------------------------------------------------------------------------------
const std::string description<livre_facture>::getTableFields(void)
{
  return "StartDate, EndDate";
}

//------------------------------------------------------------------------------
const std::string description<livre_facture>::getUpdateFields(void)
{
  return "StartDate = $start_date, EndDate = $end_date";
}

//------------------------------------------------------------------------------
const std::string description<livre_facture>::getFieldValues(const livre_facture & p_livre_facture)
{
  return "\"" + p_livre_facture.getStartDate() + "\", \"" + p_livre_facture.getEndDate() + "\"";
}

//------------------------------------------------------------------------------
void description<livre_facture>::bind_update_values(const livre_facture & p_livre_facture,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$start_date"),p_livre_facture.getStartDate().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of start date parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$end_date"),p_livre_facture.getEndDate().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of end date parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
livre_facture description<livre_facture>::getItemFromRow(sqlite3_stmt* p_stmt)
{
  return livre_facture(sqlite3_column_int(p_stmt,0),//Id
		       (const char*)sqlite3_column_text(p_stmt,1),//StartDate
		       (const char*)sqlite3_column_text(p_stmt,2) //EndDate
		       );
}

#endif
