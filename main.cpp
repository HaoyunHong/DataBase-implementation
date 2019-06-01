#include "PARSE.h"

using namespace std;

int main(){
	ALLBASES Allbases;
	PARSE Parse;
	string tem;
	while(getline(cin,tem)){
		if(tem == "exit")//输入"exit"时程序退出
			return 0;
		Parse.EXEC(Allbases, tem);
	}
	return 0;
}