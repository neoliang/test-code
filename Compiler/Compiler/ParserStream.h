//
//  ParserStream.h
//  Compiler
//
//  Created by neo on 15-10-20.
//  Copyright (c) 2015年 user. All rights reserved.
//

#ifndef __Compiler__ParserStream__
#define __Compiler__ParserStream__
#include <sstream>
#include <fstream>
#include <memory>
#include <iostream>
namespace Lex {
    
    class ParserStream
    {
        std::string::iterator end;
    private:
        std::shared_ptr<std::string> _stream;
        unsigned int _lineNum = 0;
        std::string::iterator iter;
        unsigned int _currentCol =0;
        
        void moveToNext()
        {
            if (empty()) {
                throw std::bad_exception();
            }
            ++iter;
            if (*iter == '\n') {
                ++_lineNum;
                _currentCol = 0;
            }
            else
            {
                ++_currentCol;
            }
        }
        ParserStream(std::string* stream)
        {
            //stream->unsetf(std::ios::skipws);
            _stream = std::shared_ptr<std::string>(stream);
            iter = _stream->begin();
            end = _stream->end();
        }
    public:
        ParserStream(){};
        static ParserStream fromString(const std::string& content)
        {
            return ParserStream(new std::string(content));
        }
//        static ParserStream fromFile(const std::string& file)
//        {
//            std::ifstream* fs = new std::ifstream();
//            fs->open(file);
//            if (fs->is_open()) {
//                return ParserStream(fs);
//            }
//            else
//            {
//                throw std::bad_exception();
//            }
//        }
    public:
        bool empty()const
        {
            return iter == end;
        }
        char get()const
        {
            if (empty()) {
                std::cout << "try to ge empty stream" << std::endl;
                throw std::bad_exception();
            }
            return *iter;
        }
        unsigned int lineNum()const
        {
            return _lineNum;
        }
        unsigned int colNum()const
        {
            return _currentCol;
        }
        ParserStream next()const
        {
            ParserStream temp = *this;
            temp.moveToNext();
            return temp;
        }
    };
        
}
#endif /* defined(__Compiler__ParserStream__) */
