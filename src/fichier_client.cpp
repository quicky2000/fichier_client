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
  m_facture_status_pending_modif(false),
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
}


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
  std::vector<livre_facture> l_list;
  m_db->get_all_livre_facture(l_list);
  m_user_interface->refresh_livre_facture_list(l_list);
}

//------------------------------------------------------------------------------
void fichier_client::refresh_non_attributed_facture_status_list(void)
{
  std::vector<facture_status> l_non_attributed_facture_status_list;
  get_non_attributed_status_list(l_non_attributed_facture_status_list);
  m_user_interface->set_non_attributed_facture_status_list(l_non_attributed_facture_status_list);
  if(!l_non_attributed_facture_status_list.size())
    {
      m_user_interface->display_warning_message("Status de facture manquants","Aucun statut n'a été défini pour les factures non attribuées");
    }
}



//------------------------------------------------------------------------------
void fichier_client::get_non_attributed_status_list(std::vector<facture_status> & p_result)
{
  vector<facture_status> l_facture_status_list;
  assert(m_db);
  m_db->get_all_facture_status(l_facture_status_list);

  vector<facture_status> l_tmp_status_list;
  m_db->get_facture_status_by_name(facture_status::get_ok_status(),l_tmp_status_list,true);
  assert(l_tmp_status_list.size() == 1);
  uint32_t l_ok_id = l_tmp_status_list[0].get_id();
  l_tmp_status_list.clear();
  m_db->get_facture_status_by_name(facture_status::get_non_checked_status(),l_tmp_status_list,true);
  assert(l_tmp_status_list.size() == 1);
  uint32_t l_non_checked_id = l_tmp_status_list[0].get_id();


  vector<facture_status>::const_iterator l_iter = l_facture_status_list.begin();
  vector<facture_status>::const_iterator l_iter_end = l_facture_status_list.end();
  while(l_iter != l_iter_end)
    {
      if(l_iter->get_id() != l_ok_id && l_iter->get_id() != l_non_checked_id)
	{
	  p_result.push_back(*l_iter);
	}
      ++l_iter;
    }
}

//------------------------------------------------------------------------------
void fichier_client::create_non_attributed_facture(uint32_t p_livre_facture_id)
{
  const facture_status * l_status = m_user_interface->get_non_attributed_facture_status();
  // Create facture event is possible because all values have been checked !!
  assert(l_status);
  std::string l_date = (m_user_interface->is_non_attributed_facture_date_complete() ? m_user_interface->get_non_attributed_facture_date() : "");
  facture l_facture(m_user_interface->get_non_attributed_facture_reference(),
		    0,
		    l_date,
		    m_user_interface->get_non_attributed_facture_livre_facture_id(),
		    l_status->get_id()
		    );
  cout << l_facture << endl ;
  m_db->create(l_facture);
  
  treat_facture_status_selected_event();
  treat_livre_facture_selected_event();
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
      std::vector<facture>::const_iterator l_iter = l_list_facture.begin();
      std::vector<facture>::const_iterator l_iter_end = l_list_facture.end();
      for(uint32_t l_index = 1 ; l_index <= l_selected_livre.get_nb_max_facture(); ++l_index)
	{
	  //Check if there are still defined factures
	  std::cout << "Facture " << l_index << " check if there are still defined factures" << std::endl ;
	  if(l_iter != l_iter_end)
	    {
	      // Check if currently tested facture is before current defined facture
	      std::cout << "Check if currently tested facture " << l_index << " is before current defined facture " << l_iter->get_facture_ref() << std::endl ;
	      if(l_index < l_iter->get_facture_ref())
		{
		  l_remaining_refs.push_back(l_index);
		}
	      else if(l_index == l_iter->get_facture_ref())
		{
		  std::cout << "Going to next defined facture" << std::endl ;
		  ++l_iter;
		}
	    }
	  else
	    {
	      l_remaining_refs.push_back(l_index);
	    }
	}
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
      std::vector<facture> l_facture_result;
      m_db->get_by_livre_facture_and_ref(l_facture_reference, l_livre_id,l_facture_result);
      assert(l_facture_result.size()==0);
    }
  else
    {
      l_valid = false;
    }
  const facture_status * l_status = m_user_interface->get_non_attributed_facture_status();
  std::cout << "Facture status is \"" << (l_status == NULL ? "" : l_status->getName()) << "\"" << std::endl ;
  if(l_status == NULL)
    {
      l_valid = false;
    }
  
  m_user_interface->set_facture_creation_for_selected_livre_enabled(l_valid);  
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
  string l_name = l_facture_status.getName();
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
  string l_name = l_facture_status.getName();
  assert(l_name != facture_status::get_ok_status() && l_name != facture_status::get_non_checked_status());

  m_user_interface->set_delete_facture_status_enabled(false);
  if(!m_facture_status_pending_modif)
    {
      m_facture_status_pending_modif = true;
      m_user_interface->set_facture_status_name(l_facture_status.getName());
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
      
	  l_facture_status.setName(l_new_name);
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
    assert(m_db);
    if(l_status_name != "")
      {
	m_db->get_facture_status_by_name(l_status_name,l_facture_status_list,false);
      }
    else
      {
	m_db->get_all_facture_status(l_facture_status_list);
      }

    if(!m_facture_status_pending_modif)
    {
      m_user_interface->set_create_facture_status_enabled(l_facture_status_list.size()==0);
      m_user_interface->refresh_facture_status_list(l_facture_status_list);
    }
  else
    {
      facture_status l_facture_status;
      uint32_t l_facture_status_id = m_user_interface->get_selected_facture_status_id();
      m_db->get_facture_status(l_facture_status_id,l_facture_status);
      
      bool l_name_ok = l_status_name != "" && l_status_name != facture_status::get_ok_status() && l_status_name != facture_status::get_non_checked_status();
      m_user_interface->set_modify_facture_status_action_name(l_facture_status.getName() != l_status_name ? "Appliquer" : "Annuler");

      // Check if modified value is acceptable
      if(l_name_ok)
	{
	  std::vector<facture_status> l_already;
	  m_db->get_facture_status_by_name(l_status_name,l_already,true);
	  l_name_ok = l_already.size()==0;
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

   string l_name = l_facture_status.getName();
   
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
void fichier_client::search_client(const std::string & p_name, const std::string & p_first_name, const std::string & p_city, std::vector<search_client_item> & p_result)
{
  assert(m_db);
  m_db->search_client(p_name,p_first_name,p_city,p_result);
}

//------------------------------------------------------------------------------
void fichier_client::get_achat_by_client_id(uint32_t p_client_id,std::vector<search_achat_item> & p_result)
{
  assert(m_db);
  m_db->get_achat_by_client_id(p_client_id,p_result);
}

//------------------------------------------------------------------------------
void fichier_client::get_facture_by_client_id(uint32_t p_client_id,
					      std::vector<search_facture_item> & p_result)
{
  assert(m_db);
  m_db->get_facture_by_client_id(p_client_id,p_result);
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
      if(l_client_id)
	{
	  search_client_item l_client_item;
	  m_db->get_complete_client(l_client_id,l_client_item);
	  p_result.push_back(search_facture_client_item(*l_iter,l_client_item));
	}
      else
	{
	  p_result.push_back(search_facture_client_item(*l_iter));
	}
      ++l_iter;
    }

}

const std::string fichier_client::m_tmp_db_name = "tmp_db";
const std::string fichier_client::m_version = "0.1";

//EOF
