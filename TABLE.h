#pragma once
#include "COLUMN.h"

class TABLE
{ //一个表
private:
   std::vector<std::string> ColumnName; //各列名
   std::vector<DataType> ColumnType;    //各列的数据类型:{_INT,_DOUBLE,_CHAR}
   std::string KeyColumn = "";          //键值列的名字
   int RowNum = 0;                      //行数 可以当做键值列的长度吧
public:
   std::map<std::string, COLUMN *> TableMap; //一个map：列名->列指针

   void showcolumns();                                                                           //按给定格式打印各列信息
   void addcolumn(std::string clname, DataType ctype);                                           //添加一列；参数：(列名，类型）
   void addcolumn(std::string clname, DataType ctype, std::vector<ConstraintType> &Constraints); //添加一列；参数：(列名，类型，一个存放限制信息的vector)
   void SetKey(std::string keyname);                                                             //设置键值为keyname
   void Insert(std::string cname, int d);                                                        //在cname列添加一条数据；参数：(列名，数据)
   void Insert(std::string cname, char d);                                                       //在cname列添加一条数据；参数：(列名，数据)
   void Insert(std::string cname, double d);                                                     //在cname列添加一条数据；参数：(列名，数据)
   void InsertNull(std::string cname, DataType type);                                            //向cname列加入NULL数据
   void Update(std::string aname, int avalue, std::string condition);                            //参数（列名，目标值，条件语句）；功能：修改符合条件语句的行，使这些行的对应列的值为目标值；
   void Update(std::string aname, char avalue, std::string condition);                           //参数（列名，目标值，条件语句）；功能：修改符合条件语句的行，使这些行的对应列的值为目标值；
   void Update(std::string aname, double avalue, std::string condition);                         //参数（列名，目标值，条件语句）；功能：修改符合条件语句的行，使这些行的对应列的值为目标值；
   const std::vector<int> &Select(std::string name, std::string condition);                      //查找符合条件语句(whereclause)的行，并将行下标储存在vector里面
   void show_output_from_select(const std::vector<int>& outorder, std::string name);              //将查找到的符合条件语句的行按要求输出
   void Delete(std::string condition);                                                           //删除符合条件语句的行
   DataType GetType(std::string name);                                                           //获得行name的类型
   bool Judge(std::string condition, int k);                                                     //功能：接收whereclause语句，一个整数行标（从0到行数-1），输出bool,判断此行是否满足语句的条件

   ~TABLE()
   {
      for (auto it = TableMap.begin(); it != TableMap.end(); it++)
      {
         if (it->second != NULL)
         {
            delete it->second;
         }
      }
   }
   void UpdateRow();                                //更新行数
   bool MyCompare(int a, int b, std::string cname); //比较cname列的第a+1与b+1位数据的大小
   std::vector<std::string> &GetColumnName()
   { //获得各列的名字
      return ColumnName;
   }
   void Del(int k);                     //删除第k+1行
   COLUMN *GetColumn(std::string name); //获得名字为name的列的列指针
};

inline void split(const std::string &s, std::vector<std::string> &sv, const char flag = ' '); //分割字符串
