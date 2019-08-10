#ifndef _StdAfx_
#define _StdAfx_


#define _USE_MATH_DEFINES	// Математические константы.


// WINAPI
#include <windows.h>
// STL
#include <string>		// Работа со строками.
#include <vector>		// Работа с векторами.
#include <list>			// Работа со списками.
#include <map>			// Работа со словарями.
#include <algorithm>	// Алгоритмы обработки контейнеров.

#include <iostream>		// Потоки ввода-вывода. Классы ios, istream, ostream, stream.
#include <strstream>	// Сороковые потоки. Классы istrstream, ostrstream, strstream.
#include <fstream>		// Файловые потоки. Классы ifstream, ofstream, fstream.
#include <sstream>		// use stringstream


// C
#include <math.h>
#include <time.h>



using namespace std;


string replace(string text, string sour, string dest);	// Замена подстрок в строке.
string remove(string text, string sour);				// Удаление подстрок из строки.


#endif