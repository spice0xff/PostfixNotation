#ifndef _SymbCalc_
#define _SymbCalc_


#include "stdafx.h"


// ТЗ.
// Вычислять математические выражения в которых присутствуют следующие элементы:
// - числа;
// - бинарные инфиксные операции: +, -, /, *, ^;
// - унарные префиксные операции: sin(x), cos(x), ...;
// - левая скобка: (;
// - правая скобка: ).
// Разделять скобки на отдельные типы элементов необходимо, так как иначе проверка корректности следования
// элементов излишне усложняется (см. ПРАВИЛО. Проверки выражения на ошибки порядка следования элементов);

// Инфиксная форма записи - "2 + 2". (Ин - внутри).
// Префиксная форма записи - "+ 2 2". (Пре - в переди).
// 1. Подготовка исходной строки.
// 2. Получение инфиксной формы записи элементов. На выходе вектор.
// 3. Проверка корректности порядка следования элементов в инфиксной форме.
// 4. Получение префиксной формы записи элементов из инфиксной.
// 5. Вычисление результата.

// Числа (или операнды) обозначим, как NUM.
// Бинарные инфиксные операции, как OPR.
// Унарные префиксные операции, как FUN.
// Скобки, как LBR и RBR.

// АКСИОМА. Операнд FUN должен быть заключен в скобки.
// Иначе "sin(cos(x))" превратится в "sincosx".

// ПРАВИЛО. Проверки выражения на ошибки порядка следования элементов.
// LBR:		LBR/NUM/FUN
// NUM/RBR:	OPR/RBR
// FUN:		LBR
// OPR:		NUM/FUN/LBR
// ЗАМЕЧАНИЕ. Объединить LBR и RBR в BRC нельзя по причине невозможности составления корректных правил,
// аналогичным вышеизложенным ("x+()+x", "(+)", "(sin)").

// АКСИОМА. Должна присутствовать функция, производящая предварительную подготовку исходной строки к парсингу,
// иначе ошибочными будут строки "2 + 2" (пробелы), "Sin" (верхний регистр).
// В ее обязанности должны входить все необходимые операции над исходной строкой перед парсингом:
// - Удаление пробелов;
// - Проверка строки на пустоту;
// - Проверка согласованности скобок, так как лексический анализ эту проверку не обеспечивает;
// - Перевод строки к нижнему регустру;
// - Заключение строки в скобки. Для выполнения следующего пункта и для упразднения проверки на корректность первого элемента;
// - Замена позитивной и негативной унарной префиксной операций ("+x" и "-x") на бинарные инфиксное операции сложения и вычитания. "(+">"(0+" и "(-">"(0-".



// Класс символьный калькулятор.
class SymbCalc {
public:
	SymbCalc();							// Конструктор.
	~SymbCalc();						// Деструктор.
	void	Calc(string sourceString);	// Вычисление результата.
	bool	GetError();					// Возвращение ошибки.
	double	GetResult();				// Возвращение результата.

private:
	// Класс элемент.
	class Element {
	public:
		// Возможные типы элементов.
		enum Type {
			NON, NUM, OPR, FUN, LBR, RBR
		};
		// Тип элемента.
		Type type;
		Element::Element() {
			// TODO: проверить установку для потомков.
			this->type = Element::Type::NON;
		} 
	};
	// Класс число, наследник элемента.
	class Number: public Element {
	public:
		// Данные числа.
		double data;
		// Конструктор.
		Number(double data): data(data) {
			// TODO: 1. в список инициализации; 2. Принудить задавать тип в конструкторе.
			this->type = Element::Type::NUM;
		}
	};
	// Класс операция, наследник элемента.
	class Operation: public Element {
	public:
		// Возможные типы операций.
		enum Type {
			ADD, SUB, MUL, DIV, POW, ERR
		};
		// Тип операции.
		// TODO: проверить конфликт type наследника и родителя.
		Type type;
		// Конструктор.
		Operation(char sourceSymbol) {
			((Element*)this)->type = Element::Type::OPR;
			switch (sourceSymbol) {
				case '+':	this->type = Type::ADD; break;
				case '-':	this->type = Type::SUB; break;
				case '*':	this->type = Type::MUL; break;
				case '/':	this->type = Type::DIV; break;
				case '^':	this->type = Type::POW; break;
				default:	this->type = Type::ERR; break;
			}
		}
		// Возвращение приоритета операции.
		int getPrio() {
			switch (this->type){
				case Type::ADD: return 0;
				case Type::SUB: return 0;
				case Type::MUL: return 1;
				case Type::DIV: return 1;
				case Type::POW: return 2;
			}
		}
	};
	// Класс функция, наследник элемента.
	class Function: public Element {
	public:
		// Возможные типы функций.
		enum Type {
			SIN, COS, ERR
		};
		// Тип функции.
		Type type;
		// Конструктор.
		Function(string sourceString) {
			((Element*)this)->type = Element::Type::FUN;
			this->type = Type::ERR;
			if (sourceString == "sin") {
				this->type = Type::SIN;
			}
			if (sourceString == "cos") {
				this->type = Type::COS;
			}
		}
	};
	// Класс левая скобка, наследник элемента.
	class LeftBracket: public Element {
	public:
		// Конструктор.
		LeftBracket () {
			((Element*)this)->type = Element::Type::LBR;
		}
	};
	// Класс правая скобка, наследник элемента.
	class RightBracket: public Element {
	public:
		// Конструктор.
		RightBracket() {
			((Element*)this)->type = Element::Type::RBR;
		}
	};

	// Класс элементы. Вектор указателей на элементы.
	class Elements: public vector<Element*> {
	};

	string		sourceString;				// Исходная строка.
	bool		isError;					// Ошибка.
	double		result;						// Результат.
	Elements	infixForm;					// Инфиксная форма записи элементов.
	Elements	prefixForm;					// Префиксная форма записи элементов.

	// Возможные типы символа. TODO: в класс элемент.
	enum SymbolType {
		NON,	// Нет символа.
		NUM,	// Символ - число.
		SYM,	// Символ - символ.
		OPR,	// Символ - операция.
		LBR,	// Символ - левая скобка.
		RBR,	// Символ - правая скобка.
		ERR		// Ошибочный символ.
	};
	
	void		Preparation();				// Подготовка исходной строки к парсингу.
	SymbolType	GetSymbolType(char symbol);	// Получение типа символа.
	Element*	GetPtrElement(int &pos);	// Получение указателя на элемент.
	void		GetInfixForm();				// Получение инфиксной формы записи элементов.
	void		CheckInfixForm();			// Проверка корректности порядка следования элементов в инфиксной форме.
	void		GetPrefixForm();			// Получение префиксной формы записи элементов.
	void		CalcResult();				// Вычисление результата.
};


#endif
