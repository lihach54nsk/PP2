#include "pch.h"
#include <iostream>
#include <istream>
#include <stack>
#include <C:\Users\Геральт из Ривии\source\repos\PP2\pthread.h>

using namespace std;

stack<int> SeedData(size_t count)
{
	stack<int> enter;

	for (int i = 0; i < count; i++) { enter.push(rand()); }	return enter;
}

int main()
{
	int k;
	cout << "Vvedite kolichestvo chisel:";
	cin >> k;
	SeedData(k);

	return 1;
}