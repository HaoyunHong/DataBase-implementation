#include "system.h"

DATABASE &SYSTEM::operator[](const int &k)
{
    return m_database[k];
}
std::vector<DATABASE> &SYSTEM::get_database() { return m_database; }
DATABASE &SYSTEM::search_database(const std::string& s)
{
    int length = m_database.size();
    for (int i = 0; i < length; i++)
    {
        if (s == m_database[i].get_name())
            return m_database[i];
    }
}
void SYSTEM::show_databases()
{
    int length = m_database.size();
    if(length == 0)
    {
        printf("%s\n", "There aren't any databases!");
        return;
    }
    for(int i = 0; i < length; i++)
    {
        printf("%s\n", m_database[i].get_name());
    }
}