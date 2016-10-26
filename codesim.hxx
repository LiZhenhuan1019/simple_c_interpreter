#include <string>
#include <vector>
#include <cstring>
#include <stack>
#include <fstream>
#include <iostream>
using namespace std;

#include "code_fragment.h"
#include "expr_calc.h"
#include "symbol_table.h"


#ifndef DK_COMPILER_CODE_SIMULATOR_HXX
#define DK_COMPILER_CODE_SIMULATOR_HXX

/**
 * Code Simulator
 * @UNFINISHED
 * @usage use bind() to bind Simulator with comment-free source code text.
 *        then use run() to run the simulation.
 * @output the line number will output to a *vector<int>*.
 *         the debug info will output to *cin*.
 */
class Simulator
{
public:
    Simulator(){}
    void bind(string);
    void runSimulation(vector<int>&);

//private:

    class Block
    {
    public:
        enum BlockType
        {
            Jump,           //0
            If,
            Declaration,
            For,
            While,
            DoWhile,        //5
            Calculation,
            Break,
            Empty
        };
        
        string expression;
        vector<Block*> subBlocks;
        BlockType type;
        int lineIndex;

        /**
         * @name calc
         * @return value of expression.
         */
        int calc(symbol_table& variables)
        {
#ifdef DK_TEST_TAG_DONOT_CALCULATE
            cout<<"calculation run!"<<endl;
            return 0;
#else
            expr_calc calcupator(variables);
            return calcupator.value_of(expression);
#endif
        }


        /**
         * @name run
         * @return 0 normally.
         *         1 to terminate function witch runs this block.
         */
        int run(symbol_table& v,vector<int>&out)
        {
            out.push_back(lineIndex);
            switch(type)
            {
                case Jump:
                    calc(v);
                    for(int i=0;i<(int)subBlocks.size();i++)
                        if(subBlocks[i]->run(v,out)) break;
                    break;

                case If:
                    if(calc(v))
                        subBlocks[0]->run(v,out);
                    else
                    {
                        if(subBlocks.size()>0)
                            subBlocks[1]->run(v,out);
                    }
                    break;

                case Declaration:
                    v.add(expression);
                    break;

                case For:
                    for(subBlocks[0]->run(v,out);calc(v);subBlocks[2]->run(v,out))
                        if(subBlocks[1]->run(v,out))
                            break;
                    break;

                case While:
                    while(calc(v))
                        if(subBlocks[0]->run(v,out))
                            break;
                    break;

                case DoWhile:
                    do
                    {
                        if(subBlocks[0]->run(v,out))
                            break;
                    }
                    while(calc(v));
                    break;

                case Calculation:
                    calc(v); //throw return value of this expression!
                    break;

                case Break:
                        return 1;

                case Empty:
                default: break;
            }

            return 0; //exit normally.
        }
        
        Block(){}
        Block(BlockType t,int ld):type(t),lineIndex(ld){ }
    };


    Block Main;
    stack<pair<string,int>> hiddenVars;
    symbol_table curVars;
    string code;
    int ci;  //index of code in current reading.
    int lci; //the beginning point of last ci operation.
    int linenum; //line number in current reading.

    void advance();
    void pass();
    string getword();
    char getsymbol();
    string getInParen();
    void back();

    bool eos();

    Block* build_empty(int);
    Block* build_next(int);
    Block* build_declarations(int);
    Block* build_declaration_single(int);
    Block* build_if(int);
    Block* build_block(int);
    Block* build_for(int);
    Block* build_dowhile(int);
    Block* build_while(int);
    Block* build_printf(int);
    Block* build_expression(int);
    Block* build_expression_nocomma(int);
    Block* build_break(int);
    Block& build_main(int);
};

/**
 * @name bind
 *       bind this simulator to code.
 * @param variables
 * @param codeCache
 *
 */
inline void Simulator::bind(string codeCache)
{
    code=codeCache;
    linenum=1;
    ci=0;
    Main.type=Block::Jump;
    build_main(0);
}

