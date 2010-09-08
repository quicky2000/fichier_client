#include "fichier_client_db.h"
#include "livre_facture.h"
#include "ville.h"

#include <stdio.h>
#include <stdlib.h>


#include <sqlite3.h>
#include <iostream>
using namespace std;


static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++)
    {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
  printf("\n");
  return 0;
}

template <class T> void display(const vector<T> *p_list)
{
  typename::vector<T>::const_iterator l_iter = p_list->begin();
  typename::vector<T>::const_iterator l_iter_end = p_list->end();
  cout << "------------------" << endl ;
  while(l_iter != l_iter_end)
    {
      cout << *l_iter << endl ;
      ++l_iter;
    }
  cout << "------------------" << endl ;
}

template <class T> void display(const vector<T*> *p_list)
{
  typename::vector<T*>::const_iterator l_iter = p_list->begin();
  typename::vector<T*>::const_iterator l_iter_end = p_list->end();
  cout << "------------------" << endl ;
  while(l_iter != l_iter_end)
    {
      cout << *(*l_iter) << endl ;
      ++l_iter;
    }
  cout << "------------------" << endl ;
}

int main(int argc, char **argv)
{

  fichier_client_db l_fichier_client("toto");

  livre_facture l_livre(1,"20100101","20100113");
  livre_facture l_livre2(2,"20100110", "20100115");
  
  l_fichier_client.create(l_livre);
  l_fichier_client.create(l_livre2);
  std::string l_date("20000101");
  
  cout << "Search livre containing date " << l_date << endl ;
  display(l_fichier_client.get_livre_facture_containing_date(l_date));

  l_date = "20100110";
  cout << "Search livre containing date " << l_date << endl ;
  display(l_fichier_client.get_livre_facture_containing_date(l_date));

  l_date = "20100114";
  cout << "Search livre containing date " << l_date << endl ;
  display(l_fichier_client.get_livre_facture_containing_date(l_date));

  l_livre2.setStartDate("20100309");
  l_livre2.setEndDate("20101123");

  l_fichier_client.update(l_livre2);

  const vector<livre_facture> *l_list = l_fichier_client.get_all_livre_facture();
  display(l_list);
  delete l_list;

  livre_facture * l_result = l_fichier_client.get_livre_facture(1);

  if(l_result == NULL)
    {
      cout << "Libre_facture 1 not found" << endl ;
    }
  else
    {
      cout << *l_result << endl ;
    }

  l_fichier_client.remove(l_livre);

  display(l_fichier_client.get_all_ville());

  ville l_ville1(1,"Feurs","42110");
  l_fichier_client.create(l_ville1);
  ville l_ville2(2,"Montrond-les-bains","42210");
  l_fichier_client.create(l_ville2);
  ville l_ville3(3,"Aix-les-bains","72050");
  l_fichier_client.create(l_ville3);

  display(l_fichier_client.get_all_ville());

  display(l_fichier_client.get_ville_by_name("bain"));
  display(l_fichier_client.get_ville_by_code_postal("7"));

  l_ville3.setName("Chamonix");
  l_fichier_client.update(l_ville3);
  display(l_fichier_client.get_all_ville());

  l_fichier_client.remove(*(l_fichier_client.get_ville(3)));
  
  display(l_fichier_client.get_all_ville());


  // Testing Marque management
  //---------------------------
  marque l_marque(1,"BOSCH");
  cout << l_marque << endl ;

  l_fichier_client.create(l_marque);

  display(l_fichier_client.get_all_marque());

  marque l_marque2(2,"SIEMANS");
  cout << l_marque2 << endl ;

  l_fichier_client.create(l_marque2);

  display(l_fichier_client.get_all_marque());

  l_marque2.setName("SIEMENS");
  cout << l_marque2 << endl ;
  l_fichier_client.update(l_marque2);

  display(l_fichier_client.get_all_marque());

  l_fichier_client.create(marque(12,"DUMMY"));

  display(l_fichier_client.get_all_marque());

  marque * l_dummy_marque = l_fichier_client.get_marque(12);
  l_fichier_client.remove(*l_dummy_marque);

  display(l_fichier_client.get_all_marque());

  // Testing Type_Achat management
  //---------------------------
  type_achat l_type_achat(1,"Frigo");
  cout << l_type_achat << endl ;

  l_fichier_client.create(l_type_achat);

  display(l_fichier_client.get_all_type_achat());

  type_achat l_type_achat2(2,"Fur");
  cout << l_type_achat2 << endl ;

  l_fichier_client.create(l_type_achat2);

  display(l_fichier_client.get_all_type_achat());

  l_type_achat2.setName("Four");
  cout << l_type_achat2 << endl ;
  l_fichier_client.update(l_type_achat2);

  display(l_fichier_client.get_all_type_achat());

  l_fichier_client.create(type_achat(12,"DUMMY"));

  display(l_fichier_client.get_all_type_achat());

  type_achat * l_dummy_type_achat = l_fichier_client.get_type_achat(12);
  l_fichier_client.remove(*l_dummy_type_achat);

  display(l_fichier_client.get_all_type_achat());

  display(l_fichier_client.get_type_achat_by_name("F"));

  // Testing Achat management
  //---------------------------
  achat l_achat(1,1,"2010-08-15",2,7,5,"MKV",100.0,15.0,false);
  cout << l_achat << endl ;

  l_fichier_client.create(l_achat);

  display(l_fichier_client.get_all_achat());

  achat l_achat2(2,13,"2010-09-03",1,2,3,"PQ",300,45.0,true);
  cout << l_achat2 << endl ;

  l_fichier_client.create(l_achat2);

  display(l_fichier_client.get_all_achat());

  l_achat2.set_date("1981-11-23");
  cout << l_achat2 << endl ;
  l_fichier_client.update(l_achat2);

  display(l_fichier_client.get_all_achat());

  l_fichier_client.create(achat(3,2012,"2012-01-01",1,2012,2012,"Fin du Monde",0.0,0.0,false));

  display(l_fichier_client.get_all_achat());

  achat * l_dummy_achat = l_fichier_client.get_achat(3);
  l_fichier_client.remove(*l_dummy_achat);

  display(l_fichier_client.get_all_achat());

  display(l_fichier_client.get_achat_by_date(""));

  // Testing Client management
  //---------------------------
  client l_client(1,"THEVENON","Julien","12 rue des bains","06-83-03-37-49",10);
  cout << l_client << endl ;

  l_fichier_client.create(l_client);

  display(l_fichier_client.get_all_client());

  client l_client2(2,"LANCEART","Celine","Le Zephyr","04-77-26-05-25",10);
  cout << l_client2 << endl ;

  l_fichier_client.create(l_client2);

  display(l_fichier_client.get_all_client());

  l_client2.set_tel("06-88-77-89-00");
  cout << l_client2 << endl ;
  l_fichier_client.update(l_client2);

  display(l_fichier_client.get_all_client());

  l_fichier_client.create(client(3,"DURAND", "Joel","rue de je sais pas ou","",12));

  display(l_fichier_client.get_all_client());

  client * l_dummy_client = l_fichier_client.get_client(3);
  l_fichier_client.remove(*l_dummy_client);

  display(l_fichier_client.get_all_client());

  // Database coherency checking
  l_fichier_client.check_db_coherency();

  exit(-1);

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
  return 0;
}


