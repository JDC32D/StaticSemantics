// /*
// <program>  ->     <vars> <block>
// <block>    ->     void <vars> <stats> end
// <vars>     ->     empty | var Identifier : Integer <vars> 
// <expr>     ->     <A> + <expr> | <A> - <expr> | <A>
// <A>        ->     <N> / <A> | <N>
// <N>        ->     <M> * <N> | <M>
// <M>        ->     % <M> |  <R>
// <R>        ->     ( <expr> ) | Identifier | Integer
// <stats>    ->     <stat> ; <mStat>
// <mStat>    ->     empty |  <stat>  ;  <mStat>
// <stat>     ->     <in> | <out> | <block> | <if> | <loop> | <assign>
// <in>       ->     scan  Identifier 
// <out>      ->     print <expr>
// <if>       ->     cond [ <expr> <RO> <expr> ] <stat>
// <loop>     ->     iter [ <expr> <RO> <expr> ] <stat>
// <assign>   ->     Identifier  = <expr>  
// <RO>       ->     < | = <  | >  | = > | < >   |   =            
// */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <sstream>
#include "Parser.h"

const bool DEBUG = 0;

void Parser::error(std::string msg){
	std::printf("ERROR %s\n\tToken: ", msg.c_str());
	PrintToken();
	return;
}

void PrintNodeTokens(Node *node, 
	int lvl, const std::string& fileName) {

	if(DEBUG)
		std::printf("%lu", node->Tokens.size());

	std::stringstream out;
	out << node->Label;

	if(node->Tokens.size() > 0) {
		out << " Tokens: ";
		for(unsigned int iter=0 ; iter < node->Tokens.size()-1 ; iter++) {
			out << node->Tokens[iter].instance << ", ";
		}

		out << node->Tokens[node->Tokens.size()-1].instance;
	}

	for(int iter=0 ; iter < lvl ; ++iter)
		std::printf("	");

	std::printf( "%s\n", out.str().c_str() );
	std::fstream file;
	file.open( fileName, std::fstream::app );

	for(int iter=0 ; iter < lvl ; ++iter)
		file << "\t";

	file << out.str() << "\n";
	file.close();
}

void Parser::PreOrder(Node* node, const std::string& file) {
	std::string fileName = file;
	std::function<void(Node*,int)> print = [&](Node* node, int lvl){
		if(!node){
			return;
		}
		PrintNodeTokens(node,lvl,fileName);
		for(unsigned int iter=0 ; iter < node->Leaves.size() ; iter++) {
			print(node->Leaves[iter],lvl+1);
		}
	};
	print(node,0);
}

void Parser::Print(const std::string& file) {
	this->PreOrder(this->root,file);
}

// <vars> -> empty | var Identifier : Integer <vars>
Node* Parser::Vars() {
	if(DEBUG)
		std::printf("-->Vars\n");
	Node* token = this->NewNode("<vars>");

	if(this->tk.id == Token::varTk) { 
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if(this->tk.id == Token::identifierTk) {
			//token = this->NewNode("<vars>");
			token->Tokens.push_back(this->tk);
			this->GetToken();

			if(this->tk.id == Token::colonTk) {
				token->Tokens.push_back(this->tk);
				this->GetToken();

				if(this->tk.id == Token::integerTk) {
					token->Tokens.push_back(this->tk);
					this->GetToken();
					Node* retrn = this->Vars();

					if(retrn != nullptr) {
						token->Leaves.push_back(retrn);
						if(DEBUG){ std::printf("Vars-->\n"); }
						return token;
					} else
						if(DEBUG){std::printf("Vars: returned ε\n");}
				} else 
					error("Vars: Expected intTk");
			} else
				error("Vars: Expected colonTk");
		} else
			error("Vars: Expected identifierTk");
	} 
	return token;
}


// <in> -> scan Identifier
Node* Parser::In() {
	if(DEBUG)
		std::printf("-->In\n");
	Node* token = this->NewNode("<in>");

	if(this->tk.id == Token::scanTk){
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if(this->tk.id == Token::identifierTk){
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if(DEBUG){ std::printf("In-->\n"); }
			return token;
		} else
			error("In: Expected identifierTk");

	} else
		error("In: Expected scanTk");
	return nullptr;
}

