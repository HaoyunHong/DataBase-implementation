# 大作业第二阶段实验报告

## **第一阶段框架代码分析与修改**
### 1、对第一阶段代码及第二阶段需求的分析
第一阶段代码的复用性强，接口完备，对实现第二阶段的功能带来一定便利，但对于不同数据类型的处理仍然依赖if-else语句进行处理，可能带来较大的工作量。

第二阶段大部分需求都基于SELECT关键字，因此需要对PARSE::EXEC()函数(命令处理函数)中识别SELECT类型命令的分支代码进行较精细的修改。

* 数据的导出：
是SELECT语句的一种，需要识别关键字"INTO","OUTFILE"等，使用表相关函数读取表数据，并利用ifstream相关函数将数据导出到文件

* 数据的导入：
是LOAD语句，需要增加语句处理新分支。需要利用ofstream相关函数读取文件数据，并利用表相关函数插入数据

* COUNT函数：
在SELECT语句中使用，用于对全部（非空）记录进行计数，在使用时地位等同于第一阶段SELECT语句中的列名，故可相似处理

* GROUP BY分组语句：
将表中某一字段或某些字段相同的记录归为一类，并需要在此基础上完成COUNT函数等功能，可在TABLE类中加一vector变量作为分类器，将分为同一类的记录标为相同

* ORDER BY 排序语句：
在GROUP语句的基础上使用，对搜索得到的数据可以自定输出的顺序，可在group的分类器中每一类找出一个代表元素进行排序，最后统一输出

* 多表whereclause：
需要修改原代码中默认FROM之后表名只有一个的部分，并且对输入的列名以及whereclause里的列名进行处理(因为多表whereclause中的列名表示法为table_name.column_name)

* UNION：
用于连接两个表的SELECT语句句的结果组合到一个结果集合中，可选是否删除重复数据，并且可在此基础上使用ORDER语句，可借鉴归并排序的思想，在两个表各自的记录中，使用ORDER中的方法按要求字段排序，然后依次比较两个表中排在首位的记录，直到一表全部记录输出后，输出另一表剩余所有记录

### 2、对第一阶段方法的扩展
#### 2.1、对Select方法的修改

原有select方法包括了查找符合要求的行和输出两种功能,为了实现多表whereclause以及含UNION,JOIN,ORDER,GROUP BY的SELECT语句，将查找、输出功能分离开来。

原函数：

	void TABLE::Select(string name, string condition){......}

修改之后的查找函数：

	const std::vector<int> &TABLE::Select
	(const std::vector<std::string> &col_name, string condition){......}
	//函数内存在一个static std::vector<int>，在函数体内通过clear()维护稳定性
	储存的是符合whereclause子句condition的所有行的下标，即找到所有符合要求的行这一函数的返回值是这个静态vector变量的常量引用
	作用范围：单表
	作用其他特点：对符合要求的行按照主键顺序排序，方便了之后直接调用各种输出函数


修改之后的输出函数:

	void TABLE::show_output_from_select
	(const std::vector<std::string> &col_name, const std::vector<int> &outorder)
	//用于普通select语句，输出一个表中某些行的语句，附带打印表头

### 3、第二阶段基础功能实现
#### 3.1、数据导入和导出
主要利用fstream相关函数进行导入和导出，附加字符串处理

导出：

在select部分的处理中增加分支，调用下列函数：

	auto outorder = curTb->Select(col_name, condition); 	//outorder储存了需要输出的行的下标
	curTb->write_into_outfile(out_file_name, outorder, col_name);

其中write_into_outfile函数声明如下：

	void TABLE::write_into_outfile(const std::string &out_file_name, const std::vector<int> &outorder, const std::vector<std::string> &col_name);
	//函数实现位于TABLE.cpp的第1056行

其实现中包含了对输出行数是否为0的判断（如为0则不需要进行输出），以及对"NULL"数据的特殊处理。

同时，我们添加了附加功能，支持在导出语句的后端增加whereclause语句，同时支持对任意指定列的输出,附加功能的实现得益于第一部分对原代码中select函数的修改。

我们的测试代码如下（见文件夹"输出到文件支持whereclause语句"）：

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

相应的输出如下：

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


导入：

在PARSE函数中增加对"LOAD"关键字开头的语句的处理，并调用下列函数：

	void TABLE::load_data_from_file(const std::string &in_file_name,
	const std::vector<<std::string>> &col_name);
	//函数实现位于TABLE.cpp的第1166行

函数内部主要利用ifstream类相关方法读取文件中的数据。
由于LOAD语句关键词的特殊性，我们增加了在输入不合法时的报错语句，如：

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
同时，我们增加了对于导入信息不完整时的默认值填充处理，对没有输入的列默认用0（int类型）/0.0（double类型）/char(0)（char类型）填充：

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

