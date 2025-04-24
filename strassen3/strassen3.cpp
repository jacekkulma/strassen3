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
	float e, f;
	e = a.get(0, 0);
	f = b.get(0, 0);
	Matrix<float> g(0.0f);
	Matrix<float> h(0.0f);
	g.resize(1);
	h.resize(1);
	g.set(0, 0, e);
	h.set(0, 0, f);
	//cout << e;
	//auto d = g.strassen3(h);
	auto d = a.strassen3(b);
	cout << d;

	Matrix<Matrix<float>> i;
	i.resize(3);
	i = a.partition(3);
	//cout << i.get(2, 2);
	
	//cout << e;
	//cout << g;
	return 0;
}
