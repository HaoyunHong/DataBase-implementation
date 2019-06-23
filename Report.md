[TOC]

# 大作业第二阶段实验报告

## 1、对第一阶段代码及第二阶段需求的分析
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

## 2、对第一阶段方法的扩展
### 2.1、对Select方法的修改

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

## 3、第二阶段基础功能实现
### 3.1、数据导入和导出
主要利用fstream相关函数进行导入和导出，附加字符串处理

导出：

在select部分的处理中增加分支，调用下列函数：

	auto outorder = curTb->Select(col_name, condition); 	//outorder储存了需要输出的行的下标
	curTb->write_into_outfile(out_file_name, outorder, col_name);

其中write_into_outfile函数声明如下：

	void TABLE::write_into_outfile(const std::string &out_file_name, const std::vector<int> &outorder, const std::vector<std::string> &col_name);
	//函数实现位于TABLE.cpp的第1215行

其实现中包含了对输出行数是否为0的判断（如为0则不需要进行输出），以及对"NULL"数据的特殊处理。

导入：

在PARSE函数中增加对"LOAD"关键字开头的语句的处理，并调用下列函数：

	void TABLE::load_data_from_file(const std::string &in_file_name,
	const std::vector<<std::string>> &col_name);
	//函数实现位于TABLE.cpp的第1325行

* 函数内部主要利用ifstream类相关方法读取文件中的数据，因为存档和读档的格式统一，所以此处不再赘述文档结构
* 通过查找命令中的列名并一一对应，再从输入文件中读取数据，按行利用第一阶段的insert相关方法插入表
  
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
	}//与此相似的还有几个

### 3.2、COUNT函数
在简单的SELECT语句（无GROUP和ORDER）中使用的COUNT函数由TABLE的以下成员函数实现：

	int Count(std::string expression);

仅返回全部（非空）记录的数量。
在GROUP和ORDER语句中的COUNT有不同的功能和实现。

###3.3、GROUP BY分组语句和ORDER BY排序语句：

主要由TABLE中的以下几个成员函数实现：

	void classify(const std::vector<std::string> &group_col); //实现对选中分类，在GROUP语句中使用
	void Select_Group(const std::vector<std::string> &col_name, bool has_order); //实现GROUP
	void Select_Order(const std::vector<std::string> &col_name, const std::string &order_col); //实现ORDER

并涉及到以下成员变量：

	std::vector<int> classifier; //在GROUP命令中使用，为指定的列中的数据分类，数据一样即分类相同，分类从0开始
	std::vector<int> num_of_each_kind; //记录每个种类的行数
	int kind; //记录分类的种数

如在第一部分对第二阶段需求分析中描述一样，先按照GROUP作用的字段对所有记录进行分类，并记录每个种类的记录数量，为后续功能实现做准备。

GROUP语句的实现是在分类的基础上，在每一类中取一条记录作为代表，由于分组后非GROUP作用的字段可能会数据不同，故SELECT只能作用于GROUP作用的字段或是调用COUNT函数，此时等价于对取出的代表进行输出。

ORDER语句的实现也是建立在分类的基础上，对每一类取出的代表进行所需要的排序，此时等价于对所有分组排序。

## 4、第二阶段附加功能实现
### 4.1、多表whereclause语句
* 设计思路

所有的变量都是"表名.列名"的形式，且输出的每一行都是每个表中各取一行并组合之后得到的。
所以，先将表按顺序排列并对每张表的每一行进行遍历。

先对表达式进行处理，找到含某个表信息的所有语句并替换为常数，直到剩下的表达式只含一个表名table，再将剩余"表名.列名"中的表名去掉，得到与单表whereclause相同的语句，复用之前的函数进行处理。

* 核心函数

添加了下列处理表达式的函数：

	std::string PARSE::constify(std::string condition,
	const std::vector<std::string> &tbname,
	TABLE *ptb, int line_num,
	const std::vector<int> outorder,
	const std::vector<std::string> &col_name,
	const std::vector<std::string> &col_tb_name)
	//函数实现位于PARSE.cpp的第971行
