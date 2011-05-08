#include "fichier_client.h"
#include "fichier_client_db.h"
#include "external_sql_importer.h"
#include <unistd.h>
#include <iostream>

using namespace std;

fichier_client::fichier_client(void):
  m_db(NULL),
  m_db_name("tmp_db")
{
  int l_result = unlink(m_db_name.c_str());
  cout << "Unlink result = " << l_result << endl ;
  m_db = new fichier_client_db(m_db_name);
}

fichier_client::~fichier_client(void)
{
  delete m_db;
  
}

void fichier_client::check_db_coherency(void)
{
  m_db->check_db_coherency();
}

void fichier_client::import_external_sql(const std::string & p_name)
{
  cout << "Importing file name \"" << p_name << "\"" << endl ;
  external_sql_importer::import(p_name,*m_db);
  cout << "Import successfull" << endl ;
}
