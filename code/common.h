//
//  common.h
//  NetworkEmbedding
//
//  Created by Feng Jun on 15/10/27.
//  Copyright (c) 2015年 Feng Jun. All rights reserved.
//

#ifndef __NetworkEmbedding__common__
#define __NetworkEmbedding__common__

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <map>
#include <set>


#define EXP_TABLE_SIZE 100
#define MAX_EXP 10
#define MAX_CODE_LENGTH 50

using namespace std;

const int size = 80000;
const int unigram_table_size = 1e8;

struct Node
{
    double score;
    int label;
    
    Node(double _s, int _l): score(_s), label(_l) {}
};

struct Triplet
{
    int head;
    int relation;
    int tail;
    Triplet(int _h, int _r, int _t): head(_h), relation(_r), tail(_t) {}
};

struct NodeInfo {
    int count;							// the frequency of this node in the file
    char *word;						// the node id
    
    int codelen;							// the length of the path, indicates the length of the following two arrays too.
    int *point;							// the parent points of this word on the Huffman tree, from root to leaf
    char *code;							// the code (0: min1, 1: min2) on the paths on the Huffman tree, from root to leaf
    
    NodeInfo(int _cn, char *_w, int _cl, int *_p, char *_c): count(_cn), word(_w), codelen(_cl), point(_p), code(_c) {}
};

struct PathInfo {
    int centerNode;
    vector<int> path;
    int type;
    double confidence;
    PathInfo(int _c, vector<int> _p, int _t, double _co): centerNode(_c), path(_p), type(_t) , confidence(_co){}
};

extern vector<PathInfo> paths;
extern vector<PathInfo> trainPaths;

extern map<pair<int, int>, set<vector<int> > > relations;

extern string dataSourcePath;
extern string outProcessFile;
extern string dataSource;
extern string experiment;

extern double headTimes;
extern double relationTimes;


extern vector<Triplet> trainData;
extern vector<Triplet> validData;
extern vector<Triplet> testData;
extern vector<Triplet> headData[size];
extern vector<Triplet> relationData[size];
extern vector<Triplet> tailData[size];

//extern map<pair<int, int>, vector<vector<int>>> paths;
extern set<pair<int, int> > flagPair;

extern vector<double> w;
extern vector<double> bestW;

extern vector<double> nw;
extern vector<double> bestNW;


extern vector<NodeInfo> nodes;

extern int entityNum;
extern int relationNum;
extern int nodeNum;
extern set<double> trueTriplet;
extern set<double> trainTriplet;

extern vector<double> word_vector;
extern vector<double> best_vector;
extern vector<double> eg_word_vector;
extern vector<double> ex_word_vector;
extern vector<double> theta_u_neg;
extern vector<double> theta_u;
extern vector<double> best_theta_u;
extern vector<double> eg_theta_u;
extern vector<double> ex_theta_u;

extern vector<int> node2id;

extern double *expTable;

extern int loopNum;
extern int dim;
extern int window;
extern int sampling_number;
extern double alpha;
extern double starting_alpha;
extern int negative;
extern string method;
extern string testMethod;
extern int threadSize;
extern int threadNum;
extern int softmax;
extern int attentionLabel;
extern double rho;
extern double constant;
extern int pathPairNum;
extern int neighborPairNum;
extern double pathContext;
extern double neighborContext;
extern int neighborNum;

extern int maxPathLen;
extern int entityType[40000];
extern int entityTypeNum;


extern map<pair<int, int>, vector<int> > testPathPair;
extern map<pair<int, int>, vector<vector<int> > > testRelationPath;
#endif /* defined(__NetworkEmbedding__common__) */
