#ifndef LEXICALANALYZER_H_INCLUDED
#define LEXICALANALYZER_H_INCLUDED


class Token{
public:
    int kind;//�����Ϣ
    int line;//�к�
    int column;//�к�
    Token *next;//�¸�token
    int data;//token������Ϣ
};
class CharQueue{//���ڱ���Ŀǰ��ȡ�����ַ���
private:
    int maxLength;
    int length;
    char *p;
public:
    CharQueue(){
        maxLength=10;
        p=new char[maxLength];
        length=0;
    }
    int getLength(){
        return length;
    }
    int addChar(char ch){
        if(length>=maxLength){//�ռ䲻�㣬��������ռ�
            char *temp=p;
            maxLength+=10;
            p=new char[maxLength];
            for(int i=0;i<length;i++){
                p[i]=temp[i];
            }
            delete temp;
        }
        p[length++]=ch;
        return length;
    }
    int clearChars(){
        delete p;
        maxLength=10;
        p=new char[maxLength];
        length=0;
        return length;
    }
    char * getChars(int low,int high){
        int n=high-low+1;
        char * temp=new char[n+1];
        for(int i=0;i<n;i++){
            temp[i]=p[low++];
        }
        temp[n]='\0';
        return temp;
    }
    ~CharQueue(){
        delete p;
    }
};
class ExistedTokenQueue{
private:
    char **p;
    int maxLength;
    int length;
    int compare(char* src,char* des){
        while(*(src++)==*(des++)){
            if(*(src-1)=='\0'){
                return 1;
            }
        }
        return 0;
    }
public:
    ExistedTokenQueue(){
        maxLength=10;
        p=new char*[maxLength];
        length=0;
    }
    int isExisted(char *token){
        for(int i=0;i<length;i++){
            if(compare(p[i],token)==1){//���
                return i;
            }
        }
        return 0;
    }
    int add(char * token){
        for(int i=0;i<length;i++){
            if(compare(p[i],token)==1){//���
                return i;
            }
        }
        //����µ�token
        if(length>=maxLength){
            char** temp=p;
            maxLength+=10;
            p=new char*[maxLength];
            for(int i=0;i<length;i++){
                p[i]=temp[i];
            }
            delete temp;
        }
        p[length++]=token;
        return length-1;
    }
    ~ExistedTokenQueue(){
        for(int i=0;i<length;i++){
            delete p[i];
        }
        delete p;
    }
    char * getChars(int index){
        if(index>=length){
            return NULL;
        }else{
            return p[index];
        }
    }
    void showAll(){
        for(int i=0;i<length;i++){
            char *temp=p[i];
            cout<<i<<":";
            while(*temp!='\0'){
                cout<<*(temp++);
            }
            cout<<endl;
        }
    }
    int getLength(){
        return length;
    }
};
class TokenQueue{
private:
    Token *head;
    Token *curr;
    Token *tail;
public:
    TokenQueue(){
        head=NULL;
        curr=NULL;
        tail=NULL;
    }
    int addToken(Token *token){
        if(head==NULL){//�ն���
            head=token;
            tail=token;
            curr=head;
        }else{
            tail->next=token;
            tail=token;
        }
        return 0;
    }
    Token* getNext(){
        Token *temp=curr;
        if(curr!=NULL){
            curr=curr->next;
        }
        return temp;
    }
    void resetHead(){
        curr=head;
    }
    ~TokenQueue(){
        resetHead();
        Token *temp;
        while((temp=getNext())!=NULL){
            delete temp;
        }
    }
    void showAll(ExistedTokenQueue* keywords){
        resetHead();
        Token *temp;
        while((temp=getNext())!=NULL){
            cout<<"kind:"<<temp->kind<<"\tline:"<<temp->line<<"\tinfo:"<<keywords->getChars(temp->data)<<endl;
        }
    }
};

