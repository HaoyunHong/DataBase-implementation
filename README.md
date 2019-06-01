# 说明文档
## 1. 数据库整体结构

**数据库的层级结构: ALLBASES -> DATABASE -> TABLE -> COLUMN -> DATA**

* 其中DATA为基类，并实现了INT，CHAR，DOUBLE三种派生类用于存储数据。
* PARSE类用于处理输入语句。

## 2. 数据库各个类的基本数据与封装
* 基本数据的定义
```
    enum ConstraintType{//列的约束类型枚举
        NOT_NULL,//非空
        UNIQUE,
        PRIMARY_KEY,//主键
        FOREIGN_KEY,
        CHECK,
        DEFAULT
    };

    enum DataType{//数据类型枚举
        _INT,
        _DOUBLE,
        _CHAR
    };
```
* DATA类
```
    class DATA{//数据基类，支持NULL功能
    protected:
        bool NullFlag = false;//指示数据是否为空数据,true->NULL
    };

    //INT,CHAR,DOUBLE为三个派生类,公有继承自DATA
    class INT:public DATA{//以INT类为例（DATA派生类）
    private:
        int Value;//用于储存数据值
    };
```
+ COLUMN类
```
    class COLUMN{//一列数据
    private:
        std::vector<pDATA> ColumnData;//一列数据
        bool DefaultFlag = false;//用于指示是否有默认值：true->有default值
        pDATA Default;//默认值（pDATA即DATA*）
        std::vector<ConstraintType> Constraints;//限制的集合
        std::string extra;//储存EXTRA
    };
```
+ TABLE类
```
    class TABLE{//一个表
    private:
       std::vector<std::string> ColumnName;//各列名
       std::vector<DataType> ColumnType;//各列的数据类型集合:{_INT,_DOUBLE,_CHAR}
       std::string KeyColumn = "";//键值列的名字
       int RowNum = 0; //表的行数
    public:
       std::map<std::string,COLUMN*> TableMap;//一个map：列名->列指针
    };
```
+ DATABASE类
```
    class DATABASE{//一个数据库
    private:
        std::vector<std::string> TableName;//表名的集合
    public:
        std::map<std::string,TABLE*> DataBaseMap;//表名->表指针的映射
    };
```
+ ALLBASES类 
```
    class ALLBASES{//所有数据库的集合
    private:
        std::vector<std::string> DBName;//数据库名的集合
    public:
        std::map<std::string,DATABASE*> AllBasesMap; //数据库名->数据库指针
    };
```
## 3. 数据库各个类的主要接口
+  DATA类
```
    bool isNull();//获取NullFlag
    void SetNull();//设置为NULL
    void UnsetNull();//设置为不是NULL
```
+ INT类新增成员函数:
```
    INT(int value);//以value为值构造INT
    int GetValue();//取值
    void SetValue(int d);//设置值
```
+ CHAR类新增成员函数:
```
    CHAR(char value);//以value为值构造CHAR
    char GetValue();//取值
    void SetValue(char d);//设置值
```
+ DOUBLE类新增成员函数:
```
    DOUBLE(double value);//以value为值构造DOUBLE
    double GetValue();//取值
    void SetValue(double d);//设置值
```
+ COLUMN类
```
    void del(int k);//删除第k+1个数据

    void update(int k, int v);//更新第k+1个数据为v
    void update(int k, char v);//更新第k+1个数据为v
    void update(int k, double v);//更新第k+1个数据为v

    bool Get_IsNull(int k);//判断第k+1个数据是否为NULL
    int Get_INT_Value(int k);//获取第k+1个值
    double Get_DOUBLE_Value(int k);//获取第k+1个值
    char Get_CHAR_Value(int k);//获取第k+1个值

    void push_back(int d);//直接加入数据
    void push_back(char d);//直接加入数据
    void push_back(double d);//直接加入数据
    void push_back_null(double d);//直接加入NULL数据
    void push_back_null(int d);//直接加入NULL数据
    void push_back_null(char d);//直接加入NULL数据

    void SetConstraint(ConstraintType ct);//添加限制
    bool isNotNULL();//判断有无NOT_NULL限制

    bool hasDefault();//判断有无默认值
    pDATA GetDefault();//获取默认值
    void SetDefault(int d);//设置默认值
    void SetDefault(char d);//设置默认值
    void SetDefault(double d);//设置默认值

    std::string GetExtra();//获取extra

    int size(){//获得当前列数据的个数（行数）
        return (int)ColumnData.size();
    }
```
+  TABLE类
```
    void showcolumns();//按给定格式打印各列信息
    
    void addcolumn(std::string clname, DataType ctype);//添加一列；参数：(列名，类型）
    
    void addcolumn(std::string clname, DataType ctype, std::vector<ConstraintType>& Constraints);//添加一列；参数：(列名，类型，一个存放限制信息的vector)

    void SetKey(std::string keyname);//设置键值为keyname

    void Insert(std::string cname, int d);//在cname列添加一条数据；参数：(列名，数据)
    void Insert(std::string cname, char d);//在cname列添加一条数据；参数：(列名，数据)
    void Insert(std::string cname, double d);//在cname列添加一条数据；参数：(列名，数据)
    void InsertNull(std::string cname, DataType type);//向cname列加入NULL数据

    void Update(std::string aname, int avalue, std::string condition);//参数（列名，目标值，条件语句）；功能：修改符合条件语句的行，使这些行的对应列的值为目标值；
    void Update(std::string aname, char avalue, std::string condition);//参数（列名，目标值，条件语句）；功能：修改符合条件语句的行，使这些行的对应列的值为目标值；
    void Update(std::string aname, double avalue, std::string condition);//参数（列名，目标值，条件语句）；功能：修改符合条件语句的行，使这些行的对应列的值为目标值；
    
    void Select(std::string name, std::string condition);//查找符合条件语句(whereclause)的行，并输出列name的信息，若name为"*"，则输出所有列的信息
    
    void Delete(std::string condition);//删除符合条件语句的行
    
    DataType GetType(std::string name);//获得行name的类型
    
    bool Judge(std::string condition, int k);//功能：接收whereclause语句，一个整数行标（从0到行数-1），输出bool,判断此行是否满足语句的条件

    void UpdateRow();//更新行数

    COLUMN* GetColumn(std::string name);//获得名字为name的列的列指针

```
+ DATABASE类
```
    TABLE* GetTable(std::string name);//获得名字为name的表的表指针
    
    void addtable(std::string tbname, TABLE* ptable);//添加表；参数（表名， 表指针）
    
    void deltable(std::string tbname);//删除表；参数（表名）  
    
    void showtables();//直接列出现有表

```
+  ALLBASES类 
```
    DATABASE* GetDB(std::string name);//获得名为name的数据库指针

    void create(std::string dbname);//建立名为dbname的新数据库

    void del(std::string name);//Drop名为name的数据库

    void show();//按字典序列出当前数据库
```

## 4. 开发者指导
建议在主函数基于Allbases调用相应命令语句的执行函数，新增功能以新增成员函数的方式开发。









