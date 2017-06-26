//
//  main.cpp
//  NetworkEmbedding
//
//  Created by Feng Jun on 15/10/21.
//  Copyright (c) 2015年 Feng Jun. All rights reserved.
//

// The attention is add to both entities and relations, the path subjects contains both entities and relations
#include "common.h"
#include "CBOWPath.h"
#include "TripleClassification.h"
#include "GenerateContext.h"


int GetIntNum(string s)
{
    int tmp = 0;
    for (int i  = 0; i < s.size(); i ++)
        tmp = tmp * 10 + s[i] - '0';
    return tmp;
}

double GetDoubleNum(string s)
{
    double tmp = 0;
    int startIndex = -1;
    for (int i = 0; i < s.size(); i ++)
    {
        if (s[i] == '.')
        {
            startIndex = i + 1;
            break;
        }
        tmp = tmp * 10 + s[i] - '0';
    }
    double base = 0.1;
    for (int i = startIndex; i < s.size(); i ++)
    {
        tmp += (s[i] - '0') * base;
        base /= 10;
    }
    
    return tmp;
}




void ReadKnowledgeFile(string fileName, string filePath)
{
    string trainFile = filePath + fileName + "/train.txt";
    string validFile = filePath + fileName + "/valid.txt";
    string testFile = filePath + fileName + "/test.txt";
    
    entityNum = 0;
    relationNum = 0;
    
    trueTriplet.clear();
    
    FILE *fin = fopen(trainFile.c_str(), "r");
    if (fin == NULL)
    {
        printf("Fail to read file %s\n", trainFile.c_str());
        return;
    }
    
    int num = 0;
    trainData.clear();
    int a, b, c;
    while (fscanf(fin, "%d%d%d", &a, &b, &c) != EOF)
    {
        trainData.push_back(Triplet(a, b, c));
        if (a > entityNum)
            entityNum = a;
        if (c > entityNum)
            entityNum = c;
        if (b > relationNum)
            relationNum = b;
        trueTriplet.insert(a * headTimes + b * relationTimes + c);
    }
    fclose(fin);
    
    fin = fopen(validFile.c_str(), "r");
    if (fin == NULL)
    {
        printf("Fail to read file %s\n", validFile.c_str());
        return;
    }
    validData.clear();
    while (fscanf(fin, "%d%d%d", &a, &b, &c) != EOF)
    {
        validData.push_back(Triplet(a, b, c));
        if (a > entityNum)
            entityNum = a;
        if (c > entityNum)
            entityNum = c;
        if (b > relationNum)
            relationNum = b;
        
        trueTriplet.insert(a * headTimes + b * relationTimes + c);
    }
    fclose(fin);
    
    fin = fopen(testFile.c_str(), "r");
    if (fin == NULL)
    {
        printf("Fail to read file %s\n", testFile.c_str());
        return;
    }
    testData.clear();
    
    num = 0;
    while (fscanf(fin, "%d%d%d", &a, &b, &c) != EOF)
    {
        num ++;
      if (a > entityNum)
            entityNum = a;
        if (c > entityNum)
            entityNum = c;
        if (b > relationNum)
            relationNum = b;
        
//        if (num > 100) continue;
        testData.push_back(Triplet(a, b, c));
        
        trueTriplet.insert(a * headTimes + b * relationTimes + c);
    }
    fclose(fin);
    entityNum ++;
    relationNum ++;
    
    relationNum *= 2;

    nodeNum = entityNum + relationNum;
    for (int i = 0; i < nodeNum; i ++)
    {
        headData[i].clear();
        tailData[i].clear();
        relationData[i].clear();
    }
    int nn = 0;
    for (int i = 0; i < trainData.size(); i ++)
    {
        headData[trainData[i].head + relationNum].push_back(Triplet(-1, trainData[i].relation, trainData[i].tail + relationNum));
        relationData[trainData[i].relation].push_back(Triplet(trainData[i].head + relationNum, -1, trainData[i].tail + relationNum));
        tailData[trainData[i].tail + relationNum].push_back(Triplet(trainData[i].head + relationNum, trainData[i].relation, -1));
        tailData[trainData[i].head + relationNum].push_back(Triplet(trainData[i].tail + relationNum, trainData[i].relation + relationNum / 2, -1));
        relationData[trainData[i].relation].push_back(Triplet(trainData[i].tail + relationNum, -1, trainData[i].head + relationNum));
        headData[trainData[i].tail + relationNum].push_back(Triplet(-1, trainData[i].relation + relationNum / 2, trainData[i].head + relationNum));
    }
    
   
    int trainSize = trainData.size();
    for (int i = 0; i < trainSize; i ++)
    {
        trainData.push_back(Triplet(trainData[i].tail, trainData[i].relation + relationNum / 2, trainData[i].head));
    }
//    sort(nodes.begin(), nodes.end(), SortFunction);
    printf("%d %d %d\n", entityNum, relationNum, trainData.size());
   
}

