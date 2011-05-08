#ifndef EXTERNAL_SQL_IMPORTER_H
#define EXTERNAL_SQL_IMPORTER_H

#include <string>

class fichier_client_db;

namespace external_sql_importer
{
  void import(const std::string p_file_name,fichier_client_db & p_db);
}

#endif
