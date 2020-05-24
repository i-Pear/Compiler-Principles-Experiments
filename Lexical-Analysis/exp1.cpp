#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
using namespace std;

#define READ_CHAR c=ifs.get();

enum Type{
    Label=1, Char, String, Digit, Keyword, Bound
};

string getType(Type type){
    const static string trans[]={"iT", "cT", "sT", "CT", "KT", "PT"};
    return trans[type-1];
}

class TrieNode{
public:
    int next[128];
    int id;
    Type type;

    TrieNode(int id, Type type){
        memset(next, 0, sizeof(next));
        this->type=type;
        this->id=id;
    }
};

class AutoMachine{
public:
    static AutoMachine*last;
    Type type;
    int count;
    bool autoIncrease;
    vector<TrieNode> nodes;
    int cnt;
    static string cache;

    AutoMachine(Type type, bool autoIncrease=false){
        this->autoIncrease=autoIncrease;
        cnt=0;
        count=0;
        this->type=type;
        nodes.emplace_back(0, type);
    }

    int getToken(int p=-1){
        if(p==-1)p=cnt;
        if(nodes[p].id!=0){
            return nodes[p].id;
        } else{
            return nodes[p].id=++count;
        }
    }

    void reset(){
        if(!cnt)return;
        output();
        cache="";
    }

    void output(){
        if(!cnt)return;
        if(nodes[cnt].id==0)getToken();
        printf("<%s %2d>\t%s\n", getType(nodes[cnt].type).c_str(), nodes[cnt].id, cache.c_str());
        cnt=0;
    }

    void insert(const string&s, Type t){
        int p=0;
        int len=s.length();
        for(int i=0; i<len; i++){
            if(nodes[p].next[s[i]]!=0){
                p=nodes[p].next[s[i]];
            } else{
                nodes.emplace_back(0, this->type);
                nodes[p].next[s[i]]=nodes.size()-1;
                p=nodes[p].next[s[i]];
            }
        }
        getToken(p);
        nodes[p].type=t;
    }

    static void clearLast(){
        if(!last)return;
        if(last){
            last->reset();
            last=nullptr;
        }
    }

    void next(char c){
        if(last!=this){
            clearLast();
        }
        last=this;

        if(nodes[cnt].next[c]!=0){
            cnt=nodes[cnt].next[c];
        } else{
            if(autoIncrease){
                nodes.emplace_back(0, type);
                nodes[cnt].next[c]=nodes.size()-1;
                cnt=nodes[cnt].next[c];
            } else{
                reset();
                next(c);
                return;
            }
        }
        cache+=c;
    }
};

AutoMachine*AutoMachine::last=nullptr;
string AutoMachine::cache;

const string boundsArray[]={
        "[", "]", "(", ")", ".",
        "->", "-", "++", "--", "*",
        "&", "!", "~", "/", "%",
        "+", "-", "<<", ">>", ">",
        ">=", "<", "<=", "==", "!=",
        "^", "|", "&&", "||", "?",
        ":", "=", "/=", "*=", "%=",
        "+=", "-=", "<<=", ">>=", "&=",
        "^=", "|=", ",", "{", "}", ";"
};
const string keywordsArray[]={
        "if", "else", "while", "signed", "throw", "union", "this", "int", "char", "double", "unsigned", "const", "goto",
        "virtual", "for", "float", "break", "auto", "class", "operator", "case", "do", "long", "typedef", "static",
        "friend", "template",
        "default", "new", "void", "register", "extern", "return", "enum", "inline", "try", "short", "continue",
        "sizeof", "switch",
        "private", "protected", "asm", "while", "catch", "delete", "public", "volatile", "struct", "true", "false"
};

AutoMachine mLabel(Label, true), mChar(Char, true),
        mString(String, true), mConst(Digit, true),
        mBound(Bound);

bool isEmptyChar(char c){
    return c==' '||c=='\r'||c=='\n'||c=='\t';
}

bool isBound(char c){
    return ((!isalnum(c))&&(c!='_'));
}

int main(){
    // construct auto-machines
    for(const auto&i : boundsArray){
        mBound.insert(i, Bound);
    }
    for(const auto&i : keywordsArray){
        mLabel.insert(i, Keyword);
    }
    mLabel.count=0;
    // read file
    ifstream ifs("code.txt");
    char c=ifs.get();
    do{
        if(isEmptyChar(c)){
            AutoMachine::clearLast();
            READ_CHAR
        } else if(c=='\''){
            // specially check strings with ''
            char left=c;
            while((c=ifs.get())!=left){
                mChar.next(c);
            }
            READ_CHAR
        } else if(c=='\"'){
            // specially check strings with ""
            char left=c;
            while((c=ifs.get())!=left){
                mString.next(c);
            }
            READ_CHAR
        } else if(isBound(c)){
            mBound.next(c);
            READ_CHAR
        } else if(isdigit(c)){
            do{
                mConst.next(c);
            } while(c=ifs.get(), isdigit(c)||c=='.');
        } else{
            do{
                mLabel.next(c);
            } while(c=ifs.get(), !isBound(c));
        }
    } while(c!=EOF);
    AutoMachine::clearLast(); // deal with the last object
    ifs.close();
}
