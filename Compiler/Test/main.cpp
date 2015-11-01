//
//  main.cpp
//  Test
//
//  Created by neo on 15-10-28.
//  Copyright (c) 2015年 user. All rights reserved.
//

#include "rapidcheck.h"


#include <fstream>
#include <iostream>
#include "Lex.h"
#include "TestTinyParser.h"
using namespace std;
using namespace Lex;

void TestLex();
int main() {
    TestLex();
    TestTinyParser();
    return 0;
}
