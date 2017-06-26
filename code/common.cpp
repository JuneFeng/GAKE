//
//  common.cpp
//  NetworkEmbedding
//
//  Created by Feng Jun on 15/10/27.
//  Copyright (c) 2015年 Feng Jun. All rights reserved.
//

#include "common.h"

double headTimes = 1000000000;
double relationTimes = 100000;

/*
string dataSourcePath = "../data/";
string outProcessFile = "result/";
*/
string dataSourcePath = "/home/fengjun/NetworkEmbedding/Data/";
string outProcessFile = "/home/fengjun/NetworkEmbedding/githubCode/Result/";

string dataSource = "fb15k";
string experiment = "TripleClassification";

vector<PathInfo> paths;
vector<PathInfo> trainPaths;

vector<Triplet> trainData;
vector<Triplet> validData;
vector<Triplet> testData;

vector<Triplet> headData[size];
vector<Triplet> relationData[size];
vector<Triplet> tailData[size];

vector<NodeInfo> nodes;

//bool flag[size][size];

set<double> trueTriplet;
set<double> trainTriplet;

int entityNum;
int relationNum;
int nodeNum;

vector<double> word_vector;
vector<double> best_vector;
vector<double> eg_word_vector;
vector<double> ex_word_vector;
vector<double> theta_u_neg;
vector<double> theta_u;
vector<double> best_theta_u;
vector<double> eg_theta_u;
vector<double> ex_theta_u;


//map<pair<int, int>, vector<vector<int>>> paths;
set<pair<int, int> > flagPair;

map<pair<int, int>, set<vector<int> > > relations;

vector<int> node2id;

vector<double> w;
vector<double> bestW;

vector<double> nw;
vector<double> bestNW;

int entityType[40000];
int entityTypeNum = 0;

double *expTable;

int loopNum = 200;
int dim = 128;
int window = 4;
int sampling_number = 5;
double alpha = 0.25;
double starting_alpha;
int negative = 0;

int threadSize;
int threadNum = 5;
int softmax = 0;
int attentionLabel = 1;
int pathContextNum = 100;
int edgeContextNum = 100;
double pathRate = 0;
double edgeRate = 0;
int edgeNum = 0;


int maxPathLen = 0;

//pathType == 0, add entity and relation into path
//pathTyep == 1, only add relation into the path
int pathType = 0;
double rho = 0.95;
double constant = 0.000001;

map<pair<int, int>, vector<int> > testPathPair;
map<pair<int, int>, vector<vector<int> > > testRelationPath;
