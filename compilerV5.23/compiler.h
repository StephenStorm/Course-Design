class compiler : public GrammarAnalyzer{
	private:
	    int LLMatrix[notNum][Num+1];//LL(1)分析方法的分析表
		int matrix[300][Num+notNum+1];//SLR(1)方法的分析表
		vector<set<exp> > state;//LRSM0
	public:
		bool equalSet(set<exp> s1,set<exp> s2);
		//判断两个集合是否相等
		set<exp>  closure(set<exp> a);
		//求集合的闭包
		bool inFollow(int a,int in);
		void analysis();
		void driver();
		void showMatrix();
		void showState(int a);
		void generateLL();
		void showLLMatrix();
		void LLDriver();
		//void init(int a);
		//void initial();

};


//求状态机的闭包i
set<exp>  compiler::closure(set<exp> a){
    int tstart,tpos,lastSize;
    set<exp> b;
    do{
        lastSize = a.size();
        for(set<exp>::iterator it=a.begin();it!=a.end();it++){
            tpos=it->pos;
            if(tpos<it->follow.size()&&it->follow[0]!=eps)  tstart = it->follow[tpos];
            else  continue;//该表达式分析完毕
            //分析到最后边或者是右部为eps 则不进行拓展
            for(int i=0;i<productionRules.size();i++){
                if(productionRules[i].start==tstart){
                    b.insert(productionRules[i]);
                    //插入新表达式
                }

            }
        }
        a.insert(b.begin(),b.end());
        b.clear();
    }while(a.size()!=lastSize);
    return a;
}
/*
void compiler::initial(){
    string  filename("./SourceRoutine.txt");
    LexicalAnalyzer test(filename);                        //词法分析器
    //test.showTokens();                           //显示token序列
//    this->tokens=test.getTokens();         //获得token序列
    ExistedTokenQueue *symbols=test.getSymbols();//获得符号表
}

void compiler::init(int a){
    int tmp;
    switch(a){
    case 1:
        freopen("in2.txt","r",stdin);
        for(int i=1;i<=7;i++){
            exp e;
            cin>>e.start;
            cin>>tmp;
            while(tmp!=22){
                e.follow.push_back(tmp);
                cin>>tmp;
            }
            e.pos = 0;
            productionRules.push_back(e);
        }
        follow[0].push_back(6); follow[0].push_back(0); follow[0].push_back(4); follow[0].push_back(5);
        follow[1].push_back(6); follow[1].push_back(0); follow[1].push_back(4); follow[1].push_back(1); follow[1].push_back(5);
        follow[2].push_back(6); follow[2].push_back(0); follow[2].push_back(4); follow[2].push_back(1); follow[2].push_back(5);
        input.push_back(2);
        input.push_back(1);
        input.push_back(2);
        input.push_back(0);
        input.push_back(2);
        input.push_back(5);
        input.push_back(ends);
        break;
    case 2:
        freopen("in3.txt","r",stdin);
        for(int i=1;i<=3;i++){
            exp e;
            cin>>e.start;
            cin>>tmp;
            while(tmp!=22){
                e.follow.push_back(tmp);
                cin>>tmp;
            }
            e.pos = 0;
            productionRules.push_back(e);
        }
        input.push_back(1); input.push_back(2); input.push_back(1); input.push_back(2); input.push_back(0);input.push_back(ends);
        follow[0].push_back(0);follow[0].push_back(2);
        follow[1].push_back(3);
        break;
    }
}
*/
//判断两个集合是否相等
bool compiler::equalSet(set<exp> s1,set<exp> s2){
	if(s1.size()!=s2.size()) return false;
	set<exp>::iterator it1=s1.begin(),it2=s2.begin();
	while(it1!=s1.end()&&*it1==*it2) {
		it1++;
		it2++;
	}
	if(it1==s1.end()) return true;
	return false;
}


//判断是否在follow  集中
bool compiler::inFollow(int a,int in){
    in=in-Num-1;
	for(int i=0;i<follow[in].size();i++){
        if(follow[in][i]==a) return true;
	}
	return false;
}