功能：将whereclause子句condition，对于表指针ptb指向的表的第line_num行，进行“常数化”处理，并返回处理后的字符串，剩下的传入值都是从whereclause中读取到的信息，只供查阅和报错使用。

* 函数功能示例

对于样例中的whereclause子句

	life1.grade<life2.eat AND life1.girls="a";

锁定表life1的第一行，经过一次constify(condition, ......)之后，life1.grade被替换为表life中第一行grade列的常数值95，life1.girls也被替换为第一行girls列的常数值"a"
进入下一层时，life2作为最后一个表名，life2.eat被直接替换为eat，得到与单表whereclause相同形式的语句

	95<eat AND "a"="a";

之后对life1表中后面每一行执行相同的操作

* 其他改良

为了同时支持多表和单表，在SELECT中，不再使用单个字符串对表名进行读入，而是使用vector存储，对vector的size进行判断，size=1的时候完全复用原有代码。
修改了原Judge函数只支持左变量右常量的比较语句这一局限之处，在实现多表的同时，也使得单表的whereclause子句能够支持更多形式。
与计算器形式的区分（在后面逻辑运算符、算术运算符部分会提及）

* 优点

没有对第一阶段功能的实现部分做出任何改动，在此基础上以增加函数的形式实现了第二阶段附加功能的核心部分

极大程度复用了第一阶段whereclause判断的代码，没有花太多时间在修改判断函数上（仅仅是增加了两种情况的讨论）

将不同表之间的行组合这一繁琐的便利过程使用"表达式常数化"来化为先前已经能够处理的基本问题

* 局限性

由于规模和时间问题，目前只支持两张表之间的多表whereclause（三张表需要的遍历层数更多，复杂度为指数级）
算法速度还有待提高，线性遍历的效率仍然不高

### 4.2、UNION关键字

* 设计思路

UNION语句用于连接两个表的SELECT语句句的结果组合到一个结果集合中，可选是否删除重复数据，并且可在此基础上使用ORDER语句对输出进行排序。

* 核心函数

主要由TABLE类中的以下函数实现：

	void Order_in_Union(const std::vector<std::string> &col_name, const std::string &order_col, bool has_all);

上述函数实现了在两个目标表中，按照ORDER作用字段的顺序分别进行排序，在函数中有个参数是bool类型，用来判断是否是UNION ALL。如果存在ALL，则需要输出全部记录，则需要对两表内全部记录排序；否则，可以调用GROUP语句中的classify函数，并只对每一类的代表排序。

分别排序完成后，借鉴**归并排序**的思想，依次比较两表内首位记录，输出后移至下一位次，直至某一表为空，按顺序输出另一张表的剩余所有记录。

* 函数功能示例

对于给出测例"UNION.sql"中的指令

	CREATE DATABASE OOP_is_so_difficult;
	USE OOP_is_so_difficult;
	CREATE TABLE class_liu(score INT, gpa DOUBLE, name1 CHAR, PRIMARY KEY(name1));
	CREATE TABLE class_huang(score INT, gpa DOUBLE, name2 CHAR, PRIMARY KEY(name2));
	INSERT INTO class_liu(score, gpa, name1) VALUES (100, 4.0, "a");
	INSERT INTO class_liu(score, gpa, name1) VALUES (95, 4.0, "b");
	INSERT INTO class_liu(score, gpa, name1) VALUES (89, 3.6, "c");
	INSERT INTO class_liu(score, gpa, name1) VALUES (84, 3.3, "d");
	INSERT INTO class_huang(score, gpa, name2) VALUES (100, 4.0, "A");
	INSERT INTO class_huang(score, gpa, name2) VALUES (90, 4.0, "B");
	INSERT INTO class_huang(score, gpa, name2) VALUES (88, 3.6, "C");
	INSERT INTO class_huang(score, gpa, name2) VALUES (83, 3.3, "D");
	SELECT gpa, name1 from class_liu UNION gpa, name2 from class_huang ORDER BY gpa;
	SELECT gpa, name2 from class_huang UNION gpa, name1 from class_liu ORDER BY gpa;
	SELECT gpa, name1 from class_liu UNION ALL gpa, name2 from class_huang ORDER BY gpa;
	SELECT gpa, name2 from class_huang UNION ALL gpa, name1 from class_liu ORDER BY gpa;
	SELECT score, name1 from class_liu UNION score, name2 from class_huang ORDER BY score;
	SELECT score, name2 from class_huang UNION score, name1 from class_liu ORDER BY score;

