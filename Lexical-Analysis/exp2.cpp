#include <iostream>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>

#define ERROR throw runtime_error("ERROR");

enum Type{
    w0, w1, I, LP, RP
};

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
                } else if(c=='+'||c=='/'){
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

        vector<pair<Type, string>> words;

        virtual void parse() = 0;
    };

    class RecursiveDescentSubroutine : public Analyzer{
    public:

        void read(Type t){
            if(words.front().first==t){
                words.erase(words.begin());
            } else{
                ERROR
            }
        }

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
        }
    };

}
using namespace std;

int main(){
    try{
        Grammar::RecursiveDescentSubroutine parser;
        parser.parse();
    } catch(runtime_error&re){
        cout<<"ERROR"<<endl;
        return 0;
    }
    cout<<"OK"<<endl;
}