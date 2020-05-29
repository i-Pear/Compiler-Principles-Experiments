#include <iostream>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>

namespace Lexical{

#define PARSE_WORD parseWord(cache);
#define RESET_AUTOMACHINE cache="";status=0;autoMachine.reset();

    using namespace std;

    vector<pair<string, string> > result;

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

    vector<pair<string, string> > parse(){
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
            mp("<加法运算符>",T("PLUS")+T("MINU")),
            mp("<乘法运算符>",T("MULT")+T("DIV")),
            mp("<关系运算符>",T("LSS")+T("LEQ")+T("GRE")+T("GEQ")+T("EQL")+T("NEQ")),
            mp("<字符>",T("CHARCON")),
            mp("<字符串>",T("STRCON")),
            mp("<程序>",),
            mp("<常量说明>",),
            mp("<常量定义>",),
            mp("<无符号整数>",),
            mp("<整数>",),
            mp("<标识符>",),
            mp("<声明头部>",),
            mp("<变量说明>",),
            mp("<变量定义>",),
            mp("<类型标识符>",T("INTTK")+T("CHARTK")),
            mp("<有返回值函数定义>",G("<声明头部>")*T("LPARENT")*G("<参数表>")*T("RPARENT")*T("LBRACE")*G("<复合语句>")*T("RBRACE")),
            mp("<无返回值函数定义>",T("VOIDTK")*T("IDENFR")*T("LPARENT")*G("<参数表>")*T("RPARENT")*T("LBRACE")*G("<复合语句>")*T("RBRACE")),
            mp("<复合语句>",),
            mp("<参数表>",G("<类型标识符>")*T("IDENFR")),
            mp("<主函数>",T("VOIDTK")*T("MAINTK")),
            mp("<表达式>",),
            mp("<项>",),
            mp("<因子>",),
            mp("<语句>",),
            mp("<赋值语句>",),
            mp("<条件语句>",),
            mp("<条件>",),
            mp("<循环语句>",),
            mp("<步长>",),
            mp("<有返回值函数调用语句>",),
            mp("<无返回值函数调用语句>",),
            mp("<值参数表>",),
            mp("<语句列>",),
            mp("<读语句>",),
            mp("<写语句>",),
            mp("<返回语句>",),
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
 */

namespace Grammar{

    using namespace std;

    vector<pair<string, string> > words;
    map<string,bool> func_info;

#define N words[0].first
#define R cout<<words[0].first<<" "<<words[0].second<<endl; words.erase(words.begin());
#define W(s) cout<<s<<endl; cout.flush();

    class Analyze{
    public:

        static bool STRING(){
            if(N=="STRCON"){
                R
                W("<字符串>");
                return true;
            } else{
                return false;
            }
        }

        static bool UNSIGNED(){
            if(N=="INTCON"){
                R
                W("<无符号整数>")
                return true;
            } else{
                return false;
            }
        }

        static bool SIGNED(){
            if(N=="PLUS"||N=="MINU"){
                R
            }
            if(UNSIGNED()){
                W("<整数>");
                return true;
            } else{
                return false;
            }
        }

        static bool ADD_OPR(){
            if(N=="PLUS"||N=="MINU"){
                R
                return true;
            } else{
                return false;
            }
        }

        static bool MUL_OPR(){
            if(N=="MULT"||N=="DIV"){
                R
                return true;
            } else{
                return false;
            }
        }

        static bool CMP_OPR(){
            if(N=="LSS"||N=="LEQ"||N=="GRE"||N=="GEQ"||N=="EQL"||N=="NEQ"){
                R
                return true;
            } else{
                return false;
            }
        }

        static bool PROG(){
            CONST_DECLARATION();
            VAR_DECL();
            while(FUNC_WITH_VAL()||FUNC_WITHOUT_VAL());
            MAIN_FUNC();
            W("<程序>");
            return true;
        }

        static bool CONST_DECLARATION(){
            if(N=="CONSTTK"){
                R
                CONST_DEF();
                R
                while (N=="CONSTTK"){
                    R
                    CONST_DEF();
                    R
                }
                W("<常量说明>");
                return true;
            } else{
                return false;
            }
        }

        static bool CONST_DEF(){
            if(N=="INTTK"){
                R
                R
                R
                SIGNED();
                while(N=="COMMA"){
                    R
                    R
                    R
                    SIGNED();
                }
                W("<常量定义>");
                return true;
            } else if(N=="CHARTK"){
                R
                R
                R
                R
                while(N=="COMMA"){
                    R
                    R
                    R
                    R
                }
                W("<常量定义>");
                return true;
            }{
                return false;
            }
        }

        static bool DECL_HEADER(){
            if(N=="INTTK"||N=="CHARTK"){
                R
                func_info[words[0].second]=true;
                R
                W("<声明头部>");
                return true;
            } else{
                return false;
            }
        }

        static bool VAR_DECL(){
            if(words[2].first=="LPARENT")return false;
            if(VAR_DEFINE()){
                R
                while(words[2].first!="LPARENT"&&VAR_DEFINE()){
                    R
                }
                W("<变量说明>");
                return true;
            } else{
                return false;
            }
        }

        static bool VAR_DEFINE(){
            if(words[1].first=="MAINTK")return false;
            if(TYPE_SYMBOL()){
                R
                if(N=="LBRACK"){
                    R
                    UNSIGNED();
                    R
                }
                while(N=="COMMA"){
                    R
                    R
                    if(N=="LBRACK"){
                        R
                        UNSIGNED();
                        R
                    }
                }
                W("<变量定义>");
                return true;
            } else{
                return false;
            }
        }

        static bool TYPE_SYMBOL(){
            if(N=="INTTK"||N=="CHARTK"){
                R
                return true;
            } else{
                return false;
            }
        }

        static bool FUNC_WITH_VAL(){
            if(DECL_HEADER()){
                R
                ARGUMENTS();
                R
                R
                MIXED_SENTENCE();
                R
                W("<有返回值函数定义>");
                return true;
            } else{
                return false;
            }
        }

        static bool FUNC_WITHOUT_VAL(){
            if(words[1].first=="MAINTK")return false;
            if(N=="VOIDTK"){
                R
                func_info[words[0].second]=false;
                R

                R
                ARGUMENTS();
                R

                R
                MIXED_SENTENCE();
                R
                W("<无返回值函数定义>");
                return true;
            } else{
                return false;
            }
        }

        static bool MIXED_SENTENCE(){
            CONST_DECLARATION();
            VAR_DECL();
            if(SENTENCE_LIST()){
                W("<复合语句>");
                return true;
            } else{
                return false;
            }
        }

        static bool ARGUMENTS(){
            if(TYPE_SYMBOL()){
                R
                while(N=="COMMA"){
                    R
                    TYPE_SYMBOL();
                    R
                }
            }
            W("<参数表>");
            return true;
        }

        static bool MAIN_FUNC(){
            if(N=="VOIDTK"){
                R
                R
                R
                R
                R
                MIXED_SENTENCE();
                R
                W("<主函数>");
                return true;
            } else{
                return false;
            }
        }

        static bool EXPRESSION(){
            if(N=="PLUS"||N=="MINU"){
                R
            }
            if(ITEM()){
                while(ADD_OPR()){
                    ITEM();
                }
                W("<表达式>");
                return true;
            } else{
                return false;
            }
        }

        static bool ITEM(){
            if(FACTOR()){
                while(MUL_OPR()){
                    FACTOR();
                }
                W("<项>");
                return true;
            } else{
                return false;
            }
        }

        static bool FACTOR(){
            if(FUNC_CALL_VAL()){
                W("<因子>");
                return true;
            }else if(N=="IDENFR"){
                R
                if(N=="LBRACK"){
                    R
                    EXPRESSION();
                    R
                }
                W("<因子>");
                return true;
            } else if(N=="LPARENT"){
                R
                EXPRESSION();
                R
                W("<因子>");
                return true;
            } else if(SIGNED()){
                W("<因子>");
                return true;
            } else if(N=="CHARCON"){
                R
                W("<因子>");
                return true;
            } else{
                return false;
            }
        }

        static bool SENTENCE(){
            if(CONDITION_SENTENCE()){
                W("<语句>");
                return true;
            }
            if(LOOP_SENTENCE()){
                W("<语句>")
                return true;
            }
            if(N=="LBRACE"){
                R
                SENTENCE_LIST();
                R
                W("<语句>");
                return true;
            }
            if(FUNC_CALL_VAL()){
                R
                W("<语句>");
                return true;
            }
            if(FUNC_CALL_VOID()){
                R
                W("<语句>");
                return true;
            }
            if(ASSIGNMENT()){
                R
                W("<语句>");
                return true;
            }
            if(READ_SENTENCE()){
                R
                W("<语句>");
                return true;
            }
            if(WRITE_SENTENCE()){
                R
                W("<语句>");
                return true;
            }
            if(RETURN_SENTENCE()){
                R
                W("<语句>");
                return true;
            }
            if(N=="SEMICN"){
                R
                W("<语句>");
                return true;
            }
            return false;
        }

        static bool ASSIGNMENT(){
            if(N=="IDENFR"){
                R
                if(N=="ASSIGN"){
                    R
                    EXPRESSION();
                } else{
                    R
                    EXPRESSION();
                    R
                    R
                    EXPRESSION();
                }
                W("<赋值语句>");
                return true;
            } else{
                return false;
            }
        }

        static bool CONDITION_SENTENCE(){
            if(N=="IFTK"){
                R
                R
                CONDITION();
                R
                SENTENCE();
                if(N=="ELSETK"){
                    R
                    SENTENCE();
                }
                W("<条件语句>");
                return true;
            } else{
                return false;
            }
        }

        static bool CONDITION(){
            if(EXPRESSION()){
                if(CMP_OPR()){
                    EXPRESSION();
                }
                W("<条件>");
                return true;
            } else{
                return false;
            }
        }

        static bool LOOP_SENTENCE(){
            if(N=="WHILETK"){
                R
                R
                CONDITION();
                R
                SENTENCE();
                W("<循环语句>");
                return true;
            }
            if(N=="DOTK"){
                R
                SENTENCE();
                R
                R
                CONDITION();
                R
                W("<循环语句>");
                return true;
            }
            if(N=="FORTK"){
                R
                R
                R
                R
                EXPRESSION();
                R
                CONDITION();
                R
                R
                R
                R
                R
                STEP();
                R // )
                SENTENCE();
                W("<循环语句>");
                return true;
            }
            return false;
        }

        static bool STEP(){
            if(UNSIGNED()){
                W("<步长>");
                return true;
            } else{
                return false;
            }
        }

        static bool FUNC_CALL_VAL(){
            if(words[1].first!="LPARENT")return false;
            if(N=="IDENFR"&&func_info[words[0].second]){
                R
                R
                VAL_ARGUMENTS();
                R
                W("<有返回值函数调用语句>");
                return true;
            } else{
                return false;
            }
        }

        static bool FUNC_CALL_VOID(){
            if(words[1].first!="LPARENT")return false;
            if(N=="IDENFR"&&(!func_info[words[0].second])){
                R
                R
                VAL_ARGUMENTS();
                R
                W("<无返回值函数调用语句>");
                return true;
            } else{
                return false;
            }
        }

        static bool VAL_ARGUMENTS(){
            if(EXPRESSION()){
                while(N=="COMMA"){
                    R
                    EXPRESSION();
                }
            }
            W("<值参数表>");
            return true;
        }

        static bool SENTENCE_LIST(){
            while(SENTENCE());
            W("<语句列>");
            return true;
        }

        static bool READ_SENTENCE(){
            if(N=="SCANFTK"){
                R
                R
                R
                while(N=="COMMA"){
                    R
                    R
                }
                R
                W("<读语句>");
                return true;
            } else{
                return false;
            }
        }

        static bool WRITE_SENTENCE(){
            if(N=="PRINTFTK"){
                R
                R
                if(STRING()){
                    if(N=="COMMA"){
                        R
                        EXPRESSION();
                        R
                    } else{
                        R
                    }
                } else{
                    EXPRESSION();
                    R
                }
                W("<写语句>");
                return true;
            } else{
                return false;
            }
        }

        static bool RETURN_SENTENCE(){
            if(N=="RETURNTK"){
                R
                if(N=="LPARENT"){
                    R
                    EXPRESSION();
                    R
                }
                W("<返回语句>");
                return true;
            } else{
                return false;
            }
        }

    };

    void parse(){
        vector<pair<string, string> > lex=Lexical::parse();
        for(int i=0;i<lex.size();i++){
            pair<string, string>& item=lex[i];
            words.push_back(item);
            // cout<<i.first<<" "<<i.second<<endl;
        }
        //cout<<"--------------------------"<<endl;

        Analyze::PROG();
        //cout<<"ok"<<endl;
    }
}

int main(){
    freopen("output.txt","w",stdout);
    Grammar::parse();
}