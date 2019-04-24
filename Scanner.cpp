#include "Scanner.h"

const bool DEBUG = 0;
const bool ERRMSG = 1;

char delimAndOpArr[] = {
        '=','<','>',':','+','-','*','/','%','.',
        '(',')',',','{','}',';','[',']'
};


 std::string Keywords[] = {
        "iter",
        "void",
        "var",
        "return",
        "scan",
        "print",
        "program",
        "cond",
        "then",
        "let",
        "int"
 };


int Scanner::filter(){

        if( !this->file ){
                perror("ERROR: File pointer invalid\n");
                return -1;
        }
        return 1;
}

Token::Token Scanner::getToken() {

        Token::Token token;
        char input;
        if ( line == 0 )
                line++;

        while(1) {

                input = fgetc(file);

                // Check for tabs, spaces, newline(increment line if so), carriage return.
                while ( isspace(input) ) {
                        if ( input == '\n' )
                                line++;

                        if(DEBUG)
                                std::printf("%c character found\n", input);

                        input = fgetc(file);
                }

                /*
                delimAndOpArr has the same values in Id and IdNames in Token.h
                However, Id and IdNames start with the keywords,
                so I need to skip over the starting keyword values.
                This is why line: token.id = static_cast<Token::Id>(11+i); has (11+i)
                */
                for(int i = 0; i <= 17; i++ ) {
                        if (input == delimAndOpArr[i]) {
                                buffer += input;
                                token.line = line;
                                token.instance = buffer;
                                buffer.clear();
                                token.id = static_cast<Token::Id>(11+i);
                                return token;
                        }
                }

                // EOF check
                if (input == EOF) {
                        token.line = line;
                        token.instance = "EOF";
                        buffer.clear();
                        token.id = Token::Id::eofTk;
                        return token;
                }

                // Process Letter
                if (isalpha(input)){

                        // First letter of an id / keyword may not be uppercase
                        if ( isupper(input) ) {
                                token.line = line;
                                token.instance = "ERROR";
                                buffer.clear();
                                token.id = Token::Id::errTk;
                                if(ERRMSG)
                                        std::printf("ERROR: '%c' first letter of a id/keyword may not be uppercase\n", input);
                                return token;
                        }

                        // Process any letters or digits
                        while ( isalnum(input) ) {
                        //while ( ( isalpha(input) || isdigit(input) ) && input != ('\n' || '\r' || ' ') ) {
                                buffer += input;
                                input = fgetc(file);
                        }

                        /*
                        Check against Keywords array for matches
                        Keywords array matches beginning of enum Id and IdNames
                        So I cast token.id to i and it will match the Id enum
                        */
                        for (int i = 0; i <= 10; i++) {
                                if (buffer == Keywords[i]) {
                                        token.line = line;
                                        token.instance = buffer;
                                        buffer.clear();
                                        token.id = static_cast<Token::Id>(i);
                                        return token;
                                }
                        }

                        // If there is a space/newline after the word, valid id/keyword. Otherwise, error
                        // It would probably be wise to include EOF condition as well
                        if ( isspace(input) ) {
                                token.line = line;
                                token.instance = buffer;
                                buffer.clear();
                                token.id = Token::Id::identifierTk;
                                return token;
                        }

                        token.line = line;
                        token.instance = "ERROR";
                        buffer.clear();
                        token.id = Token::Id::errTk;
                        if(ERRMSG)
                                std::printf("ERROR: '%c' found while processing letter\n", input);
                        return token;
                }

                // Process Digit
                if(isdigit(input)){

                        while (1) {

                                if( input == ' ' ) {
                                        token.line = line;
                                        token.instance = buffer;
                                        buffer.clear();
                                        token.id = Token::Id::integerTk;
                                        return token;
                                }

                                if( input == '\n' ) {
                                        token.line = line;
                                        token.instance = buffer;
                                        buffer.clear();
                                        token.id = Token::Id::integerTk;
                                        return token;
                                }

                                // Check for letters
                                if ( isalpha(input) ){
                                        token.line = line;
                                        token.instance = buffer;
                                        token.id = Token::Id::errTk;
                                        if(ERRMSG)
                                                std::printf("ERROR: '%c' found while processing digit\n", input);
                                        return token;
                                }

                                // Check for delim and op
                                for(int i = 0; i <= 17; i++ ) {
                                        if (input == delimAndOpArr[i]) {
                                                token.line = line;
                                                token.instance = "ERROR";
                                                buffer.clear();
                                                token.id = Token::Id::errTk;
                                                if(ERRMSG)
                                                        printf("ERROR: '%c' found while processing digit\n", input);
                                                return token;
                                        }
                                }

                                // If the last token is a number with no space at the end, need to catch EOF
                                if (input == EOF) {
                                        token.line = line;
                                        token.instance = buffer;
                                        buffer.clear();
                                        token.id = Token::Id::integerTk;
                                        fputc(input, file); // Put EOF back into the file to be caught
                                        return token;
                                }

                                buffer += input;
                                input = fgetc(file);
                        }

                        // token.line = line;
                        // token.instance = buffer;
                        // buffer.clear();
                        // token.id = Token::Id::integerTk;
                        // return token;

                }

                // Check for comment
                if (input == '&') {

                        // Read until I get to newline
                        while (1) {
                                input = fgetc(file);

                                if(DEBUG)
                                        printf("%c", input);

                                if (input == '\n') {
                                        buffer.clear();
                                        break;
                                }

                                // If I reach the end of file instead, return
                                if (input == EOF) {
                                        token.line = line;
                                        token.instance = "EOF";
                                        buffer.clear();
                                        token.id = Token::Id::eofTk;
                                        return token;
                                }

                        }
                        if(DEBUG)
                                std::printf("\n");
                }

                // Fallthrough condition
                else {
                        buffer = "ERROR";
                        token.line = line;
                        token.instance = buffer;
                        buffer.clear();
                        token.id = Token::Id::errTk;
                        if(ERRMSG)
                                std::printf("ERROR: '%c' found\n", input);
                        return token;
                }
        }
}


