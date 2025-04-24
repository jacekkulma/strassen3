#include "matrix.h"

using namespace std;

int main()
{
	Matrix<float> a, b;
	cin >> a >> b;
	
	auto c = a * b;
	cout << c;

	auto d = strassen3(a, b);
	cout << d;

	return 0;
}