例如，输入测试代码：

	CREATE DATABASE OOP;
	USE OOP;
	CREATE TABLE oop_info(stu_id INT NOT NULL, stu_name CHAR, stu_height INT, PRIMARY KEY(stu_id));
	LOAD DATA INFILE 'output_file' INTO TABLE oop_info(stu_id, stu_name);
	SELECT * FROM oop_info;
	DROP DATABASE OOP;

其中output_file内容为：

	2018011343	a
	2018011344	b
	2018011345	c

此处，输入文件中只有两列，但表有三列，程序会自动用默认值补全没有导入要求的列，并在程序运行界面给予提示，输出结果为：

	Column stu_height does not have any input.
	Automatically filling 0(int) as default.
	stu_id	stu_name	stu_height	
	2018011343	a	0	
	2018011344	b	0	
	2018011345	c	0	
### 4、第二阶段附加功能实现
#### 4.1、多表whereclause语句
##### 设计思路：

所有的变量都是"表名.列名"的形式，且输出的每一行都是每个表中各取一行并组合之后得到的。
所以，先将表按顺序排列并对每张表的每一行进行遍历。

先对表达式进行处理，找到含某个表信息的所有语句并替换为常数，直到剩下的表达式只含一个表名table，再将剩余"表名.列名"中的表名去掉，得到与单表whereclause相同的语句，复用之前的函数进行处理。

##### 核心函数：
添加了下列处理表达式的函数：

	std::string PARSE::constify(std::string condition,
	const std::vector<std::string> &tbname,
	TABLE *ptb, int line_num,
	const std::vector<int> outorder,
	const std::vector<std::string> &col_name,
	const std::vector<std::string> &col_tb_name)
	//函数实现位于PARSE.cpp的第971行
功能：将whereclause子句condition，对于表指针ptb指向的表的第line_num行，进行“常数化”处理，并返回处理后的字符串，剩下的传入值都是从whereclause中读取到的信息，只供查阅和报错使用。

##### 函数功能示例：
对于样例中的whereclause子句

	life1.grade<life2.eat AND life1.girls="a";

锁定表life1的第一行，经过一次constify(condition, ......)之后，life1.grade被替换为表life中第一行grade列的常数值95，life1.girls也被替换为第一行girls列的常数值"a"
进入下一层时，life2作为最后一个表名，life2.eat被直接替换为eat，得到与单表whereclause相同形式的语句

	95<eat AND "a"="a";

之后对life1表中后面每一行执行相同的操作

##### 其他改良：
为了同时支持多表和单表，在SELECT中，不再使用单个字符串对表名进行读入，而是使用vector存储，对vector的size进行判断，size=1的时候完全复用原有代码。
修改了原Judge函数只支持左变量右常量的比较语句这一局限之处，在实现多表的同时，也使得单表的whereclause子句能够支持更多形式。
与计算器形式的区分（在后面逻辑运算符、算术运算符部分会提及）

##### 优点：
没有对第一阶段功能的实现部分做出任何改动，在此基础上以增加函数的形式实现了第二阶段附加功能的核心部分

极大程度复用了第一阶段whereclause判断的代码，没有花太多时间在修改判断函数上（仅仅是增加了两种情况的讨论）

将不同表之间的行组合这一繁琐的便利过程使用"表达式常数化"来化为先前已经能够处理的基本问题

##### 局限性：
由于规模和时间问题，目前只支持两张表之间的多表whereclause（三张表需要的遍历层数更多，复杂度为指数级）
算法速度还有待提高，线性遍历的效率仍然不高

#### 4.2、UNION关键字
#### 4.3、sql数字函数
#### 4.4、sql算数运算符
#### 4.5、sql逻辑运算符
#### 4.6、存档和读档
##### 设计思路：
此功能难度主要在于读档，要从零开始创建与原来完全相同的数据库和表，除了要在文件中储存数据之外，还需要储存表名、数据库名、列名、主键等关键信息。这些都是基础功能中写入文件这一功能的更高要求。

因此，我们先利用

	system("mkdir -p data");
这一语句创建名为"data"的文件夹作为存档的路径文件夹，便于存档和之后访问

之后，创建

	index.txt 
	[database_name].txt 
	[databasename.tablename].txt 
index.txt为总目录，储存数据库数量以及每个数据库的名字；
文件名如[database_name].txt的文件储存名为database_name的数据库的信息，包括表的数量以及每张表的名字；
文件名如[database_name.table_name].txt的文件储存名为database_name的表中名为table_name的表的信息，包括列数、各列名称、各列数据类型、主键名字、每一列从第一行开始到最后一行的所有数据（每个单元格之间用'\t'分隔，每列之间用'\n'分隔）