运行程序后正确输出：

	gpa	name1	
	3.3	D	
	3.6	C	
	4	A	
	gpa	name2	
	3.3	d	
	3.6	c	
	4	a	
	gpa	name1	
	3.3	D	
	3.3	d	
	3.6	C	
	3.6	c	
	4	A	
	4	B	
	4	a	
	4	b	
	gpa	name2	
	3.3	d	
	3.3	D	
	3.6	c	
	3.6	C	
	4	a	
	4	b	
	4	A	
	4	B	
	score	name1	
	83	D	
	84	d	
	88	C	
	89	c	
	90	B	
	95	b	
	100	A	
	score	name2	
	83	D	
	84	d	
	88	C	
	89	c	
	90	B	
	95	b	
	100	a	

* 优点

借鉴归并排序算法进行算法实现，有效降低时间复杂度；复用GROUP以及ORDER语句中使用过的函数，减少代码量；对于非法输入进行报错。

* 缺点

对于可以接受的语句的形式比较单一，未能进行有效扩展或与第一阶段其他函数联系。

### 4.3、sql数字函数

* 设计思路：

sql数字函数的本质是用```select```关键词后加数字函数，对数据进行处理，在其中，我们实现了取绝对值```ABS```函数、取反余弦值```ACOS```函数、取反正弦值```ASIN```函数、取反正切值```ATAN```函数，还有向上取整```CEIL```函数、```CEILING```函数，一共6个函数。

* 功能示例：

取绝对值函数的输入：

	SELECT ABS(-1);
输出：

	1
取反余弦值的输入：

	SELECT ACOS(0.25);
输出：

	75.5225 degree
取反正弦值的输入：

	SELECT ASIN(0.25);
输出：

	14.4775 degree
取反正切值的输入：

	SELECT ATAN(2.5);
输出：

	68.1986 degree
向上取整的输入：

	SELECT CEIL(1.5);
	SELECT CEIL(1.0);
	SELECT CEILING(1.5);
	SELECT CEILING(1.00); 
输出：

	2
	1
	2
	1

* 优点

直接在```Parse.EXEC```函数体内```else if (ele1 == "SELECT")```的分支内加分支进行扩展，无需改动其它代码。

通过在判断时，使用大写字母化后的表达式进行比对的方法，实现对关键词和运算符的大小写不敏感。

实现了6个函数，大于5个，要想再加数字函数可以直接继续在分支里扩展。

* 局限性与展望

时间比较紧迫，没有与```whereclauses```有机结合，最理想的方式是像excel那样，对数值列中的数据进行数字函数操作，比如对数据表中的某个或某列数值数据取绝对值等，使数据库更加灵活。

### 4.4、sql字符串函数
* 设计思路

sql字符串函数是用select关键词加字符串函数，输出处理后的字符串，在其中，我们实现了获取字符串长度的```CHAR_LENGTH```函数、```CHARACTER_LENGTH```函数，按顺序连接字符串的```CONCAT```函数（默认连接符为" "，即空格）、按顺序连接字符串并给定连接符的```CONCAT_WS```函数一共4个函数。

* 功能示例

获取字符串长度的输入：

	SELECT CHAR_LENGTH("RUNOOB") AS LengthOfString;
	SELECT CHAR_LENGTH("RUNOOB")AS LengthOfString;
	SELECT CHARACTER_LENGTH("RUNOOB") AS LengthOfString;
	SELECT CHARACTER_LENGTH("RUNOOB")AS LengthOfString;
输出：

	The length of the entered string "RUNOOB" is 6.
	The length of the entered string "RUNOOB" is 6.
	The length of the entered string "RUNOOB" is 6.
	The length of the entered string "RUNOOB" is 6.
