#include <fstream>
//#include "Parser.h"
#include "StaticSemantics.h"

int main(int argc,char** argv) {
	std::string fileName;
	Parser parser;
	StaticSemantics statSem(parser);
	const bool DEBUG = 0;

	if(argc > 1) {
		fileName = argv[1];
		fileName.append(".sp19");

		std::FILE * file = std::fopen(fileName.c_str(),"r");
		if(!file) {
			std::printf("Error opening file: \"%s\", exiting\n"
				,argv[1]);
			return -1;
		}

		parser.GetFile(file);
		if(parser.Parse()) {
			if(DEBUG)
				std::printf("Parser completed execution\n");
			//parser.Print(argv[1]);
		} else {
			if(DEBUG)
				std::printf("Parsing could not complete\n");
		}

		statSem.doStatSem(parser.root);

		std::fclose(file);	
	} else {
		parser.GetFile(stdin);
		
		if(parser.Parse()){
			if(DEBUG)
				std::printf("Parser completed execution\n");
				//parser.Print("out");
		}
		else {
			if(DEBUG)
				std::printf("Parser could not complete\n");
		}
		statSem.doStatSem(parser.root);
	}

	return 0;
}