// <R>	->	( <expr> ) | Identifier | Integer
Node* Parser::R() {
	if(DEBUG)
		std::printf("-->R\n");

	Node* token = NewNode("<R>");

	// <R> -> ( <expr> )
	if(this->tk.id == Token::lparenTk) {
		token->Tokens.push_back(this->tk);
		this->GetToken();
		Node* leaf = this->Expr();

		if( leaf != nullptr ) {

			token->Leaves.push_back(leaf);

			if(this->tk.id == Token::rparenTk) {
				token->Tokens.push_back(this->tk);
				this->GetToken();
				if(DEBUG){ std::printf("R-->\n"); }
				return token;
			} else {
				error("R: Expected )");
			}

		} else {
			error("R: Expr returned nullptr");
		}
		return nullptr;
	}

	if(this->tk.id == Token::identifierTk) {
		token->Tokens.push_back(this->tk);
		this->GetToken();
		if(DEBUG){ std::printf("R-->\n"); }
		return token;
	}

	if(this->tk.id == Token::integerTk) {
		token->Tokens.push_back(this->tk);
		this->GetToken();
		if(DEBUG){ std::printf("R-->\n"); }
		return token;
	}

	error("R: Expected (, identifier, integer");
	return nullptr;
}

// <M>	->	% <M> |  <R>
Node* Parser::M() {
	if(DEBUG)
		std::printf("-->M\n");

	Node* token = NewNode("<M>");

	if(this->tk.id == Token::modTk) {
		token->Tokens.push_back(this->tk);
		this->GetToken();
		Node* leaf = this->M();
		if(leaf != nullptr) {
			token->Leaves.push_back(leaf);
			if(DEBUG)
				printf("M-->\n");
			return token;
		}
		error("M: returned nullptr");
		return nullptr;
	}

	if(this->tk.id == Token::lparenTk ||
		this->tk.id == Token::identifierTk ||
		this->tk.id == Token::integerTk) {

		Node* leaf = this->R();
		if(leaf != nullptr) {
			token->Leaves.push_back(leaf);
			if(DEBUG)
				printf("M-->\n");
			return token;
		}
		error("M: returned nullptr");
		return nullptr;
	}

	error("M: Expected ( or %");
	return nullptr;
}

// <N>	->	<M> * <N> | <M>
Node* Parser::N() {
	if(DEBUG)
		std::printf("-->N\n");

	Node* token = this->NewNode("<N>");

	if(this->tk.id == Token::modTk ||
		this->tk.id == Token::lparenTk ||
		this->tk.id == Token::identifierTk ||
		this->tk.id == Token::integerTk) {

		Node* leaf = this->M();

		if(this->tk.id == Token::multTk) {

			token->Tokens.push_back(this->tk);
			this->GetToken();
			leaf = this->N();

			if(leaf != nullptr) {
				token->Leaves.push_back(leaf);
			} else {
				error("N: Returned nullptr");
				return nullptr;
			}

		}

		if(leaf != nullptr) {
			if(DEBUG){ std::printf("N-->\n"); }
			token->Leaves.push_back(leaf);
			return token;
		}
		// if(DEBUG){ std::printf("N-->\n"); }
		// return token;
		error("N: returning nullptr");
		return nullptr;
	}

	error("N: Expected %");
	return nullptr;
}

// <A>	->	<N> / <A> | <N>
Node* Parser::A() {
	if(DEBUG)
		std::printf("-->A\n");

	Node* token = this->NewNode("<A>");

	// <A> -> <N>
	if(this->tk.id == Token::modTk ||
		this->tk.id == Token::lparenTk ||
		this->tk.id == Token::identifierTk ||
		this->tk.id == Token::integerTk) {

		Node* leaf = this->N();

		// <A> -> <N> / <A>
		if(this->tk.id == Token::bslashTk) {
			token->Tokens.push_back(this->tk);
			this->GetToken();
			leaf = this->A();

			if(leaf != nullptr) {
				if(DEBUG){ std::printf("A-->\n"); }
				token->Leaves.push_back(leaf);
				return token;
			}
		}

		if(leaf != nullptr) {
			if(DEBUG){ std::printf("A-->\n"); }
			token->Leaves.push_back(leaf);
			return token;
		}
		// if(DEBUG){ std::printf("A-->\n"); }
		// return token;
		error("A: Something");
		return nullptr;
	} else 
		error("A: Expected % ( identifier integer");
	return nullptr;
}

