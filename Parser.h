#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include <vector>

typedef struct Node {
	std::string Label;
	std::vector<Token::Token> Tokens;
	std::vector<Node*> Leaves;
	Node(const std::string& lbl):Label(lbl){}
	Node():Label(""){}
} Node;

class Parser {

	Scanner scanner;
	Token::Token tk;
	

	Node* Program();
	Node* Block();
	Node* Vars();
	Node* Expr();
	Node* A();
	Node* N();
	Node* M();
	Node* R();
	Node* Stats();
	Node* MStat();
	Node* Stat();
	Node* In();
	Node* Out();
	Node* If();
	Node* Loop();
	Node* Assign();
	Node* RO();
	void error(std::string);
	void GetToken();
	void PrintToken();
	Node* NewNode(const std::string&);

public:
	Node* root;
	Parser():root(nullptr){}
	Parser(std::FILE* fp):scanner(fp){}
	void GetFile(std::FILE* fp){ this->scanner.GetFile(fp); }
	bool Parse();
	void PreOrder(Node*, const std::string&);
	void Print(const std::string&);
};

#endif