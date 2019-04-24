#include "StaticSemantics.h"

const bool DEBUG = 1;

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

	int compare;

	for(unsigned int i=0 ; i < Variables.size(); i++ ) {
		// if(DEBUG)
		// 	printf("Variables[%i]: %s\n", i, Variables[i].c_str());
		compare = Variables[i].compare(var);
		if(compare == 0) {
			if(DEBUG)
				std::printf("%s is already inside the scope\n", var.c_str());
			return false;
		}
	}
	Insert(var);
	return true;

}

bool StaticSemantics::Traverse(Node* node) {

	// std::function<bool(Node*,int)> find = [&](Node* node, int lvl) {
	// 	if(!node) 
	// 		return false;
	// 	for(unsigned int i=0 ; i < node->Leaves.size() ; i++ ) {
	// 		find(node->Leaves[i],lvl+1);
	// 		if(!Verify(var)) 
	// 			return false;
	// 	}
	// 	return true;
	// };
	// find(node,0);
	// return true;


	std::function<bool(Node*,int)> find = [&](Node* node, int lvl) {
		
		if(!node) 
			return false;

		//Descend the tree to the left
		for(unsigned int i=0 ; i < node->Leaves.size() ; i++ ) {
			find(node->Leaves[i],lvl+1);

			if(node->Tokens.size() > 0) {

				//Iterate through Leave's Tokens vector
				for(unsigned int it=0; it<node->Tokens.size()-1;it++) {
					
					if(node->Tokens[it].id == Token::identifierTk) {
						
						// if(DEBUG)
						// 	printf("%s",node->Tokens[it].instance.c_str() );

						//Insert/Verify
						if(Variables.size() > 0) {
							if( !Verify(node->Tokens[it].instance) ) {
								return false;
							}
						} else {
							Insert(node->Tokens[it].instance);
							//Variables[0] = node->Tokens[it].instance;
						}

					
					}
				
				}

			}
			
			// if(!Verify(var)) 
			// 	return false;
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