例如：现有数据库oop，fop，oop下有两张表：class1，class2，fop下有两张表，class3，class4.
则data文件夹里将会存在以下文件：

	index.txt
	oop.txt, fop.txt
	oop.class1.txt, oop.class2.txt
	fop.class3.txt, fop.class4.txt

读档时，利用相关函数按照次序读档并建立数据库、建立表、添加列、加入数据。

##### 核心函数：
创建存档文件的函数：

	void create_table_file(const std::string &dbname); //表(TABLE)类函数，存档
	void create_database_file();//数据库(DATABASES)类函数，存档
	void create_allbases_file();//数据总库（ALLBASES）类函数，创建index.txt

读取存档文件并新建内容的函数：

	void load_all_databases();//数据总库（ALLBASES）类函数，读index.txt，创建数据库
	void load_database();//数据库（DATABASE）类函数，读相关数据库存档，创建表
	void load_table(const std::string &dbname);//表（TABLE）类函数，读相关表存档，创建列并写入数据

说明：
表的存读档函数需要传入数据库名，因为这两者独立性相对于另外几者而言更强（考虑到原框架的设计）

不同层之间的函数有相互调用关系，例如，create_allbases_file()会调用ALLBASES类中每一个DATABASE成员的create_database_file()函数，而后者又会调用这个DATABASE类中每一个TABLE类的create_table_file()函数

但这一结构也同时支持对低层函数的单独调用，我们也额外增加了只对某个表/数据库进行存档的命令（SAVE命令）

##### 新添加的命令：

	RELOAD; //程序刚开始运行时，直接使用LOAD会从data文件夹中搜索上次的存档，并进行读档
	SAVE TABLE tbname;//将名为tbname的表进行存档，即对这个表对应的.txt文件进行重写或者存档
	SAVE DATABASE dbname;//将名为dbname的数据库进行存档，即对这个数据库对应的.txt文件以及其下辖所有表的.txt文件进行重写或者存档
	SAVE ALL;//对所有内容进行存档
	这些命令用于手动存读档，在每个会对存储数据进行操作的分支后，都有自动存档语句，无需使用命令就可自动存档
	例如：

		else if (ele1 == "UPDATE")
		{
			......//操作语句，省略
			curTb->create_table_file(curDb->get_name());
		}//UPDATE操作只对一个表进行了修改，所以重新覆盖该表对应的数据文件即可

		//DELETE 和 INSERT 操作同理，不再列举


		if (ele1 == "CREATE")
		{
			......//操作语句，省略
			if(ele2 == "TABLE")//指代"CREATE TABLE......"命令
			{
				......
				curDb->create_database_file();//需要修改当前数据库的文件
			}
			else if(ele2 == "DATABASE")//指代"CREATE DATABASE......"命令
			{
				......
				Allbases.create_allbases_file();//需要修改总文件index.txt
			}
		}
		//DROP 操作同理，不再列举

##### 实现方式：
核心为fstream类，通过访问文件的方式写入/获得数据

存档函数（以第一个函数为例）：

	void ALLBASES::create_allbases_file()
	{
		std::string filename = "./data/" + std::string("index.txt");//存档路径
		ofstream fst(filename);//打开相应文件
		int h = DBName.size();//获取总数据库数
		fst << h;//输出数据库总数到文件
		fst << "\n";//换行符分隔
		for (int i = 0; i < DBName.size(); i++)
			fst << DBName[i] << "\n";//输出每个数据库的名字，换行符分隔
		fst.flush();
		fst.close();//关闭文件

		for (int i = 0; i < DBName.size(); i++)
		{
			AllBasesMap[DBName[i]]->create_database_file();
		}//调用下一层函数（需要对每个数据库生成存档文件，避免出现数据库已创建但找不到存档文件的问题）
	}

读档函数（以第一个函数为例）：

	void ALLBASES::load_all_databases()
	{
		fstream fst;
		string filename = "./data/" + string("index.txt");//完整路径名
		fst.open(filename); //默认把总管文件命名为index.txt
		int database_num;
		fst >> database_num;//数据库总数，此数据保存在文档的最前面
		std::string dbname;//反复输入这一个字符串遍历所有数据库名
		fst.get();//读掉换行符
		for(int i = 0; i < database_num; i++)
		{
			getline(fst, dbname);
			create(dbname);   //建立名为dbname的新数据库
		}
		fst.close();
		for(int i = 0; i < DBName.size(); i++)
			AllBasesMap[DBName[i]]->load_database();//调用下一层函数
	} 

主要实现方式依赖于fstream类的相关函数