/**
 * @name run
 */
inline void Simulator::runSimulation(vector<int>&out)
{ Main.run(curVars,out); }

/**
 * @name eos
 * @return if ci arrived the tail of the string.
 */
inline bool Simulator::eos()
{ return ci==(int)(code.size()-1); }


/**
 * =========================================================================
 * @NOTICE:
 *      nearly ALL functions below will change the value of code index "ci".
 *      where they're applied,ci will stand on the right of the final character
 *      that the function prcessed.
 * =========================================================================
*/


/**
 * @name advance
 *       let ci go on one character.
 *       adjust line number.
 */
inline void Simulator::advance()
{
    if(eos()) return;
    lci=ci;
    if(code[ci]=='\n') linenum++;
    ci++;
}

/**
 * @name pass
 *       let ci go across spaces and tabs.
 */
inline void Simulator::pass()
{
    lci=ci;
    while(  !eos() &&
            (code[ci]=='\0' ||
             code[ci]==' ' ||
             code[ci]=='\r' ||
             code[ci]=='\n' ||
             code[ci]=='\t'))
        advance();
}

/**
 * @name getword
 *      let ci stand on the right of next word.
 *      where code[ci-1] is the last character of word gotten.
 *      ci will advance as possible if failed.
 * @return the string this function got. Empty if failed.
 * @attention Character space ' ' will NOT terminate reading.
 *            Character '{' is considered as a word. Not the same as '}'.
 *            This function will read:
 *                 letter 'a'~'z','A'~'Z'
 *                 character '{' '_'
 */
inline string Simulator::getword()
{
    pass();
    lci=ci;
    if(code[ci]=='{')
    {
        advance();
        return string("{");
    }

    string t("");
    while(  !eos() && (
            (code[ci]>='a' && code[ci]<='z') ||
            (code[ci]>='A' && code[ci]<='Z') ||
             code[ci]=='_') )
    {
        t.append(1,code[ci]);
        advance();
    }
    return t;
}

/**
 * @name back
 *  undo the ci moving.
 */
inline void Simulator::back()
{ ci=lci; }

/**
 * @name getsymbol
 *      let ci stand on the right of next symbol.
 *      ci will advance as possible if failed;
 *      Character space ' ' will NOT terminate advancing.
 * @process ends with having read the next symbol.
 * @return the symbol got. -1 if failed.
 */
inline char Simulator::getsymbol()
{
    char t=-1;
    pass();
    if( !eos() && (
        code[ci]=='(' ||
        code[ci]==')' ||
        code[ci]==';' ||
        code[ci]==',' ||
        code[ci]=='}') )
    {
        t = code[ci];
        advance();
    }
    return t;
}

/**
  * @name getsymbol
  *     let ci stand on the right of tail of *parenthesis*.
  *     ci will be as before if failed.
  *     the left paren '(' is thrown before call.
  * @process ends with ')'
  * @return the string included by parenthese. empty if failed.
 */
inline string Simulator::getInParen()
{
    string t("");
    pass();
    while(!eos() && code[ci]!=')')
    {
        t.append(1,code[ci]);
        advance();
    }
    getsymbol(); //skip ')'
    return t;
}

/*
===========================================================
===========================================================
*/

/**
 * @name build_next
 * @return a new block alongside with next code block or line.
 */
inline Simulator::Block* Simulator::build_next(int depth=0)
{
    //for(int i=0;i<depth;i++) printf("|   ");
    //printf("create new block: [ci:%d][size:%d][eos:%d] \n",ci,code.size(),eos());
    string token=getword();
    Block*t;
    if(token.compare("int")==0)         t=build_declarations(depth);
    else if(token.compare("if")==0)     t=build_if(depth);
    else if(token.compare("{")==0)      t=build_block(depth);
    else if(token.compare("for")==0)    t=build_for(depth);
    else if(token.compare("do")==0)     t=build_dowhile(depth);
    else if(token.compare("while")==0)  t=build_while(depth);
    else if(token.compare("printf")==0) t=build_printf(depth);
    else if(token.compare("break")==0)  t=build_break(depth);
    else //word is unrecognized or unable to read word.
    {
        back();
        t=build_expression(depth);
        getsymbol(); //skip ';'
    }
    return t;
}

