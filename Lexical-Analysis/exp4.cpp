#include <iostream>
#include <cstdio>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>

namespace Lexical{

#define PARSE_WORD parseWord(cache);
#define RESET_AUTOMACHINE cache="";status=0;autoMachine.reset();

    using namespace std;

    vector<pair<string, string>> result;

    class TrieNode{
    public:
        int next[128];
        bool isBound;

        TrieNode(bool isBound=false){
            memset(next, 0, sizeof(next));
            this->isBound=isBound;
        }
    };

    class Trie{
    public:
        vector<TrieNode> nodes;
        int cnt;

        Trie(){
            cnt=0;
            nodes.push_back(TrieNode());
        }

        void insert(const string&s){
            int p=0;
            for(int i=0; i<s.length(); i++){
                if(nodes[p].next[s[i]]!=0){
                    p=nodes[p].next[s[i]];
                } else{
                    nodes.push_back(TrieNode());
                    nodes[p].next[s[i]]=nodes.size()-1;
                    p=nodes[p].next[s[i]];
                }
            }
            nodes[p].isBound=true;
        }

        bool available(char c){
            return nodes[cnt].next[c]!=0;
        }

        void reset(){
            cnt=0;
        }

        void next(char c){
            if(nodes[cnt].next[c]!=0){
                cnt=nodes[cnt].next[c];
            }
        }
    };

    Trie autoMachine;

    int builtin_tokens_count=13;
    string builtin_tokens_array[][2]={
            {"const",  "CONSTTK"},
            {"int",    "INTTK"},
            {"char",   "CHARTK"},
            {"void",   "VOIDTK"},
            {"main",   "MAINTK"},
            {"if",     "IFTK"},
            {"else",   "ELSETK"},
            {"do",     "DOTK"},
            {"while",  "WHILETK"},
            {"for",    "FORTK"},
            {"scanf",  "SCANFTK"},
            {"printf", "PRINTFTK"},
            {"return", "RETURNTK"},
    };
    int builtin_symbols_count=19;
    string builtin_symbols_array[][2]={
            {"+",  "PLUS"},
            {"-",  "MINU"},
            {"*",  "MULT"},
            {"/",  "DIV"},
            {"<",  "LSS"},
            {"<=", "LEQ"},
            {">",  "GRE"},
            {">=", "GEQ"},
            {"==", "EQL"},
            {"!=", "NEQ"},
            {"=",  "ASSIGN"},
            {";",  "SEMICN"},
            {",",  "COMMA"},
            {"(",  "LPARENT"},
            {")",  "RPARENT"},
            {"[",  "LBRACK"},
            {"]",  "RBRACK"},
            {"{",  "LBRACE"},
            {"}",  "RBRACE"},
    };
    map<string, string> builtin_tokens, builtin_symbols;

    bool emptyChar(char c){
        return c==' '||c=='\n'||c=='\r'||c=='\t';
    }

    void parseWord(const string&s){
        if(s.empty())return;
        if(isdigit(s[0])){
            // const token
            result.push_back(make_pair("INTCON", s));
        } else if(builtin_symbols.find(s)!=builtin_symbols.end()){
            // bound symbols
            result.push_back(make_pair(builtin_symbols[s], s));
        } else if(builtin_tokens.find(s)!=builtin_tokens.end()){
            // keywords
            result.push_back(make_pair(builtin_tokens[s], s));
        } else{
            // identifier
            result.push_back(make_pair("IDENFR", s));
        }
    }

