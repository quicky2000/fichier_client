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

  // Disable customer data identity
  m_user_interface->set_customer_data_identity_fields_enabled(false);
  m_user_interface->set_customer_data_create_customer_enabled(false);
  m_user_interface->set_customer_data_modify_customer_enabled(false);
  m_user_interface->set_customer_data_delete_customer_enabled(false);
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
 
  std::vector<search_achat_item> l_list_achat;
  m_db->get_achat_by_client_id(l_client_id,l_list_achat);
  m_user_interface->update_search_customer_list_achat(l_list_achat);

  std::vector<search_facture_item> l_list_facture;
  m_db->get_facture_by_client_id(l_client_id,l_list_facture);
  m_user_interface->update_search_customer_list_facture(l_list_facture);

  m_user_interface->set_customer_search_delete_customer_enabled(l_list_facture.size() == 0 && l_list_achat.size() == 0);
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
}

//------------------------------------------------------------------------------
void fichier_client::treat_search_customer_delete_customer_event(void)
{
  std::cout << "Fichier_client Event::search customer delete customer event" << std::endl;
  m_user_interface->set_customer_data_identity_fields_enabled(false);
}

// Customer data related events
//------------------------------------------------------------------------------
void fichier_client::treat_postal_code_modification_event(void)
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
void fichier_client::treat_city_selection_event(void)
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
  m_db->search_client(l_name,l_first_name,l_address,l_city->get_name(),l_client_list);
  assert(l_client_list.size()==0);

  client l_client(l_name,l_first_name,l_address,l_phone,l_city->get_id());
  m_db->create(l_client);
  m_current_customer_id = l_client.get_id();
  
  m_user_interface->set_customer_data_create_customer_enabled(false);
  m_user_interface->set_customer_data_modify_customer_enabled(false);
  m_user_interface->set_customer_data_delete_customer_enabled(true);

  // Enable bill edition
  m_user_interface->set_customer_bill_fields_enabled(true);
  m_user_interface->set_customer_bill_list_enabled(true);
 
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
  l_client.set_tel(l_phone);
  l_client.set_address(l_address);
  l_client.set_ville_id(l_city->get_id());
  
  m_db->update(l_client);
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

   std::vector<search_achat_item> l_list_achat;
   m_db->get_achat_by_client_id(l_client.get_id(),l_list_achat);
   assert(l_list_achat.size()==0);

  std::vector<search_facture_item> l_list_facture;
  m_db->get_facture_by_client_id(l_client.get_id(),l_list_facture);
   assert(l_list_facture.size()==0);

   m_db->remove(l_client);
   m_current_customer_id = 0;


   disable_customer_identity();

   assert(m_user_interface);
   m_user_interface->set_focus_on_customer_search();
   
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
void fichier_client::disable_customer_identity(void)
{
  assert(m_user_interface);
  m_user_interface->set_customer_name("");
  m_user_interface->set_customer_first_name("");
  m_user_interface->set_customer_phone("");
  m_user_interface->set_customer_address("");
  m_user_interface->set_customer_postal_code("");
  std::vector<ville> l_cities;
  m_user_interface->set_customer_city_list(l_cities);
  m_user_interface->set_customer_data_identity_fields_enabled(false);
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
      m_db->search_client(l_name,l_first_name,l_address,l_city->get_name(),l_client_list);
      std::cout << "Corresponding list size " << l_client_list.size() << std::endl ;
    }

  if(m_current_customer_id)
    {
      std::cout << "Existing customer" << std::endl;
      if(l_client_list.size()==0 && l_complete)
	{
	  std::cout << "Compelte and no conflicts" << std::endl;
	  client l_client;  
	  m_db->get_client(m_current_customer_id,l_client);

	  bool l_modified = l_client.get_name() != l_name || 
	    l_client.get_first_name() != l_first_name || 
	    l_client.get_tel() != l_phone ||
	    l_client.get_address() != l_address ||
	    l_client.get_ville_id() != l_city->get_id();
	  m_user_interface->set_customer_data_modify_customer_enabled(l_modified);
	}
      m_user_interface->set_customer_data_create_customer_enabled(false);
    }
  else
    {
      m_user_interface->set_customer_data_create_customer_enabled(l_client_list.size() == 0 && l_complete);
      m_user_interface->set_customer_data_modify_customer_enabled(false);
    }
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
      m_user_interface->set_facture_modification_for_selected_livre_enabled(true);      
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

      vector<facture> l_list_facture;
      assert(m_db);
      // Get factures related to selected livre_facture
      m_db->get_by_livre_facture(l_livre_id,l_list_facture);

      // Get livre_facture object
      livre_facture l_related_livre;
      m_db->get_livre_facture(l_livre_id,l_related_livre);
    
      // Computing remaining references
      std::vector<uint32_t> l_remaining_refs;
      get_remaining_refs(l_related_livre,l_list_facture,l_remaining_refs);
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
	  l_name_ok = l_facture_status_list.size() == 0;
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
}






