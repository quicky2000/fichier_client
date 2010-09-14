#ifndef DESCRIPTION_CLIENT_H
#define DESCRIPTION_CLIENT_H

#include "description.h"
#include "client.h"

template <> class description<client>
{

 public:
  inline static const std::string getClassType(void);
  inline static const std::string getTableFieldsDeclaration(void);
  inline static const std::string getTableFields(void);
  inline static const std::string getUpdateFields(void);
  inline static const std::string getFieldValues(const client & p_client);
  inline static void bind_update_values(const client & p_client,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static client getItemFromRow(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<client>::getClassType(void)
{
  return "Client";
}

//------------------------------------------------------------------------------
const std::string description<client>::getTableFieldsDeclaration(void)
{
  return "Name TEXT, FirstName TEXT, Address TEXT, Tel TEXT, VilleId INTEGER";
}

//------------------------------------------------------------------------------
const std::string description<client>::getTableFields(void)
{
  return "Name, FirstName, Address, Tel, VilleId";
}

//------------------------------------------------------------------------------
const std::string description<client>::getUpdateFields(void)
{
  return "Name = $name, FirstName = $first_name, Address = $address , Tel = $tel , VilleId = $ville_id";
}

//------------------------------------------------------------------------------
const std::string description<client>::getFieldValues(const client & p_client)
{
  std::stringstream l_result;
  l_result << "\"" << p_client.get_name() << "\", \"" << p_client.get_first_name() << "\", \"" << p_client.get_address() <<  "\", \"" << p_client.get_tel() << "\", " << p_client.get_ville_id() ;
  return l_result.str();
}

//------------------------------------------------------------------------------
void description<client>::bind_update_values(const client & p_client,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$name"),p_client.get_name().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of name parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  

  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$first_name"),p_client.get_first_name().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of first_name parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  

  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$address"),p_client.get_address().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of address parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  

  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$tel"),p_client.get_tel().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of tel parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  

  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$ville_id"),p_client.get_ville_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of ville_id parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  
}

//------------------------------------------------------------------------------
client description<client>::getItemFromRow(sqlite3_stmt* p_stmt)
{
  return client(sqlite3_column_int(p_stmt,0),//Client Id
		(const char*)sqlite3_column_text(p_stmt,1),//Name
		(const char*)sqlite3_column_text(p_stmt,2),//First Name
		(const char*)sqlite3_column_text(p_stmt,3),//Address
		(const char*)sqlite3_column_text(p_stmt,4),//Tel
		sqlite3_column_int(p_stmt,5)//VilleId
		);
}

#endif
