#include "matrix.h"

using namespace std;

int main()
{
	Matrix<float> a, b;
	cin >> a >> b;

	// tu jest ok
	auto c = a + b;
	cout << c;

	// strassen
	auto d = strassen3(a, b);

	// tu juz zle
	auto e = a + b;
	cout << e;

	return 0;
}
