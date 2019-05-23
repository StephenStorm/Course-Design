#ifndef GRAMMERANALYZER_H_INCLUDED
#define GRAMMERANALYZER_H_INCLUDED

/*
    0-38终极符 39空串
    40-106非终极符
*/
class exp{
public:
    int start;
    vector<int> follow;
    int pos;
    bool operator==(const exp e2)const{
        if(this->start == e2.start&&this->follow==e2.follow&&this->pos == e2.pos)
            return true;
        else
            return false;
    }
    bool operator<(const exp e2)const{
        if(this->start!=e2.start)
            return this->start<e2.start;
        else{
            if(this->pos!=e2.pos)
                return this->pos<e2.pos;
            else
            	if(this->follow!=e2.follow)
            		return this->follow<e2.follow;
        }
    }
    void output()const{
        cout<<"-----------------------------"<<endl;
        cout<<start<<endl;
        for(int i=0;i<follow.size();i++){
            cout<<follow[i]<<" ";
        }
        cout<<endl;
        cout<<pos;
        cout<<endl;
    }
};
class GrammarAnalyzer{
public:
    vector<exp> productionRules;
    vector<int> follow[67];
    vector<int> predict[104];
    ExistedTokenQueue symbols;
private:

    //终极符--非终极符池
    string keywordsFileName;//终极符文件名
    string grammerFileName;//文法文件名
    vector<int> first[67];  //所有的First集合
    int boundary;           //非终极符的最小下标
    int maxIndex;           //非终极符的最大下标
    int startIndex;         //开始符的下标
    char delta[2];
    void generatePredicts(){
        for(unsigned int i=0;i<productionRules.size();i++){
            for(unsigned int j=0;j<productionRules[i].follow.size();j++){
                int curr=productionRules[i].follow[j];
                if(curr<boundary-1){
                    addFirst(&(predict[i]),curr);
                    break;
                }else if(curr==boundary-1){
                    unionFirst(&(predict[i]),&(follow[productionRules[i].start-boundary]));
                    break;
                }else if(!isExistedNull(curr)){//不含空串
                    unionFirst(&(predict[i]),&(first[curr-boundary]));
                    break;
                }else if(j==productionRules[i].follow.size()-1){//含空串，是最后一项
                    unionFirst(&(predict[i]),&(first[curr-boundary]));
                    deleteFirst(&(predict[i]),symbols.add(delta));
                    unionFirst(&(predict[i]),&(follow[productionRules[i].start-boundary]));
                }else{//含空串，不是最后一项
                    unionFirst(&(predict[i]),&(first[curr-boundary]));
                    deleteFirst(&(predict[i]),symbols.add(delta));
                }
            }
        }
    }
    int generateFollows(){
        follow[startIndex-boundary].push_back(symbols.add(delta));
        bool change=true;
        while(change){
            change=false;
            for(unsigned int i=0;i<productionRules.size();i++){
                for(unsigned int j=0;j<(productionRules[i].follow).size();j++){
                    if(productionRules[i].follow[j]>=boundary){
                    for(unsigned int k=j+1;k<(productionRules[i].follow).size();k++){
                        int curr=productionRules[i].follow[k];
                        if(curr<boundary-1){
                            ;//将curr加入到j的Follow
                            if(addFirst(&(follow[(productionRules[i].follow)[j]-boundary]),curr))
                                change=true;
                            break;
                        }else if(this->isExistedNull(curr)){//含有空串
                            ;//将不含空串的curr的First合并到j对应的Follow
                            if(unionFirst(&(follow[(productionRules[i].follow)[j]-boundary]),&(first[curr-boundary])))
                                change=true;
                            deleteFirst(&(follow[(productionRules[i].follow)[j]-boundary]),symbols.add(delta));
                            if(k==(productionRules[i].follow).size()-1){
                                ;//将i的左部的Foloow合并到j对应的Follow
                                 if(unionFirst(&(follow[(productionRules[i].follow)[j]-boundary]),&(follow[productionRules[i].start-boundary])))
                                    change=true;
                            }
                        }else{
                            ;//将不含空串的curr的First合并到j对应的Follow
                            if(unionFirst(&(follow[(productionRules[i].follow)[j]-boundary]),&(first[curr-boundary])))
                                change=true;
                            break;
                        }

                    }
                    if(j==(productionRules[i].follow).size()-1){
                        ;//将i的左部的Foloow合并到j对应的Follow
                        if(unionFirst(&(follow[(productionRules[i].follow)[j]-boundary]),&(follow[productionRules[i].start-boundary])))
                            change=true;
                    }
                    }

                }
            }
        }
        return 0;
    }
    bool addFirst(vector<int>* src,int data){//向First集中添加data
        for(unsigned int i=0;i<(*src).size();i++){
            if((*src)[i]==data){
                return false;
            }
        }
        (*src).push_back(data);
        return true;
    }
    void deleteFirst(vector<int>* src,int data){//从src中删除data
        for(unsigned int i=0;i<(*src).size();i++){
            if((*src)[i]==data){
                (*src).erase((*src).begin()+i,(*src).begin()+i+1);
                return;
            }
        }
    }
//    void unionFirst(int des,int src){       //src的First集合并到des的First集
    bool unionFirst(vector<int>*des,vector<int>*src){       //src的First集合并到des的First集
        bool result=false;
        for(unsigned int i=0;i<src->size();i++){
            bool flag=false;
            for(unsigned int j=0;j<des->size();j++){
                if((*des)[j]==(*src)[i]){
                    flag=true;
                    break;
                }
            }
            if(!flag){
                des->push_back((*src)[i]);
                if((*src)[i]!=symbols.add(delta)){
                    result=true;
                }
            }
        }
        return result;//发生更新返回真
    }
    void generateFirsts(){      //生成全部的First集合
        for(int i=boundary;i<=maxIndex;i++){
            searchFirst(i);
        }
    }