//const char * Alphabet[] = {
//      "a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r",
//      "s","t","u","v","w","x","y","z","A","B","C","D","E","F","G","H","I","J",
//      "K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","0","1",
//      "2","3","4","5","6","7","8","9","=","<",">",":","+","-","*","/","%",".",
//      "(",")",",","{","}",";","[","]"," ","\n","\r","&"
//};
//
//const char * tokenArray[] = {
//      "iter","void","return","scan","print",
//      "program","cond","then","let","int"
//      "=","<",">",":","+","-","*","/","%",".",
//      "(",")",",","{","}",";","[","]","identifier",
//      "integer","EOF","error"
//};

//char Alphabet[] = {
//      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r',
//      's','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H','I','J',
//      'K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','0','1',
//      '2','3','4','5','6','7','8','9','=','<','>',':','+','-','*','/','%','.',
//      '(',')',',','{','}',';','[',']',' ','\n','\r','&'
//};
//
//const char * tokenArray[] = {
//      'iter','void','return','scan','print',
//      'program','cond','then','let','int'
//      ,'=','<','>',':','+','-','*','/','%','.',
//      '(',')',',','{','}',';','[',']','identifier',
//      'integer','EOF','error'
//};

// Check for spaces
                // while (input == ' ') {
                //      input = fgetc(file);
                // }

                // // Windows newline is \r\n
                // if (input == '\r') {
                //      if(DEBUG)
                //              std::printf("\\r character found\n");
                //      input = fgetc(file);
                // }

                // // Tabs
                // if (input == '\t') {
                //      input = fgetc(file);
                // }

                // // Normal newline
                // if (input == '\n'){
                //      if(DEBUG)
                //              std::printf("\\n character found\n");
                //      line++;
                //      input = fgetc(file);
                // }

// if (input == '=') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::assignTk;
                //      return token;
                // }

                // if (input == '<') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::lessTk;
                //      return token;
                // }

                // if (input == '>') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::greaterTk;
                //      return token;
                // }

                // if (input == ':') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::colonTk;
                //      return token;
                // }

                // if (input == '+') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::plusTk;
                //      return token;
                // }

                // if (input == '-') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::minusTk;
                //      return token;
                // }

                // if (input == '*') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::minusTk;
                //      return token;
                // }

                // if (input == '/') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::bslashTk;
                //      return token;
                // }

                // if (input == '%') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::modTk;
                //      return token;
                // }

                // if (input == '.') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::dotTk;
                //      return token;
                // }

                // if (input == '(') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::lparenTk;
                //      return token;
                // }

                // if (input == ')') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::rparenTk;
                //      return token;
                // }

                // if (input == ',') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::commaTk;
                //      return token;
                // }

                // if (input == '{') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::lcbracketTk;
                //      return token;
                // }

                // if (input == '}') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::rcbracketTk;
                //      return token;
                // }

                // if (input == ';') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::semicolonTk;
                //      return token;
                // }

                // if (input == '[') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::lbracketTk;
                //      return token;
                // }

                // if (input == ']') {
                //      buffer += input;
                //      token.line = line;
                //      token.instance = buffer;
                //      buffer.clear();
                //      token.id = Token::Id::rbracketTk;
                //      return token;
                // }