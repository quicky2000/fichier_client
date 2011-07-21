#ifndef DESCRIPTION_ACHAT_H
#define DESCRIPTION_ACHAT_H

#include "description.h"
#include "achat.h"

template <> class description<achat>
{

 public:
  inline static const std::string get_class_type(void);
  inline static const std::string get_table_fields_declaration(void);
  inline static const std::string get_table_fields(void);
  inline static const std::string get_update_fields(void);
  inline static const std::string get_field_values(void);
  inline static void bind_item_values(const achat & p_achat,sqlite3_stmt* p_stmt,sqlite3 *p_db);
  inline static achat get_item_from_row(sqlite3_stmt* p_stmt);
 private:
  
};


//------------------------------------------------------------------------------
const std::string description<achat>::get_class_type(void)
{
  return "Purchase";
}

//------------------------------------------------------------------------------
const std::string description<achat>::get_table_fields_declaration(void)
{
  return "BillId INTEGER, BrandId INTEGER, TypeId INTEGER, Reference TEXT, PriceFranc REAL, PriceEuro REAL, Warranty INTEGER";
}

//------------------------------------------------------------------------------
const std::string description<achat>::get_table_fields(void)
{
  return "BillId, BrandId, TypeId, Reference, PriceFranc, PriceEuro, Warranty";
}

//------------------------------------------------------------------------------
const std::string description<achat>::get_update_fields(void)
{
  return "BillId = $facture_id, BrandId = $marque_id , TypeId = $type_id , Reference = $reference, PriceFranc = $prix_franc , PriceEuro = $prix_euro, Warranty = $garantie";
}

//------------------------------------------------------------------------------
const std::string description<achat>::get_field_values(void)
{
  return "$facture_id, $marque_id , $type_id , $reference, $prix_franc , $prix_euro, $garantie";
}

//------------------------------------------------------------------------------
void description<achat>::bind_item_values(const achat & p_achat,sqlite3_stmt* p_stmt,sqlite3 *p_db)
{
  int l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$facture_id"),p_achat.get_facture_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of facture_id parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$marque_id"),p_achat.get_marque_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of marque_id parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$type_id"),p_achat.get_type_id());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of type_id parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_text(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$reference"),p_achat.get_reference().c_str(),-1,SQLITE_STATIC);
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of reference parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }  

  l_status = sqlite3_bind_double(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$prix_franc"),p_achat.get_prix_franc());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of prix_franc parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_double(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$prix_euro"),p_achat.get_prix_euro());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of prix_euro parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
  l_status = sqlite3_bind_int(p_stmt,sqlite3_bind_parameter_index(p_stmt,"$garantie"),p_achat.get_garantie());
  if(l_status != SQLITE_OK)
    {
      std::cout << "ERROR during binding of garantie parameter for update statement of " << get_class_type() << " : " << sqlite3_errmsg(p_db) << std::endl ;     
      exit(-1);
    }
  
}

//------------------------------------------------------------------------------
achat description<achat>::get_item_from_row(sqlite3_stmt* p_stmt)
{
  return achat(sqlite3_column_int(p_stmt,0),//Purchase Id
	       sqlite3_column_int(p_stmt,1),//BillId
	       sqlite3_column_int(p_stmt,2),//BrandId
	       sqlite3_column_int(p_stmt,3),//TypeId
	       (const char*)sqlite3_column_text(p_stmt,4),//Reference
	       sqlite3_column_double(p_stmt,5),//PriceFranc
	       sqlite3_column_double(p_stmt,6),//PriceEuro
	       sqlite3_column_int(p_stmt,7)//Warranty
	       );
}

#endif
