#include "column.h"

COLUMN::COLUMN(std::string s, int i) : m_name(s), m_is_primary(false), m_type(i) {}
COLUMN::~COLUMN() {} //纯虚析构函数必须被实现
INT_COLUMN::INT_COLUMN(std::string s) : COLUMN(s, 0) {}
DOUBLE_COLUMN::DOUBLE_COLUMN(std::string s) : COLUMN(s, 1) {}
CHAR_COLUMN::CHAR_COLUMN(std::string s) : COLUMN(s, 2) {}

std::string &COLUMN::get_name() { return m_name; }
bool COLUMN::get_is_primary() { return m_is_primary; }
void COLUMN::set_is_primary()
{
    m_is_primary = true;
}
int COLUMN::get_type() { return m_type; }
std::vector<int> &INT_COLUMN::get_value() { return m_value; }
std::vector<double> &DOUBLE_COLUMN::get_value() { return m_value; }
std::vector<std::string> &CHAR_COLUMN::get_value() { return m_value; }
