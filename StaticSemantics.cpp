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

#include "StaticSemantics.h"

const bool DEBUG = 0;
bool ERROR = 0;

bool StaticSemantics::doStatSem(Node* root) {
	if( Traverse(root) )
		return true;

	if(DEBUG)
		std::printf("statSem returning false\n");
	return false;
}

void StaticSemantics::Insert(std::string var) {
	if(DEBUG)
		std::printf("Inserting: %s\n", var.c_str() );
	Variables.push_back(var);
}

bool StaticSemantics::Verify(std::string var) {
	if(DEBUG)
		std::printf("Verifying: %s\n\t", var.c_str() );

	//Could do a simple == here
	int compare;
	for(unsigned int i=0 ; i < Variables.size(); i++ ) {
		compare = Variables[i].compare(var);
		if(compare == 0) {
			if(DEBUG)
				std::printf("%s has already been declared\n", var.c_str());
			return true;
		}
	}
	return false;
}


bool StaticSemantics::Traverse(Node* node) {

	std::function<bool(Node*,int)> find = [&](Node* node, int lvl) {
		
		if(ERROR){ return false; } //Exit in ERROR

		if(!node) 
			return false; //For safety but shouldnt be hit

		/*
		This next section could be made more readable with one loop and a switch 
		checking the label but it works the same.
		*/

		//Vars --> If re delaring error, else insert
		if(node->Label == "<vars>") {
			if(node->Tokens.size() > 0) {
				for(unsigned int it=0; it<node->Tokens.size()-1;it++) {
					if(node->Tokens[it].id == Token::identifierTk) {
						if(Variables.size() > 0) {
							if( Verify(node->Tokens[it].instance) ) {
								std::printf("ERROR: %s has already been declared, exiting\n",
									node->Tokens[it].instance.c_str());
								ERROR = 1;
								return false;
							} else {
								Insert(node->Tokens[it].instance);
							}
						} else {
							Insert(node->Tokens[it].instance);
						}						
					}
				}
			} else {
				if(DEBUG)
					std::printf("%s has no tokens\n", node->Label.c_str());
			}
		} 

		//<in><assign><R> --> if identifier has not been declared, error
		else if (node->Label == "<in>" || node->Label == "<assign>" || node->Label == "<R>" ) {
			if(node->Tokens.size() > 0) {
				for(unsigned int it=0; it<node->Tokens.size();it++) {
					if(node->Tokens[it].id == Token::identifierTk) {
						if( Verify(node->Tokens[it].instance) ) {

						} else {
							std::printf("ERROR: %s has not been defined, exiting\n",
								node->Tokens[it].instance.c_str());
							ERROR = 1;
							return false;
						}
					}
				}
			}
		}

		//Traverse the tree
		for(unsigned int i=0 ; i < node->Leaves.size() ; i++ ) {
			find(node->Leaves[i],lvl+1); 
		}
		return true;

	};
	find(node,0);
	return true;
}

/*
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
*/