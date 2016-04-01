#include "fichier_client.h"
#include "fichier_client_db.h"
#include "external_sql_importer.h"
#include "fichier_client_UI_if.h"
#include <unistd.h>
#include <iostream>
#include <fstream>

using namespace std;

//------------------------------------------------------------------------------
fichier_client::fichier_client(void):
  m_current_customer_id(0),
  m_facture_status_pending_modif(false),
  m_facture_reason_pending_modif(false),
  m_purchase_brand_pending_modif(false),
  m_purchase_type_pending_modif(false),
  m_city_pending_modif(false),
  m_user_interface(NULL),
  m_db(NULL),
  m_db_name("")
{
  remove_tmp_db();
}

//------------------------------------------------------------------------------
fichier_client::~fichier_client(void)
{
  if(m_db)delete m_db; 
}

//------------------------------------------------------------------------------
void fichier_client::set_user_interface(fichier_client_UI_if * p_user_interface)
{
  assert(p_user_interface);
  m_user_interface = p_user_interface;

  manage_features(false);
}

//------------------------------------------------------------------------------
void fichier_client::treat_open_database_event(const std::string & p_name)
{
  std::cout << "Fichier_Client Event::open database event" << std::endl ;
  if(p_name != "")
    {
      m_user_interface->set_import_file_action_enabled(false);
      m_user_interface->set_open_file_action_enabled(false);
      m_user_interface->set_save_file_action_enabled(true);
      m_user_interface->set_save_as_file_action_enabled(true);
      m_user_interface->set_close_file_action_enabled(true);    

      // Open database
      copy(p_name,m_tmp_db_name);
      m_db_name = p_name;
      open_tmp_db();
      refresh_non_attributed_facture_status_list();
      refresh_non_attributed_facture_reason_list();
      refresh_customer_data_facture_status_list();
      refresh_brand_list();
      refresh_purchase_type_list();
      refresh_city_list();

      // Update interface
      m_user_interface->display_information_message("Open Status","File successfully opened");
      m_user_interface->set_application_title(p_name);
      manage_features(true);
    }
}

//------------------------------------------------------------------------------
void fichier_client::treat_close_database_event(void)
{
 std::cout << "Fichier_Client Event::close database event" << std::endl ;
 bool l_close = true;
 if(need_save())
   {
     l_close = m_user_interface->ask_yes_no_qestion("Close","Database has non saved modification\nAre you sure you want to close it ?");
   }
  if(l_close)
    {
      m_user_interface->set_open_file_action_enabled(true);
      m_user_interface->set_save_file_action_enabled(false);
      m_user_interface->set_save_as_file_action_enabled(false);
      m_user_interface->set_close_file_action_enabled(false);    
      manage_features(false);

      // Close db
      close_tmp_db();
      m_db_name = "";

      // Reset title
      m_user_interface->set_application_title("Fichier client");
    }

}

//------------------------------------------------------------------------------
void fichier_client::treat_save_database_event(void)
{
 std::cout << "Fichier_Client Event::save event" << std::endl ;
 save();
}

//------------------------------------------------------------------------------
void fichier_client::treat_save_as_database_event(const std::string & p_name)
{
 std::cout << "Fichier_Client Event::save as event" << std::endl ;
  if(p_name != "")
    {
      save_as(p_name);
      m_user_interface->display_information_message("Save Status","File successfully saved");
      m_user_interface->set_save_file_action_enabled(true);
      m_user_interface->set_application_title(p_name);
    }
}

//------------------------------------------------------------------------------
void fichier_client::treat_import_external_sql_event(const std::string & p_name)
{
  std::cout << "Fichier_Client Event::import external sql event" << std::endl ;
  if(p_name != "")
    {
      assert(m_user_interface);
      m_user_interface->set_import_file_action_enabled(false);
      m_user_interface->set_open_file_action_enabled(false);
      m_user_interface->set_save_as_file_action_enabled(true);
      m_user_interface->set_close_file_action_enabled(true);

      // import external sql file
      assert(m_db == NULL);
      assert(m_db_name=="");
      remove_tmp_db();
      m_db = new fichier_client_db(m_tmp_db_name);
      cout << "Importing file name \"" << p_name << "\"" << endl ;
      external_sql_importer::import(p_name,*m_db);
      m_db->create_information("created_by",m_version);
      cout << "Import successfull" << endl ;

      m_user_interface->display_information_message("Import Status","File successfully imported");
      m_user_interface->set_application_title(p_name);
      manage_features(true);
    }
}


// Search client related events
//------------------------------------------------------------------------------
void fichier_client::treat_search_customer_criteria_modification_event(void)
{
  std::cout << "Fichier_client Event::search customer criteria modification event" << std::endl;
  assert(m_user_interface);
  std::string l_name = m_user_interface->get_search_customer_name();
  std::string l_first_name = m_user_interface->get_search_customer_first_name();
  std::string l_address = m_user_interface->get_search_customer_address();
  std::string l_city = m_user_interface->get_search_customer_city();
  std::cout << "Name = \"" << l_name << "\"\tFirst name = \"" << l_first_name << "\"\tAddress = \"" << l_address << "\"\tCity = \"" << l_city << "\"" << std::endl ;

  std::vector<search_client_item> l_result ;
  assert(m_db);
  m_db->search_client(l_name,l_first_name,l_address,l_city,l_result);
  m_user_interface->update_search_customer_list(l_result);
  
  m_user_interface->set_customer_search_add_customer_enabled(l_result.size()==0);
  m_user_interface->set_customer_search_modify_customer_enabled(false);
  m_user_interface->set_customer_search_delete_customer_enabled(false);
}

//------------------------------------------------------------------------------
void fichier_client::treat_search_customer_customer_selected_event(void)
{
  std::cout << "Fichier_client Event::search customer customer selected event" << std::endl;
  assert(m_user_interface);
  uint32_t l_client_id = m_user_interface->get_selected_customer();
  assert(l_client_id);
  std::cout << "Id of selected client " << l_client_id << std::endl;
 
  std::vector<search_achat_item> l_purchase_list;
  m_db->get_achat_by_client_id(l_client_id,l_purchase_list);
  m_user_interface->update_search_customer_list_achat(l_purchase_list);

  std::vector<search_facture_item> l_list_facture;
  m_db->get_facture_by_client_id(l_client_id,l_list_facture);
  m_user_interface->update_search_customer_list_facture(l_list_facture);

  m_user_interface->set_customer_search_delete_customer_enabled(l_list_facture.size() == 0 && l_purchase_list.size() == 0);
  m_user_interface->set_customer_search_modify_customer_enabled(true);
  
}

//------------------------------------------------------------------------------
void fichier_client::treat_search_customer_no_more_customer_selected_event(void)
{
  std::cout << "Fichier_client Event::search customer no more customer selected event" << std::endl;
  assert(m_user_interface);
  m_user_interface->set_customer_search_add_customer_enabled(false);  
  m_user_interface->set_customer_search_delete_customer_enabled(false);
  m_user_interface->set_customer_search_modify_customer_enabled(false);
}

//------------------------------------------------------------------------------
void fichier_client::treat_search_customer_add_customer_event(void)
{
  std::cout << "Fichier_client Event::search customer add customer event" << std::endl;
  assert(m_user_interface);

  m_current_customer_id = 0;

  m_user_interface->set_customer_name(m_user_interface->get_search_customer_name());
  m_user_interface->set_customer_first_name(m_user_interface->get_search_customer_first_name());
  m_user_interface->set_customer_address(m_user_interface->get_search_customer_address());
  m_user_interface->set_customer_phone("");
  std::vector<ville> l_cities;
  assert(m_db);
  m_db->get_ville_by_name(m_user_interface->get_search_customer_city(),l_cities);

  m_user_interface->set_customer_postal_code(l_cities.size() != 1 ? "" : l_cities.begin()->get_postal_code());
  m_user_interface->set_customer_city_list(l_cities);
  
  m_user_interface->set_customer_data_identity_fields_enabled(true);
  m_user_interface->set_customer_data_create_customer_enabled(false);
  m_user_interface->set_customer_data_modify_customer_enabled(false);
  m_user_interface->set_customer_data_delete_customer_enabled(false);

  check_customer_identity();

  // Disable bill edition
  m_user_interface->set_customer_bill_fields_enabled(false);
  m_user_interface->set_customer_bill_list_enabled(false);
  m_user_interface->set_customer_bill_creation_enabled(false);
  m_user_interface->set_customer_bill_modification_enabled(false);
  m_user_interface->set_customer_bill_deletion_enabled(false);


  // Disable purchase edition
  m_user_interface->set_customer_purchase_fields_enabled(false);
  m_user_interface->set_customer_purchase_list_enabled(false);
  m_user_interface->set_customer_purchase_creation_enabled(false);
  m_user_interface->set_customer_purchase_modification_enabled(false);
  m_user_interface->set_customer_purchase_deletion_enabled(false);

  m_user_interface->set_focus_on_customer_data();
}

//------------------------------------------------------------------------------
void fichier_client::treat_search_customer_modify_customer_event(void)
{
  std::cout << "Fichier_client Event::search customer modify customer event" << std::endl;
  m_user_interface->set_customer_data_identity_fields_enabled(true);

  uint32_t l_client_id = m_user_interface->get_selected_customer();
  assert(l_client_id);

  client l_client;
  assert(m_db);
  m_db->get_client(l_client_id,l_client);

  m_current_customer_id = l_client_id;

  m_user_interface->set_customer_name(l_client.get_name());
  m_user_interface->set_customer_first_name(l_client.get_first_name());
  m_user_interface->set_customer_phone(l_client.get_phone());
  m_user_interface->set_customer_address(l_client.get_address());

  ville l_city;
  m_db->get_ville(l_client.get_city_id(),l_city);
  m_user_interface->set_customer_postal_code(l_city.get_postal_code());
  std::vector<ville> l_city_list;
  l_city_list.push_back(l_city);
  m_user_interface->set_customer_city_list(l_city_list);
  
  m_user_interface->set_customer_bill_fields_enabled(true);
  m_user_interface->set_customer_bill_list_enabled(true);

  refresh_customer_data_bill_list();
  refresh_customer_data_purchase_list();

  check_customer_identity();

  m_user_interface->set_focus_on_customer_data();
}

//------------------------------------------------------------------------------
void fichier_client::treat_search_customer_delete_customer_event(void)
{
  std::cout << "Fichier_client Event::search customer delete customer event" << std::endl;
  assert(m_user_interface);
  uint32_t l_customer_id = m_user_interface->get_selected_customer();

  assert(m_db);
  std::vector<search_facture_item> l_bill_list;
  m_db->get_facture_by_client_id(l_customer_id,l_bill_list);
  std::vector<search_achat_item> l_purchase_list;
  m_db->get_achat_by_client_id(l_customer_id,l_purchase_list);

  assert(l_bill_list.size()==0 && l_purchase_list.size()==0);

  client l_customer;
  m_db->get_client(l_customer_id,l_customer);
  m_db->remove(l_customer);
  
  treat_search_customer_criteria_modification_event();
}

//------------------------------------------------------------------------------
void fichier_client::treat_leave_customer_data_for_search_event(void)
{
  std::cout << "Fichier_client Event::customer data widget has been leaved to perform a search" << std::endl;
  m_current_customer_id = 0;
  set_customer_identity_enabled(false);
  set_customer_data_bill_enabled(false);
  set_customer_data_purchase_enabled(false);
  assert(m_user_interface);
  m_user_interface->clear_customer_search_criteria();
}


