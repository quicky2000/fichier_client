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
void fichier_client::refresh_non_attributed_facture_status_list(void)
{
  std::vector<facture_status> l_non_attributed_facture_status_list;
  get_non_attributed_status_list(l_non_attributed_facture_status_list);
  if(l_non_attributed_facture_status_list.size())
    {
      m_user_interface->set_non_attributed_facture_status_list(l_non_attributed_facture_status_list);
    }
  else
    {
      m_user_interface->display_warning_message("Status de facture manquants","Aucun statut n'a �t� d�fini pour les factures non attribu�es");
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
  m_user_interface->refresh_list_facture_of_livre_facture();
  refresh_non_attributed_facture_list();
		    
  //TO DELETE  // Check if there are some "non attributed" status available
  //TO DELETE  vector<facture_status> l_facture_status_list;
  //TO DELETE  assert(m_db);
  //TO DELETE  m_db->get_all_facture_status(l_facture_status_list);
  //TO DELETE
  //TO DELETE  vector<facture_status> l_tmp_status_list;
  //TO DELETE  m_db->get_facture_status_by_name(facture_status::get_ok_status(),l_tmp_status_list,true);
  //TO DELETE  assert(l_tmp_status_list.size() == 1);
  //TO DELETE  uint32_t l_ok_id = l_tmp_status_list[0].get_id();
  //TO DELETE  l_tmp_status_list.clear();
  //TO DELETE  m_db->get_facture_status_by_name(facture_status::get_non_checked_status(),l_tmp_status_list,true);
  //TO DELETE  assert(l_tmp_status_list.size() == 1);
  //TO DELETE  uint32_t l_non_checked_id = l_tmp_status_list[0].get_id();
  //TO DELETE
  //TO DELETE
  //TO DELETE  vector<facture_status> l_invalid_facture_status_list;
  //TO DELETE
  //TO DELETE  vector<facture_status>::const_iterator l_iter = l_facture_status_list.begin();
  //TO DELETE  vector<facture_status>::const_iterator l_iter_end = l_facture_status_list.end();
  //TO DELETE  while(l_iter != l_iter_end)
  //TO DELETE    {
  //TO DELETE      if(l_iter->get_id() != l_ok_id && l_iter->get_id() != l_non_checked_id)
  //TO DELETE	{
  //TO DELETE	  l_invalid_facture_status_list.push_back(*l_iter);
  //TO DELETE	}
  //TO DELETE      ++l_iter;
  //TO DELETE    }
  //TO DELETE
  //TO DELETE  if(l_invalid_facture_status_list.size())
  //TO DELETE    {
  //TO DELETE      facture l_facture;
  //TO DELETE      bool l_status = m_user_interface->create_non_attributed_facture(l_invalid_facture_status_list,l_facture);
  //TO DELETE      if(l_status)
  //TO DELETE	{
  //TO DELETE	  l_facture.set_livre_facture_id(p_livre_facture_id);
  //TO DELETE	  cout << l_facture << endl ;
  //TO DELETE	  m_db->create(l_facture);
  //TO DELETE	  m_user_interface->refresh_list_facture_of_livre_facture();
  //TO DELETE	}
  //TO DELETE    }
  //TO DELETE  else
  //TO DELETE    {
  //TO DELETE      assert(m_user_interface);
  //TO DELETE      m_user_interface->display_warning_message("Missing \"non attributed\" status","First defined a non attributed_status in facture tab");
  //TO DELETE    }
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
	  m_user_interface->display_warning_message("Facture r�f�rence invalide","Une facture portant cette r�f�rence existe d�ja");
	}
    }
  else
    {
      stringstream l_max_sstr;
      l_max_sstr << l_livre_facture.get_nb_max_facture();
      m_user_interface->display_warning_message("Facture r�f�rence invalide",string("La r�f�r�nce de la facture doit �tre comprise entre 1 et ")+l_max_sstr.str());
    }
      return l_result;
}

//------------------------------------------------------------------------------
void fichier_client::livre_facture_selected(uint32_t p_id)
{
  std::cout << "Fichier_cient Event :: Livre_facture selected " << p_id << std::endl;
  vector<facture> l_result;
  assert(m_db);
  // Get factures related to selected livre_facture
  m_db->get_by_livre_facture(p_id,l_result);

  // Get livre_facture object
  livre_facture l_selected_livre;
  m_db->get_livre_facture(p_id,l_selected_livre);

  // Check if livre facture is complete or not
  if(l_result.size() < l_selected_livre.get_nb_max_facture())
    {

      // Computing remaining references
      std::vector<uint32_t> l_remaining_refs;
      std::vector<facture>::const_iterator l_iter = l_result.begin();
      std::vector<facture>::const_iterator l_iter_end = l_result.end();
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
      l_allowed_books.push_back(p_id);
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
  
}


//------------------------------------------------------------------------------
void fichier_client::no_more_livre_facture_selected(void)
{
  std::cout << "Fichier_cient Event :: No more livre_facture selected " << std::endl;
  m_user_interface->set_delete_livre_facture_enabled(false);
  m_user_interface->set_modify_livre_facture_enabled(false);
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

//------------------------------------------------------------------------------
uint32_t fichier_client::get_livre_facture(uint32_t p_id,livre_facture & p_data)
{
  assert(m_db);
  return m_db->get_livre_facture(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client::get_all_livre_facture(std::vector<livre_facture> & p_list)
{
  assert(m_db);
  m_db->get_all_livre_facture(p_list);
}

//------------------------------------------------------------------------------
void fichier_client::create( facture_status & p_facture_status)
{
  assert(m_db);
  m_db->create(p_facture_status);
}

//------------------------------------------------------------------------------
void fichier_client::update(const facture_status & p_facture_status)
{
  assert(m_db);
  m_db->update(p_facture_status);
}

//------------------------------------------------------------------------------
void fichier_client::remove(const facture_status & p_facture_status)
{
  assert(m_db);
  m_db->remove(p_facture_status);
}


//------------------------------------------------------------------------------
uint32_t fichier_client::get_facture_status(uint32_t p_id,facture_status & p_data)
{
  assert(m_db);
  return m_db->get_facture_status(p_id,p_data);
}

//------------------------------------------------------------------------------
void fichier_client::get_all_facture_status(std::vector<facture_status> & p_list)
{
  assert(m_db);
  m_db->get_all_facture_status(p_list);
}

//------------------------------------------------------------------------------
void fichier_client::get_facture_status_by_name(const std::string & p_name,std::vector<facture_status> & p_result)
{
  assert(m_db);
  m_db->get_facture_status_by_name(p_name,p_result,false);
}

//------------------------------------------------------------------------------
void fichier_client::remove(const livre_facture & p_livre_facture)
{
  assert(m_db);
  uint32_t l_livre_facture_id = p_livre_facture.get_id();
  vector<facture> l_result;
  m_db->get_by_livre_facture(l_livre_facture_id,l_result);
  if(l_result.size() == 0)
    {
      m_db->remove(p_livre_facture);
    }
  else
    {
      assert(m_user_interface);
      m_user_interface->display_warning_message("Cannot delete","Cannot delete this facture book because it contains some facture");
    }
}

//------------------------------------------------------------------------------
void fichier_client::update(const livre_facture & p_livre_facture)
{
  assert(m_db);
  m_db->update(p_livre_facture);
}
 
//------------------------------------------------------------------------------
void fichier_client::create( livre_facture & p_livre_facture)
{
  assert(m_db);
  m_db->create(p_livre_facture);
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

const std::string fichier_client::m_tmp_db_name = "tmp_db";
const std::string fichier_client::m_version = "0.1";

//EOF