// <expr>	->	<A> + <expr> | <A> - <expr> | <A>
// <A>		->	<N> / <A> | <N>
// <N>		->	<M> * <N> | <M>
// <M>		->	% <M> |  <R>
// <R>		->	( <expr> ) | Identifier | Integer
Node* Parser::Expr() {
	if(DEBUG)
		std::printf("-->Expr\n");

	Node* token = this->NewNode("<expr>");
	Node* leaf = this->A();
	if(leaf == nullptr)
		return nullptr;
	else 
		token->Leaves.push_back(leaf);

	if(this->tk.id == Token::plusTk || this->tk.id == Token::minusTk) {
		token->Tokens.push_back(this->tk);
		this->GetToken();
		leaf = this->Expr();

		if(leaf != nullptr) {
			if(DEBUG){ std::printf("Expr-->\n"); }
			token->Leaves.push_back(leaf);
			return token;
		} else 
			error("Expr: Expr() returned nullptr");
 	} 

	if(DEBUG){ std::printf("Expr-->\n"); }
	return token;
}

// <out> ->  print <expr>
Node* Parser::Out() {
	if(DEBUG)
		std::printf("-->Out\n");
	Node* token = NewNode("<Out>");
	//Node* leaf;

	if(this->tk.id == Token::printTk){
		token->Tokens.push_back(this->tk);
		this->GetToken();
		Node* leaf = this->Expr();
		if(leaf != nullptr) {
			token->Leaves.push_back(leaf);
			return token;
		} else 
			error("Out: Expr returned nullptr");
	} else 
		error("Out: Expected printTk");
	return nullptr;
}

//<RO>	->	< | = < | > | = > | < > | =  
Node* Parser::RO() {
	if(DEBUG)
		std::printf("-->RO\n");
	// <
	if(this->tk.id == Token::lessTk){
		Node* token = this->NewNode("<RO>");
		token->Tokens.push_back(this->tk);
		this->GetToken();

		// < >
		if(this->tk.id == Token::greaterTk) {
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if(DEBUG){ std::printf("RO-->\n"); }
			return token;
		} else {
			if(DEBUG){ std::printf("RO-->\n"); }
			return token;
		}
	}

	// = 
	if(this->tk.id == Token::assignTk) {
		Node* token = this->NewNode("<RO>");
		token->Tokens.push_back(this->tk);
		this->GetToken();

		// =<
		if(this->tk.id == Token::lessTk) {
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if(DEBUG){ std::printf("RO-->\n"); }
			return token;
		} else {
			if(DEBUG){ std::printf("RO-->\n"); }
			return token;
		}

		// =>
		if(this->tk.id == Token::greaterTk) {
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if(DEBUG){ std::printf("RO-->\n"); }
			return token;
		} else {
			if(DEBUG){ std::printf("RO-->\n"); }
			return token;
		}
	}

	// >
	if(this->tk.id == Token::greaterTk) {
		Node* token = this->NewNode("<RO>");
		token->Tokens.push_back(this->tk);
		this->GetToken();
		if(DEBUG){ std::printf("RO-->\n"); }
		return token;
	}

	error("RO: Expected < > =");
	return nullptr;
}

// <if>	->	cond [ <expr> <RO> <expr> ] <stat>
Node* Parser::If() {
	if(DEBUG)
		std::printf("-->If\n");
	Node* token = this->NewNode("<if>");
	Node* leaf;

	if(this->tk.id == Token::condTk) {
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if(this->tk.id == Token::lbracketTk) {
			token->Tokens.push_back(this->tk);
			this->GetToken();
			leaf = this->Expr();

			if(leaf != nullptr) {
				token->Leaves.push_back(leaf);
				leaf = this->RO();

				if(leaf != nullptr) {
					token->Leaves.push_back(leaf);
					leaf = this->Expr();

					if(leaf != nullptr) {
						token->Leaves.push_back(leaf);

						if(this->tk.id == Token::rbracketTk) {
							token->Tokens.push_back(this->tk);
							this->GetToken();
							leaf = this->Stat();

							if(leaf != nullptr) {
								token->Leaves.push_back(leaf);
								if(DEBUG){ std::printf("If-->\n"); }
								return token;
							} else
								error("If: Stat() returned nullptr");
						} else
							error("If: Expected ]");
					} else 
						error("If: Expr() returned nullptr");
				} else
					error("If: RO() returned nullptr");
			} else
				error("If: Expr() returned nullptr");				
		} else 
			error("If: Expected [");
	} else
		error("If: Expected cond");
	return nullptr;
}

