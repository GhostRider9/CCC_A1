#include <iostream>
#include <fstream>
#include <regex>
#include <time.h>
using namespace std;

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

}

int main() {
    clock_t tStart=clock();
    string twitterData="/home/zlp/data/tinyTwitter.json";
    //string twitterData="/home/zlp/data/twitterMelb.json";
    ifstream file(twitterData);
    string eachLine;
    regex hashtags_regex("\\\"hashtags\\\" : \\[.*\\],");
    regex geo_regex("\\\"coordinates\\\" : \\[.{0,28}\\]");
    smatch m;
    int i=0;
    while (getline(file,eachLine) && i<10){
        regex_search(eachLine,m,hashtags_regex);
        cout<<m.str()<<endl;

        regex_search(eachLine,m,geo_regex);
        double co[2];
        getCoordinates(m.str(),co);
        printf("%.8f,%.8f\n",co[0],co[1]);
        i++;
    }


    //printf("The number of tweets: %d\n",i);
    printf("Time taken: %.6fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    file.close();
    return 0;
}