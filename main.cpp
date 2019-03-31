#include <iostream>
#include <fstream>
#include <regex>
#include <time.h>
using namespace std;

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
        cout<<m.str()<<endl;
        i++;
    }


    //printf("The number of tweets: %d\n",i);
    printf("Time taken: %.6fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    file.close();
    return 0;
}