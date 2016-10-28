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

void __testout(int len,const char* l)
{
#ifdef DK_COMPILER_CODE_SIMULATOR_TEST_OUTPUT
    for(int i=0;i<len;i++) printf("|   ");
    printf("%s",l);
    printf("\n");
#endif
}

/**
 * Code Simulator
 * @UNFINISHED
 * @usage use bind() to bind Simulator with comment-free source code text.
 *        then use run() to run the simulation.
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
            expr_calc calculator(variables,expression);
            int t=calculator.value_of_expr();
            printf("*** Calc:[%s]=[%d] ***\n",expression.data(),t);
            return t;
        }


        /**
         *
         * @param v symbols.
         * @param hv hidden symbols.
         * @param out to output.
         * @param depth
         * @return resault of calculation
         */
#define NPARAM v,hv,out,depth+1
        int run(symbol_table& v,
                           stack<pair<pair<string,int>,int>>&hv,
                           vector<int>&out,
                           int depth)
        {
            if(lineIndex!=-1) out.push_back(lineIndex);

            int ret=0; //normal return value;

            switch(type)
            {
                case Jump:
                    __testout(depth,"run:");
                    //calc(v);
                    for(int i=0;i<(int)subBlocks.size();i++)
                    {
                        if(subBlocks[i]->type==Break) break;
                        subBlocks[i]->run(NPARAM);
                    }
                    break;

                case If:
                    __testout(depth,"iff:");
                    if(subBlocks[0]->run(NPARAM))
                        subBlocks[1]->run(NPARAM);
                    else
                    {
                        if(subBlocks.size()==3)
                        subBlocks[2]->run(NPARAM);
                    }
                    break;

                case Declaration:
                    __testout(depth,(string("dec:")+expression).data());
                    if(v.exists(expression))
                    {
                       hv.push(pair<pair<string,int>,int>(
                                   pair<string,int>(expression,v.get(expression).value()),
                                   depth));
                        v.remove(expression);
                    }
                    v.add(expression);
                    break;

                case For:
                    __testout(depth,"for:");
                    for(subBlocks[0]->run(NPARAM);
                        subBlocks[1]->run(NPARAM);
                        subBlocks[2]->run(NPARAM))
                        if(subBlocks[3]->type==Break)
                            break;
                        else
                            subBlocks[3]->run(NPARAM);
                    break;

                case While:
                    __testout(depth,"whi");
                    while(subBlocks[0]->run(NPARAM))
                        if(subBlocks[1]->type==Break)
                            break;
                        else
                            subBlocks[1]->run(NPARAM);
                    break;

                case DoWhile:
                    __testout(depth,"dwi");
                    do
                    {
                        if(subBlocks[0]->type==Break)
                            break;
                        else
                            subBlocks[0]->run(NPARAM);
                    }
                    while(subBlocks[1]->run(NPARAM));
                    break;

                case Calculation:
                    __testout(depth,(string("cal:")+expression).data());
                    ret=calc(v); //expression mode.
                    break;

                case Break:
                    __testout(depth,"break");
                    break;

                case Empty:
                    __testout(depth,"empty");

                default: break;
            }
#undef NPARAM

            while(!hv.empty() && hv.top().second==depth)
            {
                //those pushed into this stack by a depth can now free.
                v.remove(hv.top().first.first);
                v.add(hv.top().first.first);
                v.set(hv.top().first.first,hv.top().first.second);
                hv.pop();
            }

            return ret; //exit 0 normally, 1 break.
        }

        Block(){}
        Block(BlockType t,int ld=-1):type(t),lineIndex(ld)
        { if(lineIndex!=-1) printf("[%d]",lineIndex); }
    };


    Block Main;
    stack<pair<pair<string,int>,int>> hiddenVars;
    symbol_table curVars;
    string code;
    int ci;  //index of code in current reading.
    int lci; //the beginning point of last ci operation.
    int linenum; //line number in current reading.
    int llnum; //line number of beginning point of last linenum operation.

    void advance();
    void pass();
    string getword();
    char getsymbol();
    string getInParen();
    string get_expression();
    string get_expression_nocomma();
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
 */
inline void Simulator::bind(string codeCache)
{
    code=codeCache;
    linenum=1;
    ci=0;
    Main.type=Block::Jump;
    Main.lineIndex=-1;
    build_main(0);
}

