#include <iostream>
#include <fstream>
#include <time.h>
#include <regex>
#include <unordered_map>
#include "json.hpp"
#include <mpi.h>
#include <vector>
#define root 0
#define tag_work_range 1
#define tag_tweet_info 2

using namespace std;
using json = nlohmann::json;
string twitterFile="/home/zlp/CCC_a1/data/bigTwitter.json"; //for zlp spartan
//string twitterFile="/home/zlp/data/tinyTwitter.json"; //for zlp local
//string twitterFile="/Users/eliya/CLionProjects/JsonParser/tinyTwitter2.json"; //for wxq local

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
    ifstream file("/home/zlp/CCC_a1/data/melbGrid.json"); //for zlp spartan
    //ifstream file("/home/zlp/data/melbGrid.json"); //for zlp local
    // ifstream file("/Users/eliya/CLionProjects/JsonParser/melbGrid.json"); //for wxq local

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
        transform(text.begin(),text.end(),text.begin(),::tolower);
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
int storeCoordinates(double* co, block* grid){
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

/*counting tweet dataset in line*/
int countTweetLines(){
    ifstream file(twitterFile);
    string eachLine;
    int count=0;
    while(getline(file,eachLine)){
        count++;
    }
    file.close();
    return count;
}

/*reading file in given range(start,end) and storing information in grid*/
void processTweetData(block* grid,int start,int end){
    ifstream file(twitterFile);
    string eachLine;
    int inGridNum;
    int i=0;
    while (getline(file,eachLine) && i<end){
        if(i>=start && eachLine.length()>100) {
            eachLine.pop_back();
            if (eachLine.back() == ',') {
                eachLine.pop_back();
            }
            json tweet = json::parse(eachLine);

            if (!tweet["doc"]["coordinates"].is_null()) {
                double co[2];
                co[0] = tweet["doc"]["coordinates"]["coordinates"][0];
                co[1] = tweet["doc"]["coordinates"]["coordinates"][1];
                inGridNum = storeCoordinates(co, grid);

                if (inGridNum != -1 && !tweet["doc"]["text"].is_null()) {
                    storeHashtags(tweet["doc"]["text"], &grid[inGridNum]);
                }
            }
        }
        i++;
    }
    file.close();
}

/* combining two unordered map together, if map a has the same key in map b,
 * then add two value and store it in map a, otherwise insert new key and value*/
void addMap(unordered_map<string,int>* a,unordered_map<string,int> *b){
    for(auto& it:*b){
        auto got=a->find(it.first);
        if(got==a->end()){
            a->insert({it.first,it.second});
        }else{
            got->second=got->second+it.second;
        }
    }
}

void masterDoWork(block* grid,int nproc){
    int total=countTweetLines();
    int interval=total/nproc;
    int buffer[2];
    for(int i=1;i<nproc;i++){
        buffer[0]=i*interval;
        if(i!=nproc-1){
            buffer[1]=(i+1)*interval;
        }else{
            buffer[1]=total;
        }
        MPI_Send(buffer,2,MPI_INT,i,tag_work_range,MPI_COMM_WORLD);
    }
    processTweetData(grid,0,interval);

    //receiving result from each slave node in json format
    MPI_Status status;
    int length;
    for(int i=1;i<nproc;i++){
        MPI_Probe(MPI_ANY_SOURCE,tag_tweet_info,MPI_COMM_WORLD,&status);
        MPI_Get_count(&status, MPI_CHAR, &length);
        char* c_info=new char[length];
        MPI_Recv(c_info,length,MPI_CHAR,MPI_ANY_SOURCE,tag_tweet_info,MPI_COMM_WORLD,&status);
        cout<<"receive data form "<<status.MPI_SOURCE<<" size:"<<length<<endl;
        string info(c_info,length);

        json j_grid=json::parse(info);
        for(int j=0;j<16;j++){
            grid[j].total+=int(j_grid[j*2]);
            unordered_map<string,int> eachBlock=j_grid[j*2+1];
            addMap(&grid[j].hashtag_count,&eachBlock);
        }
    }

}

void slaveDoWork(block* grid,int rank){
    int buffer[2];
    MPI_Status status;
    MPI_Recv(buffer,2,MPI_INT,root,tag_work_range,MPI_COMM_WORLD,&status);
    cout<<"process "<<rank<<" receive "<<buffer[0]<<" "<<buffer[1]<<endl;
    processTweetData(grid,buffer[0],buffer[1]);

    //sending hashmap in json format
    json j_grid;
    for(int i=0;i<16;i++){
        json j_count(grid[i].total);
        j_grid.push_back(j_count);
        json j_block(grid[i].hashtag_count);
        j_grid.push_back(j_block);
    }
    string info=j_grid.dump();
    MPI_Send(info.c_str(),info.length(),MPI_CHAR,root,tag_tweet_info,MPI_COMM_WORLD);
}

int main(int argc, char **argv) {

    MPI_Init(&argc,&argv);
    int nproc;
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double stamp;
    if(rank==root){
        stamp=MPI_Wtime();
    }

    block grid[16];
    initialGrid(grid);

    if(rank==root){
        masterDoWork(grid,nproc);
    }else{
        slaveDoWork(grid,rank);
    }

    if(rank==root){
        //printout top 5 hashtags information of grid
        for(auto & j : grid){
            cout<<j.name<<":";
            showTop5Hashtags(j);
            cout<<endl;
        }

        //printout tweet count for each block
        for(int i=0;i<16;i++){
            cout<<grid[i].name<<": "<<grid[i].total<<" posts,"<<endl;
        }
        printf("time_cost:%.16g\n",MPI_Wtime()-stamp);
    }

    MPI_Finalize();
    return 0;
}