    vector<pair<string, string>> parse(){
        // construct auto-machines
        for(int i=0; i<builtin_tokens_count; i++){
            builtin_tokens[builtin_tokens_array[i][0]]=builtin_tokens_array[i][1];
        }
        for(int i=0; i<builtin_symbols_count; i++){
            builtin_symbols[builtin_symbols_array[i][0]]=builtin_symbols_array[i][1];
            autoMachine.insert(builtin_symbols_array[i][0]);
        }
        result.clear();

        // read file
        ifstream ifs("testfile.txt");
        string cache;
        int status=0; // 0=others 1=symbol
        char c;
        while((c=ifs.get())!=EOF){
            // specially check strings with "" or ''
            if(emptyChar(c)){
                PARSE_WORD
                RESET_AUTOMACHINE
                continue;
            }
            if(c=='\''||c=='\"'){
                PARSE_WORD
                char left=c;
                ostringstream oss;
                string type=(c=='\'' ? "CHARCON" : "STRCON");
                while((c=ifs.get())!=left){
                    oss<<c;
                }
                result.push_back(make_pair(type, oss.str()));
                RESET_AUTOMACHINE
                continue;
            }
            RESTART:;
            if(autoMachine.available(c)){
                autoMachine.next(c);
                if(status==1){
                    // continue reading symbols
                    cache+=c;
                    continue;
                } else{
                    // step into bound symbol -> pop previous word
                    PARSE_WORD
                    cache="";
                    status=0;
                    cache+=c;
                    status=1;
                }
            } else{
                if(status==1){
                    // current symbol reading ends,
                    // pop symbol and reload
                    PARSE_WORD
                    RESET_AUTOMACHINE
                    goto RESTART;
                } else{
                    // continue reading labels
                    status=0;
                    cache+=c;
                }
            }
        }
        PARSE_WORD // parse final word
        ifs.close();
        return result;
    }

}

namespace Grammar{

    using namespace std;

    class Grammar{
    public:
        vector<vector<Grammar>> produce;
        bool isToken;
        bool isLeaf;
        string content;

        Grammar(const string&s, bool isToken, bool isLeaf){
            this->isToken=isToken;
            this->isLeaf=true;
            content=s;
        }

        Grammar(const string&s, bool isToken){
            this->isToken=isToken;
            isLeaf=false;
            produce.push_back({});
            produce[0].push_back(Grammar(s,isToken, true));
        }

        Grammar operator*(const Grammar&b){
            for(auto&i:produce){
                i.push_back(b);
            }
            return *this;
        }

        Grammar operator+(const Grammar&b){
            for(auto&i:b.produce){
                produce.push_back(i);
            }
            return *this;
        }

        void parse(){

        }
    };

#define G(s) Grammar(s,false)
#define T(s) Grammar(s,true)
#define mp make_pair

    pair<string,Grammar> grammar_list[]={
            mp("＜加法运算符＞",T("PLUS")+T("MINU")),
            mp("＜乘法运算符＞",T("MULT")+T("DIV")),
            mp("＜关系运算符＞",T("LSS")+T("LEQ")+T("GRE")+T("GEQ")+T("EQL")+T("NEQ")),
            mp("＜字符＞",T("CHARCON")),
            mp("＜字符串＞",T("STRCON")),
            mp("＜程序＞",),
            mp("＜常量说明＞",),
            mp("＜常量定义＞",),
            mp("＜无符号整数＞",),
            mp("＜整数＞",),
            mp("＜标识符＞",),
            mp("＜声明头部＞",),
            mp("＜变量说明＞",),
            mp("＜变量定义＞",),
            mp("＜类型标识符＞",T("INTTK")+T("CHARTK")),
            mp("＜有返回值函数定义＞",G("＜声明头部＞")*T("LPARENT")*G("＜参数表＞")*T("RPARENT")*T("LBRACE")*G("＜复合语句＞")*T("RBRACE")),
            mp("＜无返回值函数定义＞",T("VOIDTK")*T("IDENFR")*T("LPARENT")*G("＜参数表＞")*T("RPARENT")*T("LBRACE")*G("＜复合语句＞")*T("RBRACE")),
            mp("＜复合语句＞",),
            mp("＜参数表＞",G("＜类型标识符＞")*T("IDENFR")),
            mp("＜主函数＞",T("VOIDTK")*T("MAINTK")),
            mp("＜表达式＞",),
            mp("＜项＞",),
            mp("＜因子＞",),
            mp("＜语句＞",),
            mp("＜赋值语句＞",),
            mp("＜条件语句＞",),
            mp("＜条件＞",),
            mp("＜循环语句＞",),
            mp("＜步长＞",),
            mp("＜有返回值函数调用语句＞",),
            mp("＜无返回值函数调用语句＞",),
            mp("＜值参数表＞",),
            mp("＜语句列＞",),
            mp("＜读语句＞",),
            mp("＜写语句＞",),
            mp("＜返回语句＞",),
    };

    map<string, Grammar> grammars;
    queue<pair<string, string>> tokens;

    void parse(){
        while(!tokens.empty())tokens.pop();
        auto res=Lexical::parse();
        for(auto&token:res){
            tokens.push(token);
            cout<<token.first<<" "<<token.second<<endl;
        }

    }

}

int main(){
    Grammar::parse();
}