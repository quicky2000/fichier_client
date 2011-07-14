#ifndef DESCRIPTION_VILLE_H
#define DESCRIPTION_VILLE_H

#include "description.h"
#include "ville.h"

template <> class description<ville>
{

 public:
  inline static const std::string get_class_type(void);
  inline static const std::string get_table_fields_declaration(void);
  inline static const std::string get_table_fields(void);
  inline static const std::string get_update_fields(void);
  inline static const std::string get_field_values(void);
  inline static void bind_item_values(const ville & p_ville,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static ville get_item_from_row(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<ville>::get_class_type(void)
{
  return "Ville";
}

//------------------------------------------------------------------------------
const std::string description<ville>::get_table_fields_declaration(void)
{
  return "Name TEXT, CodePostal TEXT";
}

//------------------------------------------------------------------------------
const std::string description<ville>::get_table_fields(void)
{
  return "Name, CodePostal";
}

//------------------------------------------------------------------------------
const std::string description<ville>::get_update_fields(void)
{
  return "Name = $name, CodePostal = $code_postal";
}

//------------------------------------------------------------------------------
const std::string description<ville>::get_field_values(void)
{
  return "$name, $code_postal";
}

//------------------------------------------------------------------------------
void description<ville>::bind_item_values(const ville & p_ville,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$name"),p_ville.get_name().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of name parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$code_postal"),p_ville.get_postal_code().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of postal code parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
ville description<ville>::get_item_from_row(sqlite3_stmt* p_stmt)
{
  return ville(sqlite3_column_int(p_stmt,0),//Id
	       (const char*)sqlite3_column_text(p_stmt,1),//Name
	       (const char*)sqlite3_column_text(p_stmt,2) //CodePostal
	       );
}

#endif
