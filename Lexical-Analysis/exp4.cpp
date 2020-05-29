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

/*
 namespace Grammar{

    using namespace std;

    class Grammar{
    public:
        vector<vector<Grammar>> produce;
        bool isLeaf;
        string content;

        Grammar(const string&s, bool isLeaf){
            this->isLeaf=true;
            content=s;
        }

        Grammar(const string&s){
            isLeaf=false;
            produce.push_back({});
            produce[0].push_back(Grammar(s, true));
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

    typedef Grammar G;

    map<string, Grammar> grammars;
    queue<pair<string, string>> tokens;

    void parse(){
        auto a=G("a");
        auto b=G("b");
        auto c=a*b;
        int i=1;

        while(!tokens.empty())tokens.pop();
        auto res=Lexical::parse();
        for(auto&token:res){
            tokens.push(token);
        }

    }

}
 */

namespace Grammar{

    using namespace std;

    queue<pair<string,string>> words;
#define N words.front().first

    class Analyze{
    public:
        bool 程序(){
            常量说明();
            变量说明();
            while (有返回值函数定义()||无返回值函数定义());
            主函数();
        }
        bool 常量说明(){
            if(N=="CONSTTK")
        }
        bool 常量定义(){

        }
        bool 无符号整数(){

        }
        bool 整数(){

        }
        bool 声明头部(){

        }
        bool 变量说明(){

        }
        bool 变量定义(){

        }
        bool 类型标识符(){

        }
        bool 有返回值函数定义(){

        }
        bool 无返回值函数定义(){

        }
        bool 复合语句(){

        }
        bool 参数表(){

        }
        bool 主函数(){

        }
        bool 表达式(){

        }
        bool 项(){

        }
        bool 因子(){

        }
        bool 语句(){

        }
        bool 赋值语句(){

        }
        bool 条件语句(){

        }
        bool 条件(){

        }
        bool 循环语句(){

        }
        bool 步长(){

        }
        bool 有返回值函数调用语句(){

        }
        bool 无返回值函数调用语句(){

        }
        bool 值参数表(){

        }
        bool 语句列(){

        }
        bool 读语句(){

        }
        bool 写语句(){

        }
        bool 返回语句(){

        }

    };

    void parse(){
        auto lex=Lexical::parse();
        for(auto&i:lex){
            words.push(i);
        }

    }
}

int main(){
    Grammar::parse();
}