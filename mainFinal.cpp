#include <iostream>
#include <string>
#include <list>
#include <sys/resource.h>
#include "../include/indexadorHash.h"

using namespace std;
double getcputime(void) {
	struct timeval tim;
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	tim=ru.ru_utime;
	double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
	tim=ru.ru_stime;
	t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
	return t;
}
main() {
	long double aa=getcputime();
	IndexadorHash b("./StopWordsEspanyol.txt", ". ,:", false, true,
	"./indicePruebaEspanyol", 0, true);
	b.Indexar("listaFicheros.txt");
	cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;
}