/**
 * @name run
 * @param out output of line number in running.
 */
inline void Simulator::runSimulation(vector<int>&out)
{ Main.run(curVars,hiddenVars,out,0); }

/**
 * @name eos
 * @return if ci arrived the tail of the string.
 */
inline bool Simulator::eos()
{ return code[ci]=='\0'; }


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
    llnum=linenum;
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
    llnum=linenum;
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
{
    ci=lci;
    linenum=llnum;
}

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
    lci=ci;
    llnum=linenum;
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
  * @BANNED!!!!!!!!
  *     this function will not be used.
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

/**
 * @name get_expression
 * @return
 * @process ends with the last character of expression
 */
inline string Simulator::get_expression()
{
    string e("");
    while(!eos() && (code[ci]!=';' && code[ci]!=':' && code[ci]!=')'))
    {
        e.append(1,code[ci]);
        advance();
    }
    //printf("fc:%c-%d\n",code[ci],code[ci]);
    return e;
}

/**
 * @name get_expression_nocomma
 * @return
 * @process ends with the last character of expression
 */
inline string Simulator::get_expression_nocomma()
{
    string e("");
    while(!eos() && (code[ci]!=';' && code[ci]!=':' && code[ci]!=')' && code[ci]!=','))
    {
        e.append(1,code[ci]);
        advance();
    }
    //printf("fc:%c-%d\n",code[ci],code[ci]);
    return e;
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
    __testout(depth,"empty.");
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
    t->expression=get_expression();
    getsymbol(); //skip ';' or ')'
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
    for(int i=0;i<depth;i++) printf("|   "); printf("blk : \n");
    Block* t=new Block(Block::Jump);
    char c;
    c=getsymbol();
    while(c!='}')
    {
        back();
        t->subBlocks.push_back(build_next(depth+1));
        c=getsymbol();
    }
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
    Block* t=new Block(Block::If);
    t->subBlocks.push_back(build_expression(depth+1));
    t->subBlocks.push_back(build_next(depth+1));
    if(getword().compare("else")==0) //has 'else' paired.
        t->subBlocks.push_back(build_next(depth+1));
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
    Block* t=new Block(Block::Jump);
    char c;
    do
    {
        int orci=ci;
        t->subBlocks.push_back(build_declaration_single(depth+1)); //Declaration only!
        pass();
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
    t->expression=get_expression_nocomma();
    getsymbol(); //skip ';' or ','
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
    for(int i=0;i<depth;i++) printf("|   "); printf("for : \n");
    Block*t=new Block(Block::For);
    getsymbol(); //skip '('
    t->subBlocks.push_back(build_next(depth+1));
    t->subBlocks.push_back(build_expression(depth+1));
    t->subBlocks.push_back(build_expression(depth+1));
    t->subBlocks.push_back(build_next(depth+1));
    for(int i=0;i<depth;i++) printf("|   "); printf("End:for\n");
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
    for(int i=0;i<depth;i++) printf("|   "); printf("whil:\n");
    Block*t=new Block(Block::While);
    getsymbol(); //skip '('
    t->subBlocks.push_back(build_expression(depth+1));
    t->subBlocks.push_back(build_next(depth+1));
    for(int i=0;i<depth;i++) printf("|   "); printf("End:whi\n");
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
    for(int i=0;i<depth;i++) printf("|   "); printf("dwi: ");
    Block*t=new Block(Block::DoWhile);
    t->subBlocks.push_back(build_next(depth+1));
    getword(); //skip "while"
    getsymbol(); //skip '('
    t->subBlocks.push_back(build_expression(depth+1));
    getsymbol(); //skip ';'
    for(int i=0;i<depth;i++) printf("|   "); printf("End:dwi\n");
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
    for(int i=0;i<depth;i++) printf("|   "); printf("prin: \n");
    Block*t=new Block(Block::Jump);

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
    for(int i=0;i<depth;i++) printf("|   "); printf("brk :\n");
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
    for(int i=0;i<depth;i++) printf("|   "); printf("cmin:[size:%d]\n",(int)code.size());
    while(!eos())
    {
        Main.subBlocks.push_back(build_next(depth + 1));
        pass();
        //printf("[%c][%d]\n",code[ci],code[ci]);
    }
    return Main;
}

#endif //DK_COMPILER_CODE_SIMULATOR_HXX

