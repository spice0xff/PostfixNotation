#include "stdafx.h"
#include "SymbCalc.h"


// Класс символьный калькулятор.
// Конструктор.
SymbCalc::SymbCalc(): sourceString(""), isError(false), result(0), infixForm(), prefixForm() {
}
// Деструктор.
SymbCalc::~SymbCalc() {
	// Уничтожение всех элементов инфиксной формы записи.
	for (Elements::iterator itrElem = this->infixForm.begin(); itrElem != this->infixForm.end(); itrElem++) {
		delete (*itrElem);
	}
	// Элементы элементов постфиксной формы записи - ссылки на элементы инфикстной формы записи => их уничтожать не надо.
}

// Вычисление результата.
void SymbCalc::Calc(string sourceString) {
	this->sourceString = sourceString;

	// Подготовка исходной строки к парсингу.
	this->Preparation();
	if (this->isError) {
		return;
	}
	
	// Получение инфиксной формы записи элементов.
	this->GetInfixForm();
	if (this->isError) {
		return;
	}
	
	// Проверка корректности порядка следования элементов в инфиксной форме.
	this->CheckInfixForm();
	if (this->isError) {
		return;
	}

	// Получение префиксной формы записи элементов.
	this->GetPrefixForm();

	// Вычисление результата.
	this->CalcResult();
}

// Возвращение ошибки.
bool SymbCalc::GetError() {
	return this->isError;
}

// Возвращение результата.
double SymbCalc::GetResult() {
	return this->result;
}

// Подготовка исходной строки к парсингу.
void SymbCalc::Preparation() {
	// Удаление пробелов.
	this->sourceString = replace(this->sourceString, " ", "");

	// Если исходная строка пуста, установка ошибки.
	if (this->sourceString.empty()) {
		this->isError = true;
		return;
	}

	// Проверка согласованности скобок.
	int bracketBalance = 0;
	for (int idx = 0; idx != this->sourceString.size(); idx++) {
		if (this->sourceString[idx] == '(') {
			bracketBalance++;
		}
		if (this->sourceString[idx] == ')') {
			bracketBalance--;
		}
		if (bracketBalance < 0) {
			this->isError = true;
			return;
		}
	}
	if (bracketBalance != 0) {
		this->isError = true;
		return;
	}

	// Перевод строки в нижний регистр.
	// TODO: как поведет себя функция tolower с некорректными символами? Изучить функцию tolower.
	for (string::iterator itr = this->sourceString.begin(); itr != this->sourceString.end(); itr++) {
		*itr = tolower(*itr);
	}
	
	// Заключение строки в скобки.
	this->sourceString = "(" + this->sourceString + ")";
	
	// Замена позитивных и негативных префиксных операций на сложение и вычитание.
	this->sourceString = replace(this->sourceString, "(+", "(0+");
	this->sourceString = replace(this->sourceString, "(-", "(0-");
}

