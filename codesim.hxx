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
            return -1;
#else
            expr_calc calcupator(variables);
            code_fragment cc(expression);
            return calcupator.value_of(cc);
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

    
private:
    Block Main;
    stack<pair<string,int>> hiddenVars;
    symbol_table curVars;
    string code;
    int ci;
    int linenum;

    void advance();
    void pass();
    string getword();
    char getsymbol();
    string getInParen();

    bool eos();

    Block* build_empty();
    Block* build_next();
    Block* build_declarations();
    Block* build_declaration_single();
    Block* build_if();
    Block* build_block();
    Block* build_for();
    Block* build_dowhile();
    Block* build_while();
    Block* build_printf();
    Block* build_expression();
    Block* build_expression_nocomma();
    Block* build_break();
    Block& build_main();
};

/**
 * @name bind
 *       bind this simulator to code.
 * @param variables
 * @param codeCache
 *
 */
void Simulator::bind(string codeCache)
{
    code=codeCache;
    linenum=1;
    ci=0;
    build_main();
}

/**
 * @name run
 */
void Simulator::runSimulation(vector<int>&out)
{ Main.run(curVars,out); }

/**
 * @name eos
 * @return if ci arrived the tail of the string.
 */
bool Simulator::eos()
{ return ci==(int)code.size(); }


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
void Simulator::advance()
{
    if(eos()) return;
    if(code[ci]=='\n') linenum++;
    ci++;
}


/**
 * @name pass
 *       let ci go across spaces and tabs.
 */
void Simulator::pass()
{ while(!eos() && (code[ci]==' ' || code[ci]=='\r' || code[ci]=='\n' || code[ci]=='\t')) advance(); }


/**
 * @name getword
 *      let ci stand on the right of next word.
 *      where code[ci-1] is the last character of word gotten.
 *      ci will advance as possible if failed.
 * @return the string this function got. Empty if failed.
 * @attention Character space ' ' will NOT terminate reading.
 *            Character '{' is considered as word. Not the same as '}'.
 *            This function will read:
 *                 letter 'a'~'z','A'~'Z'
 *                 character '{' '_'
 */
string Simulator::getword()
{
    string t("");
    pass();
    while(  !eos() && (
            (code[ci]>='a' && code[ci]<='z') ||
            (code[ci]>='A' && code[ci]<='Z') ||
            code[ci]=='{') )
    {
        t.append(1,code[ci]);
        advance();
    }
    return t;
}

/**
 * @name getsymbol
 *      let ci stand on the right of next symbol.
 *      ci will advance as possible if failed;
 *      Character space ' ' will NOT terminate advancing.
 * @return the symbol got. -1 if failed.
 */
char Simulator::getsymbol()
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
  * @return the string included by parenthese. empty if failed.
 */
string Simulator::getInParen()
{
    string t("");
    pass();
    while(!eos() && code[ci]!=')')
    {
        t.append(1,code[ci]);
        advance();
    }
    advance(); //skip ')'
    return t;
}

/**
 * @name build_next
 * @return a new block alongside with next code block or line.
 */
Simulator::Block* Simulator::build_next()
{
    string token=getword();
    if(token.compare("int")==0)         return build_declarations();
    else if(token.compare("if")==0)     return build_if();
    else if(token.compare("{")==0)      return build_block();
    else if(token.compare("for")==0)    return build_for();
    else if(token.compare("do")==0)     return build_dowhile();
    else if(token.compare("while")==0)  return build_while();
    else if(token.compare("printf")==0) return build_printf();
    else if(token.compare("")==0)       return build_empty();
    else if(token.compare("break")==0) return build_break();
    else return build_expression();
}

/**
 * @name build_empty
 * @return a do-nothing block.
 */
Simulator::Block* Simulator::build_empty()
{
    Block*t=new Block(Block::Empty,linenum);
    t->expression=string("");
    getsymbol(); //read ';'
    return t;
}

/**
 * @name build_expression_comma
 * @return a block builded following of expression grammer.
 * @attention comma will not appear in this expression.
 *          also, comma will terminate expression building.
 */
Simulator::Block* Simulator::build_expression_nocomma()
{
    Block* t=new Block(Block::Calculation,linenum);
    string e("");
    while(!eos() && (code[ci]!=',' && code[ci]!=';' && code[ci]!=':' && code[ci]!=')'))
    {
        e.append(1,code[ci]);
        ci++;
    }
    t->expression=e;
    return t;
}

/**
 * @name build_expression
* @return a block builded following of expression grammer.
*/
Simulator::Block* Simulator::build_expression()
{
    Block* t=new Block(Block::Calculation,linenum);
    string e("");
    while(!eos() && (code[ci]!=';' && code[ci]!=':' && code[ci]!=')'))
    {
        e.append(1,code[ci]);
        ci++;
    }
    t->expression=e;
    return t;
}


