// strassen3.cpp : Defines the entry point for the application.
//

#include "strassen3.h"
#include "matrix.h"

using namespace std;

int main()
{
	Matrix<float> a(0.0f), b(0.0f);
	cin >> a >> b;
	auto c = a * b;
	cout << c;
	a *= b;
	cout << a;
	return 0;
}