// <loop> -> iter [ <expr> <RO> <expr> ] <stat>
Node* Parser::Loop() {
	if(DEBUG)
		std::printf("-->Loop\n");
	Node* token = this->NewNode("<loop>");
	Node* leaf;

	if(this->tk.id == Token::iterTk) {
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if(this->tk.id == Token::lbracketTk) {
			token->Tokens.push_back(this->tk);
			this->GetToken();
			leaf = this->Expr();

			if(leaf != nullptr) {
				//token = this->NewNode("<loop>");
				token->Leaves.push_back(leaf);
				leaf = this->RO();

				if(leaf != nullptr) {
					token->Leaves.push_back(leaf);
					leaf = this->Expr();

					if(leaf != nullptr) {
						token->Leaves.push_back(leaf);

						if(this->tk.id == Token::rbracketTk) {
							token->Tokens.push_back(this->tk);
							this->GetToken();
							leaf = this->Stat();

							if(leaf != nullptr) {
								token->Leaves.push_back(leaf);
								if(DEBUG)
									std::printf("Loop-->");
								return token;
							} else
								error("Loop: Stat() returned nullptr");
						} else
							error("Loop: Expected ]");
					} else
						error("Loop: Expr() returned nullptr");
				} else
					error("Loop: RO() returned nullptr");
			} else
				error("Loop: Expr() returned nullptr");
		} else
			error("Loop: Expected [");
	} else
		error("Loop: Expected iter");
	return nullptr;
}

// <assign>	-> Identifier = <expr>  
Node* Parser::Assign() {
	if(DEBUG)
		std::printf("-->Assign\n");

	Node* token = this->NewNode("<assign>");
	if(this->tk.id == Token::identifierTk) {
		token->Tokens.push_back(this->tk);
		this->GetToken();

		if(this->tk.id == Token::assignTk) {
			token->Tokens.push_back(this->tk);
			this->GetToken();
			Node* leaf = this->Expr();
			if(leaf != nullptr) {
				token->Leaves.push_back(leaf);
				if(DEBUG){ std::printf("Assign-->"); }
				return token;
			} else {
				error("Assign: Expr() returned nullptr;");
			}
		} else {
			error("Assign: Expected =");
		}

	} else {
		error("Assign: Expected Identifier");
	}
	
	error("Assign: Returning nullptr");
	return nullptr;
}

// <stat>  ->  <in> | <out> | <block> | <if> | <loop> | <assign>
Node* Parser::Stat() {
	if(DEBUG)
		std::printf("-->Stat\n");
	Node* token = this->NewNode("<stat>");

	if(this->tk.id == Token::scanTk) {
		//this->GetToken();
		Node* leaf = this->In();
		if(leaf != nullptr) {
			token->Leaves.push_back(leaf);
			if(DEBUG){ std::printf("Stat-->\n"); }
			return token;
		}
	} else if (this->tk.id == Token::printTk) {
		//this->GetToken();
		Node* leaf = this->Out();
		if(leaf != nullptr) {
			token->Leaves.push_back(leaf);
			if(DEBUG){ std::printf("Stat-->\n"); }
			return token;
		}
	} else if (this->tk.id == Token::voidTk) {
		//this->GetToken();
		Node* leaf = this->Block();
		if(leaf != nullptr) {
			token->Leaves.push_back(leaf);
			if(DEBUG){ std::printf("Stat-->\n"); }
			return token;
		}
	} else if (this->tk.id == Token::condTk) {
		//this->GetToken();
		Node* leaf = this->If();
		if(leaf != nullptr) {
			token->Leaves.push_back(leaf);
			if(DEBUG){ std::printf("Stat-->\n"); }
			return token;
		}
	} else if (this->tk.id == Token::iterTk) {
		//this->GetToken();
		Node* leaf = this->Loop();
		if(leaf != nullptr) {
			token->Leaves.push_back(leaf);
			if(DEBUG){ std::printf("Stat-->\n"); }
			return token;
		}
	} else if (this->tk.id == Token::identifierTk) {
		//this->GetToken();
		Node* leaf = this->Assign();
		if(leaf != nullptr) {
			token->Leaves.push_back(leaf);
			if(DEBUG){ std::printf("Stat-->\n"); }
			return token;
		}
	}

	error("Stat: No valid Stat tokens");
	return nullptr;
}

