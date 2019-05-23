#include<fstream>
#include<iostream>
#include<cstdlib>
#include<vector>
#include<set>
#include<map>
using namespace std;
/*
//init 1
#define notNum 3 //67
#define Num 6//39
#define ends 6//39
#define eps 6//39
*/

//init 0
#define notNum 67
#define Num 39
#define ends 39
#define eps 39
//*/
/*
//init 2
#define notNum 2
#define Num 3
#define ends 3
#define eps 3
*/

#define success 10000
#include "LexicalAnalyzer.h"                     //词法分析头文件
#include "GrammerAnalyzer.h"
#include "compiler.h"

int main()
{
    //freopen("in3.txt","r",stdin);
    //freopen("Matrix.txt","w",stdout);
    compiler com;
    //com.symbols.showAll();
    /*
    //SLR（1） 分析方法

    com.analysis();
    com.driver();
         //com.symbols.showAll();
         // com.showState(-1);
         //com.showMatrix();
    */
    ///*
    //LL(1)分析方法
    com.generateLL();
    //com.showLLMatrix();
    com.LLDriver();
    //*/
    return 0;
}