/**
 * @name build_block
 * @return a block builded by {...;...;} etc.
 * @attention character '{' and '}' are considered as keywords.
 *          character '{' is thrown before call.
 */
Simulator::Block* Simulator::build_block()
{
    Block* t=new Block(Block::Jump,linenum);
    //expression field in *t is not used.
    do { t->subBlocks.push_back(build_next()); } while(getsymbol()!='}');
    return t;
}


/**
 * @name build_if
 * @return a block builded following  if() ...; else ...;
 * @attention word "if" is thrown befor call.
 */
Simulator::Block* Simulator::build_if()
{    
    Block* t=new Block(Block::If,linenum);
    string exp=getInParen();
    
    t->subBlocks.push_back(build_next());
    
    string nextword=getword();
    if(nextword.compare("else")==0) //has 'else' paired.
        t->subBlocks.push_back(build_next());
    else
        ci-=nextword.size(); //the next word is not "else",
                             //must be something else can't be thrown away.
    return t;
}


/**
 * @name build_declareations
 * @return a block builded following the  int ..., ... = ...; and etc.
 * @attention word "int" is thrown before call.
 */
Simulator::Block* Simulator::build_declarations()
{
    Block* t=new Block(Block::Jump,linenum);
    do
    {
        int orci=ci;
        t->subBlocks.push_back(build_declaration_single()); //Declaration only!
        char c=getsymbol();
        if(c=='=')
        {
            ci=orci;
            t->subBlocks.push_back(build_expression_nocomma());
        }
    }
    while(getsymbol()!=';'); //otherwise ',' definately not '='.
    return t;
}

    
/**
 * @name build_declareation_single
 * @return a block declaring a variable.
 * @description a sub-function of build_declaration.
 * @attention Declaration ONLY! another assignment function should be placed.
 *            Following the  "... = ... ," or single "...,".
 */
Simulator::Block* Simulator::build_declaration_single()
{
    string w=getword();
    Block*t=new Block(Block::Declaration,linenum);
    t->expression=w;
    return t;
}

/**
 * @name build_for
 * @return a block of For loop.
 * @attention folling grammer "for( ... ; ... ; ...[,...] ) ..."
 *            word 'for' is thrown before call.
 */
Simulator::Block* Simulator::build_for()
{
    Block*t=new Block(Block::For,linenum);
    getsymbol(); //read '('
    t->subBlocks.push_back(build_expression());
    getsymbol(); //read the first ';'
    t->subBlocks.push_back(build_expression());
    getsymbol(); //read the second ';'
    t->subBlocks.push_back(build_expression());
    getsymbol(); //read ')'
    t->subBlocks.push_back(build_next());
    return t;
}

/**
 * @name build_while
 * @return a block of For loop.
 * @attention folling grammer "while(...) ...;"
 *            word "while" is thrown before call.
 */
Simulator::Block* Simulator::build_while()
{
    Block*t=new Block(Block::While,linenum);
    getsymbol(); //read '('
    t->expression=getInParen();
    t->subBlocks.push_back(build_next());
    return t;
}

/**
 * @name build_dowhile
 * @return a block of For loop.
 * @attention folling grammer "while(...) ...;"
 *            word "do" is thrown before call.
 */
Simulator::Block* Simulator::build_dowhile()
{
    Block*t=new Block(Block::DoWhile,linenum);
    t->subBlocks.push_back(build_next());
    getword(); //read "while"
    getsymbol(); //read '('
    t->expression=getInParen();
    return t;
}

/**
 * @name build_printf
 * @return a block for printf function.
 * @attention following grammer printf("..." "..." "...", ..., ..., ...);
 *            word "printf" is thrown before call.
 */
Simulator::Block* Simulator::build_printf()
{
    Block*t=new Block(Block::Jump,linenum);
    getsymbol(); //read '('
    bool tag=0;
    while(true)
    {
        if(code[ci]=='\"' && code[ci-1]!='\\') tag^=1;
        if(tag==0 && code[ci]==',') break;
    }
    getsymbol(); //read ',' right on the right of const char array(arrays).
    t->subBlocks.push_back(build_expression());
    getsymbol(); //read ')'
    getsymbol(); //read ';'
    return t;
}

/**
 * @name build_break
 * this function will forcely terminate current block.
 * @attention will functionally applied by it's father function in run().
 */
Simulator::Block* Simulator::build_break()
{
    Block*t=new Block(Block::Break,linenum);
    return t;
}

/**
 * @name build_main
 * build the main funtion as a block.
 */
Simulator::Block& Simulator::build_main()
{
    while(ci!=(int)code.size()) Main.subBlocks.push_back(build_next());
    return Main;
}

#endif //DK_COMPILER_CODE_SIMULATOR_HXX

