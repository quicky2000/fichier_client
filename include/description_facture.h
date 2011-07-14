#ifndef DESCRIPTION_FACTURE_H
#define DESCRIPTION_FACTURE_H

#include "description.h"
#include "facture.h"

template <> class description<facture>
{

 public:
  inline static const std::string get_class_type(void);
  inline static const std::string get_table_fields_declaration(void);
  inline static const std::string get_table_fields(void);
  inline static const std::string get_update_fields(void);
  inline static const std::string get_field_values(void);
  inline static void bind_item_values(const facture & p_facture,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static facture get_item_from_row(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<facture>::get_class_type(void)
{
  return "Facture";
}

//------------------------------------------------------------------------------
const std::string description<facture>::get_table_fields_declaration(void)
{
  return "FactureRef INTEGER, ClientId INTEGER, Date TEXT, LivreFactureId INTEGER, Status INTEGER, ReasonId INTEGER";
}

//------------------------------------------------------------------------------
const std::string description<facture>::get_table_fields(void)
{
  return "FactureRef, ClientId, Date, LivreFactureId, Status, ReasonId";
}

//------------------------------------------------------------------------------
const std::string description<facture>::get_update_fields(void)
{
  return "FactureRef = $facture_ref, ClientId = $client_id, Date = $date, LivreFactureId = $livre_facture_id, Status = $status, ReasonId = $reason_id";
}

//------------------------------------------------------------------------------
const std::string description<facture>::get_field_values(void)
{
  return "$facture_ref, $client_id, $date, $livre_facture_id, $status, $reason_id";
}

//------------------------------------------------------------------------------
void description<facture>::bind_item_values(const facture & p_facture,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$facture_ref"),p_facture.get_facture_ref());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of facture_ref parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$client_id"),p_facture.get_client_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of client_id parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$date"),p_facture.get_date().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of date parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  

  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$livre_facture_id"),p_facture.get_livre_facture_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of livre_facture_id parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$status"),p_facture.get_status());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of status parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$reason_id"),p_facture.get_reason_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of reason_id parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
}

//------------------------------------------------------------------------------
facture description<facture>::get_item_from_row(sqlite3_stmt* p_stmt)
{
  return facture(sqlite3_column_int(p_stmt,0),//Facture Id
		 sqlite3_column_int(p_stmt,1),//FactureRef
		 sqlite3_column_int(p_stmt,2),//ClientId
		 (const char*)sqlite3_column_text(p_stmt,3),//Date
		 sqlite3_column_int(p_stmt,4),//LivreFactureId
		 sqlite3_column_int(p_stmt,5),//Status
		 sqlite3_column_int(p_stmt,6)//ReasonId
	       );
}

#endif
