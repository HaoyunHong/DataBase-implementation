# 大作业第二阶段说明文档

## **第一阶段框架代码分析与修改**
### 1、对第一阶段代码及第二阶段需求的分析：
第一阶段代码的复用性强，接口完备，对实现第二阶段的功能带来一定便利，但对于不同数据类型的处理仍然依赖if-else语句进行处理，可能带来较大的工作量。

第二阶段大部分需求都基于SELECT关键字，因此需要对PARSE::EXEC()函数(命令处理函数)中识别SELECT类型命令的分支代码进行较精细的修改。

数据的导出：
是SELECT语句的一种，需要识别关键字"INTO","OUTFILE"等，使用表相关函数读取表数据，并利用ifstream相关函数将数据导出到文件

数据的导入：
是LOAD语句，需要增加语句处理新分支。需要利用ofstream相关函数读取文件数据，并利用表相关函数插入数据

COUNT函数：

GROUP BY分组语句：

ORDER BY 排序语句：


多表whereclause：
需要修改原代码中默认FROM之后表名只有一个的部分，并且对输入的列名以及whereclause里的列名进行处理(因为多表whereclause中的列名表示法为table_name.column_name)

UNION：



### 2、对第一阶段方法的修改：
#### 2.1、对Select方法的修改

原有select方法包括了查找符合要求的行和输出两种功能,为了实现多表whereclause以及含UNION,JOIN,ORDER,GROUP BY的SELECT语句，将查找、输出功能分离开来

原函数：
```
void TABLE::Select(string name, string condition){......}
```
修改之后的查找函数：
```
const std::vector<int> &TABLE::Select
(const std::vector<std::string> &col_name, string condition){......}
//函数内存在一个static std::vector<int>，在函数体内通过clear()维护稳定性
储存的是符合whereclause子句condition的所有行的下标，即找到所有符合要求的行
这一函数的返回值是这个静态vector变量的常量引用
作用范围：单表
作用其他特点：对符合要求的行按照主键顺序排序，方便了之后直接调用各种输出函数
```

修改之后的输出函数:
```
void TABLE::show_output_from_select
(const std::vector<std::string> &col_name,
 const std::vector<int> &outorder)
 //用于普通select语句，输出一个表中某些行的语句，附带打印表头
 ```




### 3、第二阶段基础功能实现
#### 3.1、数据导入和导出
主要利用fstream相关函数进行导入和导出，附加字符串处理

导入：

在select部分的处理中增加分支，调用下列函数：
```
auto outorder = curTb->Select(col_name, condition); 
//outorder储存了需要输出的行的下标
curTb->write_into_outfile(out_file_name, outorder, col_name);
```

其中write_into_outfile函数声明如下：
```
void TABLE::write_into_outfile(const std::string &out_file_name, const std::vector<int> &outorder, const std::vector<std::string> &col_name);
//函数实现位于TABLE.cpp的第1056行
```
其实现中包含了对输出行数是否为0的判断（如为0则不需要进行输出），以及对"NULL"数据的特殊处理。

同时，我们添加了附加功能，支持在导出语句的后端增加whereclause语句，同时支持对任意指定列的输出,附加功能的实现得益于第一部分对原代码中select函数的修改。

我们的测试代码如下（见文件夹"输出到文件支持whereclause语句"）：
```
CREATE DATABASE OOP;
USE OOP;
CREATE TABLE poi(lkj INT, mnb INT, bvc INT, dfj CHAR, PRIMARY KEY(mnb));
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (1, 2, 3, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (2, 3, 4, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (3, 4, 5, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (4, 5, 6, "a");
INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (5, 6, 7, "a");
INSERT INTO poi(mnb, bvc, dfj) VALUES (7, 7, "a");
DELETE FROM poi WHERE lkj>3 OR mnb=7 AND lkj=10;
SELECT * from poi;
SELECT * INTO OUTFILE 'output_file' FROM poi WHERE bvc>4;
SELECT bvc INTO OUTFILE 'outout_file2' FROM poi WHERE bvc<7;
DROP DATABASE OOP;
```
相应的输出如下：
```
powershell输出：
lkj     mnb     bvc     dfj
1       2       3       a
2       3       4       a
3       4       5       a
NULL    7       7       a
输出文件output_file:
3	4	5	a	
NULL	7	7	a	
输出文件outout_file2:
3	
4	
5	
```

导出：

在PARSE函数中增加对"LOAD"关键字开头的语句的处理，并调用下列函数：
```
void TABLE::load_data_from_file(const std::string &in_file_name, const std::vector<<std::string>> &col_name);
//函数实现位于TABLE.cpp的第1166行
```
函数内部主要利用ifstream类相关方法读取文件中的数据。
由于LOAD语句关键词的特殊性，我们增加了在输入不合法时的报错语句，如：
```
if (ele0_upper != "DATA")
{
    cout << ele0_upper << endl;
    cout << "invalid command :: expect \"DATA\" here but not found." << endl;
    return;
}
if (ele0_upper != "INFILE")
{
    cout << "invalid command :: expect \"INFILE\" here but not found." << endl;
    return;
}
```
同时，我们增加了对于导入信息不完整时的默认值填充处理，对没有输入的列默认用0（int类型）/0.0（double类型）/char(0)（char类型）填充：
```
if (col_name.size() != ColumnName.size())//如果命令输入的列数少于总列数
{
	for (int i = 0; i < ColumnName.size(); i++)
	{
		col_map[ColumnName[i]] = false;
	}
	for (int i = 0; i < col_name.size(); i++)
	{
		col_map[col_name[i]] = true; //代表这些列会被输入
	}
	for (int i = 0; i < ColumnName.size(); i++)
	{
		if (!col_map[ColumnName[i]]) //如果这个列没有接受输入数据？？
		{
			cout << "Column " << ColumnName[i] << " does not have any input." << endl;
			if (GetType(ColumnName[i]) == _INT)
			{
				cout << "Automatically filling 0(int)as default." << endl;
			}
			else if (GetType(ColumnName[i]) == _DOUBLE)
			{
				cout << "Automatically filling 0.0(double) as default." << endl;
			}
			else if (GetType(ColumnName[i]) == _CHAR)
			{
				cout << "Automatically filling 0(ascii to char) as default." << endl;
			}
		}
	}
}
```


### 4、第二阶段附加功能实现
#### 4.1、多表whereclause语句
设计思路：

所有的变量都是"表名.列名"的形式，且输出的每一行都是每个表中各取一行并组合之后得到的。
所以，先将表按顺序排列并对每张表的每一行进行遍历。

先对表达式进行处理，找到含某个表信息的所有语句并替换为常数，直到剩下的表达式只含一个表名table，再将剩余"表名.列名"中的表名去掉，得到与单表whereclause相同的语句，复用之前的函数进行处理。

添加了下列处理表达式的函数：
```
std::string PARSE::constify(std::string condition, const std::vector<std::string> &tbname, TABLE *ptb, int line_num, const std::vector<int> outorder, const std::vector<std::string> &col_name, const std::vector<std::string> &col_tb_name)
//函数实现位于PARSE.cpp的第971行
```
功能：将whereclause子句condition，对于表指针ptb指向的表的第line_num行，进行“常数化”处理，并返回处理后的字符串，剩下的传入值都是从whereclause中读取到的信息，只供查阅和报错使用。

此外，为了将多表和单表兼容，在SELECT中，不再使用单个字符串对表名进行读入，而是使用vector存储，对vector的size进行判断，size=1的时候完全复用原有代码。






