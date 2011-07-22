#ifndef _COHERENCY_REPORT_ITEM_H_
#define _COHERENCY_REPORT_ITEM_H_

#include <stdint.h>
#include <string>

class coherency_report_item
{
 public:
  typedef enum {WARNING,ERROR,UNDEFINED} t_report_level;

  coherency_report_item(void);
  coherency_report_item(t_report_level p_report_level,uint32_t p_id,const std::string & p_table_name,const std::string & p_message);
  t_report_level get_report_level(void)const;
  uint32_t get_id(void)const;
  const std::string & get_table_name(void)const;
  const std::string & get_message(void)const;

  static const std::string to_string(t_report_level p_report_level);
 private:
  t_report_level m_report_level;
  uint32_t m_id;
  std::string m_table_name;
  std::string m_message;  
};

#endif /* _COHERENCY_REPORT_ITEM_H_ */
