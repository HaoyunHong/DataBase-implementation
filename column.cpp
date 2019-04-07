#include "column.h"

COLUMN::COLUMN(std::string s, int i) : m_name(s), m_is_primary(false), m_type(i) {}
COLUMN::~COLUMN() {} //纯虚析构函数必须被实现
INT_COLUMN::INT_COLUMN(std::string s) : COLUMN(s, 0) {}
DOUBLE_COLUMN::DOUBLE_COLUMN(std::string s) : COLUMN(s, 1) {}
CHAR_COLUMN::CHAR_COLUMN(std::string s) : COLUMN(s, 2) {}

std::string &COLUMN::get_name() { return m_name; }//获取名字
bool COLUMN::get_is_primary() { return m_is_primary; }//获取是否主键
void COLUMN::set_is_primary()
{
    m_is_primary = true;
}//将某列设置为主键
int COLUMN::get_type() { return m_type; }//获取数据类型
std::vector<int> &INT_COLUMN::get_value() { return m_value; }
std::vector<double> &DOUBLE_COLUMN::get_value() { return m_value; }
std::vector<std::string> &CHAR_COLUMN::get_value() { return m_value; }//获取存储数据的vector
