#include<fstream>
#include<iostream>
#include<cstdlib>
#include<vector>
#include<set>
#include<map>
using namespace std;

#define notNum 67
#define Num 39
#define ends 39
#define eps 39


#define success 10000
#include "LexicalAnalyzer.h"                     //词法分析头文件
#include "GrammerAnalyzer.h"
#include "compiler.h"

int main()
{
    compiler com;
    int choise;
    cin>>choise;
    if(choise==1){
        com.analysis();
        com.driver();
    }
    else{
        com.generateLL();
        //com.showLLMatrix();
        com.LLDriver();
    }

    //com.symbols.showAll();
    ///*
    //SLR（1） 分析方法


         //com.symbols.showAll();
         // com.showState(-1);
         //com.showMatrix();
   // */
    /*
    //LL(1)分析方法
    com.generateLL();
    //com.showLLMatrix();
    com.LLDriver();
    */
    return 0;
}
