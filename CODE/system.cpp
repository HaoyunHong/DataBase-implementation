#include "system.h"

DATABASE &SYSTEM::operator[](const int &k)
{
    return m_database[k];
}
std::vector<DATABASE> &SYSTEM::get_database() { return m_database; }