##### 存档文件示例和重新读档运行界面说明
以以下测试代码为例（数据库附加功能测试/存档与读档1.sql）：

	CREATE DATABASE qaz;
	CREATE DATABASE edc;
	CREATE DATABASE ijn;
	USE ijn;
	CREATE TABLE nba(team INT, player INT, income DOUBLE, influence DOUBLE, PRIMARY KEY(team));
	CREATE TABLE cba(team INT, player INT, income DOUBLE, influence DOUBLE, PRIMARY KEY(team));
	CREATE TABLE wnba(team INT, player INT, income DOUBLE, influence DOUBLE, PRIMARY KEY(team));
	INSERT INTO cba(team, player, income, influence) VALUES (2, 8, 700.0, 100.0);
	INSERT INTO cba(team, player, income, influence) VALUES (1, 9, 720.0, 190.0);
	INSERT INTO cba(team, player, income, influence) VALUES (9, 2, 990.0, 180.0);
	INSERT INTO cba(team, player, income, influence) VALUES (1, 5, 788.0, 600.0);
	INSERT INTO nba(team, player, income, influence) VALUES (100, 20, 7222.0, 60000.0);
	INSERT INTO nba(team, player, income, influence) VALUES (80, 50, 7111.0, 61293.0);
	INSERT INTO wnba(team, player, income, influence) VALUES (7, 4, 9.3, 6.0);
	SAVE TABLE cba FROM DATABASE ijn;
	SHOW DATABASES;
	CREATE DATABASE acs;
	SHOW DATABASES;
	//总计三个数据库：qaz,edc,ijn
	qaz、edc数据库下没有表，ijn数据库下有nba,cba,wnba三张表
	表cba中有4条记录，nba中有2条记录，wnba中有1条记录

运行结束后，data文件夹中存在以下文件：
	
	acs.txt
	edc.txt
	ijn.cba.txt
	ijn.nba.txt
	ijn.txt
	ijn.wnba.txt
	index.txt
	qaz.txt
	(按照字母顺序排列)

其中index.txt内容如下：

	4
	edc
	ijn
	qaz
	acs
acs.txt和edc.txt内容如下：

	0
ijn.txt内容如下：

	3
	nba
	cba
	wnba
ijn.cba.txt内容如下（//之后的部分是标注说明，本来不存在于文件中）：

	4	 //列数
	4	 //行数
	team     //主键名称
	team
	_INT
	player
	_INT
	income
	_DOUBLE
	influence
	_DOUBLE //以上是每一列以及每一列的数据类型
	2	1	9	1	
	8	9	2	5	
	700.0000	720.0000	990.0000	788.0000	
	100.0000	190.0000	180.0000	600.0000	
	//每一列的数据（顺序与保存顺序相同，也与数据库创建时的顺序相同）

在输入上面的代码之后，关闭程序，再次输入下列代码（数据库附加功能测试/存档与读档2.sql）：

	RELOAD;
	SHOW DATABASES;
	USE ijn;
	SHOW TABLES;
	SELECT * FROM nba;
	SELECT * FROM cba where team=1;

此段代码功能为：读档、展示数据库、进入数据库ijn、展示数据库ijn中所有表、从表nba中选出所有数据、从表cba中选出符合team=1的数据

运行结果如下：

	Database
	acs
	edc
	ijn
	qaz
	Tables_in_ijn
	cba
	nba
	wnba
	team    player  income  influence
	80      50      7111.0000       61293.0000
	100     20      7222.0000       60000.0000
	team    player  income  influence
	1       9       720.0000        190.0000
	1       5       788.0000        600.0000

读档后，数据库程序回到了关闭之前的状态，并可以进行与之前完全相同的操作

##### 优势：
利用了之前的文件导入、导出函数进行改写；
使用文件名标记对应储存的表，并使用分层结构，用index.txt为总目录，分层查找，减小了遗漏数据的可能性；
按列存储，并在表文件中记录总行数，方便创建一列后马上输入数据，防止了多次更新"行数"的重复操作
将存档函数设置为每一个类的成员函数而不是作为最高层级的成员函数，使得分开调用、局部存档变为可能，比起每一次操作都覆盖所有文件的最初想法来说，复杂度降低了很多，并且可以根据操作层面的不同对文件做最小程度的修改；
用户退出程序后，可以自行选择删除存档或是保留存档，增加了项目的灵活度；

##### 缺点：
其实说是缺点，更多的是因为时间不允许而没有能够做到更好的遗憾吧

首先，原本的设想是利用fstream类的读、写双接口将修改文件的复杂度降低至线性复杂度以下，但因为时间原因没有能够实现，改为使用分层做法尽可能优化；

其次，对于创建文件夹这一命令，先后尝试了多种做法，包括<direct.h>中的mkdir函数、手动创建并拷贝路径、利用system()操纵shell等等，最终还是没有能够完全解决兼容性问题（mkdir函数在linux环境、windows环境下属于不同的库，system函数在检测到data文件夹时会在运行界面报出警告“data已存在”，虽然不影响运行但是不够美观）

最后，对于文件读取的效率应该还有优化空间。








