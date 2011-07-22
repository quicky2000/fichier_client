#include "coherency_report_item.h"
#include <iostream>
#include <stdlib.h>

//------------------------------------------------------------------------------
coherency_report_item::coherency_report_item(void):
  m_report_level(UNDEFINED),
  m_id(0),
  m_table_name(""),
  m_message("")
{

}

//------------------------------------------------------------------------------
coherency_report_item::coherency_report_item(t_report_level p_report_level,uint32_t p_id,const std::string & p_table_name,const std::string & p_message):
  m_report_level(p_report_level),
  m_id(p_id),
  m_table_name(p_table_name),
  m_message(p_message)
{

}

//------------------------------------------------------------------------------
coherency_report_item::t_report_level coherency_report_item::get_report_level(void)const
{
  return m_report_level;
}

//------------------------------------------------------------------------------
uint32_t coherency_report_item::get_id(void)const
{
  return m_id;
}

//------------------------------------------------------------------------------
const std::string & coherency_report_item::get_table_name(void)const
{
  return m_table_name;
}

//------------------------------------------------------------------------------
const std::string & coherency_report_item::get_message(void)const
{
  return m_message;
}

//------------------------------------------------------------------------------
const std::string coherency_report_item::to_string(t_report_level p_report_level)
{
  std::string l_result = "";
  switch(p_report_level)
    {
    case WARNING:
      l_result = "Warning";
      break;
    case ERROR:
      l_result = "Erreur";
      break;
    case UNDEFINED:
      l_result = "Non defini";
      break;
    default:
      std::cout << "Unknown report_level value " << p_report_level << std::endl ;
      exit(-1);
      break;
    }
  return l_result;
}

//EOF
