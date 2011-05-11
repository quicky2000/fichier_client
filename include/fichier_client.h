#ifndef FICHIER_CLIENT_H
#define FICHIER_CLIENT_H
#include "search_client_item.h"
#include <string>
#include <vector>
class fichier_client_db;

class fichier_client
{
 public:
  fichier_client(void);
  ~fichier_client(void);

  void import_external_sql(const std::string & p_name);
  void check_db_coherency(void);
  void search_client(const std::string & p_name, const std::string & p_first_name, const std::string & p_city, std::vector<search_client_item> & p_result);
 private:
  fichier_client_db *m_db;
  const std::string m_db_name; 
  
};

#endif
//EOF
