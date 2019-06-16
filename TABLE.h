#pragma once
#include "COLUMN.h"

class TABLE
{ //一个表
private:
	std::string name;					 //表名
	std::vector<std::string> ColumnName; //各列名
	std::vector<DataType> ColumnType;	//各列的数据类型:{_INT,_DOUBLE,_CHAR}
	std::string KeyColumn = "";			 //键值列的名字
	int RowNum = 0;						 //行数 可以当做键值列的长度吧

	std::vector<int> classifier;	   //在GROUP命令中使用，为指定的列中的数据分类，数据一样即分类相同，分类从0开始
	std::vector<int> num_of_each_kind; //记录每个种类的行数
	int kind;						   //记录分类的种数

public:
	std::map<std::string, COLUMN *> TableMap;																  //一个map：列名->列指针
	std::vector<int> column_to_order;																		  //在Select ... Order，Union中使用
	void showcolumns();																						  //按给定格式打印各列信息
	void addcolumn(std::string clname, DataType ctype);														  //添加一列；参数：(列名，类型）
	void addcolumn(std::string clname, DataType ctype, std::vector<ConstraintType> &Constraints);			  //添加一列；参数：(列名，类型，一个存放限制信息的vector)
	void SetKey(std::string keyname);																		  //设置键值为keyname
	void Insert(std::string cname, int d);																	  //在cname列添加一条数据；参数：(列名，数据)
	void Insert(std::string cname, char d);																	  //在cname列添加一条数据；参数：(列名，数据)
	void Insert(std::string cname, double d);																  //在cname列添加一条数据；参数：(列名，数据)
	void InsertNull(std::string cname, DataType type);														  //向cname列加入NULL数据
	void Update(std::string aname, int avalue, std::string condition);										  //参数（列名，目标值，条件语句）；功能：修改符合条件语句的行，使这些行的对应列的值为目标值；
	void Update(std::string aname, char avalue, std::string condition);										  //参数（列名，目标值，条件语句）；功能：修改符合条件语句的行，使这些行的对应列的值为目标值；
	void Update(std::string aname, double avalue, std::string condition);									  //参数（列名，目标值，条件语句）；功能：修改符合条件语句的行，使这些行的对应列的值为目标值；
	const std::vector<int> &Select(const std::vector<std::string> &col_name, std::string condition);		  //查找符合条件语句(whereclause)的行，并将行下标储存在vector里面
	void show_output_from_select(const std::vector<std::string> &col_name, const std::vector<int> &outorder); //将查找到的符合条件语句的行按要求输出
	void show_output_from_col(const std::string &colname, const std::vector<int> &outorder, int k);			  //输出colname列，outorder这个vector中下标为k的项对应的行对应单元格内的数据
	void Delete(std::string condition);																		  //删除符合条件语句的行
	DataType GetType(std::string name);																		  //获得列name的类型
	bool Judge(std::string condition, int k);
	std::string Get_KeyColumn(); //功能：接收whereclause语句，一个整数行标（从0到行数-1），输出bool,判断此行是否满足语句的条件

	TABLE(const std::string a) : name(a) {}
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
	void UpdateRow();								 //更新行数
	bool MyCompare(int a, int b, std::string cname); //比较cname列的第a+1与b+1位数据的大小
	const std::vector<std::string> &GetColumnName()
	{ //获得各列的名字
		return ColumnName;
	}
	void Del(int k);					 //删除第k+1行
	COLUMN *GetColumn(std::string name); //获得名字为name的列的列指针
	int GetRowNum() { return RowNum; }
	int Count(std::string expression);						  //实现COUNT语句,返回一个int
	void classify(const std::vector<std::string> &group_col); //实现对选中分类，在GROUP语句中使用
	void Select_Group(const std::vector<std::string> &col_name, bool has_order);
	void Select_Order(const std::vector<std::string> &col_name, const std::string &order_col);
	void Order_in_Union(const std::vector<std::string> &col_name, const std::string &order_col);
	void write_into_outfile(const std::string &out_file_name, const std::vector<int> &outorder, const std::vector<std::string> &col_name); //输出到目标文件
	void load_data_from_file(const std::string &in_file_name, const std::vector<std::string> &col_name);
	void bubble_sort(std::vector<int> &order);		   //按照关键列对行下标的vector进行冒泡排序
	void create_table_file(const std::string &dbname); //存档
	void load_table(const std::string &dbname);		   //读档
};

inline void split(const std::string &s, std::vector<std::string> &sv, const char flag = ' '); //分割字符串