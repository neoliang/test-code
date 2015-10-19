//
//  Lex.cpp
//  Compiler
//
//  Created by neo on 15-10-17.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include "Lex.h"
namespace Lex{
    ParserType<std::string>::Parser strParser(const std::string& str)
    {
        if (str.empty()) {
            return ParserType<std::string>::ret("");
        }
        else
        {
            auto x = str.front();
            auto xs = str.substr(1);
            return Bind<char, std::string>(charParser(x),
                [str,xs](char)->typename ParserType<std::string>::Parser
                {
                    return Bind<std::string,std::string>(strParser(xs),
                        [str](std::string)->typename ParserType<std::string>::Parser
                    {
                        return ParserType<std::string>::ret(str);
                    });
                });
        }
    }
}