按顺序连接字符串的输入：

	SELECT CONCAT("SQL ", "Runoob ", "Gooogle ", "Facebook") AS ConcatenatedString;
	SELECT CONCAT("SQL ", "Runoob ", "Gooogle ", "Facebook")AS ConcatenatedString;

输出：

	The merged string is "SQL Runoob Gooogle Facebook".
	The merged string is "SQL Runoob Gooogle Facebook".	
按顺序连接字符串并给定间隔符（比如```-```)的输入：

	SELECT CONCAT_WS("-", "SQL", "Tutorial", "is", "fun!") AS ConcatenatedString;
	SELECT CONCAT_WS("-", "SQL", "Tutorial", "is", "fun!")AS ConcatenatedString;
输出：

	The merged string with entered seperation is "SQL-Tutorial-is-fun!".
	The merged string with entered seperation is "SQL-Tutorial-is-fun!".

* 优点

直接在```Parse.EXEC```函数体内```else if (ele1 == "SELECT")```的分支内加分支进行扩展，无需改动其它代码。

通过在判断时，使用大写字母化后的表达式进行比对的方法，实现对关键词和运算符的大小写不敏感。

因为```AS```关键词与函数参数表后的右括号之间有没有空格在mysql中都是合法的，我们对这个corner case作了处理，使程序更稳健。

* 局限性与展望

其实字符串函数可以不仅仅处理独立的字符串，也可以把一个表中的某些“单元格”（类比excel表）的字符串元素进行操作（比如把某行的某几列字符串数据连接起来）然后再赋值给某一“单元格”。

### 4.5、sql逻辑运算

* 设计思路

sql逻辑运算可以说就是用```select```关键字的不带括号的逻辑计算器，通过为各运算符设定优先级，再运用数据栈和运算符栈，来得到最终的```bool```值答案。

因为此处的特殊符号```!```是前后有无空格都是合法的，而我们知道取非是唯一的单目逻辑运算符，```!```是从属于它后面的数据的，```NOT```也是，所以通过函数```string PARSE::whole_expression_standardize```进行逻辑表达式的标准化，将出现```!```或```NOT```的表达式转化为符号前有空格而后面直接连数据的新表达式，以便后续通过字符串流进行分割。若分割的子段中有```!```或```NOT```，直接取逻辑非加入数据栈中。

* 功能示例

输入：

	Select 2 aNd 0;
	seLEct 2 oR 1;
	SElect noT 1;
	sEleCt 1 Xor 1;
	sEleCt 0 Xor 1 AnD 9 and !10 Or 3;
输出：

	false
	true
	false
	true
	true

* 优点

直接在```Parse.EXEC```函数体内```else if (ele1 == "SELECT")```的分支内加分支进行扩展，无需改动其它代码。

通过在判断时，使用大写字母化后的表达式进行比对的方法，实现对关键词和运算符的大小写不敏感。

对特殊符号```!```前后有无空格的corner cases进行处理，使程序更加稳健。

可以支持任何仅有与、或、非、异或的逻辑表达式，且运算符数量与组合形式不限。

* 局限性与展望

逻辑运算可以不仅仅处理独立的逻辑表达式，它可以与```whereclauses```的逻辑运算兼并，去计算筛选输出的条件，或者可以把一个表中的某些“单元格”（类比excel表）的字符串元素进行操作（比如连接起来）然后再赋值给某一“单元格”。

### 4.6、sql算术运算

* 设计思路

sql算术运算本质使```SELECT```关键字的无括号的算术计算器，运用数据栈和运算符栈以及给各运算符设定优先级，来得到最终的数值答案，最终答案小数点后保留6位。

* 功能示例

输入加法表达式：

	SeLeCt 2+1;
输出：

	2 + 1
	3.000000
输入减法表达式：

	SELect 2-1;
输出：

	2 - 1
	1.000000
输入除法表达式：

	SeLect 3/2;
	select 3 DIV 2;
输出：

	3 / 2
	1.500000
	3 DIV 2
	1.500000
输入取模表达式：

	select 100%3;
	selecT 100 MOD 3;
输出：

	100 % 3
	1.000000
	100 MOD 3
	1.000000	
输入数学符号前后没有空格的混合运算：

	SELECT 2 MOd 3+4/2-1+8/6;
