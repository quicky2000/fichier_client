#ifndef DESCRIPTION_ACHAT_H
#define DESCRIPTION_ACHAT_H

#include "description.h"
#include "achat.h"

template <> class description<achat>
{

 public:
  inline static const std::string getClassType(void);
  inline static const std::string getTableFieldsDeclaration(void);
  inline static const std::string getTableFields(void);
  inline static const std::string getUpdateFields(void);
  inline static const std::string getFieldValues(const achat & p_achat);
  inline static void bind_update_values(const achat & p_achat,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static achat* getItemFromRow(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<achat>::getClassType(void)
{
  return "Achat";
}

//------------------------------------------------------------------------------
const std::string description<achat>::getTableFieldsDeclaration(void)
{
  return "ClientId INTEGER, Date TEXT, LivreFactureId INTEGER, MarqueId INTEGER, TypeId INTEGER, Reference TEXT, PrixFranc REAL, PrixEuro REAL, Garantie INTEGER";
}

//------------------------------------------------------------------------------
const std::string description<achat>::getTableFields(void)
{
  return "ClientId, Date, LivreFactureId, MarqueId, TypeId, Reference, PrixFranc, PrixEuro, Garantie";
}

//------------------------------------------------------------------------------
const std::string description<achat>::getUpdateFields(void)
{
  return "ClientId = $client_id, Date = $date, LivreFactureId = $livre_facture_id, MarqueId = $marque_id , TypeId = $type_id , Reference = $reference, PrixFranc = $prix_franc , PrixEuro = $prix_euro, Garantie = $garantie";
}

//------------------------------------------------------------------------------
const std::string description<achat>::getFieldValues(const achat & p_achat)
{
  std::stringstream l_result;
  l_result << p_achat.get_client_id() << ", \"" << p_achat.get_date() << "\", " << p_achat.get_livre_facture_id() << ", " << p_achat.get_marque_id() <<  ", " << p_achat.get_type_id() << ", \"" << p_achat.get_reference() << "\" ," << p_achat.get_prix_franc() << ", " << p_achat.get_prix_euro() <<  ", " << p_achat.get_garantie();
  return l_result.str();
}

//------------------------------------------------------------------------------
void description<achat>::bind_update_values(const achat & p_achat,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$client_id"),p_achat.get_client_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of client_id parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$date"),p_achat.get_date().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of date parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  

  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$livre_facture_id"),p_achat.get_livre_facture_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of livre_facture_id parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$marque_id"),p_achat.get_marque_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of marque_id parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$type_id"),p_achat.get_type_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of type_id parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$reference"),p_achat.get_reference().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of reference parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  

  l_status = sqlite3_bind_double(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$prix_franc"),p_achat.get_prix_franc());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of prix_franc parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_double(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$prix_euro"),p_achat.get_prix_euro());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of prix_euro parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$garantie"),p_achat.get_garantie());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of garantie parameter for update statement of " << getClassType() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
}

//------------------------------------------------------------------------------
achat* description<achat>::getItemFromRow(sqlite3_stmt* p_stmt)
{
  return new achat(sqlite3_column_int(p_stmt,0),//Achat Id
		   sqlite3_column_int(p_stmt,1),//ClientId
		   (const char*)sqlite3_column_text(p_stmt,2),//Date
		   sqlite3_column_int(p_stmt,3),//LivreFactureId
		   sqlite3_column_int(p_stmt,4),//MarqueId
		   sqlite3_column_int(p_stmt,5),//TypeId
		   (const char*)sqlite3_column_text(p_stmt,6),//Reference
		   sqlite3_column_double(p_stmt,7),//PrixFranc
		   sqlite3_column_double(p_stmt,8),//PrixEuro
		   sqlite3_column_int(p_stmt,9)//Garantie
		   );
}

#endif
