#include <iostream>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>

using namespace std;

enum Type{
    Label,Char,String,Digit,Keyword,Bound
};

class Record{
public:
    Type type;
    int id;
};

class TrieNode{
public:
    int next[128];
    Record rec;

    TrieNode(){
        memset(next,0,sizeof(next));
    }
};

class Trie{
public:
    static map<char,int> counts;
    vector<TrieNode> nodes;
    int cnt=0;

    Trie(){
        nodes.emplace_back();
    }

    int getToken(Type type){
        if(nodes[cnt].rec.id!=0){
            return nodes[cnt].rec.id;
        }else{
            nodes[cnt].rec.type=type;
            return nodes[cnt].rec.id=++counts[type];
        }
    }

    void reset(){
        cnt=0;
    }

    void insert(const string &s,Type type){
        int p=0;
        for(char c : s){
            if(nodes[p].next[c]!=0){
                p=nodes[p].next[c];
            }else{
                nodes.emplace_back();
                nodes[p].next[c]=nodes.size()-1;
                p=nodes[p].next[c];
            }
        }
        getToken(type);
    }

    bool isEmptyChar(char c){
        return c==' ' || c=='\r' || c=='\n' || c=='\t';
    }

    Record* next(char c){
        if(isEmptyChar(c)){
            cnt=0;
            return nullptr;
        }
        if(nodes[cnt].next[c]!=0){
            cnt=nodes[cnt].next[c];
            return &nodes[cnt].rec;
        }else{
            cnt=0;
            return nullptr;
        }
    }
};

Trie autoMachine;

const vector<string> boundsArray={
        "<=","==","=",">","<",
        "+","-","·","/","{",
        "}",",",";","(",")"
};
const vector<string> keywordsArray={
        "int","main","void","if","else","char"
};


int main(){
    // construct auto-machines
    for(const string &s : boundsArray){
        autoMachine.insert(s,Keyword);
    }
    for(const string &s : keywordsArray){
        autoMachine.insert(s,Bound);
    }
    // read file
    ifstream ifs("code.txt");
    string cache;
    bool inBound=false;
    char c;
    while((c=ifs.get())!=EOF){
        // specially check strings with "" or ''
        if(c=='\'' || c=='\"'){
            char left=c;
            cache+=c;
            while((c=ifs.get())!=left){
                cache+=c;
            }
            cache+=c;
            continue;
        }
        if(bound.next(c)){
            if(inBound) continue;
            // step into bound symbol -> pop previous word
            parseWord(cache);
            cache=""; // clear cache
            inBound=true;
        }else{
            if(inBound){
                // step out of bound symbol
                parseWord(cache);
                cache=""; // clear cache
                inBound=false;
            }
            cache.append(c);
        }
    }
    ifs.close();
}
