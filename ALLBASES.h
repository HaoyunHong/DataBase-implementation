#pragma once
#include "DATABASE.h"

class ALLBASES
{ //所有数据的类
private:
    std::vector<std::string> DBName; //数据库名的集合
public:
    std::map<std::string, DATABASE *> AllBasesMap; //数据库名->数据库指针

    ~ALLBASES()
    { //析构函数
        for (auto it = AllBasesMap.begin(); it != AllBasesMap.end(); it++)
        {
            if (it->second != NULL)
            {
                delete it->second;
            }
        }
    }
    DATABASE *GetDB(std::string name); //获得名为name的数据库指针
    void create(std::string dbname);   //建立名为dbname的新数据库
    void del(std::string name);        //Drop名为name的数据库
    void show();                       //按字典序列出当前数据库
};
