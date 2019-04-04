#include <iostream>
#include <fstream>
#include <time.h>
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

/*Searching coordinates numbers in raw strings and store them in an array*/
void getCoordinates(){
    //TODO
}

void getHashtags(){
    //TODO

}

int main() {
    clock_t tStart=clock();
    string twitterData="/home/zlp/data/tinyTwitter.json";
    //string twitterData="/home/zlp/data/twitterMelb.json";
    ifstream file(twitterData);

    block grid[16];
    initialGrid(grid);

    string eachLine;
    int i=0;
    getline(file,eachLine);
    while (getline(file,eachLine) && i<5){
        eachLine.pop_back();
        eachLine.pop_back();
        json tweet=json::parse(eachLine);
        //cout<<tweet.dump(4)<<endl;
        //cout<<tweet["doc"]["coordinates"]["coordinates"]<<endl;
        double co[2];
        co[0]=tweet["doc"]["coordinates"]["coordinates"][0];
        co[1]=tweet["doc"]["coordinates"]["coordinates"][1];

        cout<<tweet["doc"]["entities"]["hashtags"][0]["text"]<<endl;
        i++;
    }


    //printf("The number of tweets: %d\n",i);
    printf("Time taken: %.6fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    file.close();
    return 0;
}