class LexicalAnalyzer{
private:
    ExistedTokenQueue keywords;//�ؼ��ֱ�+�������ű�
    int countKeywords;      //�ж��Ƿ��ǹؼ���
    TokenQueue* tokens;     //���յ�token����
    int **stateTransition;  //״̬ת������
    int m;                  //״̬ת�����������
    int n;                  //״̬ת�����������
    int line;               //������¼��
    int column;             //������¼��
    string *ErrorMessage;   //������ʾ��
    int maxSuccessState;    //���ĳɹ�״̬���
    int countError;         //������ʾ����Ŀ
    string srcFileName;     //Դ�����ļ���
    string stateTransitionFileName;//״̬ת�������ļ���
    string errorMessageFileName;//������ʾ�ļ���
    string keywordsFileName;    //�ؼ����ļ���
public:
    void setStateTransition(string name){
        stateTransitionFileName=name;
    }
    void setErrorMessage(string name){
        errorMessageFileName=name;
    }
    void setKeywords(string name){
        keywordsFileName=name;
    }
    void showTokens(){
        cout<<"------Դ����<"<<srcFileName<<">��Token����������ʾ:"<<endl;
        tokens->resetHead();
        Token *temp;
        while((temp=tokens->getNext())!=NULL){
            cout<<"kind:"<<temp->kind<<"\tline:"<<temp->line<<"\tcolumn:"<<temp->column<<"\tinfo:"<<keywords.getChars(temp->data)<<endl;
        }
        tokens->resetHead();
    }
    ExistedTokenQueue * getSymbols(){
        return &keywords;
    }
    TokenQueue* getTokens(){//����ӿ�
        return tokens;
    }
    LexicalAnalyzer(string name,string statename="./config/StateTransition.txt",string errorname="./config/ErrorMessage.txt",string keyname="./config/symbols.txt"){
        srcFileName=name;
        setStateTransition(statename);
        setErrorMessage(errorname);
        setKeywords(keyname);
        tokens=new TokenQueue;
        line=1;
        countKeywords=0;
        getKeywords();
        getStateTransition();
        getErrorMessage();
        parseAll();
        //outputToFile();
        //���Բ���
        //showStateTransition();
        //tokens->showAll(&keywords);
    }
    ~LexicalAnalyzer(){
        delete ErrorMessage;
        for(int i=0;i<m;i++){
            delete stateTransition[i];
        }
        delete stateTransition;
        delete tokens;
    }
    void outputToFile(){
        string name=srcFileName;
        name.erase(name.size()-4,name.size());
        name+="_tokens.txt";
        fstream outfile(name.c_str(),ios::out);
        tokens->resetHead();
        Token *temp;
        while((temp=tokens->getNext())!=NULL){
            outfile<<"kind:"<<temp->kind<<"\tline:"<<temp->line<<"\tcolumn:"<<temp->column<<"\tinfo:"<<keywords.getChars(temp->data)<<endl;
        }
        tokens->resetHead();
        outfile.close();
    }
private:

    void getErrorMessage(){
        //fstream infile("./config/ErrorMessage.txt",ios::in);
        fstream infile(errorMessageFileName.c_str(),ios::in);
        if(!infile){
            //cout<<"the file--ErrorMessage.txt don't be opend"<<endl;
            cout<<errorMessageFileName<<" don't be opend"<<endl;
            exit(1);
        }
        infile>>countError;
        ErrorMessage=new string[countError];
        for(int i=0;i<countError;i++){
            infile>>ErrorMessage[i];
        }
    }
    void showErrorMessage(){
        for(int i=0;i<4;i++){
            cout<<ErrorMessage[i]<<endl;
        }
    }
    int parseChar(char ch){
        if((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')){
            return 0;
        }
        if(ch>='1'&&ch<='9'){
            return 2;
        }
        if(ch==' '||ch=='\t'||ch=='\n')
            return 7;
        int result=1;
        switch(ch){
            case '0':result=1;break;
            case ':':result=3;break;
            case '=':result=4;break;
            case '.':result=5;break;
            case '\'':result=6;break;
            case '{':result=8;break;
            case '}':result=9;break;
            case '+':result=10;break;
            case '-':result=11;break;
            case '*':result=12;break;
            case '/':result=13;break;
            case '<':result=14;break;
            case '(':result=15;break;
            case ')':result=16;break;
            case '[':result=17;break;
            case ']':result=18;break;
            case ';':result=19;break;
            default:result=20;break;
        }
        return result;
    }
    void getKeywords(){
        fstream infile(keywordsFileName.c_str(),ios::in);
        if(!infile){
            cout<<keywordsFileName<<" don't be opend"<<endl;
            exit(1);
        }
        CharQueue mychars;
        char ch;
        while(infile.get(ch)){
            if(ch!=' '&&ch!='\n'){
                mychars.addChar(ch);
            }else{
                if(mychars.getLength()>0){
                    keywords.add(mychars.getChars(0,mychars.getLength()-1));
                    countKeywords++;
                    mychars.clearChars();
                }
            }
        }
        if(mychars.getLength()>0){
                keywords.add(mychars.getChars(0,mychars.getLength()-1));
                countKeywords++;
                mychars.clearChars();
        }
        infile.close();
    }
    void getStateTransition(){
        fstream infile(stateTransitionFileName.c_str(),ios::in);
        if(!infile){
            cout<<stateTransitionFileName<<" don't be opend"<<endl;
            exit(1);
        }
        infile>>m>>n>>maxSuccessState;
        stateTransition=new int*[m];
        for(int i=0;i<m;i++){
            stateTransition[i]=new int[n];
            for(int j=0;j<n;j++){
                infile>>stateTransition[i][j];
            }
        }
        infile.close();
    }
    void showStateTransition(){
        for(int i=0;i<m;i++){
            cout<<"<"<<i<<">";
            for(int j=0;j<n;j++){
                cout<<stateTransition[i][j]<<" ";
            }
            cout<<endl;
        }
    }
    void parseAll(){
        //fstream infile("./SourceRoutine.txt",ios::in);
        fstream infile(srcFileName.c_str(),ios::in);
        if(!infile){
            cout<<srcFileName<<" don't be opend"<<endl;
            exit(1);
        }
        char ch;
        int state=0;
        int index;
        CharQueue chars;
        infile.get(ch);
        index=parseChar(ch);
        column=1;
        while(infile){
            state=0;
            while(infile!=0&&stateTransition[state][index]>0){
                if(ch=='\n'){
                    line++;
                    column=0;
                }
                chars.addChar(ch);
                state=stateTransition[state][index];
                infile.get(ch);
               // if(ch=='\t'){
               //     column+=8;
               // }else
                    column++;
                index=parseChar(ch);
            }
            if((state>=1&&state<=7)||(state>=10&&state<=maxSuccessState)){//�ɹ�״̬
                if(stateTransition[state][index]==-2){
                    cout<<"��"<<line<<"�е�"<<column<<"��:"<<ErrorMessage[2]<<'\''<<ch<<"\',�Ƿ��ı�ʶ��"<<endl;
                    exit(1);
                }
                Token* token=new Token;
                token->next=NULL;
                token->line=line;
                token->column=column-chars.getLength();
                if(state==7){
                    token->data=keywords.add(chars.getChars(1,chars.getLength()-2));
                    token->kind=token->data;
                }else{
                    token->data=keywords.add(chars.getChars(0,chars.getLength()-1));
                    if(state==1){//�����ǹؼ��ֻ���������ʶ��
                        if(token->data>38){
                            token->kind=0;
                        }else{
                            token->kind=token->data;
                        }
                    }else if(state==2||state==3){//0 D
                        token->kind=1;
                    }else if(state==21){//,
                        token->kind=2;
                    }else if(state==20||state==19){//] ;
                        token->kind=state-19+7;
                    }
                    else{
                        token->kind=token->data;
                    }
                }
                tokens->addToken(token);
                chars.clearChars();
            }else if(state!=8&&state!=9){//ʧ��״̬
                    state=stateTransition[state][index];
                    state=-state;
                    if(!infile){
                        cout<<"��"<<line<<"�е�"<<column<<"��:"<<ErrorMessage[0]<<endl;
                    }else if(state==2){
                        cout<<"��"<<line<<"�е�"<<column<<"��:"<<ErrorMessage[state]<<'\''<<ch<<'\''<<endl;
                    }else{
                        cout<<"��"<<line<<"�е�"<<column<<"��:"<<ErrorMessage[state]<<endl;
                    }
                exit(1);
            }else{                      //ע�ͻ�հ�
                chars.clearChars();
            }
        }



        Token* token=new Token;
        token->kind=ends;
        token->next=NULL;
        token->line=line;
        token->column=column-chars.getLength();
        tokens->addToken(token);
        infile.close();
    }
};



#endif // LEXICALANALYZER_H_INCLUDED
