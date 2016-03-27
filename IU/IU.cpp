// IU.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "Token.h"
#include <fstream>

using namespace std;

int main()
{
	char buffer[1024];
	ifstream in("c:/Users/hongzhe/documents/workfile.txt");
	if (!in.is_open())
	{
		cout << "error opening file;";
		exit(1);
	}
	char c = (char)in.get();
	in.get();
	in.unget();
	c = (char)in.get();
	cout << "read " << c << endl;
	string str = "hehe";
	str.append(1, 'x');
	char ch = '4';
	ch = '\n';
	cout << "ch is " << ch << endl;
	string em;
	if (em.empty()) {
		cout << "em is empty string." << endl;
	}
	cout << str << endl;

	string newline = "a string \
		with newline.";
	cout << newline << endl;
    return 0;
}

