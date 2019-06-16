#include "PARSE.h"
#include <direct.h>
#include <io.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

int main()
{
	if(access("data", 00) != -1)
		mkdir("data");
	ALLBASES Allbases;
	PARSE Parse;
	string tem;
	while (getline(cin, tem))
	{
		if (tem == "exit") //输入"exit"时程序退出
			return 0;
		Parse.EXEC(Allbases, tem);
	}
	return 0;
}