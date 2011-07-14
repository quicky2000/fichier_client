#ifndef DESCRIPTION_TYPE_ACHAT_H
#define DESCRIPTION_TYPE_ACHAT_H

#include "description.h"
#include "type_achat.h"

template <> class description<type_achat>
{

 public:
  inline static const std::string get_class_type(void);
  inline static const std::string get_table_fields_declaration(void);
  inline static const std::string get_table_fields(void);
  inline static const std::string get_update_fields(void);
  inline static const std::string get_field_values(void);
  inline static void bind_item_values(const type_achat & p_type_achat,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static type_achat get_item_from_row(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<type_achat>::get_class_type(void)
{
  return "Type_achat";
}

//------------------------------------------------------------------------------
const std::string description<type_achat>::get_table_fields_declaration(void)
{
  return "Name TEXT";
}

//------------------------------------------------------------------------------
const std::string description<type_achat>::get_table_fields(void)
{
  return "Name";
}

//------------------------------------------------------------------------------
const std::string description<type_achat>::get_update_fields(void)
{
  return "Name = $name";
}

//------------------------------------------------------------------------------
const std::string description<type_achat>::get_field_values(void)
{
  return "$name";
}

//------------------------------------------------------------------------------
void description<type_achat>::bind_item_values(const type_achat & p_type_achat,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$name"),p_type_achat.get_name().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of name parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
type_achat description<type_achat>::get_item_from_row(sqlite3_stmt* p_stmt)
{
  return type_achat(sqlite3_column_int(p_stmt,0),(const char*)sqlite3_column_text(p_stmt,1));
}

#endif
