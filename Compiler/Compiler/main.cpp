//
//  main.cpp
//  Compiler
//
//  Created by neo on 15-10-17.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include <iostream>
#include "Lex.h"
#include <fstream>
#include <sstream>
#include "TinyParser.h"
#include "RunTime.h"
#include <iostream>


using namespace std;
int main(int argc, const char * argv[]) {
    
    if (argc < 2) {
        return  1;
    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == nullptr) {
        cout << "open file error " << argv[1] << endl;
        return 1;
    }
    
    fseek(fp,0,SEEK_END);
    long s = ftell(fp);
    fseek(fp,0,SEEK_SET);
    char *pBuffer  = new  char[s];
    s = fread(pBuffer,sizeof(char), s,fp);
    fclose(fp);
    std::string str(pBuffer,pBuffer +s);
    auto r = Parser::ParserStatementSeq(Lex::ParserStream::fromString(str));
    TinyRunTime runtime;
    if (!r->isNone()) {
        runtime.ExeStatmentSeq(r->value());
    }
    else
    {
        cout << "parser error in " << r->remain().lineNum() << endl;
    }
    return 0;
}