int main(int argc, const char * argv[]) {

    dim = GetIntNum(argv[1]);
    window = GetIntNum(argv[2]);
    alpha = GetDoubleNum(argv[3]);
    loopNum = GetIntNum(argv[4]);
    //0:no attention 1:attention
    attentionLabel = GetIntNum(argv[5]);
    pathContextNum = GetIntNum(argv[6]);
    edgeContextNum = GetIntNum(argv[7]);
    edgeNum = GetIntNum(argv[8]);
    pathRate = GetDoubleNum(argv[9]);
    edgeRate = GetDoubleNum(argv[10]);
    
    starting_alpha = alpha;
    
    ReadKnowledgeFile(dataSource, dataSourcePath);
  
    string outFile = outProcessFile + "out";
    string embeddingResult = outProcessFile + "embedding";
    string linkResult = outProcessFile + "result_LinkPrediction";
    string relationResult = outProcessFile + "result_RelationPrediction";
    string tripleResult = outProcessFile + "result_TripleClassification";
    string pathString = outProcessFile + "path";
    string neighborString = outProcessFile + "neighbor";
    for (int i = 1; i < 11; i ++)
    {
        outFile = outFile + "_" + argv[i];
        embeddingResult = embeddingResult + "_" + argv[i];
        linkResult = linkResult + "_" + argv[i];
        relationResult = relationResult + "_" + argv[i];
        tripleResult = tripleResult + "_" + argv[i];
        pathString = pathString + "_" + argv[i];
        neighborString = neighborString + "_" + argv[i];
    }
    ("%s\n", outFile.c_str());
    string pathFile = outProcessFile + "path.txt";

    clock_t start, end;
    start = clock();
    printf("start\n");
    GenerateContext::GetContext(pathString, neighborString);
    printf("max path length = %d\n", maxPathLen);
    
    for (int i = 0; i < nodeNum; i ++)
        nodes.push_back(NodeInfo(0, NULL, 0, (int *)calloc(MAX_CODE_LENGTH, sizeof(int)), (char *)calloc(MAX_CODE_LENGTH, sizeof(char))));
    for (int i = 0; i < paths.size(); i ++)
    {
        nodes[paths[i].centerNode].count ++;
    }
    for (int i = 0; i < nodeNum; i ++)
        if (nodes[i].count == 0)
            nodes[i].count = 1;
    sort(nodes.begin(), nodes.end(), SortFunction);
    
    
    trainPaths.clear();
    for (int i = 0; i < paths.size(); i ++)
        trainPaths.push_back(paths[i]);
    CBOWPath::TrainCBOW(embeddingResult, outFile, 5);
    
    end=clock();
    cout<<"Run time: "<<(double)(end - start) / CLOCKS_PER_SEC<<"S"<<endl;
    printf("end\n");
    TripleClassification::Test(1, tripleResult, dataSource, dataSourcePath);
    
    return 0;
}
