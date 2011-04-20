#include "fichier_client_db.h"
#include "livre_facture.h"
#include "ville.h"

#include <stdio.h>
#include <stdlib.h>


#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#ifdef DUMMY
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++)
    {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
  printf("\n");
  return 0;
}
#endif

template <class T> void display(const vector<T> &p_list)
{
  typename::vector<T>::const_iterator l_iter = p_list.begin();
  typename::vector<T>::const_iterator l_iter_end = p_list.end();
  cout << "------------------" << endl ;
  while(l_iter != l_iter_end)
    {
      cout << *l_iter << endl ;
      ++l_iter;
    }
  cout << "------------------" << endl ;
}

string extract_until(const std::string & p_string,char p_char,string & p_result)
{
  size_t l_pos = p_string.find(p_char);
  assert(l_pos != string::npos);
  p_result = p_string.substr(0,l_pos);
  return p_string.substr(l_pos+1);
}

string remove_leading_space(const std::string p_string)
{
  string l_result = p_string;
  while(l_result[0]==' ')
    {
      l_result.erase(0,1);
    }
  return l_result;
}

void import(const std::string p_file_name,fichier_client_db & p_db)
{
  ifstream l_input_file(p_file_name.c_str());
  if(l_input_file==NULL)
    {
      cout << "ERROR : unable to open file \"" << p_file_name << "\"" << endl ;
      exit(-1);
    }

  string l_line = "";
  do
    {
      getline(l_input_file,l_line);
      if(l_line.find("INSERT INTO") != string::npos)
	{

	  // Extract table name
	  size_t l_pos = l_line.find('`');
	  assert(l_pos != string::npos);
	  string l_table_name = l_line.substr(l_pos+1,l_line.find('`',l_pos+1)-l_pos-1);
	  // Extract data
	  l_pos = l_line.find('(');
	  assert(l_pos != string::npos);
	  string l_data = l_line.substr(l_pos+1,l_line.find(')',l_pos+1)-l_pos-1);

	  //Treat data
	  if(l_table_name == "achat")
	    {
	      string l_result("");
	      l_data = extract_until(l_data,',',l_result);
	      uint32_t l_achat_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
	      l_data = extract_until(l_data,',',l_result);
	      uint32_t l_client_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
	      l_data = extract_until(l_data,',',l_result);
	      l_result = remove_leading_space(l_result);
	      string l_date = l_result.substr(1,l_result.size()-2);
	      l_data = extract_until(l_data,',',l_result);
	      uint32_t l_marque_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
	      l_data = extract_until(l_data,',',l_result);
	      uint32_t l_type_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
	      l_data = extract_until(l_data,',',l_result);
	      l_result = remove_leading_space(l_result);
	      string l_reference = l_result.substr(1,l_result.size()-2);
	      l_data = extract_until(l_data,',',l_result);
	      l_result = remove_leading_space(l_result);
	      float l_prix_franc = strtof(l_result.substr(1,l_result.size()-2).c_str(),NULL);
	      l_data = extract_until(l_data,',',l_result);
	      l_result = remove_leading_space(l_result);
	      float l_prix_euro = strtof(l_result.substr(1,l_result.size()-2).c_str(),NULL);
	      l_result = remove_leading_space(l_data);
	      bool l_garantie = (l_result.substr(1,l_result.size()-2)) != "non";


	      
	      achat l_achat(l_achat_id,l_client_id,l_date,0,l_marque_id,l_type_id,l_reference,l_prix_franc,l_prix_euro,l_garantie);
#ifdef DEBUG_IMPORT_ACHAT
	      cout << "Achat_id = \"" << l_achat_id << "\"" << endl ;
	      cout << "Client_id = \"" << l_client_id << "\"" << endl ;
	      cout << "Date = \"" << l_date << "\"" << endl ;
	      cout << "Marque_id = \"" << l_marque_id << "\"" << endl ;
	      cout << "type_id = \"" << l_type_id << "\"" << endl ;
	      cout << "Reference = \"" << l_reference << "\"" << endl ;
	      cout << "Prix_Franc = \"" << l_prix_franc << "\"" << endl ;
	      cout << "Prix_Euro = \"" << l_prix_euro << "\"" << endl ;
	      cout << "Garantie = \"" << l_garantie << "\"" << endl ;
	      cout << l_achat << endl ;
#endif
	      p_db.create(l_achat);

	    }
	  else if(l_table_name == "client")
	    {
	      string l_result("");
	      l_data = extract_until(l_data,',',l_result);
	      uint32_t l_client_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
	      l_data = extract_until(l_data,',',l_result);
	      l_result = remove_leading_space(l_result);
	      string l_name = l_result.substr(1,l_result.size()-2);
	      l_data = extract_until(l_data,',',l_result);
	      l_result = remove_leading_space(l_result);
	      string l_first_name = l_result.substr(1,l_result.size()-2);
	      size_t l_pos = l_data.find("'");
	      l_pos = l_data.find("'",l_pos+1);
	      l_pos = l_data.find(",",l_pos);
	      string l_address = remove_leading_space(l_data.substr(0,l_pos));
	      l_data = l_data.substr(l_pos+1);
	      l_data = extract_until(l_data,',',l_result);
	      l_result = remove_leading_space(l_result);
	      string l_tel = l_result.substr(1,l_result.size()-2);
	      uint32_t l_ville_id = strtol(remove_leading_space(l_data).c_str(),NULL,10);

	      client l_client(l_client_id,l_name,l_first_name,l_address,l_tel,l_ville_id);

#ifdef DEBUG_IMPORT_CLIENT
	      cout << "Client_id = \"" << l_client_id << "\"" << endl ;	      
	      cout << "Name = \"" << l_name << "\"" << endl ;	      
	      cout << "First name = \"" << l_first_name << "\"" << endl ;	      
	      cout << "Address = \"" << l_address << "\"" << endl ;
	      cout << "Tel = \"" << l_tel << "\"" << endl ;
	      cout << "Ville_id = \"" << l_ville_id << "\"" << endl ;
	      cout << l_client;
#endif
	      p_db.create(l_client);
	      
	    }
	  else if(l_table_name == "marque")
	    {
	      string l_result("");
	      l_data = extract_until(l_data,',',l_result);
	      uint32_t l_marque_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
	      l_result = remove_leading_space(l_data);
	      string l_marque_name = l_result.substr(1,l_result.size()-2);	      
	      marque l_marque(l_marque_id,l_marque_name);

#ifdef DEBUG_IMPORT_MARQUE
	      cout << "l_marque_id =\"" << l_marque_id << "\"" << endl ;
	      cout << "Marque = \"" << l_marque_name << "\"" << endl ;
	      cout << l_marque << endl ;
#endif
	      p_db.create(l_marque);

	      
	    }
	  else if(l_table_name == "type")
	    {
	      string l_result("");
	      l_data = extract_until(l_data,',',l_result);
	      uint32_t l_type_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
	      l_result = remove_leading_space(l_data);
	      string l_type_name = l_result.substr(1,l_result.size()-2);	      

	      type_achat l_type_achat(l_type_id,l_type_name);
#ifdef DEBUG_IMPORT_TYPE
	      cout << "type id = \"" << l_type_id << "\"" << endl ;
	      cout << "type name = \"" << l_type_name << "\"" << endl ;
	      cout << l_type_achat << endl ;
#endif
	      p_db.create(l_type_achat);
	    }
	  else if(l_table_name == "ville")
	    {
	      cout << l_line << endl ;
	      cout << "table : \"" << l_table_name << "\"" << endl ;
	      cout << "data : \"" << l_data << "\"" << endl ;
	      string l_result("");
	      l_data = extract_until(l_data,',',l_result);
	      uint32_t l_ville_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
	      l_data = extract_until(l_data,',',l_result);
	      l_result = remove_leading_space(l_result);
	      string l_ville_name = l_result.substr(1,l_result.size()-2);
	      l_result = remove_leading_space(l_data);
	      string l_postal_code = l_result.substr(1,l_result.size()-2);	      
 
	      ville l_ville(l_ville_id,l_ville_name,l_postal_code);
#ifdef DEBUG_IMPORT_VILLE
	      cout << "Ville id = \"" << l_ville_id << "\"" << endl ;
	      cout << "Ville name = \"" << l_ville_name << "\"" << endl ;
	      cout << "Postal code = \"" << l_postal_code << "\"" << endl ;
	      cout << l_ville << endl ;
#endif
	      p_db.create(l_ville);
	    }
	  

	}
    }
  while(!l_input_file.eof() && l_line != "");

  l_input_file.close();

}

