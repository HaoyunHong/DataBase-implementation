#include "database.h"

TABLE &DATABASE::operator[](const int &k)
{
    return m_table[k];
}
std::vector<TABLE> &DATABASE::get_table() { return m_table; }
std::string &DATABASE::get_name() { return m_name; }
TABLE &DATABASE::search_table(const std::string &s)
{
    int length = m_table.size();
    for (int i = 0; i < length; i++)
    {
        if (s == m_table[i].get_name)
            return m_table[i];
    }
}
void DATABASE::show_tables()
{
    int length = m_table.size();
    if(length == 0)
    {
        printf("%s\n", "There aren't any tables!");
        return;
    }
    for (int i = 0; i <= length; i++)
        printf("%s\n", m_table[i].get_name());
}
DATABASE::DATABASE(std::string s) : m_name(s) {}