    bool searchFirst(int id){                       //生成id对应的First集
        for(unsigned int i=0;i<productionRules.size();i++){
            if(productionRules[i].start==id){
                for(unsigned int j=0;j<productionRules[i].follow.size();j++){
                    int cur=productionRules[i].follow[j];
                    if(cur<boundary){               //加入终极符
                        addFirst(&(first[id-boundary]),cur);
                        break;
                    }else if(cur==symbols.add(delta)){//加入空串
                        addFirst(&(first[id-boundary]),cur);
                        break;
                    }else{
                        if(!searchFirst(cur)){      //cur的First集合中没有空串
                            unionFirst(&first[id-boundary],&first[cur-boundary]);
                            break;
                        }else{
                            unionFirst(&first[id-boundary],&first[cur-boundary]);
                            deleteFirst(&(first[id-boundary]),symbols.add(delta));
                        }
                    }
                }
            }
        }
        return isExistedNull(id);
    }
    bool isExistedNull(int x){              //是否存在空串
        for(unsigned int i=0;i<first[x-boundary].size();i++){
            if(first[x-boundary][i]==symbols.add(delta)){
                return true;
            }
        }
        return false;
    }
    void getProductionRules(){              //从文件中获取生成式
        fstream infile(grammerFileName.c_str(),ios::in);
        if(!infile){
            cout<<grammerFileName<<" don't be opend"<<endl;
            exit(1);
        }
         CharQueue mychars;
        char ch;
        while(infile){
            exp pr;
            while(infile&&infile.get(ch)){//读取左部
                if(ch!=' '&&ch!='\n'){
                    mychars.addChar(ch);
                }else{
                    if(mychars.getLength()>0){
                        pr.start=symbols.add(mychars.getChars(0,mychars.getLength()-1));
                        mychars.clearChars();
                        break;
                    }
                }
            }
            while(infile&&infile.get(ch)&&ch!='\n'){//读取右部
                if(ch!=' '){
                    mychars.addChar(ch);
                }else{
                    if(mychars.getLength()>0){
                        pr.follow.push_back(symbols.add(mychars.getChars(0,mychars.getLength()-1)));
                        mychars.clearChars();
                    }
                }
            }
            if(mychars.getLength()>0){
                pr.follow.push_back(symbols.add(mychars.getChars(0,mychars.getLength()-1)));
                mychars.clearChars();
            }
            pr.pos=0;
            productionRules.push_back(pr);
        }
        infile.close();
    }
    void showProductionRules(){//显示所有生成式
        for(unsigned int i=0;i<productionRules.size();i++){
            cout<<productionRules[i].start<<":";
            for(unsigned int j=0;j<productionRules[i].follow.size();j++){
                cout<<" "<<productionRules[i].follow[j];
            }
            cout<<endl;
        }
    }
    void getSymbols(string filename){//生成常量池
        fstream infile(filename.c_str(),ios::in);
        if(!infile){
            cout<<filename<<" don't be opend"<<endl;
            exit(1);
        }
        CharQueue mychars;
        char ch;
        while(infile.get(ch)){
            if(ch!=' '&&ch!='\n'){
                mychars.addChar(ch);
            }else{
                if(mychars.getLength()>0){
                    symbols.add(mychars.getChars(0,mychars.getLength()-1));
                  //  countKeywords++;
                    mychars.clearChars();
                }
            }
        }
        if(mychars.getLength()>0){
                symbols.add(mychars.getChars(0,mychars.getLength()-1));
             //   countKeywords++;
                mychars.clearChars();
        }
        infile.close();
    }
public:
    void showPredicts(){
        cout<<"------所有生成式的Predict集合如下所示:"<<endl;
        for(unsigned int i=0;i<productionRules.size();i++){
            cout<<symbols.getChars(productionRules[i].start)<<":";
            for(unsigned int j=0;j<predict[i].size();j++){
                cout<<" "<<symbols.getChars(predict[i][j]);
            }
            cout<<endl;
        }
    }
    bool isExistedFollow(int f,int id){
        for(unsigned int i=0;i<follow[f-boundary].size();i++){
            if(follow[f-boundary][i]==id){
                return true;
            }
        }
        return false;
    }
     void showFirsts(){          //显示所有First集
        cout<<"------所有非终极符的First集合如下所示:"<<endl;
        for(int i=0;i<=maxIndex-boundary;i++){
            cout<<symbols.getChars(i+boundary)<<":";
            for(unsigned int j=0;j<first[i].size();j++){
                cout<<" "<<symbols.getChars(first[i][j]);
            }
            cout<<endl;
        }
    }
    void outputToFirstFile(){
        fstream outfile("./results/firsts.txt",ios::out);
        for(int i=0;i<=maxIndex-boundary;i++){
            outfile<<symbols.getChars(i+boundary)<<":";
            for(unsigned int j=0;j<first[i].size();j++){
                outfile<<" "<<symbols.getChars(first[i][j]);
            }
            outfile<<endl;
        }
        outfile.close();
    }
    void showFollows(){          //显示所有First集
        cout<<"------所有非终极符的Folllow集合如下所示:"<<endl;
        for(int i=0;i<=maxIndex-boundary;i++){
            cout<<symbols.getChars(i+boundary)<<":";
            for(unsigned int j=0;j<follow[i].size();j++){
                cout<<" "<<symbols.getChars(follow[i][j]);
            }
            cout<<endl;
        }
    }
    void outputToFollowFile(){
        fstream outfile("./results/follows.txt",ios::out);
        for(int i=0;i<=maxIndex-boundary;i++){
            outfile<<symbols.getChars(i+boundary)<<":";
            for(unsigned int j=0;j<follow[i].size();j++){
                outfile<<" "<<symbols.getChars(follow[i][j]);
            }
            outfile<<endl;
        }
        outfile.close();
    }
    void outputToPredictFile(){
        fstream outfile("./results/predicts.txt",ios::out);
        for(int i=0;i<104;i++){
            outfile<<i<<":";
            for(unsigned int j=0;j<predict[i].size();j++){
                outfile<<" "<<symbols.getChars(predict[i][j]);
            }
            outfile<<endl;
        }
        outfile.close();
    }
    GrammarAnalyzer(string grammername="./config/Grammer.txt",string keyname="./config/symbols.txt"){

        boundary=40;
        maxIndex=106;
        delta[0]='#';
        delta[1]='\0';
        startIndex=boundary;
        grammerFileName=grammername;
        keywordsFileName=keyname;
        getSymbols(keywordsFileName);
        getSymbols(grammerFileName);
        getProductionRules();

        //showProductionRules();
        generateFirsts();
       // showFirsts();
        generateFollows();
      //  showFollows();
        //symbols.showAll();
        generatePredicts();
        //showPredicts();

    }
    ~GrammarAnalyzer(){
    }
};

#endif // GRAMMERANALYZER_H_INCLUDED
