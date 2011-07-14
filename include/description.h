#ifndef DESCRIPTION
#define DESCRIPTION

#include <string>

class sqlite3;
class sqlite3_stmt;

template <class T> class description
{

 public:
  // Static methods needed for table management in database
  inline static const std::string get_class_type(void);
  inline static const std::string get_table_fieldsDeclaration(void);
  inline static const std::string get_table_fields(void);
  inline static const std::string get_update_fields(void);
  inline static const std::string get_field_values(void);
  inline static void bind_item_values(const T & p_type_instance,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static T get_item_from_row(sqlite3_stmt* p_stmt);
 private:
  
};


#endif
