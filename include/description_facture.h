#ifndef DESCRIPTION_FACTURE_H
#define DESCRIPTION_FACTURE_H

#include "description.h"
#include "facture.h"

template <> class description<facture>
{

 public:
  inline static const std::string getClassType(void);
  inline static const std::string getTableFieldsDeclaration(void);
  inline static const std::string getTableFields(void);
  inline static const std::string getUpdateFields(void);
  inline static const std::string getFieldValues(const facture & p_facture);
  inline static void bind_update_values(const facture & p_facture,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static facture getItemFromRow(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<facture>::getClassType(void)
{
  return "Facture";
}

//------------------------------------------------------------------------------
const std::string description<facture>::getTableFieldsDeclaration(void)
{
  return "FactureRef INTEGER, ClientId INTEGER, Date TEXT, LivreFactureId INTEGER, Status INTEGER";
}

//------------------------------------------------------------------------------
const std::string description<facture>::getTableFields(void)
{
  return "FactureRef, ClientId, Date, LivreFactureId, Status";
}

//------------------------------------------------------------------------------
const std::string description<facture>::getUpdateFields(void)
{
  return "FactureRef = $facture_ref, ClientId = $client_id, Date = $date, LivreFactureId = $livre_facture_id, Status = $status";
}

//------------------------------------------------------------------------------
const std::string description<facture>::getFieldValues(const facture & p_facture)
{
  std::stringstream l_result;
  l_result << p_facture.get_facture_ref() <<","  << p_facture.get_client_id() << ", \"" << p_facture.get_date() << "\", " << p_facture.get_livre_facture_id() << ", " << p_facture.get_status();
  return l_result.str();
}

//------------------------------------------------------------------------------
void description<facture>::bind_update_values(const facture & p_facture,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$facture_ref"),p_facture.get_facture_ref());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of facture_ref parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$client_id"),p_facture.get_client_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of client_id parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$date"),p_facture.get_date().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of date parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  

  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$livre_facture_id"),p_facture.get_livre_facture_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of livre_facture_id parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$status"),p_facture.get_status());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of status parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
}

//------------------------------------------------------------------------------
facture description<facture>::getItemFromRow(sqlite3_stmt* p_stmt)
{
  return facture(sqlite3_column_int(p_stmt,0),//Facture Id
	       sqlite3_column_int(p_stmt,1),//FactureRef
	       sqlite3_column_int(p_stmt,2),//ClientId
	       (const char*)sqlite3_column_text(p_stmt,3),//Date
	       sqlite3_column_int(p_stmt,4),//LivreFactureId
	       sqlite3_column_int(p_stmt,5)//Status
	       );
}

#endif
