#include "table_facture_status.h"
#include <vector>

//------------------------------------------------------------------------------
table_facture_status::table_facture_status(void):
  named_table<facture_status>()
{
}

//------------------------------------------------------------------------------
table_facture_status::~table_facture_status(void)
{
}

//------------------------------------------------------------------------------
void table_facture_status::set_db(sqlite3 *p_db)
{
  named_table<facture_status>::set_db(p_db);
  check_mandatory_status(facture_status::get_non_checked_status());
  check_mandatory_status(facture_status::get_ok_status());
}

//------------------------------------------------------------------------------
void table_facture_status::check_mandatory_status(const std::string & p_status)
{
  std::vector<facture_status> l_result;
  this->get_by_name(p_status,l_result);
  std::vector<facture_status>::const_iterator l_iter = l_result.begin();
  std::vector<facture_status>::const_iterator l_iter_end = l_result.end();
  bool l_found = false;
  while(l_iter != l_iter_end && !l_found)
    {
      l_found = (l_iter->get_name() == p_status);
      ++l_iter;
    }
  if(!l_found)
    {
      facture_status l_status(p_status);
      this->create(l_status);
    }
}

//EOF
