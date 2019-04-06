#include "database.h"

TABLE &DATABASE::operator[](const int &k)
{
    return m_table[k];
}
std::vector<TABLE> &DATABASE::get_table() { return m_table; }
std::string &DATABASE::get_name() { return m_name; }