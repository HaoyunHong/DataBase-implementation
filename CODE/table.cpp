#include "table.h"

COLUMN *TABLE::operator[](const int &k)
{
    return m_column[k];
}
std::vector<COLUMN *> &TABLE::get_column() { return m_column; }
std::string &TABLE::get_name() { return m_name; }