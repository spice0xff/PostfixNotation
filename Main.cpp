#include "stdafx.h"
#include "Main.h"
#include "SymbCalc.h"


class MathSeqGene {
public:
	string	result;
	int		len;

	MathSeqGene(int len): len(len) {
	}
	
	void getMathSeq() {
		srand(time(NULL));
		
		// Возможные типы элементов.
		enum ElementType {
			NUM, OPR, FUN, LBR, RBR, ET_LEN
		};
		// Возможные типы операций.
		enum OperationType {
			ADD, SUB, MUL, DIV, OT_LEN
		};
		// Возможные типы функций.
		enum FunctionType {
			SIN, COS, FT_LEN
		};

		stringstream	out;
		int				bracketBalance = 0;
		int				rnd = 0;

		//ElementType elemType = ElementType::LBR;
		ElementType elemType = ElementType::NUM;
		while (true) {
			bool end_need	= (out.str().size() >= this->len);
			bool rbr_need	= end_need && (bracketBalance != 0);
			if (end_need && (bracketBalance == 0)) {
				break;
			}

			switch (elemType) {
				case ElementType::LBR:
					out << "(";
					bracketBalance++;

					if (rbr_need) {
						elemType = ElementType::NUM;
					} else {
						rnd = rand() % 2;
						if (rnd == 0) {
							elemType = ElementType::LBR;
						}
						if (rnd == 1) {
							elemType = ElementType::NUM;
						//}
						//if (rnd == 2) {
						//	elemType = ElementType::FUN;
						}
					}

					break;
				case ElementType::NUM:
					out << "(" << rand() - RAND_MAX / 2 << ")";
				
					elemType = ElementType::OPR;
					break;
					
					if (rbr_need) {
						elemType = ElementType::RBR;
					} else {
						rnd = rand() % 2;
						if (rnd == 0) {
							elemType = ElementType::OPR;
						}
						if (rnd == 1) {
							if (bracketBalance >= 1) {
								elemType = ElementType::RBR;
							} else {
								elemType = ElementType::OPR;
							}
						}
					}
					break;
				case ElementType::RBR:
					out << ")";
					bracketBalance--;

					if (rbr_need) {
						elemType = ElementType::RBR;
					} else {
						rnd = rand() % 2;
						if (rnd == 0) {
							elemType = ElementType::OPR;
						}
						if (rnd == 1) {
							if (bracketBalance >= 1) {
								elemType = ElementType::RBR;
							} else {
								elemType = ElementType::OPR;
							}
						}
					}
					break;
				case ElementType::FUN: {
					FunctionType funcType = FunctionType(rand() % (FunctionType::FT_LEN));
					switch (funcType) {
						case FunctionType::SIN:
							out << "sin";
							break;
						case FunctionType::COS:
							out << "cos";
							break;
						default:
							cout << "!FIXBUG! default switch";
							break;
					}
				
					elemType = ElementType::LBR;
					break;
				}
				case ElementType::OPR: {
					OperationType operType = OperationType(rand() % (OperationType::OT_LEN));
					switch (operType) {
						case OperationType::ADD:
							out << "+";
							break;
						case OperationType::SUB:
							out << "-";
							break;
						case OperationType::MUL:
							out << "*";
							break;
						case OperationType::DIV:
							out << "/";
							break;
						default:
							cout << "!FIXBUG! default switch";
							break;
					}

					elemType = ElementType::NUM;
					break;

					if (rbr_need) {
						elemType = ElementType::RBR;
					} else {
						rnd = rand() % 2;
						if (rnd == 0) {
							elemType = ElementType::NUM;
						}
						if (rnd == 1) {
							elemType = ElementType::LBR;
						}
						//if (rnd == 2) {
						//	elemType = ElementType::FUN;
						//}
					}

					break;
				}
				default: {
					cout << "!FIXBUG! default switch";
					break;
				}
			}
		}

		this->result =  string(out.str());
	}
};


// Точка входа.
int __cdecl main(int argc, char *argv[], char *envp[]) {
	MathSeqGene	mathSeqGene(100000);
	//mathSeqGene.getMathSeq();
	//cout << mathSeqGene.result << endl;

	//fstream file("mathSeq.txt", ios::out);
	//file << mathSeqGene.result;
	//file.close();

	//fstream file("mathSeq.txt", ios::in);
	//file >> mathSeqGene.result;
	//file.close();

	//SymbCalc symbCalc;
	//DWORD tickCount = GetTickCount();
	//symbCalc.Calc(mathSeqGene.result);
	//cout << GetTickCount() - tickCount << endl;
	//cout << symbCalc.GetResult() << endl;

	SymbCalc symbCalc;
	symbCalc.Calc("(1+cos(0)*3)/(4-2)+1");
	if (symbCalc.GetError()) {
		cout << "error";
	} else {
		cout << "result: " << symbCalc.GetResult() << endl;
	}

	cin.get();
	return 0;
}
