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

bool prime(int n) 
{
	for (int i = 2; i <= sqrt(n); i++)
		if (n%i == 0)
			return false;
	return true;
}

void main()
{
	int k;
	cout << "Vvedite kolichestvo chisel:";
	cin >> k;
	SeedData(k);
	cout << "Vvedite kolichestvo potokov:";
}