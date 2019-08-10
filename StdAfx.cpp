#include "stdafx.h"


// Замена подстрок в строке.
string replace(string text, string sour, string dest) {
	for (DWORD idx = text.find(sour, 0); idx != string::npos; idx = text.find(sour, idx)) {
		text.replace(idx, sour.length(), dest);
	}
	return text;
}

// Удаление подстрок из строки.
string remove(string text, string sour) {
	for (DWORD idx = text.find(sour, 0); idx != string::npos; idx = text.find(sour, idx)) {
		text.erase(idx, sour.length());
	}
	return text;
}