// Facture reason related events
//--------------------------------
//------------------------------------------------------------------------------
void fichier_client::treat_no_more_facture_reason_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::no more facture_reason selected" << std::endl;
  assert(m_user_interface);
  m_user_interface->set_delete_facture_reason_enabled(false);
  m_user_interface->set_modify_facture_reason_enabled(false);
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

  // Check this is not predefined reason
  string l_name = l_facture_reason.get_name();

  m_user_interface->set_delete_facture_reason_enabled(false);
  if(!m_facture_reason_pending_modif)
    {
      m_facture_reason_pending_modif = true;
      m_user_interface->set_facture_reason_name(l_facture_reason.get_name());
      m_user_interface->set_modify_facture_reason_action_name("Annuler");
      m_user_interface->set_facture_reason_list_enabled(false);
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
	  l_name_ok = l_facture_reason_list.size()==0;
	}
      m_user_interface->set_modify_facture_reason_enabled(l_name_ok);
    }  
 
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
void fichier_client::treat_facture_reason_selected_event(void)
{
  std::cout << "Fichier_ClientEvent::facture reason selection event" << std::endl;  
  assert(m_user_interface);

  uint32_t l_facture_reason_id = m_user_interface->get_selected_facture_reason_id();
  
  // Get selected reason
  assert(m_db);
  facture_reason l_facture_reason;
  m_db->get_facture_reason(l_facture_reason_id,l_facture_reason);

  string l_name = l_facture_reason.get_name();
   
  // Check that no facture are using this reason
  std::vector<facture> l_facture_list;
  m_db->get_by_reason(l_facture_reason_id,l_facture_list);
  m_user_interface->set_delete_facture_reason_enabled(l_facture_list.size()==0);
  m_user_interface->set_modify_facture_reason_enabled(true);
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
bool fichier_client::need_save(void)const
{
  return (m_db ? m_db->is_modified():false);
}

//------------------------------------------------------------------------------
void fichier_client::check_db_coherency(void)
{
  assert(m_db);
  m_db->check_db_coherency();
}

//------------------------------------------------------------------------------
void fichier_client::import_external_sql(const std::string & p_name)
{
  assert(m_db == NULL);
  assert(m_db_name=="");
  remove_tmp_db();
  m_db = new fichier_client_db(m_tmp_db_name);
  cout << "Importing file name \"" << p_name << "\"" << endl ;
  external_sql_importer::import(p_name,*m_db);
  m_db->create_information("created_by",m_version);
  cout << "Import successfull" << endl ;
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
void fichier_client::open_db(const std::string & p_name)
{
  copy(p_name,m_tmp_db_name);
  m_db_name = p_name;
  open_tmp_db();
  refresh_non_attributed_facture_status_list();
  refresh_non_attributed_facture_reason_list();
}

//------------------------------------------------------------------------------
void fichier_client::close_db(void)
{
  close_tmp_db();
  m_db_name = "";
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
  if(l_input_file==NULL)
    {
      cout << "ERROR : Unable to open input file \"" << p_src << "\"" << endl ;
      exit(-1);
    }

  ofstream l_output_file(p_dest.c_str(),ios::out | ios::binary);
  if(l_output_file==NULL)
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


const std::string fichier_client::m_tmp_db_name = "tmp_db";
const std::string fichier_client::m_version = "0.1";

//EOF
