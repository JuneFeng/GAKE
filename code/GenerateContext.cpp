//
//  GenerateContext.cpp
//  NetworkEmbedding
//
//  Created by Feng Jun on 16/1/25.
//  Copyright (c) 2016年 Feng Jun. All rights reserved.
//

#include "GenerateContext.h"

namespace GenerateContext {
    
    map<int, string> idToEntity;
    map<int, string> idToRelation;
    map<string, string> entityToLabel;
    
    struct CmpByValue {
        bool operator()(const pair<vector<int>,int> & lhs, const pair<vector<int>,int> & rhs)
        {return lhs.second > rhs.second;}
    };
    
    void GetTripleContext()
    {
        paths.clear();
        
        for (int i = 0; i < trainData.size(); i ++)
        {
            vector<int> p;
            p.clear();
            p.push_back(trainData[i].head + relationNum), p.push_back(trainData[i].relation);
            paths.push_back(PathInfo(trainData[i].tail + relationNum, p, 0, 1));
        }
        maxPathLen = 2;
        
    }
    
    void GetNeighborContext(string neighborString)
    {
        vector<set<int> > entityRelation;
        vector<vector<int> > relations;
        
        for (int i = 0; i < entityNum; i ++)
        {
            set<int> tmp;
            entityRelation.push_back(tmp);
            vector<int> tt;
            relations.push_back(tt);
        }
        for (int i = 0; i < trainData.size(); i ++)
        {
            entityRelation[trainData[i].tail].insert(trainData[i].relation);
        }
        
        for (int i = 0; i < entityNum; i ++)
        {
            for(set<int>::iterator it = entityRelation[i].begin(); it != entityRelation[i].end(); it ++)
                relations[i].push_back(*it);
        }
        
        for (int ii = 0; ii < neighborPairNum; ii ++)
        {
            int i = rand() % entityNum;
            vector<int> p;
            if (relations[i].size() < neighborNum)
            {
                continue;
//                for (int j = 0; j < relations[i].size(); j ++)
//                    p.push_back(relations[i][j]);
            }
            else {
                for (int j = 0; j < neighborNum; j ++)
                {
                    int id = rand() % relations[i].size();
                    p.push_back(relations[i][id]);
                }
            }
            paths.push_back(PathInfo(i + relationNum, p, 2, neighborContext));
            if (p.size() > maxPathLen)
                maxPathLen = p.size();
        }
 }
    
    void GetRandomPathContext(string pathString)
    {
        for (int i = 0; i < testData.size(); i ++)
        {
            vector<int> tmp;
            tmp.clear();
            testPathPair[make_pair(testData[i].head + relationNum, testData[i].tail + relationNum)] = tmp;
        }
        for (int id = 0; id < pathPairNum; id ++)
        {
            int l = rand() % (window - 1) + 2; // l from 2 to window
            int st = rand() % entityNum + relationNum;
            vector<int> p;
            
            
            p.push_back(st);
            //p.push_back(st + relationNum);
            for (int i = 0; i < l; i ++)
            {
                if (headData[st].size() == 0) break;
                int j = rand() % headData[st].size();
                p.push_back(headData[st][j].relation);
                p.push_back(headData[st][j].tail);
                st = headData[st][j].tail;
            }
            int len = p.size();
            if (len <=3 )
                continue;
            vector<int> pa;
            for (int j = 0; j < len - 1; j ++)
                pa.push_back(p[j]);
            paths.push_back(PathInfo(p[len - 1], pa, 1, pathContext));
            if (len - 1 > maxPathLen)
                maxPathLen = len - 1;
            
            if (testPathPair.find(make_pair(p[0], p[len - 1])) != testPathPair.end())
            {
                testPathPair[make_pair(p[0], p[len - 1])].push_back(paths.size() - 1);
            }
        }
    }
    
   
    void GetContext(string pathString, string neighborString)
    {
        GetTripleContext();
        srand((unsigned)time(NULL));
        
        if (pathContext > 0)
        {
            if (strcmp(method.c_str(), "random") == 0)
                GetRandomPathContext(pathString);
        }
        if (neighborContext > 0)
            GetNeighborContext(neighborString);
    }
}

