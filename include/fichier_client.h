#ifndef FICHIER_CLIENT_H
#define FICHIER_CLIENT_H
#include <string>

class fichier_client_db;

class fichier_client
{
 public:
  fichier_client(void);
  ~fichier_client(void);

  void import_external_sql(const std::string & p_name);
  void check_db_coherency(void);
 private:
  fichier_client_db *m_db;
  const std::string m_db_name; 
  
};

#endif
//EOF