int main(int argc, char **argv)
{

  fichier_client_db l_fichier_client("toto");

  import("fichier_client_20101030.sql",l_fichier_client);
  l_fichier_client.check_db_coherency();

  exit(-1);

  // Testing Livre facture management
  //---------------------------

  livre_facture l_livre(1,"20100101","20100113");
  livre_facture l_livre2(2,"20100110", "20100115");
  
  l_fichier_client.create(l_livre);
  l_fichier_client.create(l_livre2);
  std::string l_date("20000101");

  cout << "Search livre containing date " << l_date << endl ;
  vector<livre_facture> l_list_livre_facture;
  l_fichier_client.get_livre_facture_containing_date(l_date,l_list_livre_facture);
  display(l_list_livre_facture);

  l_date = "20100110";
  cout << "Search livre containing date " << l_date << endl ;
  l_list_livre_facture.clear();
  l_fichier_client.get_livre_facture_containing_date(l_date,l_list_livre_facture);
  display(l_list_livre_facture);

  l_date = "20100114";
  cout << "Search livre containing date " << l_date << endl ;
  l_list_livre_facture.clear();
  l_fichier_client.get_livre_facture_containing_date(l_date,l_list_livre_facture);
  display(l_list_livre_facture);

  l_livre2.setStartDate("20100309");
  l_livre2.setEndDate("20101123");

  l_fichier_client.update(l_livre2);

  l_list_livre_facture.clear();
  l_fichier_client.get_all_livre_facture(l_list_livre_facture);
  display(l_list_livre_facture);

  livre_facture l_livre_facture;
  
  if(l_fichier_client.get_livre_facture(1,l_livre_facture))
    {
      cout << l_livre_facture << endl ;
    }
  else
    {
      cout << "Livre_facture 1 not found" << endl ;
    }

  l_fichier_client.remove(l_livre);

  // Testing Ville management
  //---------------------------
  vector<ville> l_result_ville;
  l_fichier_client.get_all_ville(l_result_ville);
  display(l_result_ville);

  ville l_ville1(1,"Feurs","42110");
  l_fichier_client.create(l_ville1);
  ville l_ville2(2,"Montrond-les-bains","42210");
  l_fichier_client.create(l_ville2);
  ville l_ville3(3,"Aix-les-bains","72050");
  l_fichier_client.create(l_ville3);

  l_result_ville.clear();
  l_fichier_client.get_all_ville(l_result_ville);

  display(l_result_ville);

  l_result_ville.clear();
  l_fichier_client.get_ville_by_name("bain",l_result_ville);
  display(l_result_ville);

  l_result_ville.clear();
  l_fichier_client.get_ville_by_code_postal("7",l_result_ville);
  display(l_result_ville);

  l_ville3.setName("Chamonix");
  l_fichier_client.update(l_ville3);

  l_result_ville.clear();
  l_fichier_client.get_all_ville(l_result_ville);
  display(l_result_ville);


  ville l_ville_to_remove;
  l_fichier_client.get_ville(3,l_ville_to_remove);
  l_fichier_client.remove(l_ville_to_remove);
  
  l_result_ville.clear();
  l_fichier_client.get_all_ville(l_result_ville);
  display(l_result_ville);


  // Testing Marque management
  //---------------------------
  marque l_marque(1,"BOSCH");
  cout << l_marque << endl ;

  l_fichier_client.create(l_marque);

  vector<marque> l_result_marque;
  l_fichier_client.get_all_marque(l_result_marque);
  display(l_result_marque);

  marque l_marque2(2,"SIEMANS");
  cout << l_marque2 << endl ;

  l_fichier_client.create(l_marque2);

  l_result_marque.clear();
  l_fichier_client.get_all_marque(l_result_marque);
  display(l_result_marque);

  l_marque2.setName("SIEMENS");
  cout << l_marque2 << endl ;
  l_fichier_client.update(l_marque2);

  l_result_marque.clear();
  l_fichier_client.get_all_marque(l_result_marque);
  display(l_result_marque);

  l_fichier_client.create(marque(12,"DUMMY"));

  l_result_marque.clear();
  l_fichier_client.get_all_marque(l_result_marque);
  display(l_result_marque);

  marque l_dummy_marque;
  l_fichier_client.get_marque(12,l_dummy_marque);
  l_fichier_client.remove(l_dummy_marque);

  l_result_marque.clear();
  l_fichier_client.get_all_marque(l_result_marque);
  display(l_result_marque);

  // Testing Type_Achat management
  //---------------------------
  type_achat l_type_achat(1,"Frigo");
  cout << l_type_achat << endl ;

  l_fichier_client.create(l_type_achat);

  vector<type_achat> l_result_type_achat;
  l_fichier_client.get_all_type_achat(l_result_type_achat);
  display(l_result_type_achat);

  type_achat l_type_achat2(2,"Fur");
  cout << l_type_achat2 << endl ;

  l_fichier_client.create(l_type_achat2);

  l_result_type_achat.clear();
  l_fichier_client.get_all_type_achat(l_result_type_achat);
  display(l_result_type_achat);

  l_type_achat2.setName("Four");
  cout << l_type_achat2 << endl ;
  l_fichier_client.update(l_type_achat2);

  l_result_type_achat.clear();
  l_fichier_client.get_all_type_achat(l_result_type_achat);
  display(l_result_type_achat);

  l_fichier_client.create(type_achat(12,"DUMMY"));

  l_result_type_achat.clear();
  l_fichier_client.get_all_type_achat(l_result_type_achat);
  display(l_result_type_achat);

  type_achat l_dummy_type_achat;
  l_fichier_client.get_type_achat(12,l_dummy_type_achat);
  l_fichier_client.remove(l_dummy_type_achat);

  l_result_type_achat.clear();
  l_fichier_client.get_all_type_achat(l_result_type_achat);
  display(l_result_type_achat);

  l_result_type_achat.clear();
  l_fichier_client.get_type_achat_by_name("F",l_result_type_achat);
  display(l_result_type_achat);

  // Testing Achat management
  //---------------------------
  achat l_achat(1,1,"2010-08-15",2,7,5,"MKV",100.0,15.0,false);
  cout << l_achat << endl ;

  l_fichier_client.create(l_achat);

  vector<achat> l_result_achat;
  l_fichier_client.get_all_achat(l_result_achat);
  display(l_result_achat);

  achat l_achat2(2,13,"2010-09-03",1,2,3,"PQ",300,45.0,true);
  cout << l_achat2 << endl ;

  l_fichier_client.create(l_achat2);

  l_result_achat.clear();
  l_fichier_client.get_all_achat(l_result_achat);
  display(l_result_achat);

  l_achat2.set_date("1981-11-23");
  cout << l_achat2 << endl ;
  l_fichier_client.update(l_achat2);

  l_result_achat.clear();
  l_fichier_client.get_all_achat(l_result_achat);
  display(l_result_achat);

  l_fichier_client.create(achat(3,2012,"2012-01-01",1,2012,2012,"Fin du Monde",0.0,0.0,false));

  l_result_achat.clear();
  l_fichier_client.get_all_achat(l_result_achat);
  display(l_result_achat);

  achat l_dummy_achat;
  l_fichier_client.get_achat(3,l_dummy_achat);
  l_fichier_client.remove(l_dummy_achat);

  l_result_achat.clear();
  l_fichier_client.get_all_achat(l_result_achat);
  display(l_result_achat);

  l_result_achat.clear();
  l_fichier_client.get_achat_by_date("",l_result_achat);
  display(l_result_achat);

  // Testing Client management
  //---------------------------
  client l_client(1,"THEVENON","Julien","12 rue des bains","06-83-03-37-49",10);
  cout << l_client << endl ;

  l_fichier_client.create(l_client);

  vector<client> l_result_client;
  l_fichier_client.get_all_client(l_result_client);
  display(l_result_client);

  client l_client2(2,"LANCEART","Celine","Le Zephyr","04-77-26-05-25",2);
  cout << l_client2 << endl ;

  l_fichier_client.create(l_client2);

  l_result_client.clear();
  l_fichier_client.get_all_client(l_result_client);
  display(l_result_client);

  l_client2.set_tel("06-88-77-89-00");
  cout << l_client2 << endl ;
  l_fichier_client.update(l_client2);

  l_result_client.clear();
  l_fichier_client.get_all_client(l_result_client);
  display(l_result_client);

  l_fichier_client.create(client(3,"DURAND", "Joel","rue de je sais pas ou","",12));

  l_result_client.clear();
  l_fichier_client.get_all_client(l_result_client);
  display(l_result_client);

  client l_dummy_client;
  l_fichier_client.get_client(3,l_dummy_client);
  l_fichier_client.remove(l_dummy_client);

  l_result_client.clear();
  l_fichier_client.get_all_client(l_result_client);
  display(l_result_client);

  // Database coherency checking
  l_fichier_client.check_db_coherency();


#ifdef DUMMY

  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  if( argc!=3 )
    {
      fprintf(stderr, "Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
      exit(1);
    }
  rc = sqlite3_open(argv[1], &db);
  printf("rc content = %i\n",rc);
  if(rc != SQLITE_OK)
    {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      exit(1);
    }
  rc = sqlite3_exec(db, argv[2], callback, 0, &zErrMsg);
  if( rc!=SQLITE_OK )
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
  sqlite3_close(db);
#endif //DUMMY
  return 0;
}


