#include <iostream>
#include <fstream>
#include <regex>
#include <time.h>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

/*Searching coordinates numbers in raw strings and store them in an array*/
void getCoordinates(string raw, double *co){
    regex x("[+-]?(\\d+(\\.\\d+)?|\\.\\d+)([eE][+-]?\\d+)?");
    auto words_begin =
            sregex_iterator(raw.begin(), raw.end(), x);
    auto words_end = sregex_iterator();
    int j=0;
    for (sregex_iterator i = words_begin; i != words_end; ++i,++j) {
        smatch match = *i;
        co[j]=stod(match.str());
    }
}

void getHashtags(){
    //TODO

}

int main() {
    clock_t tStart=clock();
    string twitterData="/home/zlp/data/tinyTwitter.json";
    //string twitterData="/home/zlp/data/twitterMelb.json";
    ifstream file(twitterData);
    string eachLine;
    int i=0;
    getline(file,eachLine);
    while (getline(file,eachLine) && i<5){
        eachLine.pop_back();
        eachLine.pop_back();
        cout<<eachLine<<endl;
        json tweet=json::parse(eachLine);
        cout<<tweet.dump()<<endl;

        i++;
    }


    //printf("The number of tweets: %d\n",i);
    printf("Time taken: %.6fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    file.close();
    return 0;
}