/**
 * @name build_empty
 * @return a do-nothing block.
 * @process ends with ';'
 */
inline Simulator::Block* Simulator::build_empty(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    printf("empt: [ci:%d] \n",ci);
    Block*t=new Block(Block::Empty,linenum);
    t->expression=string("");
    getsymbol(); //skip ';'
    return t;
}

/**
 * @name build_expression
 * @return a block builded following of expression grammer.
 * @process ends with the last character of expression.
*/
inline Simulator::Block* Simulator::build_expression(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    Block* t=new Block(Block::Calculation,linenum);
    string e("");
    while(!eos() && (code[ci]!=';' && code[ci]!=':' && code[ci]!=')'))
    {
        e.append(1,code[ci]);
        ci++;
    }
    t->expression=e;
    printf("expr: %s\n",t->expression.data());
    return t;
}


/**
 * @name build_block
 * @return a block builded by {...;...;} etc.
 * @attention character '{' and '}' are considered as keywords.
 *          character '{' is thrown before call.
 * @process ends with '}'
 */
inline Simulator::Block* Simulator::build_block(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    printf("blk : \n");
    Block* t=new Block(Block::Jump,linenum);
    //expression field in *t is not used.
    char c;
    do
    {
        t->subBlocks.push_back(build_next(depth+1));
        c=getsymbol();
        /*for test*/
        //printf("<<<%d-%c>>>\n",(int)c,c);
    }
    while(c!='}');

    return t;
}


/**
 * @name build_if
 * @return a block builded following  if() ...; else ...;
 * @attention word "if" is thrown befor call.
 */
inline Simulator::Block* Simulator::build_if(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   "); printf("if  : \n");
    Block* t=new Block(Block::If,linenum);
    string exp=getInParen();
    
    t->subBlocks.push_back(build_next(depth+1));

    //pass();
    //printf("<<<%d-%c>>>\n",(int)code[ci],code[ci]);
    if(getword().compare("else")==0) //has 'else' paired.
    {
        //printf("else read.\n");
        t->subBlocks.push_back(build_next(depth + 1));
    }
    else
        back(); //the next word is not "else",
                //must be something else can't be thrown away.
    return t;
}


/**
 * @name build_declareations
 * @return a block builded following the  int ..., ... = ...; and etc.
 * @attention word "int" is thrown before call.
 * @process ends with ';'
 */
inline Simulator::Block* Simulator::build_declarations(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   "); printf("decs:\n");
    Block* t=new Block(Block::Jump,linenum);
    char c;
    do
    {
        //printf("add one declaration:\n");
        int orci=ci;
        t->subBlocks.push_back(build_declaration_single(depth+1)); //Declaration only!
        if(code[ci]=='=')
        {
            ci = orci;
            t->subBlocks.push_back(build_expression_nocomma(depth+1));
        }
        c=getsymbol(); //';' or ','
    }
    while(!eos() && c==',');
    return t;
}

    
/**
 * @name build_declareation_single
 *       a sub-function of build_declaration.
 * @return a block declaring a variable.
 * @attention Declaration ONLY! another assignment function should be placed.
 *            Following the  "... = ... ," or single "...,".
 */
inline Simulator::Block* Simulator::build_declaration_single(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    string w=getword();
    Block*t=new Block(Block::Declaration,linenum);
    t->expression=w;
    printf("decl: %s\n",t->expression.data());
    return t;
}


/**
 * @name build_expression_comma
 * @return a block builded following of expression grammer.
 * @attention comma will not appear in this expression.
 *          also, comma will terminate expression building.
 * @process ends with the last character of expression.
 */