//获得状态集合和分析表
void compiler::analysis(){
    //state 存放所有状态
    int i,j,k,l,start;
	//exp ttoken->kind=exp;
	set<exp> test;
    test.insert(productionRules[0]);
    test = closure(test);
    state.push_back(test);

    //遍历所有状态
    for(i=0;i<state.size();i++){
    	set<exp> cur = state[i];
    	map<int,bool> vis;
    	vis.clear();
    	//遍历当前状态的所有表达式 ,查找还未被处理过的符号
    	for(set<exp>::iterator it=cur.begin();it!=cur.end();it++){
    		//到达表达式结尾,或者右部为空
    		if(it->pos==it->follow.size()||it->follow[0]==eps){
				for(k=0;k<productionRules.size();k++){
					if(productionRules[k].start==it->start&&productionRules[k].follow == it->follow){
						if(k==0){//开始表达式
						//"#"  表示 接受
							matrix[i][ends]=success;
							//结束符
						}
						else{
							//所有终极符和 # 都是r，负数表示  rn
							for(l=0;l<=Num;l++)//只对在follow集中的终极符 填入 表达式序号
								if(inFollow(l,it->start)){//l 在 it->start follow 集中
                                    if(matrix[i][l]!=0) cout<<"congtu\n";
                                    matrix[i][l]=-(k+1);
								}
						}
					}
				}
				continue;
			}
			//点的位置不在表达式尾
    		start = it->follow[it->pos];
    		if(vis[start]) continue;
    		//该符号已经分析过  跳过
    		vis[start]=true;
    		set<exp> newsta;
    		//投影
    		for(set<exp>::iterator it1=cur.begin();it1!=cur.end();it1++){
				if(it1->pos<(it1->follow.size())&&(it1->follow[it1->pos]==start)&& (it1->follow[0]!=eps) ){
					exp texp=*it1;
    				texp.pos++;
    				newsta.insert(texp);
				}
			}
			newsta=closure(newsta);
    		for(j=0;j<state.size();j++){
    			//查找是否已经存在该状态
    			if(equalSet(state[j],newsta)){
    				matrix[i][start]=j;
    				break;
				}
			}
			//如果不存在该状态
			if(j==state.size()){
				//添加新状态
				state.push_back(newsta);
				matrix[i][start]=j;
			}
		}
	}
}


void compiler::showMatrix(){
    for(int i=0;i<state.size();i++){
        cout<<"state: "<<i<<"  ";
    	for(int j=0;j<=Num;j++)
			printf("%4d",matrix[i][j]);
		cout<<endl;
	}
}


