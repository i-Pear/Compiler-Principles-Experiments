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

    vector<pair<string,string>> result;

    class TrieNode{
    public:
        int next[128];
        bool isBound;

        TrieNode(bool isBound=false){
            memset(next,0,sizeof(next));
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

        void insert(const string &s){
            int p=0;
            for(int i=0;i<s.length();i++){
                if(nodes[p].next[s[i]]!=0){
                    p=nodes[p].next[s[i]];
                }else{
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
            {"const", "CONSTTK"},
            {"int",   "INTTK"},
            {"char",  "CHARTK"},
            {"void",  "VOIDTK"},
            {"main",  "MAINTK"},
            {"if",    "IFTK"},
            {"else",  "ELSETK"},
            {"do",    "DOTK"},
            {"while", "WHILETK"},
            {"for",   "FORTK"},
            {"scanf", "SCANFTK"},
            {"printf","PRINTFTK"},
            {"return","RETURNTK"},
    };
    int builtin_symbols_count=19;
    string builtin_symbols_array[][2]={
            {"+", "PLUS"},
            {"-", "MINU"},
            {"*", "MULT"},
            {"/", "DIV"},
            {"<", "LSS"},
            {"<=","LEQ"},
            {">", "GRE"},
            {">=","GEQ"},
            {"==","EQL"},
            {"!=","NEQ"},
            {"=", "ASSIGN"},
            {";", "SEMICN"},
            {",", "COMMA"},
            {"(", "LPARENT"},
            {")", "RPARENT"},
            {"[", "LBRACK"},
            {"]", "RBRACK"},
            {"{", "LBRACE"},
            {"}", "RBRACE"},
    };
    map<string,string> builtin_tokens,builtin_symbols;

    bool emptyChar(char c){
        return c==' ' || c=='\n' || c=='\r' || c=='\t';
    }

    void parseWord(const string &s){
        if(s.empty())return;
        if(isdigit(s[0])){
            // const token
            result.push_back(make_pair("INTCON",s));
        }else if(builtin_symbols.find(s)!=builtin_symbols.end()){
            // bound symbols
            result.push_back(make_pair(builtin_symbols[s],s));
        }else if(builtin_tokens.find(s)!=builtin_tokens.end()){
            // keywords
            result.push_back(make_pair(builtin_tokens[s],s));
        }else{
            // identifier
            result.push_back(make_pair("IDENFR",s));
        }
    }

    vector<pair<string,string>> parse(){
        // construct auto-machines
        for(int i=0;i<builtin_tokens_count;i++){
            builtin_tokens[builtin_tokens_array[i][0]]=builtin_tokens_array[i][1];
        }
        for(int i=0;i<builtin_symbols_count;i++){
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
            if(c=='\'' || c=='\"'){
                PARSE_WORD
                char left=c;
                ostringstream oss;
                string type=(c=='\'' ? "CHARCON" : "STRCON");
                while((c=ifs.get())!=left){
                    oss<<c;
                }
                result.push_back(make_pair(type,oss.str()));
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
                }else{
                    // step into bound symbol -> pop previous word
                    PARSE_WORD
                    cache="";
                    status=0;
                    cache+=c;
                    status=1;
                }
            }else{
                if(status==1){
                    // current symbol reading ends,
                    // pop symbol and reload
                    PARSE_WORD
                    RESET_AUTOMACHINE
                    goto RESTART;
                }else{
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
    typedef vector<string> P;

    class Grammar{
    public:
        vector<vector<string>> produce;
        Grammar(const vector<vector<string>>& content){
            produce=content;
        }
        void parse(){

        }
    };

    pair<string,Grammar> grammar_list[]={
            make_pair("＜加法运算符＞",Grammar({
                P({"PLUS"}),
                P({"MINU"}),
            })),
            make_pair("＜乘法运算符＞",Grammar({
                P({"MULT"}),
                P({"DIV"}),
            })),
            make_pair("＜关系运算符＞",Grammar({
                P({"LSS"}),
                P({"LEQ"}),
                P({"GRE"}),
                P({"GEQ"}),
                P({"EQL"}),
                P({"NEQ"}),
            })),
            make_pair("＜数字＞",Grammar({
                P({"INTCON"}),
            })),
            make_pair("＜字符＞",Grammar({
                P({"CHARCON"}),
            })),
            make_pair("＜字符串＞",Grammar({
                P({"STRCON"}),
            })),
            make_pair("＜程序＞",Grammar({
                P({""}),
            })),
            make_pair("＜常量说明＞",Grammar({
                P({"CONSTTK","＜常量说明＞1"}),
            })),
            make_pair("＜常量说明＞1",Grammar({
                P({"CONSTTK",""}),
            })),
            make_pair("＜常量定义＞",Grammar({
                                               P({""}),
                                                P({""}),
            })),
            make_pair("＜无符号整数＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜整数＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜标识符＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜声明头部＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜变量说明＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜变量定义＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜类型标识符＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜有返回值函数定义＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜无返回值函数定义＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜复合语句＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜参数表＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜主函数＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜表达式＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜项＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜因子＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜语句＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜赋值语句＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜条件语句＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜条件＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜循环语句＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜步长＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜有返回值函数调用语句＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜无返回值函数调用语句＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜值参数表＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜语句列＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜读语句＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜写语句＞",Grammar({
                P({""}),
                P({""}),
            })),
            make_pair("＜返回语句＞",Grammar({
                P({""}),
                P({""}),
            })),
    };
    map<string,Grammar> grammars;
    queue<pair<string,string>> tokens;

    void parse(){
        while (!tokens.empty())tokens.pop();
        auto res=Lexical::parse();
        for(auto&token:res){
            tokens.push(token);
        }

    }

}

int main()
{
    Grammar::parse();
}