#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace Token{

        enum Id {
                iterTk, voidTk, varTk, returnTk, scanTk, printTk,
                programTk, condTk, thenTk, letTk, intTk,

                assignTk, lessTk, greaterTk, colonTk, plusTk, minusTk,
                multTk, bslashTk, modTk, dotTk, lparenTk, rparenTk, commaTk,
                lcbracketTk, rcbracketTk, semicolonTk, lbracketTk, rbracketTk,

                identifierTk, integerTk, eofTk, errTk
        };

        const std::string Idname[] = {
                "iterTk","voidTk","varTk","returnTk","scanTk",
                "printTk","programTk","condTk","thenTk","letTk","intTk",

                "assignTk","lessTk","greaterTk","colonTk","plusTk","minusTK",
                "multTk","bslashTk","modTk","dotTk","lparenTk","rparenTk","commaTk",
                "lcbracketTk","rcbracketTk","semicolonTk","lbracketTk","rbracketTk",

                "identifierTk","integerTk","EOFTk","errorTk"
        };

        typedef struct Token_T {
                Id id;
                std::string instance;
                int line;
        }Token;

}
#endif

/*

// const std::string Idname[] = {
        //      "iter","void","var","return","scan","print","program","cond","then","let","int",

        //      "assign","less","greater","colon","plus","minus",
        //      "mult","bslash","mod","dot","lparen","rparen","comma",
        //      "lcbracket","rcbracket","semicolon","lbracket","rbracket",

        //      "identifier","integer","EOF","error"
        // };

//const std::string Idname[] = {
//      "iter","void","return","scan","print",
//      "program","cond","then","let","int",

//      "=","<",">",":","+","-","*","/","%",".",
//      "(",")",",","{","}",";","[","]",
//
//      "identifier","integer","EOF","error"
//};

// typedef enum tokenInstances {

//      //Keywords
//      ITER, VOIDtk, VAR, RETURN, SCAN, PRINT, PROGRAM, COND,
//      THEN, LET, INTtk,

//      //Digits / Identifiers
//      DIGIT,
//      IDENTIFIER,

//      //Operators / Delimiter / White Space
//      ASSIGN, LESS, GREATER, COLON, PLUS, MINUS, MULT, BSLASH
//      MOD, DOT, LPAREN, RPAREN, COMMA, LCBRACKET, RCBRACKET,
//      SEMICOLON, LBRACKET, RBRACKET, SPACE

// } Instance;

// const string IdName[] = {
//      "Keyword",
//      "Integer",
//      "Identifier",
//      "Operator",
//      "Delimiter",
//      "EoF",
//      "Error"
// };

*/