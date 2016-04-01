#include "external_sql_importer.h"
#include "fichier_client_db.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
using namespace std;

namespace external_sql_importer
{

  string extract_until(const std::string & p_string,char p_char,string & p_result,uint32_t p_line_nb)
  {
    size_t l_pos = p_string.find(p_char);
    assert((l_pos != string::npos ? true :(cout << "Line " << p_line_nb << endl , false)));
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
    if(!l_input_file.is_open())
      {
	cout << "ERROR : unable to open file \"" << p_file_name << "\"" << endl ;
	exit(-1);
      }

    string l_line = "";
    uint32_t l_line_nb = 0;
    vector<facture_status> l_result_facture_status;
    p_db.get_facture_status_by_name(facture_status::get_non_checked_status(),l_result_facture_status,true);
    assert(l_result_facture_status.size()==1);
    facture_status l_facture_status = l_result_facture_status[0];
    do
      {
	getline(l_input_file,l_line);
	++l_line_nb;
	if(l_line.find("INSERT INTO") != string::npos)
	  {
	    bool l_condition = true;
	    assert((l_condition ? true :(cout << "Message" << endl , false) ));

	    // Extract table name
	    size_t l_pos = l_line.find('`');
	    assert((l_pos != string::npos ? true :(cout << "Line " << l_line_nb << endl , false)));
	    string l_table_name = l_line.substr(l_pos+1,l_line.find('`',l_pos+1)-l_pos-1);
	    // Extract data
	    l_pos = l_line.find('(');
	    assert((l_pos != string::npos ? true :(cout << "Line " << l_line_nb << endl , false)));
	    string l_data = l_line.substr(l_pos+1,l_line.find(')',l_pos+1)-l_pos-1);

	    //Treat data
	    if(l_table_name == "achat")
	      {
		string l_result("");
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		uint32_t l_achat_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		uint32_t l_client_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		l_result = remove_leading_space(l_result);
		string l_date = l_result.substr(1,l_result.size()-2);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		uint32_t l_marque_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		uint32_t l_type_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		l_result = remove_leading_space(l_result);
		string l_reference = l_result.substr(1,l_result.size()-2);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		l_result = remove_leading_space(l_result);
		float l_prix_franc = strtof(l_result.substr(1,l_result.size()-2).c_str(),NULL);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		l_result = remove_leading_space(l_result);
		float l_prix_euro = strtof(l_result.substr(1,l_result.size()-2).c_str(),NULL);
		l_result = remove_leading_space(l_data);
		bool l_garantie = (l_result.substr(1,l_result.size()-2)) != "non";


		// Search corresponding facture
		vector<facture> l_facture_result;
		p_db.get_by_date_and_client_id(l_date,l_client_id,l_facture_result);
		uint32_t l_facture_id = 0;

		if(l_facture_result.size()==0)
		  {
		    facture l_facture(0,l_client_id,l_date,0,l_facture_status.get_id(),0);
		    p_db.create(l_facture);
		    l_facture_id = l_facture.get_id();
		  }
		else if(l_facture_result.size() == 1)
		  {
		    l_facture_id = l_facture_result.front().get_id();
		  }
		else
		  {
		    cout << "ERROR : facture should be unique or not existing" << endl ;
		    exit(-1);
		  }
      
	      
		//		achat l_achat(l_achat_id,l_client_id,l_date,0,l_marque_id,l_type_id,l_reference,l_prix_franc,l_prix_euro,l_garantie);
		achat l_achat(l_achat_id,l_facture_id,l_marque_id,l_type_id,l_reference,l_prix_franc,l_prix_euro,l_garantie);
#ifdef DEBUG_IMPORT_ACHAT
		cout << "Achat_id = \"" << l_achat_id << "\"" << endl ;
		cout << "Facture_id = \"" << l_facture_id << "\"" << endl ;
		//		cout << "Client_id = \"" << l_client_id << "\"" << endl ;
		//		cout << "Date = \"" << l_date << "\"" << endl ;
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
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		uint32_t l_client_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		l_result = remove_leading_space(l_result);
		string l_name = l_result.substr(1,l_result.size()-2);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		l_result = remove_leading_space(l_result);
		string l_first_name = l_result.substr(1,l_result.size()-2);
		size_t l_pos = l_data.find("'");
		l_pos = l_data.find("'",l_pos+1);
		l_pos = l_data.find(",",l_pos);
		string l_address = remove_leading_space(l_data.substr(0,l_pos));
		l_data = l_data.substr(l_pos+1);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
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
		l_data = extract_until(l_data,',',l_result,l_line_nb);
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
		l_data = extract_until(l_data,',',l_result,l_line_nb);
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
		l_data = extract_until(l_data,',',l_result,l_line_nb);
		uint32_t l_ville_id = strtol(remove_leading_space(l_result).c_str(),NULL,10);
		l_data = extract_until(l_data,',',l_result,l_line_nb);
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
    while(!l_input_file.eof() );

    l_input_file.close();

  }


}

//EOF