输出：

	2 MOD 3 + 4 / 2 - 1 + 8 / 6
	4.333333
输入数学符号前后没有空格或有空格的混合运算：

	SelECT 1/3-5 DIV 3+8 Mod 7-3/5;
	SelECT 1 / 3 -5 DIV   3 +   8   Mod 7  - 3 /5;
输出：

	1 / 3 - 5 DIV 3 + 8 MOD 7 - 3 / 5
	-0.933333
	1  /  3  - 5 DIV   3  +    8   MOD 7   -  3  / 5
	-0.933333

* 优点

直接在```Parse.EXEC```函数体内```else if (ele1 == "SELECT")```的分支内加分支进行扩展，无需改动其它代码。

通过在判断时，使用大写字母化后的表达式进行比对的方法，实现对关键词和运算符的大小写不敏感。

支持```int```和```double```数据。

因为数学符号在mysql里前后是不一定有空格的，而我们使用的方法是自动以空格作为间隔的字符串流，所以程序会在数学运算符前后加入空格，对输入的corner cases作处理，以保证运算结果的准确性与程序稳健性。

可以支持任何仅有加、减、乘、除、取模的算术表达式，无论多少个算术运算符或者任何组合都可以。

* 局限性与展望

其实算术运算可以不仅仅处理独立的字符串，可以用到```whereclases```里，进行表达式的值与数据表中数据的比较，或者可以把一个表中的某些“单元格”（类比excel表）的数值元素进行操作（比如求某一列的和）然后再赋值给某一“单元格”。

### 4.7、LIKE子句模糊匹配
* 设计思路

通过对数据表中字符串子串的查找，来进行类似正则的模糊匹配，

* 功能展示

输入，新建数据库、数据表，并将数据插入:

    CREATE DATABASE CAFE;
    USE CAFE;
    CREATE TABLE menu(price INT, cost INT, kind CHAR, PRIMARY KEY(kind));
    INSERT INTO menu(price, cost, kind) VALUES (15, 5, "milk_coffee");
    INSERT INTO menu(price, cost, kind) VALUES (20, 8, "milk_cake");
    INSERT INTO menu(price, cost, kind) VALUES (30, 12, "abc");
    INSERT INTO menu(price, cost, kind) VALUES (15, 5, "red_rose_cake");
    INSERT INTO menu(price, cost, kind) VALUES (22, 10, "pink_rose_cake");
    INSERT INTO menu(price, cost, kind) VALUES (20, 8, "ki");
显示模糊匹配结果：
输入：

    SELECT * from menu WHERE kind LIKE '%cake';
输出：

    price   cost    kind
    20      8       milk_cake
    22      10      pink_rose_cake
    15      5       red_rose_cake
输入：

    SELECT price from menu WHERE kind LIKE '%cake';
输出：
```
price
20
22
15
```
输入：

    SELECT price, kind from menu WHERE kind LIKE '%cake';
输出：
```
price   kind
20      milk_cake
22      pink_rose_cake
15      red_rose_cake
```
输入：

    SELECT * from menu WHERE kind LIKE "milk%";
输出：
    price   cost    kind
    20      8       milk_cake
    15      5       milk_coffee
输入：

    SELECT price from menu WHERE kind LIKE "milk%";
输出：
```
price
20
15
```

输入：

    SELECT price, cost from menu WHERE kind LIKE "milk%";
输出：
```
price   cost
20      8
15      5
```
输入：

    SELECT * from menu WHERE kind LIKE '%rose%';
输出：

    price   cost    kind
    22      10      pink_rose_cake
    15      5       red_rose_cake
输入：

    SELECT price from menu WHERE kind LIKE '%rose%';
输出：

    price
    22
    15
输入：

    SELECT price, cost from menu WHERE kind LIKE '%rose%';
输出：

    price   cost
    22      10
    15      5  
输入：

    SELECT * from menu WHERE kind LIKE '_i';
输出：

    price   cost    kind
    20      8       ki  
输入：

    SELECT price, kind from menu WHERE kind LIKE '_i';
输出：

    price   kind
    20      ki
