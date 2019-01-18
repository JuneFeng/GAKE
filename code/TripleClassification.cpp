//
//  TripleClassification.cpp
//  NetworkEmbedding
//
//  Created by Feng Jun on 15/11/19.
//  Copyright (c) 2015年 Feng Jun. All rights reserved.
//

#include "TripleClassification.h"

namespace TripleClassification {
    
    vector<Node> valid[30000];
    vector<double> threadsholdR;
    
    
    bool SortFunction(const Node &a, const Node &b)
    {
        return a.score > b.score;
    }
    
    //best:1, not:0
    double CountScore(int bestOrNot, int h, int r, int t, int label)
    {
        double score = 0;
        
        score += CountTmpScore(bestOrNot, 2, t, h, r, label);
        score += CountTmpScore(bestOrNot, 2, h, t, r + relationNum / 2, label);
        
        return score;
    }
    
    double Test(int bestOrNot, string outFileName, string fileName, string filePath)
    {
        string validFile = filePath + fileName + "/valid_c.txt";
        string testFile = filePath + fileName + "/test_c.txt";
        
        string newValid = outFileName + "_valid";
        string newTest = outFileName + "_test";
        FILE *fin = fopen(validFile.c_str(), "r");
        FILE *fvalid;
        if (bestOrNot == 1)
            fvalid = fopen(newValid.c_str(), "w");
        int a, b, c, d, m;
        for (int i = 0; i < relationNum; i ++)
            valid[i].clear();
        while (fscanf(fin, "%d%d%d%d", &a, &b, &c, &d) !=  EOF)
        {
            int h = a + relationNum;
            int r = b;
            int t = c + relationNum;
            double score = CountScore(bestOrNot, h, r, t, d);
//            double score1 = CountScore(1 - bestOrNot, h, r, t);
//            double totDiff = 0;
//            for (int i = 0; i < dim; i ++)
//            {
//                if (best_vector[h * dim + i] != word_vector[h * dim + i])
//                    totDiff ++;
//                if (best_vector[r * dim + i] != word_vector[r * dim + i])
//                    totDiff ++;
//                if (best_vector[t * dim + i] != word_vector[t * dim + i])
//                    totDiff ++;
//            }
//            printf("%d %lf %lf %lf\n", bestOrNot, score, score1, totDiff);
            if (bestOrNot == 1)
                fprintf(fvalid, "%d %d %d %d %lf\n", a, b, c, d, score);
            //printf("%lf\n", score);
            valid[b].push_back(Node(score, d));
        }
        fclose(fin);
        if (bestOrNot == 1)
            fclose(fvalid);
        for (int i = 0; i < relationNum; i ++)
        {
            if (valid[i].size() == 0)
            {
                threadsholdR.push_back(0);
                continue;
            }
            sort(valid[i].begin(), valid[i].end(), SortFunction);
            
            double bestAccuracy = 0;
            double TP = 0, FP = 0, TN = 0, FN = 0;
            for (int j = 0; j < valid[i].size(); j ++)
            {
                //printf("%lf %d\n", valid[i][j].score, valid[i][j].label);
                if (valid[i][j].label == 1)
                {
                    FN ++;
                }
                else {
                    TN ++;
                }
            }
            bestAccuracy = (TP + TN) / (TP + FP + TN + FN);
            threadsholdR.push_back(valid[i][0].score - 0.00001);
            for (int j = 0; j < valid[i].size(); j ++)
            {
                if (valid[i][j].label == 1)
                {
                    TP ++;
                    FN --;
                }
                else {
                    FP ++;
                    TN --;
                }
                
                if ((TP + TN) / (TP + FP + TN + FN) > bestAccuracy)
                {
                    bestAccuracy = (TP + TN) / (TP + FP + TN + FN);
                    threadsholdR[i] = valid[i][j].score;
                }
            }
        }
        
        fin = fopen(testFile.c_str(), "r");
        FILE *ftest;
        if (bestOrNot == 1)
            ftest = fopen(newTest.c_str(), "w");
        double correctNum = 0;
        double totNum = 0;
        int num1 = 0, num2 = 0;
        while (fscanf(fin, "%d%d%d%d", &a, &b, &c, &d) !=  EOF)
        {
            if (valid[b].size() == 0) continue;
            int h = a + relationNum;
            int r = b;
            int t = c + relationNum;
            
            double score = CountScore(bestOrNot, h, r, t, d);
            if (bestOrNot == 1)
                fprintf(ftest, "%d %d %d %d %lf\n", a, b, c, d, score);
            if (score >= threadsholdR[b] && d == 1)
            {
                correctNum ++;
                num1 ++;
            }
            if (score < threadsholdR[b] && d == -1)
            {
                correctNum ++;
                num2 ++;
            }
            totNum ++;
        }
        fclose(fin);
        if (bestOrNot == 1)
            fclose(ftest);
        printf("%d %d\n", num1, num2);
        printf("Accuracy = %lf\n", correctNum / totNum);
        printf("TotNum = %lf %lf\n", correctNum, totNum);
        FILE *fout = fopen(outFileName.c_str(), "w");
        if (fout != NULL)
        {
            fprintf(fout, "Accuracy = %lf\n", correctNum / totNum);
            fprintf(fout, "TotNum = %lf\n", totNum);
            
            fclose(fout);
        }
        return correctNum / totNum;
    }
}
