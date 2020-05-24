#include <iostream>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>

#define PARSE_WORD parseWord(cache,ofs);
#define RESET_AUTOMACHINE cache="";status=0;autoMachine.reset();

using namespace std;

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

void parseWord(const string &s,ofstream &ofs){
    if(s.empty())return;
    if(isdigit(s[0])){
        // const token
        ofs<<"INTCON "<<s<<endl;
    }else if(builtin_symbols.find(s)!=builtin_symbols.end()){
        // bound symbols
        ofs<<builtin_symbols[s]<<" "<<s<<endl;
    }else if(builtin_tokens.find(s)!=builtin_tokens.end()){
        // keywords
        ofs<<builtin_tokens[s]<<" "<<s<<endl;
    }else{
        // identifier
        ofs<<"IDENFR "<<s<<endl;
    }
}

int main(){
    // construct auto-machines
    for(int i=0;i<builtin_tokens_count;i++){
        builtin_tokens[builtin_tokens_array[i][0]]=builtin_tokens_array[i][1];
    }
    for(int i=0;i<builtin_symbols_count;i++){
        builtin_symbols[builtin_symbols_array[i][0]]=builtin_symbols_array[i][1];
        autoMachine.insert(builtin_symbols_array[i][0]);
    }

    // read file
    ifstream ifs("testfile.txt");
    ofstream ofs("output.txt");
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
            ofs<<(c=='\'' ? "CHARCON" : "STRCON")<<" ";
            while((c=ifs.get())!=left){
                ofs<<c;
            }
            ofs<<endl;
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
    ofs.close();
    return 0;
}

