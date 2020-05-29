#include <iostream>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stack>

#define ERROR throw runtime_error("ERROR");

enum Type{
    E, E1, T, T1, F, w0, w1, I, LP, RP, EOS
};

bool isFinal(Type t){
    return t==I||t==LP||t==RP||t==w0||t==w1||t==EOS;
}

namespace Lexical{

    using namespace std;

    bool isSymbol(char c){
        return c=='+'||c=='-'||c=='*'||c=='/'||c=='('||c==')';
    }

    string char2str(char c){
        static string s;
        s=c;
        return s;
    }

    vector<pair<Type, string>> parse(){
        cout<<"Enter arithmetic expression:"<<endl;
        string input;
        getline(cin, input);
        stringstream cache;
        vector<pair<Type, string>> res;
        for(char c:input){
            if(isSymbol(c)){
                if(!cache.str().empty()){
                    res.push_back(make_pair(I, cache.str()));
                    cache.str("");
                }
                if(c=='+'||c=='-'){
                    res.push_back(make_pair(w0, char2str(c)));
                } else if(c=='*'||c=='/'){
                    res.push_back(make_pair(w1, char2str(c)));
                }
            } else{
                if(!isdigit(c)&&c!='.'){
                    ERROR
                }
                cache<<c;
            }
        }
        if(!cache.str().empty()){
            res.push_back(make_pair(I, cache.str()));
        }
        res.push_back(make_pair(EOS, ""));
        return res;
    }

}

namespace Grammar{

    using namespace std;

    class Analyzer{
    public:
        Analyzer(){
            words=Lexical::parse();
        }

        void read(Type t){
            if(words.front().first==t){
                words.erase(words.begin());
            } else{
                ERROR
            }
        }

        vector<pair<Type, string>> words;

        virtual void parse() = 0;
    };

    class RecursiveDescentSubroutine : public Analyzer{
    public:

        Type E(){
            T();
            E1();
        }

        void E1(){
            if(words.front().first==w0){
                read(w0);
                T();
                E1();
            } else{
                return;
            }
        }

        void T(){
            F();
            T1();
        }

        void T1(){
            if(words.front().first==w1){
                read(w1);
                F();
                T1();
            } else{
                return;
            }
        }

        void F(){
            if(words.front().first==I){
                read(I);
            } else if(words.front().first==LP){
                E();
                read(RP);
            } else{
                ERROR
            }
        }

        void parse(){
            E();
            read(EOS);
        }
    };

    class LL1 : public Analyzer{
    public:

        map<Type, map<Type, vector<Type>>> table;
        LL1(){
            table[E][I]={E1,T};
            table[E][LP]={E1,T};
            table[E1][w0]={E1,T,w0};
            table[E1][RP]={};
            table[E1][EOS]={};
            table[T][I]={T1,F};
            table[T][LP]={T1,F};
            table[T1][w1]={T1,F,w1};
            table[T1][w0]={};
            table[T1][RP]={};
            table[T1][EOS]={};
            table[F][I]={I};
            table[F][LP]={RP,E,LP};
        }

        void parse(){
            stack<Type> stack;
            stack.push(EOS);
            stack.push(E);
            while (!stack.empty()){
                if(isFinal(stack.top())){
                    if(stack.top()==words.front().first){
                        words.erase(words.begin());
                        stack.pop();
                    }else{
                        ERROR
                    }
                }else{
                    if(table.find(stack.top())==table.end()||table[stack.top()].find(words.front().first)==table[stack.top()].end()){
                        ERROR
                    }
                    Type temp=stack.top();
                    stack.pop();
                    for(auto&i:table[temp][words.front().first]){
                        stack.push(i);
                    }
                }
            }
        }
    };

}
using namespace std;

int main(){
    try{
        Grammar::LL1 parser;
        parser.parse();
    } catch(runtime_error&re){
        cout<<"ERROR"<<endl;
        return 0;
    }
    cout<<"OK"<<endl;
}