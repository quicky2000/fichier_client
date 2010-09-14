#ifndef DESCRIPTION
#define DESCRIPTION

#include <string>

class sqlite3;
class sqlite3_stmt;

template <class T> class description
{

 public:
  // Static methods needed for table management in database
  inline static const std::string getClassType(void);
  inline static const std::string getTableFieldsDeclaration(void);
  inline static const std::string getTableFields(void);
  inline static const std::string getUpdateFields(void);
  inline static const std::string getFieldValues(const T & p_type_instance);
  inline static void bind_update_values(const T & p_type_instance,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static T getItemFromRow(sqlite3_stmt* p_stmt);
 private:
  
};


#endif