// Получение типа символа.
SymbCalc::SymbolType SymbCalc::GetSymbolType(char symbol) {
	// Если символ - число.
	if ((symbol >= '0' && symbol <= '9') || symbol == '.') {
		return SymbolType::NUM;
	}
	// Если символ - символ.
	if ((symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z')) {
		return SymbolType::SYM;
	}
	// Если символ - операция.
	if ((symbol == '+') || (symbol == '-') || (symbol == '*') || (symbol == '/') || (symbol == '^')) {
		return SymbolType::OPR;
	}
	// Если символ - левая скобка.
	if (symbol == '(') {
		return SymbolType::LBR;
	}
	// Если символ - правая скобка.
	if (symbol == ')') {
		return SymbolType::RBR;
	}
	// Иначе ошибочный символ.
	return SymbolType::ERR;
}
// Получение указателя на элемент.
SymbCalc::Element* SymbCalc::GetPtrElement(int &pos) {
	// TODO: причесать переменную.
	int pos0 = pos;

	// Получение типа символа.
	SymbolType	firsSymbType = SymbolType::NON;	// Тип первого символа.
	SymbolType	сurrSymbType = SymbolType::NON;	// Тип текущего символа.
	// Перебор символов строки.
	while (pos != this->sourceString.length()) {
		// Получение типа элемента для текущего символа.
		сurrSymbType = GetSymbolType(this->sourceString[pos]);
		// Если тип элемента ошибочен, то установка ошибки.
		if (firsSymbType == SymbolType::ERR) {
			this->isError = true;
			break;
		}

		// Если не определен тип первого символа, его определение.
		if (firsSymbType == SymbolType::NON) {
			firsSymbType = сurrSymbType;
		} else {
			// Если тип символа - операция или скобка, то окончание перебора. Так как они занимают один символ.
			if ((firsSymbType == SymbolType::OPR) || (firsSymbType == SymbolType::LBR) || (firsSymbType == SymbolType::RBR)) {
				break;
			}
			// Если тип перебираемого символа отличается от типа первого символа, то окончание перебора. 
			if (сurrSymbType != firsSymbType) {
				break;
			}
		}
		pos++;
	}
	// Декрементация позиции, так как либо позиция вышла за пределы строки, либо за пределы элемента.
	if (!this->isError) {
		pos--;
	}

	// Проверка корректности элемента и его возвращение.
	switch (firsSymbType) {
		case SymbolType::NUM: {
			// TODO: ошибка в записи числа.
			Number *number = new Number(atof(this->sourceString.substr(pos0, pos - pos0 + 1).c_str()));
			return (Element*)number;
		}
		case SymbolType::SYM: {
			Function *function = new Function(this->sourceString.substr(pos0, pos - pos0 + 1));
			if (function->type == Function::Type::ERR) {
				this->isError = true;
				return NULL;
			}
			return (Element*)function;
		}
		case SymbolType::OPR: {
			Operation *operation = new Operation(this->sourceString[pos]);
			if (operation->type == Operation::Type::ERR) {
				this->isError = true;
				return NULL;
			}
			return (Element*)operation;
		}
		case SymbolType::LBR: {
			return (Element*)(new LeftBracket());
		}
		case SymbolType::RBR: {
			return (Element*)(new RightBracket());
		}
		case SymbolType::ERR: {
			// Флаг ошибки уже установлен.
			return NULL;
		}
	}
}
// Получение инфиксной формы записи элементов.
void SymbCalc::GetInfixForm() {
	// Если инфиксная форма записи элементов не пуста.
	if (!this->infixForm.empty()) {
		// Очищение инфиксной формы записи элементов.
		this->infixForm.clear();
	}

	Element *element = NULL;

	//for (int pos = 0; pos != this->sourceString.size(); pos++) {
	//	// Получение элемента.
	//	element = GetPtrElement(pos);
	//	// Если ошибка.
	//	if (this->isError) {
	//		break;
	//	}
	//	// Добавление элемента в вектор.
	//	this->infixForm.push_back(element);
	//}

	int pos = 0;
	while (pos != this->sourceString.size()) {
		// Получение элемента.
		element = GetPtrElement(pos);
		// Если ошибка.
		if (this->isError) {
			break;
		}
		// Добавление элемента в вектор.
		this->infixForm.push_back(element);
		pos++;
	}
}

// Проверка корректности порядка следования элементов в префиксной форме.
void SymbCalc::CheckInfixForm() {
	//// Вывод математического выражения в инфиксной форме записи.
	//cout << "Infix" << endl;
	//for (Elements::iterator itrElem = infixForm.begin(); itrElem != infixForm.end(); itrElem++)
	//{
	//	switch ((*itrElem)->type) {
	//		case Element::Type::NUM: 
	//			cout << ((Number*)*itrElem)->data;
	//			break;
	//		case Element::Type::FUN:
	//			switch (((Function*)*itrElem)->type) {
	//				case Function::Type::SIN: cout << "SIN"; break;
	//				case Function::Type::COS: cout << "COS"; break;
	//			}
	//			break;
	//		case Element::Type::OPR:
	//			switch (((Operation*)*itrElem)->type) {
	//				case Operation::Type::ADD: cout << '+'; break;
	//				case Operation::Type::SUB: cout << '-'; break;
	//				case Operation::Type::MUL: cout << '*'; break;
	//				case Operation::Type::DIV: cout << '/'; break;
	//				case Operation::Type::POW: cout << '^'; break;
	//			}
	//			break;
	//		case Element::Type::LBR:
	//			cout << '(';
	//			break;
	//		case Element::Type::RBR:
	//			cout << ')';
	//			break;
	//		default:
	//			cout << "!FIXBUG! default switch";
	//			return;
	//	}
	//	cout << " ";
	//}
	//cout << endl;

	// Минимально возможное количество элементов - три. Его устанавливает Preparation() и подтверждает GetElement().
	for (vector<SymbCalc::Element*>::iterator itrElem = this->infixForm.begin(); itrElem != this->infixForm.end() - 1; itrElem++) {
		Element::Type nextElemType = (*(itrElem+1))->type;
		switch ((*itrElem)->type) {
			case Element::Type::LBR:
				if ((nextElemType != Element::Type::LBR) && (nextElemType != Element::Type::NUM) && (nextElemType != Element::Type::FUN)) {
					this->isError = true;
					return;
				}
				break;
			case Element::Type::RBR:
			case Element::Type::NUM:
				if ((nextElemType != Element::Type::OPR) && (nextElemType != Element::Type::RBR)) {
					this->isError = true;
					return;
				}
				break;
			case Element::Type::FUN:
				if (nextElemType != Element::Type::LBR) {
					this->isError = true;
					return;
				}
				break;
			case Element::Type::OPR:
				if ((nextElemType != Element::Type::NUM) && (nextElemType != Element::Type::FUN) && (nextElemType != Element::Type::LBR)) {
					this->isError = true;
					return;
				}
				break;
			default:
				cout << "!FIXBUG! default switch";
				this->isError = true;				
				return;
		}
	}
}

// Получение префиксной формы записи элементов.
void SymbCalc::GetPrefixForm() {
	// Если префиксная форма записи элементов не пуста.
	if (!this->prefixForm.empty()) {
		// Очищение префиксной формы записи элементов.
		this->prefixForm.clear();
	}

	// Стек элементов.	
	Elements stack;

	// Перебор элементов инфиксной формы записи элементов.
	for (Elements::iterator itrElem = this->infixForm.begin(); itrElem != this->infixForm.end(); itrElem++) {
		switch ((*itrElem)->type) {
			case Element::Type::NUM:
				prefixForm.push_back(*itrElem);
				break;
			case Element::Type::FUN:
			case Element::Type::LBR:
				stack.push_back(*itrElem);
				break;
			case Element::Type::OPR: {
				int prio = ((Operation*)*itrElem)->getPrio();
				// Пока на вершине стека оператор. Стек не пуст, так как в нем есть хотя бы левая скобка от Preparation().
				for (Element *element = stack.back(); element->type == Element::Type::OPR; element = stack.back()) {
					// Если приоритет перебираемого оператора меньше или равен приоритету оператора на вершине стека.
					if (prio <= ((Operation*)element)->getPrio()) {
						// Перемещение элемента из стека в вектор префиксной записи.
						prefixForm.push_back(stack.back());
						stack.pop_back();
					} else {
						break;
					}
				}
				stack.push_back(*itrElem);
				break;
			}
			case Element::Type::RBR:
				// Пока на вершине не левая скобка.
				while ((stack.back()->type != Element::Type::LBR)) {
					// Перемещение элемента из стека в вектор префиксной формы записи.
					prefixForm.push_back(stack.back());
					stack.pop_back();
				}
				// Удаление из стека левой скобки.
				stack.pop_back();
				if (!stack.empty()) {
					if (stack.back()->type == Element::Type::FUN) {
						// Перемещение элемента из стека в вектор префиксной формы записи.
						prefixForm.push_back(stack.back());
						stack.pop_back();
					}
				}
				break;
			default:
				cout << "!FIXBUG! default switch";
				return;
		}
	}

	// Пока есть элементы в стеке.
	while (!stack.empty()) {
		// Перемещение элемента из стека в префиксную форму записи элементов.
		prefixForm.push_back(stack.back());
		stack.pop_back();
	}

	//// Вывод математического выражения в префиксной форме записи.
	//cout << "Prefix" << endl;
	//for (Elements::iterator itrElem = prefixForm.begin(); itrElem != prefixForm.end(); itrElem++)
	//{
	//	switch ((*itrElem)->type) {
	//	case Element::Type::NUM:
	//		//cout << "NUM";
	//		cout << ((Number*)*itrElem)->data;
	//		break;
	//	case Element::Type::FUN:
	//		//cout << "FUN";
	//		switch (((Function*)*itrElem)->type) {
	//			case Function::Type::SIN: cout << "SIN"; break;
	//			case Function::Type::COS: cout << "COS"; break;
	//		}
	//		break;
	//	case Element::Type::OPR:
	//		//cout << "OPR";
	//		switch (((Operation*)*itrElem)->type) {
	//			case Operation::Type::ADD: cout << '+'; break;
	//			case Operation::Type::SUB: cout << '-'; break;
	//			case Operation::Type::MUL: cout << '*'; break;
	//			case Operation::Type::DIV: cout << '/'; break;
	//			case Operation::Type::POW: cout << '^'; break;
	//		}
	//		break;
	//	default:
	//		cout << "!FIXBUG! default switch";
	//		return;
	//	}
	//	cout << " ";
	//}
	//cout << endl;
}

// Вычисление результата.
void SymbCalc::CalcResult() {
	// Вычислительный стек.
	vector<double> stack;

	// Перебор элементов префиксной формы записи элементов.
	for (Elements::iterator itrElem = this->prefixForm.begin(); itrElem != this->prefixForm.end(); itrElem++) {
		switch ((*itrElem)->type) {
			case Element::Type::NUM:
				stack.push_back(((Number*)(*itrElem))->data);
				break;
			case Element::Type::OPR: {
				double b = stack.back();
				stack.pop_back();
				double a = stack.back();
				stack.pop_back();

				switch (((Operation*)*itrElem)->type) {
					case Operation::Type::ADD:
						stack.push_back(a + b);
						break;
					case Operation::Type::SUB:
						stack.push_back(a - b);
						break;
					case Operation::Type::MUL:
						stack.push_back(a * b);
						break;
					case Operation::Type::DIV:
						stack.push_back(a / b);
						break;
					case Operation::Type::POW:
						double result = pow(a, b);
						if (errno != 0) {
							this->isError = true;
							return;
						}
						stack.push_back(result);
						break;
				}
				break;
			}
			case Element::Type::FUN: {
				double a = stack.back();
				stack.pop_back();
				
				switch (((Function*)*itrElem)->type) {	
					case Function::Type::SIN:
						stack.push_back(sin(a));
						break;
					case Function::Type::COS:
						stack.push_back(cos(a));
						break;
				}
				break;
			}
			default:
				cout << "!FIXBUG! default switch";
				return;
		}
	}
	// Результат вычисления - на вершине стека.
	this->result = stack.front();
}
// Класс символьный калькулятор.