// <mStat>	->	empty |  <stat>  ;  <mStat>
Node* Parser::MStat() {
	if(DEBUG)
		std::printf("-->MSstat\n");
	Node* token = this->NewNode("<mStat>");

	// <mStat> -> <stat>
	if(this->tk.id == Token::scanTk ||
		this->tk.id == Token::printTk ||
		this->tk.id == Token::voidTk ||
		this->tk.id == Token::condTk ||
		this->tk.id == Token::iterTk ||
		this->tk.id == Token::identifierTk) {

		if(this->tk.id == Token::returnTk) {
			if(DEBUG){ std::printf("MStats: got return tk"); }
			return nullptr;
		}
		
		Node* leaf = this->Stat();

		if(leaf == nullptr) {
			error("MStat: Stat returned nullptr");
			return nullptr;
		} else {
			token->Leaves.push_back(leaf);
			// token->Tokens.push_back(this->tk);
			// this->GetToken();
		}

		if(this->tk.id == Token::semicolonTk) {
			token->Tokens.push_back(this->tk);
			this->GetToken();
			leaf = this->MStat();

			if(leaf == nullptr) {
				if(DEBUG){ std::printf("MStat: MStat returned empty"); }
				return token;
			}
			else {
				token->Leaves.push_back(leaf);
				return token;
			}
		}

	}

	// MStat can be empty, so not an error
	return nullptr;

}

// <stats>	->	<stat> ; <mStat>
Node* Parser::Stats() {
	if(DEBUG)
		std::printf("-->Stats\n");

	Node* token = this->NewNode("<stats>");
	//Node* leaf;
	Node* leaf = this->Stat();
	if(leaf != nullptr) {
		token->Leaves.push_back(leaf);

		if(tk.id == Token::semicolonTk){
			token->Tokens.push_back(this->tk);
			this->GetToken();
			leaf = this->MStat();

			if(leaf == nullptr) {
				if(DEBUG){ std::printf("MStats returned ε\n"); }
				return token;
			} else {
				token->Leaves.push_back(leaf);
				if(DEBUG){ std::printf("Stats-->\n"); }
				return token;
			}
		}
	}
	error("Stats: Caught nothing and returning null");
	return nullptr;
}

// <block>	->	void <vars> <stats> return
Node* Parser::Block() {
	if(DEBUG)
		std::printf("-->Block\n");

	Node* token = this->NewNode("<block>");
	Node* leaf;

	if(this->tk.id == Token::voidTk) {
		token->Tokens.push_back(this->tk);
		this->GetToken();
		leaf = this->Vars();

		if(leaf == nullptr) {
			if(DEBUG){ printf("Block: Vars() returned ε\n"); }
		} else 
			token->Leaves.push_back(leaf);

		if(this->tk.id == Token::returnTk) {
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if(DEBUG){ printf("Block: Returning token\n"); }
			return token;
		}
	}

	leaf = this->Stats();
	if(leaf != nullptr) {
		token->Leaves.push_back(leaf);

		if(this->tk.id == Token::returnTk) {
			token->Tokens.push_back(this->tk);
			this->GetToken();
			if(DEBUG){ printf("Block: Returning token\n"); }
			return token;
		}
	}
	return nullptr;
}

// <program>	->	<vars> <block>
Node* Parser::Program() {
	if(DEBUG)
		std::printf("-->Program\n");

	// <vars>
	Node* token = this->NewNode("<Program>");
	if(this->tk.id == Token::varTk) {
		Node* retrn = this->Vars();

		if(retrn == nullptr) {
			if(DEBUG){ printf("Vars returned ε.\n"); }
		} else
			token->Leaves.push_back(retrn);
	}

	// <block>
	if(this->tk.id == Token::voidTk) {
		Node* retrn = nullptr;
		retrn = this->Block();

		if(retrn == nullptr)
			error("Program: returned nullptr");
		else {
			token->Leaves.push_back(retrn);
			if(DEBUG){ printf("Program-->\n"); }
			return token;
		}
	}

	error("Program: Expected var or void");
	return nullptr;
}

void Parser::GetToken() {
	this->tk = this->scanner.getToken();
	if(DEBUG)
		PrintToken();
}

void Parser::PrintToken() {
	printf("{ %s , ""%s"", %2d }\n",
		Token::Idname[tk.id].c_str(),
		tk.instance.c_str(),
		tk.line);
}

Node* Parser::NewNode(const std::string& lbl) {
	Node* token = nullptr;
	token = new Node(lbl);
	if(!token) {
		error("NewNode: Error creating node");
		return nullptr;
	}
	return token;
}

bool Parser::Parse() {
	if(DEBUG) 
		std::printf("Parse: ");
	this->GetToken();
	this->root = this->Program();
	if(this->tk.id == Token::eofTk)
		return true;
	return false;
}