// Customer data identity information related events
//------------------------------------------------------------------------------
void fichier_client::treat_customer_postal_code_modification_event(void)
{
  std::cout << "Fichier_client Event::customer identity postal code modification event" << std::endl;
  assert(m_user_interface);
  std::string l_code_postal = m_user_interface->get_customer_postal_code();
  assert(m_db);
  std::vector<ville> l_city_list;
  m_db->get_ville_by_code_postal(l_code_postal,l_city_list);

  m_user_interface->set_customer_city_list(l_city_list);

  treat_identity_content_modification_event();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_city_selection_event(void)
{
  std::cout << "Fichier_client Event::customer identity city selection event" << std::endl;

  assert(m_user_interface);
  const ville * l_city = m_user_interface->get_customer_city();
  if(l_city)
    {
      m_user_interface->set_customer_postal_code(l_city->get_postal_code());
    }
  treat_identity_content_modification_event();
}

//------------------------------------------------------------------------------
void fichier_client::treat_identity_content_modification_event(void)
{
  std::cout << "Fichier_client Event::customer identity content modification event" << std::endl;
  check_customer_identity();
}

// Customer data identity actions related events
//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_create_customer_event(void)
{
  std::cout << "Fichier_client Event::customer_data customer creation event" << std::endl;
  assert(m_user_interface);
  std::string l_name = m_user_interface->get_customer_name();
  std::string l_first_name = m_user_interface->get_customer_first_name();
  std::string l_phone = (m_user_interface->is_customer_phone_complete() ? m_user_interface->get_customer_phone() : "");
  std::string l_address = m_user_interface->get_customer_address();
  std::string l_postal_code = m_user_interface->get_customer_postal_code();
  const ville * l_city = m_user_interface->get_customer_city();

  bool l_complete = l_name != "" && l_first_name != ""  && l_postal_code != "" && l_city != NULL && l_city->get_postal_code() == l_postal_code && (m_user_interface->is_customer_phone_complete() || m_user_interface->is_customer_phone_empty());
  assert(l_complete);

  std::vector<search_client_item> l_client_list ;
  assert(m_db);
  m_db->search_client(l_name,l_first_name,l_address,l_city->get_name(),l_client_list,true);
  assert(l_client_list.size()==0);

  client l_client(l_name,l_first_name,l_address,l_phone,l_city->get_id());
  m_db->create(l_client);
  m_current_customer_id = l_client.get_id();
  
  m_user_interface->set_customer_data_create_customer_enabled(false);
  m_user_interface->set_customer_data_modify_customer_enabled(false);
  m_user_interface->set_customer_data_delete_customer_enabled(true);

  treat_search_customer_criteria_modification_event();

  // Enable bill edition
  set_customer_data_bill_enabled(true); 
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_modify_customer_event(void)
{
  std::cout << "Fichier_client Event::customer_data customer modification event" << std::endl;

  std::string l_name = m_user_interface->get_customer_name();
  std::string l_first_name = m_user_interface->get_customer_first_name();
  std::string l_phone = (m_user_interface->is_customer_phone_complete() ? m_user_interface->get_customer_phone() : "");
  std::string l_address = m_user_interface->get_customer_address();
  std::string l_postal_code = m_user_interface->get_customer_postal_code();
  const ville * l_city = m_user_interface->get_customer_city();

  bool l_complete = l_name != "" && l_first_name != ""  && l_postal_code != "" && l_city != NULL && l_city->get_postal_code() == l_postal_code && (m_user_interface->is_customer_phone_complete() || m_user_interface->is_customer_phone_empty());
  assert(l_complete);

  assert(m_current_customer_id);
  assert(m_db);
  client l_client ;
  m_db->get_client(m_current_customer_id,l_client);

  l_client.set_name(l_name);
  l_client.set_first_name(l_first_name);
  l_client.set_phone(l_phone);
  l_client.set_address(l_address);
  l_client.set_city_id(l_city->get_id());
  
  m_db->update(l_client);
  treat_search_customer_criteria_modification_event();

  m_user_interface->set_customer_data_create_customer_enabled(false);
  m_user_interface->set_customer_data_modify_customer_enabled(false);
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_delete_customer_event(void)
{
  std::cout << "Fichier_client Event::customer_data customer deletion event" << std::endl;
   assert(m_current_customer_id);
   
  assert(m_db);
  client l_client ;
  m_db->get_client(m_current_customer_id,l_client);

   std::vector<search_achat_item> l_purchase_list;
   m_db->get_achat_by_client_id(l_client.get_id(),l_purchase_list);
   assert(l_purchase_list.size()==0);

  std::vector<search_facture_item> l_list_facture;
  m_db->get_facture_by_client_id(l_client.get_id(),l_list_facture);
   assert(l_list_facture.size()==0);

   m_db->remove(l_client);
   m_current_customer_id = 0;


   set_customer_identity_enabled(false);

   assert(m_user_interface);
   treat_search_customer_criteria_modification_event();
   m_user_interface->set_focus_on_customer_search();
   
}

// Customer data bill information related events
//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_bill_date_modification_event(void)
{
  std::cout << "Fichier_client Event::customer_data bill date modification event" << std::endl;
  assert(m_user_interface);
  std::string l_date = m_user_interface->get_customer_bill_date();

  std::vector<livre_facture> l_bill_books;
  assert(m_db);
  m_db->get_livre_facture_containing_date(l_date,l_bill_books);
  
  if(!l_bill_books.size())
    {
      m_user_interface->display_warning_message("Pas de livre correspondant","La date de la facture ne correspond a aucun des livres de facture. Rentrer le livre correspondant avant de resélectionner la facture");
    }
  std::vector<uint32_t> l_bill_book_ids;
  std::vector<livre_facture>::const_iterator l_iter = l_bill_books.begin();
  std::vector<livre_facture>::const_iterator l_iter_end = l_bill_books.end();
  while(l_iter != l_iter_end)
    {
      l_bill_book_ids.push_back(l_iter->get_id());
      ++l_iter;
    }
  m_user_interface->set_customer_facture_allowed_livre_ids(l_bill_book_ids);
  refresh_customer_bill_actions();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_bill_book_selection_event(void)
{
  std::cout << "Fichier_client Event::customer_data bill book selection event" << std::endl;
  assert(m_user_interface);
  uint32_t l_bill_book_id = m_user_interface->get_customer_bill_book_id();

  livre_facture l_book;
  assert(m_db);
  m_db->get_livre_facture(l_bill_book_id,l_book);

  std::vector<facture> l_book_list;
  m_db->get_by_livre_facture(l_bill_book_id,l_book_list);

  std::vector<uint32_t> l_remaing_refs;
  get_remaining_refs(l_book,l_book_list,l_remaing_refs);

  m_user_interface->set_customer_allowed_facture_references(l_remaing_refs);
  
  refresh_customer_bill_actions();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_bill_ref_selection_event(void)
{
  std::cout << "Fichier_client Event::customer_data bill ref selection event" << std::endl;
  refresh_customer_bill_actions();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_bill_status_selection_event(void)
{
  std::cout << "Fichier_client Event::customer_data bill status selection event" << std::endl;
  refresh_customer_bill_actions();
}

// Customer data bill list related events
//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_bill_selected_event(void)
{
  std::cout << "Fichier_client Event::customer_data bill selection event" << std::endl;
  assert(m_user_interface);
  assert(!m_user_interface->is_customer_data_bill_selection_empty());
  uint32_t l_bill_id = m_user_interface->get_customer_data_selected_bill_id();

  assert(m_db);
  facture l_bill;
  m_db->get_facture(l_bill_id,l_bill);
  std::string l_date = l_bill.get_date();
  m_user_interface->set_customer_facture_date(l_date);
  uint32_t l_bill_book_id = l_bill.get_livre_facture_id();
  std::vector<uint32_t> l_book_ids;

  std::vector<livre_facture> l_potential_book;
  m_db->get_livre_facture_containing_date(l_date,l_potential_book);

  std::vector<livre_facture>::const_iterator l_iter = l_potential_book.begin();
  std::vector<livre_facture>::const_iterator l_iter_end = l_potential_book.end();
  while(l_iter != l_iter_end)
    {
      l_book_ids.push_back(l_iter->get_id());
      ++l_iter;
    }

  m_user_interface->set_customer_facture_allowed_livre_ids(l_book_ids);

  if(l_bill_book_id == 0)
    {
      if(!l_potential_book.size())
	{
	  m_user_interface->display_warning_message("Pas de livre correspondant","La date de la facture ne correspond a aucun des livres de facture. Rentrer le livre correspondant avant de resélectionner la facture");
	}
      else if(l_potential_book.size()==1)
	{
	  treat_customer_data_bill_book_selection_event();
	}
    }
  else
    {
      bool l_found = false;

      // Search book id in list of potential book
      std::vector<uint32_t>::const_iterator l_iter_book_id = l_book_ids.begin();
      std::vector<uint32_t>::const_iterator l_iter_end_book_id = l_book_ids.end();
      while(l_iter_book_id != l_iter_end_book_id && !l_found)
	{
	  l_found = *l_iter_book_id == l_bill_book_id;
	  ++l_iter_book_id;
	}
      assert(l_found);
      m_user_interface->set_customer_facture_livre_id(l_bill_book_id);

      livre_facture l_bill_book;
      m_db->get_livre_facture(l_bill_book_id,l_bill_book);

      std::vector<uint32_t> l_refs;
      std::vector<facture> l_bill_list;
      m_db->get_by_livre_facture(l_bill_book_id,l_bill_list);

      get_remaining_refs(l_bill_book,l_bill_list,l_refs);

      std::vector<uint32_t>::iterator l_iter = l_refs.begin();
      std::vector<uint32_t>::const_iterator l_iter_end = l_refs.end();
      while(l_iter != l_iter_end && *l_iter < l_bill.get_facture_ref())
	{
	  ++l_iter;
	}
      l_refs.insert(l_iter,l_bill.get_facture_ref());


      m_user_interface->set_customer_allowed_facture_references(l_refs);
      m_user_interface->set_customer_facture_reference(l_bill.get_facture_ref());
    }
  std::vector<facture_status> l_status_list;
  m_db->get_all_facture_status(l_status_list);
  m_user_interface->set_customer_facture_status_list(l_status_list);

  m_user_interface->set_customer_facture_status(l_bill.get_status());

  set_customer_data_purchase_enabled(true);

  refresh_customer_bill_actions();
  refresh_customer_data_purchase_list();
}
 
//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_no_more_bill_selected_event(void)
{
  std::cout << "Fichier_client Event::customer_data no more bill selected event" << std::endl;
  assert(m_user_interface);
  m_user_interface->set_customer_facture_date("");
  std::vector<uint32_t> l_empty_list;
  m_user_interface->set_customer_facture_allowed_livre_ids(l_empty_list);
  m_user_interface->set_customer_allowed_facture_references(l_empty_list);
  set_customer_data_purchase_enabled(false);
  refresh_customer_bill_actions();
  refresh_customer_data_purchase_list();
}

// Customer data bill actions related events
//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_bill_creation_event(void)
{
  std::cout << "Fichier_client Event::customer_data bill creation event" << std::endl;
  assert(m_user_interface);
  assert(m_user_interface->is_customer_data_bill_selection_empty());
  assert(m_user_interface->is_customer_bill_date_complete());
  assert(m_user_interface->get_customer_bill_book_id());
  assert(m_user_interface->get_customer_bill_status());
  facture l_bill(m_user_interface->get_customer_bill_reference(),
		 m_current_customer_id,
		 m_user_interface->get_customer_bill_date(),
		 m_user_interface->get_customer_bill_book_id(),
		 m_user_interface->get_customer_bill_status()->get_id(),
		 0);
  assert(m_db);
  m_db->create(l_bill);
  m_user_interface->set_customer_facture_date("");
  std::vector<uint32_t> l_empty_list;
  m_user_interface->set_customer_facture_allowed_livre_ids(l_empty_list);
  m_user_interface->set_customer_allowed_facture_references(l_empty_list);
  refresh_customer_data_bill_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_bill_modification_event(void)
{
  std::cout << "Fichier_client Event::customer_data bill modification event" << std::endl;
  assert(m_user_interface);
  assert(!m_user_interface->is_customer_data_bill_selection_empty());
  uint32_t l_bill_id = m_user_interface->get_customer_data_selected_bill_id();
  facture l_bill;
  assert(m_db);
  uint32_t l_result = m_db->get_facture(l_bill_id,l_bill);
  assert(l_result);
  assert(m_user_interface->is_customer_bill_date_complete());
  l_bill.set_date(m_user_interface->get_customer_bill_date());
  l_bill.set_livre_facture_id(m_user_interface->get_customer_bill_book_id());
  l_bill.set_facture_ref(m_user_interface->get_customer_bill_reference());
  const facture_status *l_status = m_user_interface->get_customer_bill_status();
  assert(l_status);
  l_bill.set_status(l_status->get_id());
  
  m_db->update(l_bill);

  refresh_customer_data_bill_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_bill_deletion_event(void)
{
  std::cout << "Fichier_client Event::customer_data bill modification event" << std::endl;
  assert(m_user_interface);
  assert(!m_user_interface->is_customer_data_bill_selection_empty());
  uint32_t l_bill_id = m_user_interface->get_customer_data_selected_bill_id();
  facture l_bill;
  assert(m_db);
  uint32_t l_result = m_db->get_facture(l_bill_id,l_bill);
  assert(l_result);

  std::vector<achat> l_purchase_list;
  m_db->get_by_facture_id(l_bill_id,l_purchase_list);
  assert(l_purchase_list.size()==0);
  m_db->remove(l_bill);
  refresh_customer_data_bill_list();
}

// Livre facture related events
//------------------------------------------------------------------------------
void fichier_client::treat_delete_livre_facture_event(void)
{
  std::cout << "Fichier_cient Event::delete_livre_facture event" << std::endl;
  assert(m_user_interface);
  // Get selected livre facture id
  uint32_t l_livre_facture_id = m_user_interface->get_selected_livre_facture_id();

  assert(l_livre_facture_id);
  assert(m_db);

  // Ensure there are no related factures
  std::vector<facture> l_related_factures;
  m_db->get_by_livre_facture(l_livre_facture_id,l_related_factures);
  assert(!l_related_factures.size());

  // Remove livre facture
  livre_facture l_selected;
  m_db->get_livre_facture(l_livre_facture_id,l_selected);
  m_db->remove(l_selected);
  
  // Clear related information in UI
  m_user_interface->set_delete_livre_facture_enabled(false);
  m_user_interface->set_modify_livre_facture_enabled(false);
  m_user_interface->set_create_livre_facture_enabled(false);
  m_user_interface->clear_livre_facture_information();

  // Update livre facture list
  refresh_livre_facture_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_modify_livre_facture_event(void)
{
  std::cout << "Fichier_cient Event::modify_livre_facture event" << std::endl;

  assert(m_user_interface);
  assert(m_user_interface->is_livre_facture_start_date_complete());
  assert(m_user_interface->is_livre_facture_end_date_complete());
  std::string l_start_date = m_user_interface->get_livre_facture_start_date();
  std::string l_end_date = m_user_interface->get_livre_facture_end_date();
  assert(l_start_date < l_end_date);

  // Get selected livre facture id
  uint32_t l_livre_facture_id = m_user_interface->get_selected_livre_facture_id();
  
  assert(l_livre_facture_id);
  assert(m_db);

  // Get selected livre facture
  livre_facture l_selected;
  m_db->get_livre_facture(l_livre_facture_id,l_selected);

  // Update livre_facture
  l_selected.setStartDate(l_start_date);
  l_selected.setEndDate(l_end_date);
  m_db->update(l_selected);

  // Update UI
  m_user_interface->set_livre_facture_start_date("");
  m_user_interface->set_livre_facture_end_date("");
  refresh_livre_facture_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_create_livre_facture_event(void)
{
  std::cout << "Fichier_cient Event::Create livre_facture event" << std::endl;
  assert(m_user_interface);
  assert(m_user_interface->get_livre_facture_id().size()==6);
  assert(m_user_interface->is_livre_facture_start_date_complete());
  assert(m_user_interface->is_livre_facture_end_date_complete());
  std::string l_start_date = m_user_interface->get_livre_facture_start_date();
  std::string l_end_date = m_user_interface->get_livre_facture_end_date();
  assert(l_start_date < l_end_date);

  uint32_t l_livre_facture_id = strtol(m_user_interface->get_livre_facture_id().c_str(),NULL,10);

  livre_facture l_dummy_livre;
  assert(m_db);
  assert(m_db->get_livre_facture(l_livre_facture_id,l_dummy_livre)==0);

  // Create livre
  livre_facture l_livre(l_livre_facture_id,
			l_start_date,
			l_end_date);
  m_db->create(l_livre);

  // Update UI
  m_user_interface->set_create_livre_facture_enabled(false);
  m_user_interface->set_livre_facture_start_date("");
  m_user_interface->set_livre_facture_end_date("");
  m_user_interface->set_livre_facture_id("");
  refresh_livre_facture_list();
}

//------------------------------------------------------------------------------
void fichier_client::refresh_livre_facture_list(void)
{
  assert(m_user_interface);
  std::vector<livre_facture> l_list;
  m_db->get_all_livre_facture(l_list);
  m_user_interface->refresh_livre_facture_list(l_list);
}

//------------------------------------------------------------------------------
void fichier_client::refresh_non_attributed_facture_status_list(void)
{
  std::vector<facture_status> l_non_attributed_facture_status_list;
  assert(m_db);
  m_db->get_all_facture_status(l_non_attributed_facture_status_list);
  m_user_interface->set_non_attributed_facture_status_list(l_non_attributed_facture_status_list);
  if(!l_non_attributed_facture_status_list.size())
    {
      m_user_interface->display_warning_message("Status de facture manquants","Aucun statut n'a été défini pour les factures non attribuées");
    }
}

//------------------------------------------------------------------------------
void fichier_client::refresh_non_attributed_facture_reason_list(void)
{
  std::vector<facture_reason> l_facture_reason_list;
  assert(m_db);
  m_db->get_all_facture_reason(l_facture_reason_list);

  m_user_interface->set_non_attributed_facture_reason_list(l_facture_reason_list);
  if(!l_facture_reason_list.size())
    {
      m_user_interface->display_warning_message("Raison de facture non attribuee manquante","Aucune raison n'a été définie pour les factures non attribuées");
    }
}

//------------------------------------------------------------------------------
void fichier_client::treat_create_non_attributed_facture_event(void)
{
  assert(m_user_interface);
  assert(!m_user_interface->is_livre_facture_selection_empty());

  const facture_status * l_status = m_user_interface->get_non_attributed_facture_status();
  // Create facture event is possible because all values have been checked !!
  assert(l_status);
  const facture_reason * l_reason = m_user_interface->get_non_attributed_facture_reason();
  assert(l_reason);
  std::string l_date = (m_user_interface->is_non_attributed_facture_date_complete() ? m_user_interface->get_non_attributed_facture_date() : "");
  facture l_facture(m_user_interface->get_non_attributed_facture_reference(),
		    0,
		    l_date,
		    m_user_interface->get_non_attributed_facture_livre_facture_id(),
		    l_status->get_id(),
		    l_reason->get_id()
		    );
  cout << l_facture << endl ;
  m_db->create(l_facture);
  
  refresh_non_attributed_facture_list();
  refresh_livre_facture_list();
}

// Customer data purchase information related events
//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_purchase_brand_selection_event(void)
{
  std::cout << "Fichier_client Event :: customer data purchase brand selection event" << std::endl ;
  refresh_customer_data_purchase_actions();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_purchase_type_selection_event(void)
{
  std::cout << "Fichier_client Event :: customer data purchase type selection event" << std::endl ;
  refresh_customer_data_purchase_actions();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_purchase_reference_modification_event(void)
{
  std::cout << "Fichier_client Event :: customer data purchase reference modification event" << std::endl ;
  refresh_customer_data_purchase_actions();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_purchase_euro_price_modification_event(void)
{
  std::cout << "Fichier_client Event :: customer data purchase euro price modification event" << std::endl ;
  assert(m_user_interface);
  const std::string l_euro_price_str = m_user_interface->get_customer_purchase_euro_price();
  double l_euro_price = strtod(l_euro_price_str.c_str(),NULL);
  std::cout << "Euro price = " << l_euro_price << std::endl;
  double l_franc_price = ((uint32_t)(((l_euro_price * 6.55957)*100))/100.0);
  std::cout << "Franc price = " << l_franc_price << std::endl;
  stringstream l_franc_price_stream;
  l_franc_price_stream << l_franc_price;
  std::cout << "Franc price string = " << l_franc_price_stream.str() << std::endl;
  m_user_interface->set_customer_purchase_franc_price(l_franc_price_stream.str());
  refresh_customer_data_purchase_actions();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_purchase_franc_price_modification_event(void)
{
  std::cout << "Fichier_client Event :: customer data purchase franc price modification event" << std::endl ;
  assert(m_user_interface);
  const std::string l_franc_price_str = m_user_interface->get_customer_purchase_franc_price();
  double l_franc_price = strtod(l_franc_price_str.c_str(),NULL);
  std::cout << "Franc price = " << l_franc_price << std::endl;
  double l_euro_price = ((uint32_t)(((l_franc_price / 6.55957)*100))/100.0);
  std::cout << "Euro price = " << l_euro_price << std::endl;
  stringstream l_euro_price_stream;
  l_euro_price_stream << l_euro_price;
  std::cout << "Euro price string = " << l_euro_price_stream.str() << std::endl;
  m_user_interface->set_customer_purchase_euro_price(l_euro_price_stream.str());
  refresh_customer_data_purchase_actions();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_purchase_warranty_modification_event(void)
{
  std::cout << "Fichier_client Event :: customer data purchase warranty modification event" << std::endl ;
  refresh_customer_data_purchase_actions();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_purchase_selected_event(void)
{
  std::cout << "Fichier_client Event :: customer data purchase selection event" << std::endl ;
  assert(m_user_interface);
  assert(!m_user_interface->is_customer_data_purchase_selection_empty());

  uint32_t l_purchase_id = m_user_interface->get_customer_data_selected_purchase_id();
  assert(m_db);
  achat l_purchase;
  uint32_t l_result = m_db->get_achat(l_purchase_id,l_purchase);
  assert(l_result == 1);

  m_user_interface->set_customer_purchase_brand(l_purchase.get_marque_id());
  m_user_interface->set_customer_purchase_type(l_purchase.get_type_id());
  m_user_interface->set_customer_purchase_reference(l_purchase.get_reference());
  stringstream l_euro_price_str;
  l_euro_price_str << l_purchase.get_prix_euro();
  m_user_interface->set_customer_purchase_euro_price(l_euro_price_str.str());
  stringstream l_franc_price_str;
  l_franc_price_str << l_purchase.get_prix_franc();
  m_user_interface->set_customer_purchase_franc_price(l_franc_price_str.str());
  m_user_interface->set_customer_purchase_warranty(l_purchase.get_garantie());

  refresh_customer_data_purchase_actions();
  
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_no_more_purchase_selected_event(void)
{
  std::cout << "Fichier_client Event :: customer data no more purchase selected event" << std::endl ;
  assert(m_user_interface);
  assert(m_user_interface->is_customer_data_purchase_selection_empty());
  clear_purchase_informations();
  refresh_customer_data_purchase_actions();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_purchase_creation_event(void)
{
  std::cout << "Fichier_client Event :: customer data purchase purchase creation event" << std::endl ;
  assert(m_user_interface);
  assert(m_user_interface->is_customer_data_purchase_selection_empty());

  // Purchase should always be associated to a bill
  assert(!m_user_interface->is_customer_data_bill_selection_empty());
 
  // Getting information for current purchase
  const type_achat * l_purchase_type = m_user_interface->get_customer_purchase_type();
  const marque * l_purchase_brand = m_user_interface->get_customer_purchase_brand();
  const std::string l_purchase_reference = m_user_interface->get_customer_purchase_reference();
  double l_purchase_franc_price = strtod(m_user_interface->get_customer_purchase_franc_price().c_str(),NULL);
  double l_purchase_euro_price = strtod(m_user_interface->get_customer_purchase_euro_price().c_str(),NULL);
  bool l_purchase_warranty = m_user_interface->is_customer_purchase_warranty_selected();
  
  bool l_complete = l_purchase_type && l_purchase_brand && l_purchase_reference != "" && m_user_interface->get_customer_purchase_franc_price() != "" && m_user_interface->get_customer_purchase_euro_price() != "";
  assert(l_complete);

  achat l_purchase(m_user_interface->get_customer_data_selected_bill_id(),
		   l_purchase_brand->get_id(),
		   l_purchase_type->get_id(),
		   l_purchase_reference,
		   l_purchase_franc_price,
		   l_purchase_euro_price,
		   l_purchase_warranty);
  m_db->create(l_purchase);
  clear_purchase_informations();
  refresh_customer_data_purchase_actions();
  refresh_customer_data_purchase_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_purchase_modification_event(void)
{
  std::cout << "Fichier_client Event :: customer data purchase purchase modification event" << std::endl ;
   assert(m_user_interface);
  assert(!m_user_interface->is_customer_data_purchase_selection_empty());
  uint32_t l_purchase_id = m_user_interface->get_customer_data_selected_purchase_id();
  assert(m_db);
  achat l_purchase;
  uint32_t l_result = m_db->get_achat(l_purchase_id,l_purchase);
  assert(l_result == 1);

  // Getting information for current purchase 
  const type_achat * l_purchase_type = m_user_interface->get_customer_purchase_type();
  const marque * l_purchase_brand = m_user_interface->get_customer_purchase_brand();
  const std::string l_purchase_reference = m_user_interface->get_customer_purchase_reference();
  double l_purchase_franc_price = strtod(m_user_interface->get_customer_purchase_franc_price().c_str(),NULL);
  double l_purchase_euro_price = strtod(m_user_interface->get_customer_purchase_euro_price().c_str(),NULL);
  bool l_purchase_warranty = m_user_interface->is_customer_purchase_warranty_selected();
 
  bool l_complete = l_purchase_type && l_purchase_brand && l_purchase_reference != "" && m_user_interface->get_customer_purchase_franc_price() != "" && m_user_interface->get_customer_purchase_euro_price() != "";
  assert(l_complete);

  // Updating purchase
  l_purchase.set_type_id(l_purchase_type->get_id());
  l_purchase.set_marque_id(l_purchase_brand->get_id());
  l_purchase.set_reference(l_purchase_reference);
  l_purchase.set_prix_franc(l_purchase_franc_price);
  l_purchase.set_prix_euro(l_purchase_euro_price);
  l_purchase.set_garantie(l_purchase_warranty);

  m_db->update(l_purchase);
  clear_purchase_informations();
  refresh_customer_data_purchase_actions();
  refresh_customer_data_purchase_list();  
}

//------------------------------------------------------------------------------
void fichier_client::treat_customer_data_purchase_deletion_event(void)
{
  std::cout << "Fichier_client Event :: customer data purchase purchase deletion event" << std::endl ;
  assert(m_user_interface);
  assert(!m_user_interface->is_customer_data_purchase_selection_empty());
  uint32_t l_purchase_id = m_user_interface->get_customer_data_selected_purchase_id();
  assert(m_db);
  achat l_purchase;
  uint32_t l_result = m_db->get_achat(l_purchase_id,l_purchase);
  assert(l_result == 1);
  m_db->remove(l_purchase);
  clear_purchase_informations();
  refresh_customer_data_purchase_actions();
  refresh_customer_data_purchase_list();  
}


//------------------------------------------------------------------------------
bool fichier_client::check_new_facture_id(uint32_t p_facture_id, uint32_t p_livre_facture_id)
{
  bool l_result = false;
  livre_facture l_livre_facture;
  assert(m_db);
  m_db->get_livre_facture(p_livre_facture_id,l_livre_facture);
  if(p_facture_id && p_facture_id <= l_livre_facture.get_nb_max_facture())
    {
      vector<facture> l_facture_list;
      m_db->get_by_livre_facture_and_ref(p_facture_id,p_livre_facture_id,l_facture_list);
      if(l_facture_list.size()==0)
	{
	  l_result = true;
	}
      else
	{
	  m_user_interface->display_warning_message("Facture référence invalide","Une facture portant cette référence existe déja");
	}
    }
  else
    {
      stringstream l_max_sstr;
      l_max_sstr << l_livre_facture.get_nb_max_facture();
      m_user_interface->display_warning_message("Facture référence invalide",string("La référénce de la facture doit être comprise entre 1 et ")+l_max_sstr.str());
    }
  return l_result;
}

//------------------------------------------------------------------------------
void fichier_client::treat_livre_facture_selected_event(void)
{
  std::cout << "Fichier_client Event :: Livre_facture selected " << std::endl;
  assert(m_user_interface);
  assert(!m_user_interface->is_livre_facture_selection_empty());
  uint32_t l_livre_facture_id = m_user_interface->get_selected_livre_facture_id();

  vector<facture> l_list_facture;
  assert(m_db);
  // Get factures related to selected livre_facture
  m_db->get_by_livre_facture(l_livre_facture_id,l_list_facture);

  // Allowed delete button if there are no factures
  m_user_interface->set_delete_livre_facture_enabled(!l_list_facture.size());

  // Get livre_facture object
  livre_facture l_selected_livre;
  m_db->get_livre_facture(l_livre_facture_id,l_selected_livre);

  // Set lvire facture inforamtion with information of selected livre
  stringstream l_livre_id_str;
  l_livre_id_str << l_selected_livre.get_id();
  m_user_interface->set_livre_facture_id(l_livre_id_str.str());
  m_user_interface->set_livre_facture_start_date(l_selected_livre.getStartDate());
  m_user_interface->set_livre_facture_end_date(l_selected_livre.getEndDate());

  refresh_non_attributed_facture_list();

  // Check if livre facture is complete or not
  if(l_list_facture.size() < l_selected_livre.get_nb_max_facture())
    {

      // Computing remaining references
      std::vector<uint32_t> l_remaining_refs;
      get_remaining_refs(l_selected_livre,l_list_facture,l_remaining_refs);

      // Update UI according to collected data
      m_user_interface->clear_non_attributed_facture_date();
      m_user_interface->set_non_attributed_allowed_facture_references(l_remaining_refs);
      vector<uint32_t> l_allowed_books;
      l_allowed_books.push_back(l_livre_facture_id);
      m_user_interface->set_non_attributed_facture_allowed_livre_ids(l_allowed_books);
      m_user_interface->enable_non_attributed_facture_fields(true);      
    }
  else
    {
      //Disable fields allowing to create new facture
      std::cout << "Livre facture is full" << std::endl ;
      m_user_interface->enable_non_attributed_facture_fields(false);
    }
}

//------------------------------------------------------------------------------
void fichier_client::refresh_non_attributed_facture_list(void)
{
  vector<search_facture_client_item> l_list_facture;
  assert(m_user_interface);
  if(!m_user_interface->is_livre_facture_selection_empty())
    {
      uint32_t l_livre_facture_id = m_user_interface->get_selected_livre_facture_id();
      assert(m_db);
      get_facture_by_livre_facture_id(l_livre_facture_id,l_list_facture);
    }
  m_user_interface->refresh_list_facture_of_livre_facture(l_list_facture);
}


//------------------------------------------------------------------------------
void fichier_client::treat_no_more_livre_facture_selected_event(void)
{
  std::cout << "Fichier_cient Event :: No more livre_facture selected " << std::endl;
  m_user_interface->set_delete_livre_facture_enabled(false);
  m_user_interface->set_modify_livre_facture_enabled(false);
  m_user_interface->set_create_livre_facture_enabled(false);
  refresh_non_attributed_facture_list();
  // Set lvire facture inforamtion with information of selected livre
  m_user_interface->set_livre_facture_id("");
  m_user_interface->set_livre_facture_start_date("");
  m_user_interface->set_livre_facture_end_date("");
  m_user_interface->clear_non_attributed_facture_date();
  m_user_interface->set_facture_creation_for_selected_livre_enabled(false);
}

//------------------------------------------------------------------------------
void fichier_client::treat_livre_facture_id_modif_event(void)
{
  std::cout << "Fichier_cient Event :: Livre_facture Id modif event" << std::endl;
  assert(m_user_interface);
  m_user_interface->set_create_livre_facture_enabled(false);
  std::string l_livre_id = m_user_interface->get_livre_facture_id();
  std::vector<livre_facture> l_list_livre_facture;
  assert(m_db);
  if(l_livre_id.length() == 6)
    {
      livre_facture l_list_facture ;
      uint32_t l_status = m_db->get_livre_facture(strtol(l_livre_id.c_str(),NULL,10),l_list_facture);
      if(l_status == 1)
	{
	  l_list_livre_facture.push_back(l_list_facture); 
	}
      else
	{
	  m_user_interface->display_information_message("Recherche livre facture",std::string("Pas de livre de facture avec l'Id ")+l_livre_id+"\nVous pourrez le créer en cliquant sur le bouton \"Créer\" après avoir renseigné les dates");
	  m_user_interface->set_livre_facture_start_date("");
	  m_user_interface->set_livre_facture_end_date("");
	}
    }
  else
    {
      m_db->get_all_livre_facture(l_list_livre_facture);
    }
  m_user_interface->refresh_livre_facture_list(l_list_livre_facture);
  
  check_livre_facture_information();
}

//------------------------------------------------------------------------------
void fichier_client::treat_livre_facture_content_modif_event(void)
{
  std::cout << "Fichier_client Event :: Livre_facture content modif event" << std::endl;
  check_livre_facture_information(); 
}

//------------------------------------------------------------------------------
void fichier_client::set_customer_information_enabled(bool p_enabled)
{
  set_customer_identity_enabled(false);
  set_customer_data_bill_enabled(false);
  set_customer_data_purchase_enabled(false);
}

//------------------------------------------------------------------------------
void fichier_client::set_customer_identity_enabled(bool p_enabled)
{
  assert(m_user_interface);
  m_user_interface->set_customer_name("");
  m_user_interface->set_customer_first_name("");
  m_user_interface->set_customer_phone("");
  m_user_interface->set_customer_address("");
  m_user_interface->set_customer_postal_code("");
  std::vector<ville> l_cities;
  m_user_interface->set_customer_city_list(l_cities);
  m_user_interface->set_customer_data_identity_fields_enabled(p_enabled);
  m_user_interface->set_customer_data_create_customer_enabled(false);
  m_user_interface->set_customer_data_modify_customer_enabled(false);
  m_user_interface->set_customer_data_delete_customer_enabled(false);
}

//------------------------------------------------------------------------------
void fichier_client::set_customer_data_bill_enabled(bool p_enabled)
{
 m_user_interface->set_customer_bill_fields_enabled(p_enabled);
  std::vector<search_facture_item> l_bill_list;
  m_user_interface->update_customer_data_bill_list(l_bill_list);
  m_user_interface->set_customer_bill_list_enabled(p_enabled);
  m_user_interface->set_customer_bill_creation_enabled(false);
  m_user_interface->set_customer_bill_modification_enabled(false);
  m_user_interface->set_customer_bill_deletion_enabled(false);
}

//------------------------------------------------------------------------------
void fichier_client::set_customer_data_purchase_enabled(bool p_enabled)
{
  m_user_interface->set_customer_purchase_fields_enabled(p_enabled);
  std::vector<search_achat_item> l_purchase_list;
  m_user_interface->update_customer_data_purchase_list(l_purchase_list);
  m_user_interface->set_customer_purchase_list_enabled(p_enabled);
  m_user_interface->set_customer_purchase_creation_enabled(false);
  m_user_interface->set_customer_purchase_modification_enabled(false);
  m_user_interface->set_customer_purchase_deletion_enabled(false);
}

//------------------------------------------------------------------------------
void fichier_client::refresh_customer_data_bill_list(void)
{
  std::vector<search_facture_item> l_list_facture;
  m_db->get_facture_by_client_id(m_current_customer_id,l_list_facture);
  m_user_interface->update_customer_data_bill_list(l_list_facture);

  refresh_customer_bill_actions();
}

//------------------------------------------------------------------------------
void fichier_client::refresh_customer_data_purchase_list(void)
{
  std::vector<search_achat_item> l_purchase_list;
  assert(m_user_interface);
  assert(m_db);
  if(m_user_interface->is_customer_data_bill_selection_empty())
    {
      m_db->get_achat_by_client_id(m_current_customer_id,l_purchase_list);
    }
  else
    {
      uint32_t l_bill_id = m_user_interface->get_customer_data_selected_bill_id();
      m_db->get_purchase_by_bill_id(l_bill_id,l_purchase_list);
    }

  m_user_interface->update_customer_data_purchase_list(l_purchase_list);
}


//------------------------------------------------------------------------------
void fichier_client::check_customer_identity(void)
{
  assert(m_user_interface);
  std::string l_name = m_user_interface->get_customer_name();
  std::string l_first_name = m_user_interface->get_customer_first_name();
  std::string l_phone = (m_user_interface->is_customer_phone_complete() ? m_user_interface->get_customer_phone() : "");
  std::string l_address = m_user_interface->get_customer_address();
  std::string l_postal_code = m_user_interface->get_customer_postal_code();
  const ville * l_city = m_user_interface->get_customer_city();

  std::cout << "Phone \"" << l_phone << "\"" << std::endl ;
  bool l_complete = l_name != "" && l_first_name != ""  && l_postal_code != "" && l_city != NULL && l_city->get_postal_code() == l_postal_code && (m_user_interface->is_customer_phone_complete() || m_user_interface->is_customer_phone_empty());

  std::vector<search_client_item> l_client_list ;
  assert(m_db);
  if(l_complete)
    {
      std::cout << "CLient identity is complete" << std::endl ; 
      m_db->search_client(l_name,l_first_name,l_address,l_city->get_name(),l_client_list,true);
      std::cout << "Corresponding list size " << l_client_list.size() << std::endl ;
    }

  if(m_current_customer_id)
    {
      std::cout << "Existing customer" << std::endl;
      if(l_client_list.size()==0 && l_complete)
	{
	  std::cout << "Complete and no conflicts" << std::endl;
	  client l_client;  
	  m_db->get_client(m_current_customer_id,l_client);

	  bool l_modified = l_client.get_name() != l_name || 
	    l_client.get_first_name() != l_first_name || 
	    l_client.get_phone() != l_phone ||
	    l_client.get_address() != l_address ||
	    l_client.get_city_id() != l_city->get_id();
	  m_user_interface->set_customer_data_modify_customer_enabled(l_modified);
	}
      m_user_interface->set_customer_data_create_customer_enabled(false);

      std::vector<search_facture_item> l_bill_list;
      m_db->get_facture_by_client_id(m_current_customer_id,l_bill_list);
      std::vector<search_achat_item> l_purchase_list;
      m_db->get_achat_by_client_id(m_current_customer_id,l_purchase_list);
      
      m_user_interface->set_customer_data_delete_customer_enabled(l_bill_list.size()==0 && l_purchase_list.size() ==0);
    }
  else
    {
      m_user_interface->set_customer_data_create_customer_enabled(l_client_list.size() == 0 && l_complete);
      m_user_interface->set_customer_data_modify_customer_enabled(false);
      m_user_interface->set_customer_data_delete_customer_enabled(false);
    }
}

//------------------------------------------------------------------------------
void fichier_client::clear_purchase_informations(void)
{
  assert(m_user_interface);
  m_user_interface->set_customer_purchase_brand(0);
  m_user_interface->set_customer_purchase_type(0);
  m_user_interface->set_customer_purchase_euro_price("");
  m_user_interface->set_customer_purchase_franc_price("");
  m_user_interface->set_customer_purchase_reference("");
  m_user_interface->set_customer_purchase_warranty(false);
}

//------------------------------------------------------------------------------
void fichier_client::refresh_customer_bill_actions(void)
{
  assert(m_user_interface);
  uint32_t l_bill_id = (m_user_interface->is_customer_data_bill_selection_empty() ? 0 : m_user_interface->get_customer_data_selected_bill_id());
  bool l_enable_creation = false;
  bool l_enable_modification = false;
  bool l_enable_deletion = false;

  uint32_t l_bill_book_id = m_user_interface->get_customer_bill_book_id();
  uint32_t l_bill_ref = m_user_interface->get_customer_bill_reference();
  const facture_status * l_bill_status = m_user_interface->get_customer_bill_status();

  bool l_complete = m_user_interface->is_customer_bill_date_complete() && 
    l_bill_book_id &&
    l_bill_ref &&
    l_bill_status != NULL;

  if(l_complete)
    {
      livre_facture l_bill_book ;
      m_db->get_livre_facture(l_bill_book_id,l_bill_book);
     
      std::vector<facture> l_bill_list;
      m_db->get_by_livre_facture_and_ref(l_bill_ref,l_bill_book_id,l_bill_list);

      if(l_bill_id)
	{

	  facture l_bill;
	  m_db->get_facture(l_bill_id,l_bill);
	  std::string l_date = m_user_interface->get_customer_bill_date();

	  l_enable_modification = l_date != l_bill.get_date() || 
	    l_bill_book_id != l_bill.get_livre_facture_id() ||
	    l_bill_ref != l_bill.get_facture_ref() ||
	    l_bill_status->get_id() != l_bill.get_status();
	}
      else
	{
	  assert(l_bill_list.size()==0);
	  l_enable_creation = true;
	}
    }
  if(l_bill_id)
    {
      std::vector<achat> l_purchase_list;
      m_db->get_by_facture_id(l_bill_id,l_purchase_list);
      l_enable_deletion = l_purchase_list.size()==0;
    }
  m_user_interface->set_customer_bill_creation_enabled(l_enable_creation);
  m_user_interface->set_customer_bill_modification_enabled(l_enable_modification);
  m_user_interface->set_customer_bill_deletion_enabled(l_enable_deletion);
}

//------------------------------------------------------------------------------
void fichier_client::check_livre_facture_information(void)
{

  bool l_date_ok = false;
  bool l_creation_ok = false;
  bool l_modification_ok = false;
  assert(m_user_interface);
  string l_start_date;
  string l_end_date;
  if(m_user_interface->is_livre_facture_start_date_complete() && m_user_interface->is_livre_facture_end_date_complete())
    {
      l_start_date = m_user_interface->get_livre_facture_start_date();
      l_end_date = m_user_interface->get_livre_facture_end_date();      
      if(l_start_date < l_end_date)
	{
	  l_date_ok = true;
	}
      else
	{
	  m_user_interface->display_warning_message("Problème de date","La date de fin est postérieure à la date de départ");
	}

    }

  std::string l_livre_id_str = m_user_interface->get_livre_facture_id();
  if(l_livre_id_str.length() == 6)
    {
      if(l_date_ok)
	{
	  assert(m_db);
	  livre_facture l_result ;
	  uint32_t l_livre_id = strtol(l_livre_id_str.c_str(),NULL,10);
	  uint32_t l_status = m_db->get_livre_facture(l_livre_id,l_result);
	  if(!l_status)
	    {
	      l_creation_ok = true;
	    }
	  else
	    {	   
	      if(!m_user_interface->is_livre_facture_selection_empty())
		{
		  std::pair<std::string,std::string> l_min_max_date = m_db->get_min_max_date(l_livre_id);
		  if(l_start_date > l_min_max_date.first)
		    {
		      l_date_ok = false;
		      m_user_interface->display_warning_message("Problème de date","La facture la plus ancienne du livre est plus ancienne que la nouvelle date de début : " + l_min_max_date.first + " < " + l_start_date); 
		    }
		  if(l_end_date < l_min_max_date.second)
		    {
		      l_date_ok = false;
		      m_user_interface->display_warning_message("Problème de date","La facture la plus récente du livre est plus ancienne que la nouvelle date de fin : " + l_min_max_date.second + " > " + l_end_date); 
		    }
		  l_modification_ok = (l_date_ok && (l_start_date != l_result.getStartDate() || l_end_date != l_result.getEndDate()));
		}
	    }
	  
	}
    }
  m_user_interface->set_create_livre_facture_enabled(l_creation_ok);
  m_user_interface->set_modify_livre_facture_enabled(l_modification_ok);
}

//------------------------------------------------------------------------------
void fichier_client::check_non_attributed_facture(void)
{

    if(!m_user_interface->is_list_facture_of_livre_facture_selection_empty())
    {
      m_user_interface->set_facture_creation_for_selected_livre_enabled(false);
    }

  bool l_valid = true;
  assert(m_user_interface);
  std::string l_date = "";
  if(m_user_interface->is_non_attributed_facture_date_complete())
    {
      l_date = m_user_interface->get_non_attributed_facture_date();
    }
  else if(!m_user_interface->is_non_attributed_facture_date_empty())
    {
      m_user_interface->display_warning_message("Erreur de date","Date incomplete");
      l_valid = false;
    }
  std::cout << "Date is \"" << l_date << std::endl;
  uint32_t l_livre_id = m_user_interface->get_non_attributed_facture_livre_facture_id();
  assert(m_db);
  livre_facture l_book;
  assert(m_db->get_livre_facture(l_livre_id,l_book));

  if(l_date != "" && !l_book.containsDate(l_date))
    {
      m_user_interface->display_warning_message("Erreur de date","La date ne correspond pas aux dates du livre");
      m_user_interface->clear_non_attributed_facture_date();
      l_valid = false;
    }
  uint32_t l_facture_reference = m_user_interface->get_non_attributed_facture_reference();
  std::cout << "Facture reference is " << l_facture_reference << std::endl ;
  if(l_facture_reference)
    {
      if(m_user_interface->is_list_facture_of_livre_facture_selection_empty())
	{
	  std::vector<facture> l_facture_result;
	  m_db->get_by_livre_facture_and_ref(l_facture_reference, l_livre_id,l_facture_result);
	  assert(l_facture_result.size()==0);
	}
    }
  else
    {
      l_valid = false;
    }
  const facture_status * l_status = m_user_interface->get_non_attributed_facture_status();
  std::cout << "Facture status is \"" << (l_status == NULL ? "" : l_status->get_name()) << "\"" << std::endl ;
  if(l_status == NULL)
    {
      l_valid = false;
    }
  
  const facture_reason * l_reason = m_user_interface->get_non_attributed_facture_reason();
  std::cout << "Facture reason is \"" << (l_reason == NULL ? "" : l_reason->get_name()) << "\"" << std::endl ;
  if(l_reason == NULL)
    {
      l_valid = false;
    }

  if(m_user_interface->is_list_facture_of_livre_facture_selection_empty())
    {
      m_user_interface->set_facture_creation_for_selected_livre_enabled(l_valid);  
    }
  else if(l_valid)
    {
      assert(l_status);
      assert(l_reason);
      facture l_ref_facture;
      m_db->get_facture(m_user_interface->get_list_facture_of_livre_facture_selected_id(),l_ref_facture);
      bool l_different = l_date != l_ref_facture.get_date() || 
	l_livre_id != l_ref_facture.get_livre_facture_id() || 
	l_facture_reference != l_ref_facture.get_facture_ref() || 
	l_status->get_id() != l_ref_facture.get_status() || 
	l_reason->get_id() != l_ref_facture.get_reason_id();

      m_user_interface->set_facture_modification_for_selected_livre_enabled(l_different);
    }
  else
    {
      m_user_interface->set_facture_modification_for_selected_livre_enabled(l_valid);  
    }
}

//------------------------------------------------------------------------------
void fichier_client::non_attributed_facture_date_entered(void)
{
  std::cout << "Fichier_CientEvent::non_attributed_facture_date_entered" << std::endl;
  check_non_attributed_facture();
}

//------------------------------------------------------------------------------
void fichier_client::non_attributed_facture_livre_facture_selected(void)
{
  std::cout << "Fichier_CientEvent::non_attributed_facture_livre_facture_selected" << std::endl;
  check_non_attributed_facture();
}

//------------------------------------------------------------------------------
void fichier_client::non_attributed_facture_reference_selected(void)
{
  std::cout << "Fichier_CientEvent::non_attributed_facture_reference_selected" << std::endl;
  check_non_attributed_facture();
}

//------------------------------------------------------------------------------
void fichier_client::non_attributed_facture_status_selected(void)
{
  std::cout << "Fichier_CientEvent::non_attributed_facture_status_selected" << std::endl;
  check_non_attributed_facture();
}

//------------------------------------------------------------------------------
void fichier_client::non_attributed_facture_reason_selected(void)
{
  std::cout << "Fichier_CientEvent::non_attributed_facture_reason_selected" << std::endl;
  check_non_attributed_facture();
}

//------------------------------------------------------------------------------
void fichier_client::treat_facture_selected_event(void)
{
  std::cout << "Fichier_CientEvent:: facture selected" << std::endl;
  check_non_attributed_facture();
  assert(m_user_interface);
  assert(!m_user_interface->is_list_facture_of_livre_facture_selection_empty());
  uint32_t l_facture_id = m_user_interface->get_list_facture_of_livre_facture_selected_id();

  assert(m_db);

  // Check if there are related achats
  std::vector<achat> l_related_achats;
  m_db->get_by_facture_id(l_facture_id,l_related_achats);
  std::cout << l_related_achats.size() << " related achats" << std::endl ;
  m_user_interface->set_facture_deletion_for_selected_livre_enabled(!l_related_achats.size());

  facture l_facture;
  m_db->get_facture(l_facture_id,l_facture);

  uint32_t l_client_id = l_facture.get_client_id();
  uint32_t l_reason_id = l_facture.get_reason_id();
  assert(l_reason_id || l_client_id);
  assert(l_reason_id == 0 || l_client_id == 0);
  if(l_client_id)
    {
      m_user_interface->set_facture_modification_for_selected_livre_enabled(false);      
      m_user_interface->set_non_attributed_facture_date("");
      std::vector<uint32_t> l_empty_int_vector;
      m_user_interface->set_non_attributed_facture_allowed_livre_ids(l_empty_int_vector);
      m_user_interface->set_non_attributed_allowed_facture_references(l_empty_int_vector);
      std::vector<facture_status> l_empty_vector_status;
      m_user_interface->set_non_attributed_facture_status_list(l_empty_vector_status);
      std::vector<facture_reason> l_empty_vector_reason;
      m_user_interface->set_non_attributed_facture_reason_list(l_empty_vector_reason);
    }
  else
    {
      m_user_interface->set_facture_modification_for_selected_livre_enabled(false);
      m_user_interface->set_non_attributed_facture_date(l_facture.get_date());

      uint32_t l_livre_id = l_facture.get_livre_facture_id();
      

      std::vector<uint32_t> l_livre_ids;
      l_livre_ids.push_back(l_livre_id);
      m_user_interface->set_non_attributed_facture_allowed_livre_ids(l_livre_ids);

      vector<facture> l_bill_list;
      assert(m_db);
      // Get factures related to selected livre_facture
      m_db->get_by_livre_facture(l_livre_id,l_bill_list);

      // Get livre_facture object
      livre_facture l_related_livre;
      m_db->get_livre_facture(l_livre_id,l_related_livre);
    
      // Computing remaining references
      std::vector<uint32_t> l_remaining_refs;
      get_remaining_refs(l_related_livre,l_bill_list,l_remaining_refs);
      std::vector<uint32_t>::iterator l_iter = l_remaining_refs.begin();
      std::vector<uint32_t>::iterator l_iter_end = l_remaining_refs.end();
      while(l_iter != l_iter_end && *l_iter < l_facture.get_facture_ref())
	{
	  ++l_iter;
	}
      l_remaining_refs.insert(l_iter,l_facture.get_facture_ref());
      m_user_interface->set_non_attributed_allowed_facture_references(l_remaining_refs);
      m_user_interface->set_non_attributed_facture_reference(l_facture.get_facture_ref());

      std::vector<facture_status> l_facture_status_list;
      m_db->get_all_facture_status(l_facture_status_list);
      m_user_interface->set_non_attributed_facture_status_list(l_facture_status_list);
      m_user_interface->set_non_attributed_facture_status(l_facture.get_status());
       
      std::vector<facture_reason> l_facture_reason_list;
      m_db->get_all_facture_reason(l_facture_reason_list);
      m_user_interface->set_non_attributed_facture_reason_list(l_facture_reason_list);
      m_user_interface->set_non_attributed_facture_reason(l_facture.get_reason_id());
    }
}

//------------------------------------------------------------------------------
void fichier_client::treat_no_more_facture_selected_event(void)
{
  std::cout << "Fichier_CientEvent:: no more facture selected" << std::endl;
  refresh_non_attributed_facture_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_delete_non_attributed_facture_event(void)
{
  std::cout << "Fichier_CientEvent:: delete non attributed facture required" << std::endl;
  assert(m_user_interface);
  assert(!m_user_interface->is_list_facture_of_livre_facture_selection_empty());
 
  uint32_t l_facture_id = m_user_interface->get_list_facture_of_livre_facture_selected_id();

  assert(m_db);

  facture l_facture;
  m_db->get_facture(l_facture_id,l_facture);

  // Check if there are related achats
  std::vector<achat> l_related_achats;
  m_db->get_by_facture_id(l_facture_id,l_related_achats);
  assert(!l_related_achats.size());
  m_db->remove(l_facture);
  refresh_non_attributed_facture_list();
  refresh_livre_facture_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_modify_non_attributed_facture_event(void)
{
  std::cout << "Fichier_CientEvent:: modify non attributed facture required" << std::endl;
  assert(m_user_interface);
  assert(!m_user_interface->is_livre_facture_selection_empty());

  const facture_status * l_status = m_user_interface->get_non_attributed_facture_status();
  // Create facture event is possible because all values have been checked !!
  assert(l_status);
  const facture_reason * l_reason = m_user_interface->get_non_attributed_facture_reason();
  assert(l_reason);
  std::string l_date = (m_user_interface->is_non_attributed_facture_date_complete() ? m_user_interface->get_non_attributed_facture_date() : "");

  facture l_facture;
  m_db->get_facture(m_user_interface->get_list_facture_of_livre_facture_selected_id(),l_facture);

  l_facture.set_date(l_date);
  l_facture.set_livre_facture_id(m_user_interface->get_non_attributed_facture_livre_facture_id());
  l_facture.set_facture_ref(m_user_interface->get_non_attributed_facture_reference());
  l_facture.set_status(l_status->get_id());
  l_facture.set_reason_id(l_reason->get_id());

  m_db->update(l_facture);
  
  m_user_interface->set_facture_modification_for_selected_livre_enabled(false);

  refresh_non_attributed_facture_list();


}

// Facture status related events
//--------------------------------
//------------------------------------------------------------------------------
void fichier_client::treat_no_more_facture_status_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::no more facture_status selected" << std::endl;
  assert(m_user_interface);
  m_user_interface->set_delete_facture_status_enabled(false);
  m_user_interface->set_modify_facture_status_enabled(false);
}

//------------------------------------------------------------------------------
void fichier_client::treat_delete_facture_status_event(void)
{
  std::cout << "Fichier_ClientEvent::facture status delete event" << std::endl;
  assert(m_user_interface);
  uint32_t l_facture_status_id = m_user_interface->get_selected_facture_status_id();
  
  // Get selected status
  assert(m_db);
  facture_status l_facture_status;
  m_db->get_facture_status(l_facture_status_id,l_facture_status);

  // Check this is not predefined status
  string l_name = l_facture_status.get_name();
  assert(l_name != facture_status::get_ok_status() && l_name != facture_status::get_non_checked_status());

  // Check that no facture are using this status
  std::vector<facture> l_facture_list;
  m_db->get_by_status(l_facture_status_id,l_facture_list);
  assert(l_facture_list.size()==0);

  // Remove the status
  m_db->remove(l_facture_status);

  // Update user interface
  m_user_interface->set_delete_facture_status_enabled(false);
  m_user_interface->set_modify_facture_status_enabled(false);
  m_user_interface->clear_facture_status_information();
  refresh_facture_status_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_modify_facture_status_event(void)
{
  std::cout << "Fichier_ClientEvent::facture status modify event" << std::endl;
  assert(m_user_interface);
  uint32_t l_facture_status_id = m_user_interface->get_selected_facture_status_id();
  
  // Get selected status
  assert(m_db);
  facture_status l_facture_status;
  m_db->get_facture_status(l_facture_status_id,l_facture_status);

  // Check this is not predefined status
  string l_name = l_facture_status.get_name();
  assert(l_name != facture_status::get_ok_status() && l_name != facture_status::get_non_checked_status());

  m_user_interface->set_delete_facture_status_enabled(false);
  if(!m_facture_status_pending_modif)
    {
      m_facture_status_pending_modif = true;
      m_user_interface->set_facture_status_name(l_facture_status.get_name());
      m_user_interface->set_modify_facture_status_action_name("Annuler");
      m_user_interface->set_facture_status_list_enabled(false);
      m_user_interface->set_create_facture_status_enabled(false);
    }
  else
    {
      m_user_interface->set_facture_status_list_enabled(true);
      m_user_interface->set_modify_facture_status_action_name("Modifier");
      std::string l_new_name = m_user_interface->get_facture_status_name();
      assert(l_new_name != "");

      if(l_new_name != l_name)
	{
	  std::vector<facture_status> l_list_facture_status;
	  m_db->get_facture_status_by_name(l_new_name,l_list_facture_status,true);
	  assert(l_list_facture_status.size()==0);
      
	  l_facture_status.set_name(l_new_name);
	  m_db->update(l_facture_status);
	}
      m_facture_status_pending_modif = false;
      
      m_user_interface->clear_facture_status_information();
      refresh_facture_status_list();
    }
}

//------------------------------------------------------------------------------
void fichier_client::treat_facture_status_name_modif_event(void)
{
  std::cout << "Fichier_ClientEvent::facture status name modify event" << std::endl;
  assert(m_user_interface);
  std::string l_status_name = m_user_interface->get_facture_status_name();

  vector<facture_status> l_facture_status_list;
  vector<facture_status> l_facture_status_filtered_list;
  assert(m_db);
  if(l_status_name != "")
    {
      m_db->get_facture_status_by_name(l_status_name,l_facture_status_list,true);
      m_db->get_facture_status_by_name(l_status_name,l_facture_status_filtered_list,false);
    }
  else
    {
      m_db->get_all_facture_status(l_facture_status_filtered_list);
    }

  if(!m_facture_status_pending_modif)
    {
      m_user_interface->set_create_facture_status_enabled(l_facture_status_list.size()==0 && l_status_name != "");
      m_user_interface->refresh_facture_status_list(l_facture_status_filtered_list);
    }
  else
    {
      facture_status l_facture_status;
      uint32_t l_facture_status_id = m_user_interface->get_selected_facture_status_id();
      m_db->get_facture_status(l_facture_status_id,l_facture_status);
      
      bool l_name_ok = l_status_name != "" && l_status_name != facture_status::get_ok_status() && l_status_name != facture_status::get_non_checked_status();
      m_user_interface->set_modify_facture_status_action_name(l_facture_status.get_name() != l_status_name ? "Appliquer" : "Annuler");

      // Check if modified value is acceptable
      if(l_name_ok)
	{
	  l_name_ok = l_facture_status_list.size() == 0 || l_facture_status.get_name() == l_status_name ;
	}
      m_user_interface->set_modify_facture_status_enabled(l_name_ok);
    }  
 
}

//------------------------------------------------------------------------------
void fichier_client::treat_create_facture_status_event(void)
{
  std::cout << "Fichier_ClientEvent::facture status name create event" << std::endl;
  assert(m_user_interface);
  std::string l_status_name = m_user_interface->get_facture_status_name();
   
  assert(m_db);
  std::vector<facture_status> l_already;
  m_db->get_facture_status_by_name(l_status_name,l_already,true);
  assert(l_already.size()==0);

  m_user_interface->set_create_facture_status_enabled(false);
  facture_status l_status(l_status_name);

  m_db->create(l_status);
  m_user_interface->clear_facture_status_information();
  refresh_facture_status_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_facture_status_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::facture status selection event" << std::endl;  
  assert(m_user_interface);

  uint32_t l_facture_status_id = m_user_interface->get_selected_facture_status_id();
  
  // Get selected status
  assert(m_db);
  facture_status l_facture_status;
  m_db->get_facture_status(l_facture_status_id,l_facture_status);

  string l_name = l_facture_status.get_name();
   
  if(l_name != facture_status::get_ok_status() && l_name != facture_status::get_non_checked_status())
    {
      // Check that no facture are using this status
      std::vector<facture> l_facture_list;
      m_db->get_by_status(l_facture_status_id,l_facture_list);
      m_user_interface->set_delete_facture_status_enabled(l_facture_list.size()==0);
      m_user_interface->set_modify_facture_status_enabled(true);
    }
  else
    {
      m_user_interface->set_delete_facture_status_enabled(false);
      m_user_interface->set_modify_facture_status_enabled(false);
    }
   

}


//------------------------------------------------------------------------------
void fichier_client::refresh_facture_status_list(void)
{
  vector<facture_status> l_facture_status_list;
  assert(m_db);
  m_db->get_all_facture_status(l_facture_status_list);
  assert(m_user_interface);
  m_user_interface->refresh_facture_status_list(l_facture_status_list);
  refresh_non_attributed_facture_status_list();
  refresh_customer_data_facture_status_list();
}






// Facture reason related events
//--------------------------------
//------------------------------------------------------------------------------
void fichier_client::treat_facture_reason_name_modif_event(void)
{
  std::cout << "Fichier_ClientEvent::facture reason name modify event" << std::endl;
  assert(m_user_interface);
  std::string l_reason_name = m_user_interface->get_facture_reason_name();

  vector<facture_reason> l_facture_reason_list;
  vector<facture_reason> l_facture_reason_filtered_list;
  assert(m_db);
  if(l_reason_name != "")
    {
      m_db->get_facture_reason_by_name(l_reason_name,l_facture_reason_list,true);
      m_db->get_facture_reason_by_name(l_reason_name,l_facture_reason_filtered_list,false);
    }
  else
    {
      m_db->get_all_facture_reason(l_facture_reason_filtered_list);
    }

  if(!m_facture_reason_pending_modif)
    {
      m_user_interface->set_create_facture_reason_enabled(l_facture_reason_list.size()==0 && l_reason_name != "");
      m_user_interface->refresh_facture_reason_list(l_facture_reason_filtered_list);
    }
  else
    {
      facture_reason l_facture_reason;
      uint32_t l_facture_reason_id = m_user_interface->get_selected_facture_reason_id();
      m_db->get_facture_reason(l_facture_reason_id,l_facture_reason);
      
      bool l_name_ok = l_reason_name != "";
      m_user_interface->set_modify_facture_reason_action_name(l_facture_reason.get_name() != l_reason_name ? "Appliquer" : "Annuler");

      // Check if modified value is acceptable
      if(l_name_ok)
	{
	  l_name_ok = l_facture_reason_list.size()==0 || l_facture_reason.get_name() == l_reason_name ;
	}
      m_user_interface->set_modify_facture_reason_enabled(l_name_ok);
    }  
 
}

//------------------------------------------------------------------------------
void fichier_client::treat_facture_reason_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::facture reason selection event" << std::endl;  
  assert(m_user_interface);

  uint32_t l_facture_reason_id = m_user_interface->get_selected_facture_reason_id();
  
  // Get selected reason
  assert(m_db);
  facture_reason l_facture_reason;
  m_db->get_facture_reason(l_facture_reason_id,l_facture_reason);

  // Check that no facture are using this reason
  std::vector<facture> l_facture_list;
  m_db->get_by_reason(l_facture_reason_id,l_facture_list);
  m_user_interface->set_delete_facture_reason_enabled(l_facture_list.size()==0);
  m_user_interface->set_modify_facture_reason_enabled(true);
}

//------------------------------------------------------------------------------
void fichier_client::treat_no_more_facture_reason_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::no more facture_reason selected" << std::endl;
  assert(m_user_interface);
  m_user_interface->set_delete_facture_reason_enabled(false);
  m_user_interface->set_modify_facture_reason_enabled(false);
}

//------------------------------------------------------------------------------
void fichier_client::treat_create_facture_reason_event(void)
{
  std::cout << "Fichier_ClientEvent::facture reason name create event" << std::endl;
  assert(m_user_interface);
  std::string l_reason_name = m_user_interface->get_facture_reason_name();
   
  assert(m_db);
  std::vector<facture_reason> l_already;
  m_db->get_facture_reason_by_name(l_reason_name,l_already,true);
  assert(l_already.size()==0);

  m_user_interface->set_create_facture_reason_enabled(false);
  facture_reason l_reason(l_reason_name);

  m_db->create(l_reason);
  m_user_interface->clear_facture_reason_information();
  refresh_facture_reason_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_modify_facture_reason_event(void)
{
  std::cout << "Fichier_ClientEvent::facture reason modify event" << std::endl;
  assert(m_user_interface);
  uint32_t l_facture_reason_id = m_user_interface->get_selected_facture_reason_id();
  
  // Get selected reason
  assert(m_db);
  facture_reason l_facture_reason;
  m_db->get_facture_reason(l_facture_reason_id,l_facture_reason);

  string l_name = l_facture_reason.get_name();

  m_user_interface->set_delete_facture_reason_enabled(false);
  if(!m_facture_reason_pending_modif)
    {
      m_facture_reason_pending_modif = true;
      m_user_interface->set_facture_reason_name(l_facture_reason.get_name());
      m_user_interface->set_modify_facture_reason_action_name("Annuler");
      m_user_interface->set_facture_reason_list_enabled(false);
      m_user_interface->set_create_facture_reason_enabled(false);
    }
  else
    {
      m_user_interface->set_facture_reason_list_enabled(true);
      m_user_interface->set_modify_facture_reason_action_name("Modifier");
      std::string l_new_name = m_user_interface->get_facture_reason_name();
      assert(l_new_name != "");

      if(l_new_name != l_name)
	{
	  std::vector<facture_reason> l_list_facture_reason;
	  m_db->get_facture_reason_by_name(l_new_name,l_list_facture_reason,true);
	  assert(l_list_facture_reason.size()==0);
      
	  l_facture_reason.set_name(l_new_name);
	  m_db->update(l_facture_reason);
	}
      m_facture_reason_pending_modif = false;
      
      m_user_interface->clear_facture_reason_information();
      refresh_facture_reason_list();
    }
}

//------------------------------------------------------------------------------
void fichier_client::treat_delete_facture_reason_event(void)
{
  std::cout << "Fichier_ClientEvent::facture reason delete event" << std::endl;
  assert(m_user_interface);
  uint32_t l_facture_reason_id = m_user_interface->get_selected_facture_reason_id();
  
  // Get selected reason
  assert(m_db);
  facture_reason l_facture_reason;
  m_db->get_facture_reason(l_facture_reason_id,l_facture_reason);

  // Check that no facture are using this reason
  std::vector<facture> l_facture_list;
  m_db->get_by_reason(l_facture_reason_id,l_facture_list);
  assert(l_facture_list.size()==0);

  // Remove the reason
  m_db->remove(l_facture_reason);

  // Update user interface
  m_user_interface->set_delete_facture_reason_enabled(false);
  m_user_interface->set_modify_facture_reason_enabled(false);
  m_user_interface->clear_facture_reason_information();
  refresh_facture_reason_list();
}

// Brand information related events
//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_brand_name_modif_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration brand name modif event" << std::endl;  
  assert(m_user_interface);
  std::string l_brand_name = m_user_interface->get_purchase_configuration_brand_name();

  vector<marque> l_brand_list;
  vector<marque> l_brand_filtered_list;
  assert(m_db);
  if(l_brand_name != "")
    {
      m_db->get_marque_by_name(l_brand_name,l_brand_list,true);
      m_db->get_marque_by_name(l_brand_name,l_brand_filtered_list,false);
    }
  else
    {
      m_db->get_all_marque(l_brand_filtered_list);
    }

  if(!m_purchase_brand_pending_modif)
    {
      m_user_interface->set_purchase_configuration_create_brand_enabled(l_brand_list.size()==0 && l_brand_name != "");
      m_user_interface->set_purchase_configuration_brand_list(l_brand_filtered_list);
    }
  else
    {
      marque l_brand;
      uint32_t l_brand_id = m_user_interface->get_purchase_configuration_selected_brand_id();
      m_db->get_marque(l_brand_id,l_brand);
      
      m_user_interface->set_purchase_configuration_modify_brand_action_name(l_brand.get_name() != l_brand_name ? "Appliquer" : "Annuler");

      // Check if modified value is acceptable
      bool l_name_ok = l_brand_name != "" && (l_brand_list.size() == 0 || l_brand.get_name() == l_brand_name); 
      m_user_interface->set_purchase_configuration_modify_brand_enabled(l_name_ok);
    }  
}

// Brand list related events
//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_brand_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration brand selection event" << std::endl;  
  assert(m_user_interface);

  uint32_t l_brand_id = m_user_interface->get_purchase_configuration_selected_brand_id();
  
  // Get selected status
  assert(m_db); 
  marque l_brand;
  m_db->get_marque(l_brand_id,l_brand);

  std::string l_name = l_brand.get_name();
   
  // Check that no facture are using this status
  std::vector<achat> l_purchase_list;
  m_db->get_by_brand_id(l_brand_id,l_purchase_list);
  m_user_interface->set_purchase_configuration_delete_brand_enabled(l_purchase_list.size()==0);
  m_user_interface->set_purchase_configuration_modify_brand_enabled(true);
}

//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_no_more_brand_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration no more brand selected event" << std::endl;  
  assert(m_user_interface);
  m_user_interface->set_purchase_configuration_delete_brand_enabled(false);
  m_user_interface->set_purchase_configuration_modify_brand_enabled(false);
}

// Brand actions related events
//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_create_brand_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration brand creation event" << std::endl;  
  assert(m_user_interface);
  std::string l_brand_name = m_user_interface->get_purchase_configuration_brand_name();
   
  assert(m_db);
  std::vector<marque> l_already;
  m_db->get_marque_by_name(l_brand_name,l_already,true);
  assert(l_already.size()==0);

  m_user_interface->set_purchase_configuration_create_brand_enabled(false);
  marque l_brand(l_brand_name);

  m_db->create(l_brand);
  m_user_interface->clear_purchase_configuration_brand_information();
  refresh_brand_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_delete_brand_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration brand deletion event" << std::endl;  
  assert(m_user_interface);
  uint32_t l_brand_id = m_user_interface->get_purchase_configuration_selected_brand_id();
  
  // Get selected reason
  assert(m_db);
  marque l_brand;
  m_db->get_marque(l_brand_id,l_brand);

  // Check that no purchase are using this brand
  std::vector<achat> l_purchase_list;
  m_db->get_by_brand_id(l_brand_id,l_purchase_list);
  assert(l_purchase_list.size()==0);

  // Remove the reason
  m_db->remove(l_brand);

  // Update user interface
  m_user_interface->set_purchase_configuration_delete_brand_enabled(false);
  m_user_interface->set_purchase_configuration_modify_brand_enabled(false);
  m_user_interface->clear_purchase_configuration_brand_information();
  refresh_brand_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_modify_brand_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration brand modification event" << std::endl;  
  assert(m_user_interface);

  uint32_t l_brand_id = m_user_interface->get_purchase_configuration_selected_brand_id();
  
  // Get selected brand
  assert(m_db);
  marque l_brand;
  m_db->get_marque(l_brand_id,l_brand);

  // Check this is not predefined brand
  string l_name = l_brand.get_name();

  m_user_interface->set_purchase_configuration_delete_brand_enabled(false);
  if(!m_purchase_brand_pending_modif)
    {
      m_purchase_brand_pending_modif = true;
      m_user_interface->set_purchase_configuration_brand_name(l_brand.get_name());
      m_user_interface->set_purchase_configuration_modify_brand_action_name("Annuler");
      m_user_interface->set_purchase_configuration_brand_list_enabled(false);
      m_user_interface->set_purchase_configuration_create_brand_enabled(false);     

    }
  else
    {
      m_user_interface->set_purchase_configuration_brand_list_enabled(true);
      m_user_interface->set_purchase_configuration_modify_brand_action_name("Modifier");
      std::string l_new_name = m_user_interface->get_purchase_configuration_brand_name();
      assert(l_new_name != "");

      if(l_new_name != l_name)
	{
	  std::vector<marque> l_list_brand;
	  m_db->get_marque_by_name(l_new_name,l_list_brand,true);
	  assert(l_list_brand.size()==0);
      
	  l_brand.set_name(l_new_name);
	  m_db->update(l_brand);
	}
      m_purchase_brand_pending_modif = false;
      
      m_user_interface->clear_purchase_configuration_brand_information();
      refresh_brand_list();
    }

}

// Type information related events
//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_type_name_modif_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration type name modif event" << std::endl;  
  assert(m_user_interface);
  std::string l_type_name = m_user_interface->get_purchase_configuration_type_name();

  vector<type_achat> l_type_list;
  vector<type_achat> l_type_filtered_list;
  assert(m_db);
  if(l_type_name != "")
    {
      m_db->get_type_achat_by_name(l_type_name,l_type_list,true);
      m_db->get_type_achat_by_name(l_type_name,l_type_filtered_list,false);
    }
  else
    {
      m_db->get_all_type_achat(l_type_filtered_list);
    }

  if(!m_purchase_type_pending_modif)
    {
      m_user_interface->set_purchase_configuration_create_type_enabled(l_type_list.size()==0 && l_type_name != "");
      m_user_interface->set_purchase_configuration_type_list(l_type_filtered_list);
    }
  else
    {
      type_achat l_type;
      uint32_t l_type_id = m_user_interface->get_purchase_configuration_selected_type_id();
      m_db->get_type_achat(l_type_id,l_type);
      
      m_user_interface->set_purchase_configuration_modify_type_action_name(l_type.get_name() != l_type_name ? "Appliquer" : "Annuler");

      // Check if modified value is acceptable
      bool l_name_ok = l_type_name != "" && (l_type_list.size() == 0 || l_type.get_name() == l_type_name);
      m_user_interface->set_purchase_configuration_modify_type_enabled(l_name_ok);
    }  
}

// type list related events
//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_type_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration type selection event" << std::endl;  
  assert(m_user_interface);

  uint32_t l_type_id = m_user_interface->get_purchase_configuration_selected_type_id();
  
  // Get selected status
  assert(m_db); 
  type_achat l_type;
  m_db->get_type_achat(l_type_id,l_type);

  std::string l_name = l_type.get_name();
   
  // Check that no facture are using this status
  std::vector<achat> l_purchase_list;
  m_db->get_by_type_id(l_type_id,l_purchase_list);
  m_user_interface->set_purchase_configuration_delete_type_enabled(l_purchase_list.size()==0);
  m_user_interface->set_purchase_configuration_modify_type_enabled(true);
}

//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_no_more_type_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration no more type selected event" << std::endl;  
  assert(m_user_interface);
  m_user_interface->set_purchase_configuration_delete_type_enabled(false);
  m_user_interface->set_purchase_configuration_modify_type_enabled(false);
}

// Type actions related events
//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_create_type_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration type creation event" << std::endl;  
  assert(m_user_interface);
  std::string l_type_name = m_user_interface->get_purchase_configuration_type_name();
   
  assert(m_db);
  std::vector<type_achat> l_already;
  m_db->get_type_achat_by_name(l_type_name,l_already,true);
  assert(l_already.size()==0);

  m_user_interface->set_purchase_configuration_create_type_enabled(false);
  type_achat l_type(l_type_name);

  m_db->create(l_type);
  m_user_interface->clear_purchase_configuration_type_information();
  refresh_purchase_type_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_delete_type_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration type deletion event" << std::endl;  
  assert(m_user_interface);
  uint32_t l_type_id = m_user_interface->get_purchase_configuration_selected_type_id();
  
  // Get selected reason
  assert(m_db);
  type_achat l_type;
  m_db->get_type_achat(l_type_id,l_type);

  // Check that no purchase are using this type
  std::vector<achat> l_purchase_list;
  m_db->get_by_type_id(l_type_id,l_purchase_list);
  assert(l_purchase_list.size()==0);

  // Remove the reason
  m_db->remove(l_type);

  // Update user interface
  m_user_interface->set_purchase_configuration_delete_type_enabled(false);
  m_user_interface->set_purchase_configuration_modify_type_enabled(false);
  m_user_interface->clear_purchase_configuration_type_information();
  refresh_purchase_type_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_purchase_configuration_modify_type_event(void)
{
  std::cout << "Fichier_ClientEvent::purchase_configuration type modification event" << std::endl;  
  assert(m_user_interface);
  uint32_t l_type_id = m_user_interface->get_purchase_configuration_selected_type_id();
  
  // Get selected type
  assert(m_db);
  type_achat l_type;
  m_db->get_type_achat(l_type_id,l_type);

  // Check this is not predefined type
  string l_name = l_type.get_name();

  m_user_interface->set_purchase_configuration_delete_type_enabled(false);
  if(!m_purchase_type_pending_modif)
    {
      m_purchase_type_pending_modif = true;
      m_user_interface->set_purchase_configuration_type_name(l_type.get_name());
      m_user_interface->set_purchase_configuration_modify_type_action_name("Annuler");
      m_user_interface->set_purchase_configuration_type_list_enabled(false);
      m_user_interface->set_purchase_configuration_create_type_enabled(false);     
    }
  else
    {
      m_user_interface->set_purchase_configuration_type_list_enabled(true);
      m_user_interface->set_purchase_configuration_modify_type_action_name("Modifier");
      std::string l_new_name = m_user_interface->get_purchase_configuration_type_name();
      assert(l_new_name != "");

      if(l_new_name != l_name)
	{
	  std::vector<type_achat> l_list_type;
	  m_db->get_type_achat_by_name(l_new_name,l_list_type,true);
	  assert(l_list_type.size()==0);
      
	  l_type.set_name(l_new_name);
	  m_db->update(l_type);
	}
      m_purchase_type_pending_modif = false;
      
      m_user_interface->clear_purchase_configuration_type_information();
      refresh_purchase_type_list();
    }

}

//------------------------------------------------------------------------------
void fichier_client::refresh_facture_reason_list(void)
{
  vector<facture_reason> l_facture_reason_list;
  assert(m_db);
  m_db->get_all_facture_reason(l_facture_reason_list);
  assert(m_user_interface);
  m_user_interface->refresh_facture_reason_list(l_facture_reason_list);
  refresh_non_attributed_facture_reason_list();
}

//------------------------------------------------------------------------------
void fichier_client::refresh_city_list(void)
{
  assert(m_db);
  vector<ville> l_cities;
  m_db->get_all_ville(l_cities);
  m_user_interface->set_city_list(l_cities);
}


//------------------------------------------------------------------------------
void fichier_client::refresh_customer_data_facture_status_list(void)
{
  vector<facture_status> l_facture_status_list;
  assert(m_db);
  m_db->get_all_facture_status(l_facture_status_list);
  assert(m_user_interface);
  m_user_interface->set_customer_facture_status_list(l_facture_status_list);
}

//------------------------------------------------------------------------------
void fichier_client::refresh_customer_data_purchase_actions(void)
{
  bool l_purchase_creation_enabled = false;
  bool l_purchase_modification_enabled = false;
  bool l_purchase_deletion_enabled = false;
  assert(m_user_interface);

  // Getting information for current purchase
  const type_achat * l_purchase_type = m_user_interface->get_customer_purchase_type();
  const marque * l_purchase_brand = m_user_interface->get_customer_purchase_brand();
  const std::string l_purchase_reference = m_user_interface->get_customer_purchase_reference();
  double l_purchase_franc_price = strtod(m_user_interface->get_customer_purchase_franc_price().c_str(),NULL);
  double l_purchase_euro_price = strtod(m_user_interface->get_customer_purchase_euro_price().c_str(),NULL);
  bool l_purchase_warranty = m_user_interface->is_customer_purchase_warranty_selected();

  bool l_complete = l_purchase_type && l_purchase_brand && l_purchase_reference != "" && m_user_interface->get_customer_purchase_franc_price() != "" && m_user_interface->get_customer_purchase_euro_price() != "";

  if(m_user_interface->is_customer_data_purchase_selection_empty())
    {
      l_purchase_creation_enabled = l_complete;
    }
  else
    {
      if(l_complete)
	{
	  uint32_t l_purchase_id = m_user_interface->get_customer_data_selected_purchase_id();
	  assert(m_db);
	  achat l_purchase;
	  uint32_t l_result = m_db->get_achat(l_purchase_id,l_purchase);
	  assert(l_result == 1);
	  l_purchase_modification_enabled = l_purchase.get_marque_id() != l_purchase_brand->get_id() ||
	    l_purchase.get_type_id() != l_purchase_type->get_id() ||
	    l_purchase.get_reference() != l_purchase_reference ||
	    l_purchase.get_prix_franc() != l_purchase_franc_price ||
	    l_purchase.get_prix_euro() != l_purchase_euro_price ||
	    l_purchase.get_garantie() != l_purchase_warranty;
	}
      l_purchase_deletion_enabled = true;
    }


  m_user_interface->set_customer_purchase_creation_enabled(l_purchase_creation_enabled);
  m_user_interface->set_customer_purchase_modification_enabled(l_purchase_modification_enabled);
  m_user_interface->set_customer_purchase_deletion_enabled(l_purchase_deletion_enabled);
}


// City information related events
//------------------------------------------------------------------------------
void fichier_client::treat_city_criteria_modification_event(void)
{
  std::cout << "Fichier_ClientEvent::city criteria modification event" << std::endl;

  assert(m_user_interface);
  std::string l_city_name = m_user_interface->get_city_name();
  std::string l_postal_code = m_user_interface->get_city_postal_code();
  
  std::vector<ville> l_city_list;
  std::vector<ville> l_city_filtered_list;

  assert(m_db);
  if(l_city_name != "")
    {
      m_db->get_ville_by_name(l_city_name,l_city_list,true);
    }
  if(l_city_name != "" || l_postal_code != "")
    {
      m_db->get_city_by_name_and_postal_code(l_city_name,l_postal_code,l_city_filtered_list);
    }
  else
    {
      m_db->get_all_ville(l_city_filtered_list);
    }

  if(!m_city_pending_modif)
    {
      m_user_interface->set_create_city_enabled(l_city_list.size()==0 && l_city_name != "" && m_user_interface->is_city_postal_code_complete());
      m_user_interface->set_city_list(l_city_filtered_list);
    }
  else
    {
      uint32_t l_city_id = m_user_interface->get_selected_city_id();
      ville l_city;
      uint32_t l_result = m_db->get_ville(l_city_id,l_city);
      assert(l_result);
      m_user_interface->set_modify_city_action_name(l_city.get_name() != l_city_name ? "Appliquer" : "Annuler");
      
      m_user_interface->set_modify_city_enabled(l_city_name != "" && m_user_interface->is_city_postal_code_complete() && (l_city_list.size()==0 || l_city.get_name() == l_city_name));
    }
}

// City list related events
//------------------------------------------------------------------------------
void fichier_client::treat_city_selection_event(void)
{
  std::cout << "Fichier_ClientEvent::city selection event" << std::endl;
  assert(m_user_interface);
  assert(!m_user_interface->is_city_selection_empty());

  uint32_t l_city_id = m_user_interface->get_selected_city_id();

  // Get selected city
  assert(m_db);
  ville l_city;
  m_db->get_ville(l_city_id,l_city);

  // Check that no customer are living in this city
  std::vector<client> l_customer_list;
  m_db->get_customer_by_city(l_city_id,l_customer_list);

  m_user_interface->set_delete_city_enabled(l_customer_list.size()==0);
  m_user_interface->set_modify_city_enabled(true);
}

//------------------------------------------------------------------------------
void fichier_client::treat_city_no_more_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::no more city selected event" << std::endl;
  assert(m_user_interface);
  m_user_interface->clear_city_information();
  m_user_interface->set_delete_city_enabled(false);
  m_user_interface->set_modify_city_enabled(false);
}

  // City action related events
//------------------------------------------------------------------------------
void fichier_client::treat_city_create_event(void)
{
  std::cout << "Fichier_ClientEvent::city creation event" << std::endl;
  assert(m_user_interface);
  std::string l_city_name = m_user_interface->get_city_name();
  std::string l_postal_code = m_user_interface->get_city_postal_code();

  assert(m_db);
  
  std::vector<ville> l_already;
  m_db->get_ville_by_name(l_city_name,l_already,true);
  assert(l_already.size()==0);

  m_user_interface->set_create_city_enabled(false);
  ville l_city(l_city_name,l_postal_code);

  m_db->create(l_city);
  m_user_interface->clear_city_information();
  refresh_city_list();
}

//------------------------------------------------------------------------------
void fichier_client::treat_city_modify_event(void)
{
  std::cout << "Fichier_ClientEvent::city modification event" << std::endl;
  assert(m_user_interface);
  uint32_t l_city_id = m_user_interface->get_selected_city_id();

  // Get selected city
  assert(m_db);
  ville l_city;
  m_db->get_ville(l_city_id,l_city);

  string l_name = l_city.get_name();
  string l_postal_code = l_city.get_postal_code();

  m_user_interface->set_delete_city_enabled(false);

  if(!m_city_pending_modif)
    {
      m_city_pending_modif = true;
      m_user_interface->set_city_name(l_name);
      m_user_interface->set_city_postal_code(l_postal_code);
      m_user_interface->set_modify_city_action_name("Annuler");
      m_user_interface->set_city_list_enabled(false);
      m_user_interface->set_create_city_enabled(false);
    }
  else
    {
      m_user_interface->set_city_list_enabled(true);
      m_user_interface->set_modify_city_action_name("Modifier");
      std::string l_new_name = m_user_interface->get_city_name();
      assert(l_new_name != "");
      std::string l_new_postal_code = m_user_interface->get_city_postal_code();
      assert(m_user_interface->is_city_postal_code_complete());
      if(l_new_name != l_name || l_new_postal_code != l_postal_code)
	{
	  std::vector<ville> l_list_ville;
	  m_db->get_ville_by_name(l_new_name,l_list_ville,true);
	  assert(l_list_ville.size()==0);
      
	  l_city.set_name(l_new_name);
	  l_city.set_postal_code(l_new_postal_code);
	  m_db->update(l_city);
	}
      m_city_pending_modif = false;
      
      m_user_interface->clear_city_information();
      refresh_city_list();
    }
}

//------------------------------------------------------------------------------
void fichier_client::treat_city_delete_event(void)
{
  std::cout << "Fichier_ClientEvent::city deletion event" << std::endl;
  assert(m_user_interface);
  uint32_t l_city_id = m_user_interface->get_selected_city_id();
  
  // Get selected reason
  assert(m_db);
  ville l_city;
  m_db->get_ville(l_city_id,l_city);

  // Check that no customer are living in this city
  std::vector<client> l_customer_list;
  m_db->get_customer_by_city(l_city_id,l_customer_list);
  assert(l_customer_list.size()==0);

  // Remove the reason
  m_db->remove(l_city);

  // Update user interface
  m_user_interface->set_delete_city_enabled(false);
  m_user_interface->set_modify_city_enabled(false);
  m_user_interface->clear_city_information();
  refresh_city_list();
}

//------------------------------------------------------------------------------
bool fichier_client::need_save(void)const
{
  return (m_db ? m_db->is_modified():false);
}

//------------------------------------------------------------------------------
void fichier_client::treat_check_db_coherency_event(void)
{
  std::cout << "Fichier_clientEvent::check db coherency event" << std::endl ;
  assert(m_user_interface);
  m_user_interface->set_coherency_report_launch_check_enabled(false);
  assert(m_db);
  uint32_t l_nb_error = 0;
  uint32_t l_nb_warning = 0;
  std::vector<coherency_report_item> l_error_list;
  std::vector<coherency_report_item> l_warning_list;

  m_db->check_db_coherency(l_nb_error,l_nb_warning,l_error_list,l_warning_list);

  m_user_interface->set_coherency_report_error_number(l_nb_error);
  m_user_interface->set_coherency_report_error_list(l_error_list);
  m_user_interface->set_coherency_report_warning_number(l_nb_warning);
  m_user_interface->set_coherency_report_warning_list(l_warning_list);
  m_user_interface->set_coherency_report_launch_check_enabled(true);
}

//------------------------------------------------------------------------------
void fichier_client::remove_tmp_db(void)
{
  unlink(m_tmp_db_name.c_str());
}

//------------------------------------------------------------------------------
void fichier_client::open_tmp_db(void)
{
  assert(m_db==NULL);
  m_db = new fichier_client_db(m_tmp_db_name);
}

//------------------------------------------------------------------------------
void fichier_client::close_tmp_db(void)
{
  delete m_db;
  m_db = NULL;
}

//------------------------------------------------------------------------------
void fichier_client::save(void)
{
  assert(m_db_name != "");
  save_as(m_db_name);
}

//------------------------------------------------------------------------------
void fichier_client::save_as(const std::string & p_name)
{
  assert(m_db!=NULL);
  m_db->save();
  close_tmp_db();
  copy(m_tmp_db_name,p_name);
  m_db_name = p_name;
  open_tmp_db();
}

//------------------------------------------------------------------------------
void fichier_client::copy(const std::string & p_src,const std::string & p_dest)
{
  ifstream l_input_file(p_src.c_str(),ios::in | ios::binary);
  if(!l_input_file.is_open())
    {
      cout << "ERROR : Unable to open input file \"" << p_src << "\"" << endl ;
      exit(-1);
    }

  ofstream l_output_file(p_dest.c_str(),ios::out | ios::binary);
  if(!l_output_file.is_open())
    {
      cout << "ERROR : Unable to open output file \"" << p_dest << "\"" << endl ;
      exit(-1);
    }
  const uint32_t l_size = 100000;
  char l_buffer[l_size];
  bool l_end = false;
  while(!(l_end = l_input_file.eof()))
    {
      l_input_file.read(l_buffer,l_size);
      l_output_file.write(l_buffer,l_input_file.gcount());
    }
  l_input_file.close();
  l_output_file.close();
}

//------------------------------------------------------------------------------
void fichier_client::refresh_brand_list(void)
{
  // Get list from db
  std::vector<marque> l_brand_list;
  assert(m_db);
  m_db->get_all_marque(l_brand_list);
  
  // Update user interface
  assert(m_user_interface);
  m_user_interface->set_customer_purchase_brand_list(l_brand_list);
  m_user_interface->set_purchase_configuration_brand_list(l_brand_list);
}

//------------------------------------------------------------------------------
void fichier_client::refresh_purchase_type_list(void)
{
  // Get list from db
  std::vector<type_achat> l_purchase_type_list;
  assert(m_db);
  m_db->get_all_type_achat(l_purchase_type_list);
  
  // Update user interface
  assert(m_user_interface);
  m_user_interface->set_customer_purchase_type_list(l_purchase_type_list);
  m_user_interface->set_purchase_configuration_type_list(l_purchase_type_list);
}


//------------------------------------------------------------------------------
void fichier_client::get_facture_by_livre_facture_id(uint32_t p_livre_facture_id,
						     std::vector<search_facture_client_item> & p_result)
{
  assert(m_db);
  std::vector<search_facture_item> l_facture_result;
  m_db->get_facture_by_livre_facture_id(p_livre_facture_id,l_facture_result);

  std::vector<search_facture_item>::const_iterator l_iter = l_facture_result.begin();
  std::vector<search_facture_item>::const_iterator l_iter_end = l_facture_result.end();
  while(l_iter != l_iter_end)
    {
      uint32_t l_client_id = l_iter->get_client_id();
      uint32_t l_reason_id = l_iter->get_reason_id();
      assert((l_client_id && !l_reason_id) || (!l_client_id && l_reason_id));
      if(l_client_id)
	{
	  search_client_item l_client_item;
	  m_db->get_complete_client(l_client_id,l_client_item);
	  p_result.push_back(search_facture_client_item(*l_iter,l_client_item));
	}
      else
	{
	  facture_reason l_reason;
	  m_db->get_facture_reason(l_reason_id,l_reason);
	  p_result.push_back(search_facture_client_item(*l_iter,l_reason.get_name()));
	}
      ++l_iter;
    }

}

//------------------------------------------------------------------------------
void fichier_client::get_remaining_refs(const livre_facture & p_livre,std::vector<facture> & p_list_facture,std::vector<uint32_t> & p_remaining_refs)
{
  // Computing remaining references
  std::vector<facture>::const_iterator l_iter = p_list_facture.begin();
  std::vector<facture>::const_iterator l_iter_end = p_list_facture.end();
  for(uint32_t l_index = 1 ; l_index <= p_livre.get_nb_max_facture(); ++l_index)
    {
      //Check if there are still defined factures
      std::cout << "Facture " << l_index << " check if there are still defined factures" << std::endl ;
      if(l_iter != l_iter_end)
	{
	  // Check if currently tested facture is before current defined facture
	  std::cout << "Check if currently tested facture " << l_index << " is before current defined facture " << l_iter->get_facture_ref() << std::endl ;
	  if(l_index < l_iter->get_facture_ref())
	    {
	      p_remaining_refs.push_back(l_index);
	    }
	  else if(l_index == l_iter->get_facture_ref())
	    {
	      std::cout << "Going to next defined facture" << std::endl ;
	      ++l_iter;
	    }
	}
      else
	{
	  p_remaining_refs.push_back(l_index);
	}
    }
}

//------------------------------------------------------------------------------
void fichier_client::manage_features(bool p_enabled)
{
  assert(m_user_interface);
  m_user_interface->set_customer_search_enabled(p_enabled);
  set_customer_information_enabled(p_enabled);
  m_user_interface->set_livre_facture_information_enabled(p_enabled);
  m_user_interface->set_facture_status_information_enabled(p_enabled);
  m_user_interface->set_facture_reason_information_enabled(p_enabled);
  m_user_interface->set_purchase_configuration_enabled(p_enabled);
  m_user_interface->set_city_information_enabled(p_enabled);
  m_user_interface->set_coherency_information_enabled(p_enabled);
}

const std::string fichier_client::m_tmp_db_name = "tmp_db";
const std::string fichier_client::m_version = "0.9";

//EOF