输入：

    SELECT price from menu WHERE kind LIKE 'k_';
输出：

    price
    20
输入：

    SELECT price, kind from menu WHERE kind LIKE 'k_';
输出：

    price   kind
    20      ki

* 优点

直接在```Parse.EXEC```函数体内```else if (ele1 == "SELECT")```的```where```分支调用的判断函数```TABLE:: Judge```函数内进行扩展，无需改动其它代码。

实现了mysql中所有的模糊匹配形式：

    '%a'     //以a结尾的数据
	'a%'     //以a开头的数据
	'%a%'    //含有a的数据
	'_a_'    //三位且中间字母是a的
	'_a'     //两位且结尾字母是a的
	'a_'     //两位且开头字母是a的

* 局限性

从功能上来看是比较完整了，但是实现过程是通过```string```字符串的```find```和```substr```函数，大概会分支比较多，形式上会比较散，可能通过正则表达式进行模糊匹配会更加精妙。

### 4.8、存档和读档
* 设计思路

此功能难度主要在于读档，要从零开始创建与原来完全相同的数据库和表，除了要在文件中储存数据之外，还需要储存表名、数据库名、列名、主键等关键信息。这些都是基础功能中写入文件这一功能的更高要求。

因此，我们先利用

	system("mkdir -p data");
这一语句创建名为"data"的文件夹作为存档的路径文件夹，便于存档和之后访问

之后，创建如下三类.txt文件

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

* 核心函数

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

* 新添加的命令

```
RELOAD; //程序刚开始运行时，直接使用LOAD会从data文件夹中搜索上次的存档，并进行读档
SAVE TABLE tbname;//将名为tbname的表进行存档，即对这个表对应的.txt文件进行重写或者存档
SAVE DATABASE dbname;//将名为dbname的数据库进行存档，即对这个数据库对应的.txt文件以及其下辖所有表的.txt文件进行重写或者存档
SAVE ALL;//对所有内容进行存档
```

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
	}	//DROP 操作同理，不再列举

* 实现方式

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

* 存档文件示例和重新读档运行界面说明

以以下测试代码为例（数据库附加功能测试/存档与读档1.sql）：

```
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
```

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

* 优势：

利用了之前的文件导入、导出函数进行改写；
使用文件名标记对应储存的表，并使用分层结构，用index.txt为总目录，分层查找，减小了遗漏数据的可能性；
按列存储，并在表文件中记录总行数，方便创建一列后马上输入数据，防止了多次更新"行数"的重复操作
将存档函数设置为每一个类的成员函数而不是作为最高层级的成员函数，使得分开调用、局部存档变为可能，比起每一次操作都覆盖所有文件的最初想法来说，复杂度降低了很多，并且可以根据操作层面的不同对文件做最小程度的修改；
用户退出程序后，可以自行选择删除存档或是保留存档，增加了项目的灵活度；

* 缺点：

其实说是缺点，更多的是因为时间不允许而没有能够做到更好的遗憾吧

首先，原本的设想是利用fstream类的读、写双接口将修改文件的复杂度降低至线性复杂度以下，但因为时间原因没有能够实现，改为使用分层做法尽可能优化；
其次，对于创建文件夹这一命令，先后尝试了多种做法，包括<direct.h>中的mkdir函数、手动创建并拷贝路径、利用system()操纵shell等等，最终还是没有能够完全解决兼容性问题（mkdir函数在linux环境、windows环境下属于不同的库，system函数在检测到data文件夹时会在运行界面报出警告“data已存在”，虽然不影响运行但是不够美观）

最后，对于文件读取的效率应该还有优化空间。

### 4.9、向文件导入数据时支持whereclause

* 我们支持在导出语句的后端增加whereclause语句。
* 同时支持对任意指定列的输出,附加功能的实现得益于第一部分对原代码中select函数的修改。
* 实现方法：仿照SELECT中对whereclause的处理，将输出到文件和判断是否符合要求的两个过程分开，实现对基础功能代码的高度复用

测试代码如下（见文件夹"输出到文件支持whereclause语句"）：

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

