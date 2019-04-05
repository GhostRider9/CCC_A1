#include <iostream>
#include <fstream>
#include <time.h>
#include <regex>
#include <unordered_map>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

/*block is used to store statistical information*/
struct block{
    string name;
    double xmin,xmax,ymin,ymax;
    int total=0;
    unordered_map<string,int> hashtag_count;
};

/*initializing the grid from json file
 * grid consists of multiple blocks*/
void initialGrid(block* grid){
    ifstream file("/home/zlp/data/melbGrid.json");
    string eachLine;
    int i=0,j=0;
    while (getline(file,eachLine) && i<21) {
        if(i>4){
            if(i!=20){
                eachLine.pop_back();
            }
            json f=json::parse(eachLine);
            grid[j].name=f["properties"]["id"];
            grid[j].xmin=f["properties"]["xmin"];
            grid[j].xmax=f["properties"]["xmax"];
            grid[j].ymin=f["properties"]["ymin"];
            grid[j].ymax=f["properties"]["ymax"];
            j++;
        }
        i++;
    }
}

/*storing hashtags in a hashmap of block which they belong*/
void storeHashtags(string raw, block* area){
    regex x("#\\w* ");
    auto words_begin =
            sregex_iterator(raw.begin(), raw.end(), x);
    auto words_end = sregex_iterator();
    for (sregex_iterator i = words_begin; i != words_end; ++i) {
        smatch match = *i;
        bool hasKey=area->hashtag_count.count(match.str());
        if(hasKey){
            area->hashtag_count.at(match.str())++;
        }else{
            area->hashtag_count.insert({match.str(),1});
        }
    }

}

/*comparing coordinates with each block's boundary,
 * If it's in block then store it and return grid number, otherwise return -1*/
int storeCoordinates(double* co, block* grid){
    for(int i=0;i<16;i++){
        if(co[0]>grid[i].xmin && co[0]<=grid[i].xmax && co[1]>=grid[i].ymin && co[1]<grid[i].ymax){
            grid[i].total++;
            return i;
        }
    }
    return -1;
}

void showTop5Hashtags(block* grid){
    //TODO
}

int main() {
    clock_t tStart=clock();
    string twitterData="/home/zlp/data/tinyTwitter.json";
    //string twitterData="/home/zlp/data/twitterMelb.json";
    ifstream file(twitterData);

    block grid[16];
    initialGrid(grid);

    int inGridNum;
    string eachLine;
    int i=0;
    getline(file,eachLine);
    while (getline(file,eachLine) && i<500){
        eachLine.pop_back();
        eachLine.pop_back();
        json tweet=json::parse(eachLine);
        //cout<<tweet.dump(4)<<endl;
        //cout<<tweet["doc"]["coordinates"]["coordinates"]<<endl;
        double co[2];
        co[0]=tweet["doc"]["coordinates"]["coordinates"][0];
        co[1]=tweet["doc"]["coordinates"]["coordinates"][1];
        inGridNum=storeCoordinates(co,grid);

        if(inGridNum!=-1){
            storeHashtags(tweet["doc"]["text"],&grid[inGridNum]);
        }
        i++;
    }

    showTop5Hashtags(grid);

    unordered_map<string,int>::iterator it;
    for(it=grid[13].hashtag_count.begin();it!=grid[13].hashtag_count.end();it++){
        cout<<it->first<<it->second<<endl;
    }
    cout<<grid[13].name<<":"<<grid[13].total<<endl;

    printf("Time taken: %.6fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    file.close();
    return 0;
}