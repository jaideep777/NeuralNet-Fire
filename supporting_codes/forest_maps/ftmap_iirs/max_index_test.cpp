#include <iostream>
#include <vector>
using namespace std;

template <class T> 
int max_index(T &max, vector <T> v){
	max = v[0]; int imax = 0;
	for (int i=1; i<v.size(); ++i){
		if (v[i] > max) {
			imax = i;
			max = v[i];
		}
	}
	return imax;
}

int main(){

	vector <double> v(10,0);
	for (int i=0; i<5; ++i) v[i]  = i;
	for (int i=5; i<10; ++i) v[i] = 10-i;

	for (int i=0; i< v.size(); ++i) cout << i << '\t'; cout << '\n';
	for (int i=0; i< v.size(); ++i) cout << v[i] << '\t'; cout << '\n';
	
	double max;
	cout << "max index = " << max_index(max, v) << '\n';

}