### 4.10、从文件导入信息的默认值填充处理
我们增加了对于导入信息不完整时的默认值填充处理，对没有输入的列默认用0（int类型）/0.0（double类型）/char(0)（char类型）填充：

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

### 4.11、支持对任意数量和顺序的列进行SELECT

在实现多表whereclause时，我们发现有必要实现单表多列select，因此，我们添加了此附加功能
实现思路非常简单，就是利用vector<string>储存列名，并对每一列进行输出
测试代码如下：

	CREATE DATABASE OOP;
	USE OOP;
	CREATE TABLE poi(lkj INT, mnb INT, bvc INT, dfj CHAR, PRIMARY KEY(mnb));
	INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (1, 2, 3, "a");
	INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (2, 3, 4, "a");
	INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (3, 4, 5, "a");
	INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (4, 5, 6, "a");
	INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (5, 6, 7, "c");
	UPDATE poi SET dfj="b" WHERE lkj>3;
	SELECT dfj, bvc from poi;
	UPDATE poi SET dfj="b" WHERE lkj>3 OR mnb=2;
	SELECT dfj, mnb from poi;
	DROP DATABASE OOP;
测试输出如下：
	
	dfj	bvc	
	a	3	
	a	4	
	a	5	
	b	6	
	b	7	
	dfj	mnb	
	b	2	
	a	3	
	a	4	
	b	5	
	b	6	

### 4.12、逻辑运算符用于whereclause
为了增加重要的逻辑运算符的用使用范围，我们将逻辑运算符写入了whereclause子句

* 实现思路

1. 沿用第一阶段的队列结构（将要求值的表达式和操作符分开存放）
在优先级上多做出一层考虑（将OR和XOR）进行并列
2. 对于唯一的一元运算符NOT，我们将其放在字符串处理部分进行处理，如果读到NOT，则对之后的串进行处理，在处理结束算出bool结果后，对结果作not操作
3. 利用优先级连、队列多次处理实现了多层（最多测试6层）逻辑运算符的叠加、混合

测试样例为：

	CREATE DATABASE OOP;
	USE OOP;
	CREATE TABLE poi(lkj INT, mnb INT, bvc INT, dfj CHAR, PRIMARY KEY(mnb));
	INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (1, 2, 3, "a");
	INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (2, 3, 4, "a");
	INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (3, 4, 5, "a");
	INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (4, 5, 6, "a");
	INSERT INTO poi(lkj, mnb, bvc, dfj) VALUES (5, 6, 7, "a");
	INSERT INTO poi(mnb, bvc, dfj) VALUES (7, 7, "a");
	INSERT INTO poi(mnb, bvc, dfj) VALUES (8, 8, "b");
	INSERT INTO poi(mnb) VALUES (9);
	SELECT * from poi;
	SELECT * FROM poi WHERE NOT bvc=3;
	SELECT * FROM poi WHERE bvc=7 OR mnb=5 XOR lkj=5;
	SELECT * FROM poi WHERE NOT NOT NOT NOT NOT dfj=a;
	DROP DATABASE OOP;

标准输出为：

	lkj	mnb	bvc	dfj	
	1	2	3	a	
	2	3	4	a	
	3	4	5	a	
	4	5	6	a	
	5	6	7	a	
	NULL	7	7	a	
	NULL	8	8	b	
	NULL	9	NULL	NULL	
	lkj	mnb	bvc	dfj	
	2	3	4	a	
	3	4	5	a	
	4	5	6	a	
	5	6	7	a	
	NULL	7	7	a	
	NULL	8	8	b	
	NULL	9	NULL	NULL	
	lkj	mnb	bvc	dfj	
	4	5	6	a	
	NULL	7	7	a	
	lkj	mnb	bvc	dfj	
	NULL	8	8	b	
	NULL	9	NULL	NULL	

* 主要优点

1. 将一、二元运算符分别处理，最大程度利用了原有数据结构和处理方式

2. 字符串处理沿用了一贯的字符串处理函数，只是对Judge函数进行了修改

3. 支持单表的同时以极小的改动代价支持了多表

* 存在不足

没有对括号进行处理（主要思路是通过配对括号化为波兰表达式）