//按照分析表进行 分析转换
void compiler::driver(){
    //获得Token序列
    string  filename("./SourceRoutine.txt");
    LexicalAnalyzer test(filename);                        //词法分析器
    //test.token->kind=showTokens();                           //显示token序列
    TokenQueue* tokens=test.getTokens();         //获得token序列
   // ExistedTokenQueue *symbols=test.getSymbols();//获得符号表
    tokens->resetHead();
    Token * t=tokens->getNext();

    exp texp;
    vector<int> sta,sym;
    //状态栈和符号栈
	sta.push_back(0);
	int i=0,k,l,j;
	int c1;
	while(t!=NULL){
        c1=t->kind;

        cout<<"state stack:  ";
        for(int x=0;x<sta.size();x++) cout<<sta[x]<<" ";
        cout<<"   ";
        cout<<"symbol stack:  ";
        for(int x=0;x<sym.size();x++) cout<<sym[x]<<" ";
        cout<<endl;

        k=matrix[sta.back()][c1];
        if(k==success) {
            cout<<"compile successfully !\n"<<endl;
            break;
        }
        if(k>0){
            sta.push_back(k);
            sym.push_back(c1);
            t=tokens->getNext();
        }
        else{
            if(k<0){
            //进行归约
                l=-k-1;
                texp = productionRules[l];
                if(texp.follow[0]==eps){
                    //右部为空 ，则不弹栈
                }
                else{
                    for(j=0;j<texp.follow.size();j++){
                        sta.pop_back();
                        sym.pop_back();
                    }
                }
                sym.push_back(texp.start);
                sta.push_back(matrix[sta.back()][texp.start]);
            }
            else{
                if(k==0) {
                    cout<<"error1: "<<"line: "<<t->line<<"   "<<"col: "<<t->column<<endl;
                    /*
                    cout<<"state stack:  ";
                    for(int x=0;x<sta.size();x++) cout<<sta[x]<<" ";
                    cout<<"     ";
                    cout<<"symbol stack:  ";
                    for(int x=0;x<sym.size();x++) cout<<sym[x]<<" ";
                    */
                    //cout<<"The "<<i<<"st symbol: "<<symbols.getChars(c1)<<endl;
                    //showState(sta.back());
                    break;
                }
            }
        }
	}
}
void compiler::showState(int a){
    if(a==-1){
         cout<<"\nstate num: "<<state.size()<<endl;
         for(int i=0;i<state.size();i++){
            cout<<"state: "<<i<<"  "<<"state size: "<<state[i].size()<<endl;
            for(set<exp>::iterator it=state[i].begin();it!=state[i].end();it++){
                cout<<symbols.getChars(it->start)<<"--->";
                for(int x=0;x<it->follow.size();x++)
                    cout<<symbols.getChars(it->follow[x])<<"  ";
                cout<<it->pos<<endl;
            }
            cout<<endl<<endl;
        }
    }
    else{
        for(set<exp>::iterator it=state[a].begin();it!=state[a].end();it++){
            cout<<symbols.getChars(it->start)<<"--->";
            for(int x=0;x<it->follow.size();x++)
                cout<<symbols.getChars(it->follow[x])<<"  ";
            cout<<it->pos<<endl;
        }
    }
}




//自顶向下  LL（1）分析方法
void compiler::generateLL(){
    int i,j,start,vt,rulesNum = productionRules.size();
    for(i=0;i<rulesNum;i++){
        start = productionRules[i].start;
        for(j=0;j<predict[i].size();j++){
            vt=predict[i][j];
            LLMatrix[start-40][vt] = i+1;
        }
    }
}
void compiler::showLLMatrix(){
    for(int i=0;i<notNum;i++){
        for(int j=0;j<=Num;j++)
            printf("%4d",LLMatrix[i][j]);
        cout<<endl;
    }
}
void compiler::LLDriver(){
    string  filename("./SourceRoutine.txt");
    LexicalAnalyzer test(filename);                        //词法分析器
    TokenQueue* tokens=test.getTokens();         //获得token序列
    tokens->resetHead();
    Token * t=tokens->getNext();
    vector<int> sym;
    sym.push_back(40);
    int c,exp,top;
    while(t->kind!=ends){
        if(sym.empty()) {
            cout<<"this program has more code !"<<endl;
            break;
        }
        cout<<"symbol stack: ";
        for(int x=0;x<sym.size();x++){
            printf("%3d",sym[x]);
        }
        cout<<endl;
        c=t->kind;
        top = sym.back();
        if(top>=40){//属于非终极符
            top-=40;
            exp = LLMatrix[top][c];
            if(exp==0){
                cout<<"error symbol:  "<<symbols.getChars(c)<<endl;
                cout<<"line:  "<<t->line<<"   col:  "<<t->column<<endl;

                break;
            }
            else{
                exp--;
                //判断表达式右部是不是为空
                if(productionRules[exp].follow[0]==eps){
                    sym.pop_back();
                }
                else{//产生式右部不为空
                    sym.pop_back();
                    for(vector<int>::reverse_iterator it = productionRules[exp].follow.rbegin();it!=productionRules[exp].follow.rend();it++){
                        sym.push_back(*it);
                    }
                }
            }
        }
        else{//符号栈顶元素为终极符
            if(top==c){
                sym.pop_back();
                t=tokens->getNext();
            }
            else{
                cout<<"sym not match!"<<endl;
                cout<<"line:  "<<t->line<<"   col:  "<<t->column<<endl;
                break;
            }
        }
    }
    if(sym.empty()&&t->kind==ends) cout<<"compiler successfully !"<<endl;
    else cout<<"compiler error !!!"<<endl;
}
