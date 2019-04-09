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
    file.close();
}

/*storing hashtags in a hashmap of block which they belong*/
void storeHashtags(string raw, block* area){
    regex x("#\\w* ");
    auto words_begin =
            sregex_iterator(raw.begin(), raw.end(), x);
    auto words_end = sregex_iterator();
    for (sregex_iterator i = words_begin; i != words_end; ++i) {
        smatch match = *i;
        string text=match.str();
        text.pop_back();
        bool hasKey=area->hashtag_count.count(text);
        if(hasKey){
            area->hashtag_count.at(text)++;
        }else{
            area->hashtag_count.insert({text,1});
        }
    }
}

/*comparing coordinates with each block's boundary,
 * If it's in block then store it and return grid number, otherwise return -1*/
int storeCoordinates(const double* co, block* grid){
    for(int i=0;i<16;i++){
        if(co[0]>grid[i].xmin && co[0]<=grid[i].xmax && co[1]>=grid[i].ymin && co[1]<grid[i].ymax){
            grid[i].total++;
            return i;
        }
    }
    return -1;
}

/*Printing the top 5 hashtags for a given block.
 * Hashmap in the block is transferred to a map which ranks the count value*/
void showTop5Hashtags(block b){
    map<int,string,greater<>> rank;
    for(auto & it : b.hashtag_count){
        rank.insert(make_pair(it.second,it.first));
    }
    int count=0;
    int previous=0;
    for(auto & it : rank){
        if(previous!=it.first){
            count++;
            previous=it.first;
        }
        if(count<=5){
            cout<<"("<<it.second<<","<<it.first<<"),";
        }
    }
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
    getline(file,eachLine); //get rid of first line
    while (getline(file,eachLine) && eachLine.length()>10){
        eachLine.pop_back();
        if(eachLine.back()==','){
            eachLine.pop_back();
        }
        json tweet = json::parse(eachLine);
        //cout<<tweet.dump(4)<<endl;
        //cout<<tweet["doc"]["coordinates"]["coordinates"]<<endl;
        double co[2];
        co[0] = tweet["doc"]["coordinates"]["coordinates"][0];
        co[1] = tweet["doc"]["coordinates"]["coordinates"][1];
        inGridNum = storeCoordinates(co, grid);

        if (inGridNum != -1) {
            storeHashtags(tweet["doc"]["text"], &grid[inGridNum]);
        }
        i++;
    }

    //printout top 5 hashtags information of grid
    for(auto & j : grid){
        cout<<j.name<<":";
        showTop5Hashtags(j);
        cout<<endl;
    }

    //printout tweet count for each block
    for(int i=0;i<16;i++){
        cout<<"Block "<<i<<" tweet count:"<<grid[i].total<<endl;
    }

    printf("Time taken: %.6fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    file.close();
    return 0;
}