inline Simulator::Block* Simulator::build_expression_nocomma(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    Block* t=new Block(Block::Calculation,linenum);
    string e("");
    while(!eos() && (code[ci]!=',' && code[ci]!=';' && code[ci]!=':' && code[ci]!=')'))
    {
        e.append(1,code[ci]);
        ci++;
    }
    t->expression=e;
    printf("expn: %s \n",t->expression.data());
    return t;
}


/**
 * @name build_for
 * @return a block of For loop.
 * @attention folling grammer "for( ... ; ... ; ...[,...] ) ..."
 *            word 'for' is thrown before call.
 */
inline Simulator::Block* Simulator::build_for(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    printf("for : \n");
    Block*t=new Block(Block::For,linenum);
    getsymbol(); //skip '('
    t->subBlocks.push_back(build_next(depth+1));
    getsymbol(); //skip the first ';'
    t->subBlocks.push_back(build_expression(depth+1));
    getsymbol(); //skip the second ';'
    t->subBlocks.push_back(build_expression(depth+1));
    getsymbol(); //skip ')'
    t->subBlocks.push_back(build_next(depth+1));
    return t;
}

/**
 * @name build_while
 * @return a block of For loop.
 * @attention folling grammer "while(...) ...;"
 *            word "while" is thrown before call.
 */
inline Simulator::Block* Simulator::build_while(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    printf("whil: \n");
    Block*t=new Block(Block::While,linenum);
    getsymbol(); //read '('
    t->expression=getInParen();
    t->subBlocks.push_back(build_next(depth+1));
    return t;
}

/**
 * @name build_dowhile
 * @return a block of For loop.
 * @attention folling grammer "while(...) ...;"
 *            word "do" is thrown before call.
 * @process ends with ';'
 */
inline Simulator::Block* Simulator::build_dowhile(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    printf("dwhi:\n");
    Block*t=new Block(Block::DoWhile,linenum);
    t->subBlocks.push_back(build_next(depth+1));
    getword(); //skip "while"
    getsymbol(); //skip '('
    t->expression=getInParen(); //will skip ')'
    getsymbol(); //skip ';'
    return t;
}

/**
 * @name build_printf
 * @return a block for printf function.
 * @attention assume that there's NO string between the parathesis.
 *            following grammer printf(,,,,, ..., ..., ...);
 *            word "printf" is thrown before call.
 * @process ends with ';'
 */
inline Simulator::Block* Simulator::build_printf(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    printf("prin:\n");
    Block*t=new Block(Block::Jump,linenum);

    /* disabled. too difficult to complete! */
    /*
    getsymbol(); //skip '('
    bool tag=0;
    while(true)
    {
        if(code[ci]=='\"' && code[ci-1]!='\\') tag^=1;
        if(tag==0 && code[ci]==',') break;
        ci++;
    }
    getsymbol(); //read ',' right on the right of const char array(arrays).

    t->subBlocks.push_back(build_expression(depth+1));
    getsymbol(); //skip ')'
    getsymbol(); //skip ';'
     */

    getsymbol(); //skip '('
    t->subBlocks.push_back(build_expression(depth+1));
    getsymbol(); //skip ')'
    getsymbol(); //skip ';'

    return t;
}

/**
 * @name build_break
 * this function will forcely terminate current block.
 * @attention will functionally applied by it's father function in run().
 * @procession ends with ';'
 */
inline Simulator::Block* Simulator::build_break(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    printf("brk :\n");
    Block*t=new Block(Block::Break,linenum);
    getsymbol(); //skip ';'
    return t;
}

/**
 * @name build_main
 * build the main funtion as a block.
 * @process ends with '}'
 */
inline Simulator::Block& Simulator::build_main(int depth=0)
{
    for(int i=0;i<depth;i++) printf("|   ");
    printf("cmin:[size:%d]\n",(int)code.size());
    while(!eos())
        Main.subBlocks.push_back(build_next(depth+1));
    return Main;
}

#endif //DK_COMPILER_CODE_SIMULATOR_HXX

