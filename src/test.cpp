#include "fichier_client.h"
#include "fichier_client_db.h"
#include "livre_facture.h"
#include "ville.h"
#include "test.h"

#include <stdio.h>
#include <stdlib.h>


#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

namespace test
{
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

  void display_info(const pair<string,string> & p_info)
  {
    cout << "information[\"" << p_info.first << "\"]=\"" << p_info.second << "\"" << endl ;
  }

  void display(const vector<pair<string,string> > & p_infos)
  {
    vector<pair<string,string> >::const_iterator l_iter_info = p_infos.begin();
    vector<pair<string,string> >::const_iterator l_iter_info_end = p_infos.end();
    while(l_iter_info != l_iter_info_end)
      {
	display_info(*l_iter_info);
	++l_iter_info;
      }
  }

  void test(void)
  {

#ifdef TEST_CLIENT_SEARCH
    fichier_client l_fichier;
    //  l_fichier.import_external_sql("fichier_client_20110426.sql");
    l_fichier.import_external_sql("fichier_client_20101030.sql");
    l_fichier.check_db_coherency();

    string l_name ;
    string l_first_name;
    string l_city;
    while(l_name != "STOP")
      {
	cout << "Nom ?" ;
	getline(cin,l_name);
	cout << "Prenom ?" ;
	getline(cin,l_first_name);
	cout << "Ville ?" ;
	getline(cin,l_city);
	vector<search_client_item> l_result;
	l_fichier.search_client(l_name,l_first_name,l_city,l_result);
	display(l_result);
      }

    exit(-1);
#endif

    fichier_client_db l_fichier_client("toto");

    // Testing information_table
    //---------------------------
    vector<pair<string,string> > l_informations;
    cout << "Get all informations" << endl ;
    l_fichier_client.get_all_information(l_informations);
    cout << "Display result" << endl ;
    display(l_informations);
    
    cout << "get schena db version info " << endl ;
    pair<string,string> l_db_schema_version;
    uint32_t l_valid = l_fichier_client.get_information("db_schema_version",l_db_schema_version);
    if(l_valid)
      {
	display_info(l_db_schema_version);
      }
    else
      {
	cout << "create schena db version info " << endl ;
	l_fichier_client.create_information("db_schema_version","0.0");
      }

    cout << "get all informations" << endl ; 
    l_informations.clear();
    l_fichier_client.get_all_information(l_informations);
    cout << "display result" << endl ;
    display(l_informations);

    cout << "Create dummy info" << endl ;
    l_fichier_client.create_information("dummy_key","dummy_value");
  
    cout << "get all informations" << endl ; 
    l_informations.clear();
    l_fichier_client.get_all_information(l_informations);
    cout << "display result" << endl ;
    display(l_informations);

    cout << "Modify schema verison" << endl ;
    l_fichier_client.update_information("db_schema_version","1.0");

    cout << "get all informations" << endl ; 
    l_informations.clear();
    l_fichier_client.get_all_information(l_informations);
    cout << "display result" << endl ;
    display(l_informations);

    cout << "Remove dummy info " << endl ;
    l_fichier_client.remove_information("dummy_key");

    cout << "get all informations" << endl ; 
    l_informations.clear();
    l_fichier_client.get_all_information(l_informations);
    cout << "display result" << endl ;
    display(l_informations);

    // Testing facture management
    //-----------------------------
    facture l_facture(1,13,"20110519",200,0,255);
    cout << "Insert in db " << l_facture << endl ;
    l_fichier_client.create(l_facture);
    cout << l_facture << endl ;
    facture l_facture2(2,23,"20110519",400,0,128);
    cout << "Insert in db "<< l_facture2 << endl ;
    l_fichier_client.create(l_facture2);
    cout << l_facture2 << endl ;

    vector<facture> l_result;
    cout << "Search facture with ref=1 qnd lvire_id = 100" << endl ;
    l_fichier_client.get_by_livre_facture_and_ref(1,100,l_result);
    display(l_result);
    l_result.clear();
    cout << "Search facture with ref=2 and livre_id = 200" << endl ;
    l_fichier_client.get_by_livre_facture_and_ref(1,200,l_result);
    display(l_result);

    // Testing Livre facture management
    //---------------------------
    livre_facture l_livre(1,"20100101","20100113");
    livre_facture l_livre2(2,"20100110", "20100115");
    livre_facture l_livre3(3,"20100110", "",false);
  
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

    l_list_livre_facture.clear();
    l_fichier_client.get_all_livre_facture(l_list_livre_facture);
    display(l_list_livre_facture);

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

    l_ville3.set_name("Chamonix");
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

    l_marque2.set_name("SIEMENS");
    cout << l_marque2 << endl ;
    l_fichier_client.update(l_marque2);

    l_result_marque.clear();
    l_fichier_client.get_all_marque(l_result_marque);
    display(l_result_marque);

    marque l_marque3(12,"DUMMY");
    l_fichier_client.create(l_marque3);

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

    l_type_achat2.set_name("Four");
    cout << l_type_achat2 << endl ;
    l_fichier_client.update(l_type_achat2);

    l_result_type_achat.clear();
    l_fichier_client.get_all_type_achat(l_result_type_achat);
    display(l_result_type_achat);

    type_achat l_type_achat3(12,"DUMMY");
    l_fichier_client.create(l_type_achat3);

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
    l_fichier_client.get_type_achat_by_name("F",l_result_type_achat,false);
    display(l_result_type_achat);

    // Testing facture_status management
    //-----------------------------------
    cout << "Create facture status : "<< endl ;
    facture_status l_facture_status("OK");
    cout << l_facture_status << endl ;

    cout << "Insert status in database" << endl ;
    l_fichier_client.create(l_facture_status);

    cout << "Display all facture status : " << endl ;
    vector<facture_status> l_result_facture_status;
    l_fichier_client.get_all_facture_status(l_result_facture_status);
    display(l_result_facture_status);

    cout << "Create new facture status" << endl ;
    facture_status l_facture_status2("NON_CHECKED");
    cout << l_facture_status2 << endl ;

    cout << "Insert status in database" << endl ;
    l_fichier_client.create(l_facture_status2);

    cout << "Display all facture status : " << endl ;
    l_result_facture_status.clear();
    l_fichier_client.get_all_facture_status(l_result_facture_status);
    display(l_result_facture_status);

    cout << "Create new facture status" << endl ;
    facture_status l_facture_status3("DUMMY");
    cout << l_facture_status3 << endl ;

    cout << "Insert status in database" << endl ;
    l_fichier_client.create(l_facture_status3);

    cout << "Display all facture status : " << endl ;
    l_result_facture_status.clear();
    l_fichier_client.get_all_facture_status(l_result_facture_status);
    display(l_result_facture_status);

    cout << "Modify status" << endl ;
    l_facture_status2.set_name("Non v�rif�e");
    cout << l_facture_status2 << endl ;

    cout << "Update db" << endl ;
    l_fichier_client.update(l_facture_status2);

    cout << "Display all facture status : " << endl ;
    l_result_facture_status.clear();
    l_fichier_client.get_all_facture_status(l_result_facture_status);
    display(l_result_facture_status);

    cout << "Remove dummy status" << endl ;
    l_fichier_client.remove(l_facture_status3);

    cout << "Display all facture status : " << endl ;
    l_result_facture_status.clear();
    l_fichier_client.get_all_facture_status(l_result_facture_status);
    display(l_result_facture_status);

    // Testing Achat management
    //---------------------------
    achat l_achat(1,1,7,5,"MKV",100.0,15.0,false);
    cout << l_achat << endl ;

    l_fichier_client.create(l_achat);

    vector<achat> l_result_achat;
    l_fichier_client.get_all_achat(l_result_achat);
    display(l_result_achat);

    achat l_achat2(2,13,2,3,"PQ",300,45.0,true);
    cout << l_achat2 << endl ;

    l_fichier_client.create(l_achat2);

    l_result_achat.clear();
    l_fichier_client.get_all_achat(l_result_achat);
    display(l_result_achat);

    //    l_achat2.set_date("1981-11-23");
    //    cout << l_achat2 << endl ;
    //    l_fichier_client.update(l_achat2);

    l_result_achat.clear();
    l_fichier_client.get_all_achat(l_result_achat);
    display(l_result_achat);

    achat l_achat3(3,2012,2012,2012,"Fin du Monde",0.0,0.0,false);
    l_fichier_client.create(l_achat3);

    l_result_achat.clear();
    l_fichier_client.get_all_achat(l_result_achat);
    display(l_result_achat);

    achat l_dummy_achat;
    l_fichier_client.get_achat(3,l_dummy_achat);
    l_fichier_client.remove(l_dummy_achat);

    l_result_achat.clear();
    l_fichier_client.get_all_achat(l_result_achat);
    display(l_result_achat);

    //    l_result_achat.clear();
    //    l_fichier_client.get_achat_by_date("",l_result_achat);
    //    display(l_result_achat);

    // Testing Client management
    //---------------------------
    client l_client(1,"THEVENON","Julien","12 rue des bains","0683033749",10);
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

    l_client2.set_phone("0688778900");
    cout << l_client2 << endl ;
    l_fichier_client.update(l_client2);

    l_result_client.clear();
    l_fichier_client.get_all_client(l_result_client);
    display(l_result_client);

    client l_client3(3,"DURAND", "Joel","rue de je sais pas ou","",12);
    l_fichier_client.create(l_client3);

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
    uint32_t l_nb_error;
    uint32_t l_nb_warning;
    std::vector<coherency_report_item> l_error_list;
    std::vector<coherency_report_item> l_warning_list;
    l_fichier_client.check_db_coherency(l_nb_error,l_nb_warning,l_error_list,l_warning_list);

  }

}

//EOF
