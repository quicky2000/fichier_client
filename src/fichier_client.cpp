#include "fichier_client.h"
#include "fichier_client_db.h"
#include "external_sql_importer.h"
#include <unistd.h>
#include <iostream>
#include <fstream>

using namespace std;

//------------------------------------------------------------------------------
fichier_client::fichier_client(void):
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
