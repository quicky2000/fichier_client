#ifndef DESCRIPTION_LIVRE_FACTURE_H
#define DESCRIPTION_LIVRE_FACTURE_H

#include "description.h"
#include "livre_facture.h"

template <> class description<livre_facture>
{

 public:
  inline static const std::string get_class_type(void);
  inline static const std::string get_table_fields_declaration(void);
  inline static const std::string get_table_fields(void);
  inline static const std::string get_update_fields(void);
  inline static const std::string get_field_values(void);
  inline static void bind_item_values(const livre_facture & p_livre_facture,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static livre_facture get_item_from_row(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<livre_facture>::get_class_type(void)
{
  return "LivreFacture";
}

//------------------------------------------------------------------------------
const std::string description<livre_facture>::get_table_fields_declaration(void)
{
  return "StartDate TEXT, EndDate TEXT";
}

//------------------------------------------------------------------------------
const std::string description<livre_facture>::get_table_fields(void)
{
  return "StartDate, EndDate";
}

//------------------------------------------------------------------------------
const std::string description<livre_facture>::get_update_fields(void)
{
  return "StartDate = $start_date, EndDate = $end_date";
}

//------------------------------------------------------------------------------
const std::string description<livre_facture>::get_field_values(void)
{
  return "$start_date, $end_date";
}

//------------------------------------------------------------------------------
void description<livre_facture>::bind_item_values(const livre_facture & p_livre_facture,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$start_date"),p_livre_facture.getStartDate().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of start date parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$end_date"),p_livre_facture.getEndDate().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of end date parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
livre_facture description<livre_facture>::get_item_from_row(sqlite3_stmt* p_stmt)
{
  return livre_facture(sqlite3_column_int(p_stmt,0),//Id
		       (const char*)sqlite3_column_text(p_stmt,1),//StartDate
		       (const char*)sqlite3_column_text(p_stmt,2) //EndDate
		       );
}

#endif
