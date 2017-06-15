//
//  CBOWPath.cpp
//  NetworkEmbedding
//
//  Created by Feng Jun on 15/12/15.
//  Copyright (c) 2015年 Feng Jun. All rights reserved.
//

#include "CBOWPath.h"

namespace CBOWPath {
    
    vector<int> wId; //the node id in the window
    vector<double> x_w;//the parameter x_w, thus the output of the input layer
    vector<double> e;//the parameter e
    vector<double> attention;

    
    vector<int> shuffleIndex;
    
    int number[10][50][50];
    
    map<vector<int>,string> path2s;
    int trainedWordNum;
    int num = 0, numSmall = 0, numLarge = 0;
    int lp;
    
    void InitNet()
    {
        theta_u.clear();
        best_theta_u.clear();
        theta_u_neg.clear();
        unsigned long long next_random = 1;
        
        if (negative == 0)
        {
            for (int i = 0; i < nodeNum; i++)
            {
                for (int j = 0; j < dim; j++)
                {
                    theta_u.push_back(0);		//the jth value of word i's theta_u is stored here.
                    best_theta_u.push_back(0);
                }
            }
        }
        else {
            for (int i = 0; i < nodeNum; i++)
            {
                for (int j = 0; j < dim; j++)
                {
                    theta_u_neg.push_back(0);		//the jth value of word i's theta_u is stored here.
                }
            }
        }

        word_vector.clear();
        best_vector.clear();
        //init the word vector for each word using random values
        for (int i = 0; i < nodeNum; i++)
        {
            for (int j = 0; j < dim; j++)
            {
                next_random = next_random * (unsigned long long)25214903917 + 11;
                word_vector.push_back((((next_random & 0xFFFF) / (double)65536) - 0.5) / dim);		//a random value
                best_vector.push_back(0);
                //printf("%lf\n", word_vector[j]);
            }
        }
        
        
        w.clear();
        bestW.clear();
        if (attentionLabel == 1)
        {
            for (int i = 0; i < (relationNum + entityNum) * maxPathLen; i ++)
            {
                w.push_back(0);
                bestW.push_back(0);
            }
        }
    }

    void Init()
    {
        InitNet();
        InitExpTable();
        CreateBinaryTree();
    }
    
    double CountTmpLoss(int id)
    {
        int centerNode = trainPaths[id].centerNode;
        int type = trainPaths[id].type;
        double tmp = 0;
        for (int i = 0; i < dim; i ++)
        {
            x_w[i] = 0;
            e[i] = 0;
        }
        int cw = trainPaths[id].path.size();
        wId.clear();
        // in -> hidden
        for (int i = 0; i < trainPaths[id].path.size(); i ++)
        {
            wId.push_back(trainPaths[id].path[i]);
        }
        //cw indicates the number of words in the context of the current word
        
        
        //for a subwindow  [sentence_position - (window - b), sentence_position + (window - b)]
        //we obtain the input vector x_w by adding all the word vectors of words in this subwindow
        int index = cw;
        double totAttention = 0;
        attention.clear();
        
        if (trainPaths[id].type != 0)
        {
            for (int i = 0; i < wId.size(); i ++)
            {
                if (attentionLabel == 0)
                    attention.push_back(1);
                if (attentionLabel == 1)
                {
                    if (trainPaths[id].type == 2)
                        attention.push_back(exp(w[wId[i] * maxPathLen]));
                    else attention.push_back(exp(w[wId[i] * maxPathLen + cw - i - 1]));
                }
                //                printf("%lf %lf\n", w[type][index][i], attention[i]);
                totAttention += attention[i];
            }
            for (int i = 0; i < wId.size(); i ++)
                attention[i] /= totAttention;
        }
        
        for (int i = 0; i < wId.size(); i ++)
        {
            for (int c = 0; c < dim; c ++)
            {
                if (trainPaths[id].type == 0)
                {
                    x_w[c] += word_vector[c + wId[i] * dim];
                }
                else {
                    x_w[c] += attention[i] * word_vector[c + wId[i] * dim];
                }

            }
        }
        
        if (trainPaths[id].type == 0 || attentionLabel == 0)
        {
            for (int c = 0; c < dim; c ++)
                x_w[c] /= cw;
        }

        
        for (int j = 0; j < nodes[centerNode].codelen; j ++)
        {
            double f = 0;
            int l2 = nodes[centerNode].point[j] * dim;
            for (int c = 0; c < dim; c++)
            {
                f += x_w[c] * theta_u[c + l2];
            }
            
            //the exp Table only allows input f in (-MAX_EXP, MAX_EXP), otherwise the resultant value
            //index will be invalid.
            //as the result, we should skip the corner cases
            //see the defination of exp Table at the beginning for details
            if (f <= -MAX_EXP)
            {
                f = -MAX_EXP + 1;
                numSmall ++;
                //continue;
            }
            else if (f >= MAX_EXP)
            {
                f = MAX_EXP - 1;
                numLarge ++;
                //continue;
            }
            //when the input f is in (-MAX_EXP, MAX_EXP), the output f will be in [0, EXP_TABLE_SIZE], indicating the corresponding value of tao(f)
            f = expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))];
            tmp += (1 - nodes[centerNode].code[j]) * log(f) + nodes[centerNode].code[j] * log(1 - f);
            num ++;
        }

        //printf("%lf\n", tmp);
        return tmp * trainPaths[id].confidence;
    }
    
    double CountLoss()
    {
        num = 0;
        numSmall = 0;
        numLarge = 0;
        double tmp = 0;
        for (int i = 0; i < trainPaths.size(); i ++)
        {
            if (negative == 0)
            {
                tmp += CountTmpLoss(i);
            }
            
        }
        return tmp;
    }
    
    void UpdateEmbedding(int id)
    {
       
        unsigned long long next_random = (long long) trainPaths[id].centerNode;
        int centerNode = trainPaths[id].centerNode;
        int type = trainPaths[id].type;
        
        if (trainedWordNum % 1000000 == 0)
        {
            alpha = starting_alpha * (1 - 1.0 * trainedWordNum / (loopNum * trainPaths.size()));
            if (alpha < starting_alpha * 0.0001)
                alpha = starting_alpha * 0.0001;
        }
        for (int i = 0; i < dim; i ++)
        {
            x_w[i] = 0;
            e[i] = 0;
        }
        int cw = trainPaths[id].path.size();
        if (cw == 0) return;
        wId.clear();
        // in -> hidden
        for (int i = 0; i < trainPaths[id].path.size(); i ++)
        {
            wId.push_back(trainPaths[id].path[i]);
        }
        //
        int index = cw;
        double totAttention = 0;
        attention.clear();
        if (trainPaths[id].type != 0)
        {
            for (int i = 0; i < wId.size(); i ++)
            {
                if (attentionLabel == 0)
                    attention.push_back(1);
                if (attentionLabel == 1)
                {
                    if (trainPaths[id].type == 2)
                        attention.push_back(exp(w[wId[i] * maxPathLen]));
                    else attention.push_back(exp(w[wId[i] * maxPathLen + cw - i - 1]));
                }
                
                totAttention += attention[i];
            }
            for (int i = 0; i < wId.size(); i ++)
            {
                 attention[i] /= totAttention;
//                printf("%lf ", attention[i]);
            }
//            printf("\n");
        }
        
        for (int i = 0; i < wId.size(); i ++)
        {
            for (int c = 0; c < dim; c ++)
            {
                if (trainPaths[id].type == 0)
                {
                    x_w[c] += word_vector[c + wId[i] * dim];
                }
                else {
                    x_w[c] += attention[i] * word_vector[c + wId[i] * dim];
                }
            }
        }
        if (trainPaths[id].type == 0 || attentionLabel == 0)
        {
            for (int c = 0; c < dim; c ++)
                x_w[c] /= cw;
        }
        int target = 0;
        int label = 1;
        double f, g;
        
        if (negative == 0)
        {
            //the update function of the SGD:
            //
            // f = x_w \cdot theta^u_{j-1}
            // q = tao(f)
            // g = alpha * ( 1 - code(j) - q)
            // e = e + g * theta^u_{j-1}
            // theta^u_{j-1} = theta^u_{j-1} + g * x_w
            //
            for (int d = 0; d < nodes[centerNode].codelen; d++)
            {
                f = 0;
                
                //l2 is the offset of the dth node on the path from the root to word
                //using this offset, the following theta_u[c + l2] can represent
                //the cth value of this nodes' sigmod function parameter
                int l2 = nodes[centerNode].point[d] * dim;
                
                // Propagate hidden -> output
                
                // here f = x_w \cdot theta_u
                for (int c = 0; c < dim; c++)
                {
                    f += x_w[c] * theta_u[c + l2];
                }
                
                //the exp Table only allows input f in (-MAX_EXP, MAX_EXP), otherwise the resultant value
                //index will be invalid.
                //as the result, we should skip the corner cases
                //see the defination of exp Table at the beginning for details
                if (f <= -MAX_EXP)
                {
                    continue;
                }
                else if (f >= MAX_EXP)
                {
                    continue;
                }
                else
                {
                    //when the input f is in (-MAX_EXP, MAX_EXP), the output f will be in [0, EXP_TABLE_SIZE], indicating the corresponding value of tao(f)
                    f = expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))];
                }
                
                // "g" is the gradient multiplied by the learning rate
                g = (1 - nodes[centerNode].code[d] - f) * alpha * trainPaths[id].confidence;
                // Propagate errors output -> hidden
                for (int c = 0; c < dim; c++)
                {
                    e[c] += g * theta_u[c + l2];
                }
                
                // Learn weights hidden -> output
                //                double len = 0;
                for (int c = 0; c < dim; c++)
                {
                    if (centerNode < relationNum)
                    {
                        theta_u[c + l2] += 0.01 * g * x_w[c];
                    }
                    else theta_u[c + l2] += g * x_w[c];
                    //printf("%d %lf\n", c + l2, theta_u[c + l2]);
                    //                    len += theta_u[c + l2] * theta_u[c + l2];
                }
            }
        }
        
        
        // hidden -> in
        
        //for all the words in the context, we update their word vectors with the estimated error vector
        double tmp = 0;
        for (int i = 0; i < wId.size(); i ++)
        {
            double updateW = 0;
            //            double len = 0;
            for (int c = 0; c < dim; c++)			//update the word vectors of this word with the estimated error vector
            {
                updateW += word_vector[c + wId[i] * dim] * e[c];
                if (trainPaths[id].type == 0)
                {
                    if (wId[i] < relationNum)
                    {
                        word_vector[c + wId[i] * dim] += 0.01 * e[c];
                    }
                    else {
                        word_vector[c + wId[i] * dim] += e[c];
                    }
                }
                else {
                    if (wId[i] < relationNum)
                    {
                        word_vector[c + wId[i] * dim] += 0.01 * attention[i] * e[c];
                    }
                    else {
                        word_vector[c + wId[i] * dim] += attention[i] * e[c];
                    }
                }
                
                //                    if (wId[i] < relationNum)
                //                        word_vector[c + wId[i] * dim] += 0.01 * e[c];
                //                    else word_vector[c + wId[i] * dim] += e[c];
                //                len += word_vector[c + wId[i] * dim] * word_vector[c + wId[i] * dim];
            }
            //printf("%d %d %d %lf\n", type, index, i, updateW);
            if (trainPaths[id].type != 0 && attentionLabel != 0)
            {
                double tt = 0;
                tt = exp(w[wId[i]]) * (totAttention - exp(w[wId[i]])) / totAttention / totAttention;
                number[type][index][i] ++;
                if (attentionLabel == 1)
                {
//                    printf("%lf\n", updateW * tt);
                    if (trainPaths[id].type == 2)
                        w[wId[i] * maxPathLen] = 10 * updateW * tt;
                    else w[wId[i] * maxPathLen + cw - i - 1] = 10 * updateW * tt;
                }
                //            if (type == 0 && index == 0 && i == 0)
                //                printf("%lf %lf %lf\n", updateW, tt, w[type][index][i]);
            }
        }
    }
    
    void TrainCBOW(string fileName, string outFile, int modNum)
    {
        FILE *fout = fopen(outFile.c_str(), "w");
        FILE *fembedding = fopen(fileName.c_str(), "w");
        
        x_w.clear();
        e.clear();
        for (int i = 0; i < dim; i ++)
        {
            x_w.push_back(0);
            e.push_back(0);
        }
        alpha = starting_alpha;
        if (word_vector.size() == 0)
            Init();
        trainedWordNum = 0;
        shuffleIndex.clear();
        for (int i = 0; i < trainPaths.size(); i ++)
            shuffleIndex.push_back(i);
        double loss = CountLoss();
        //double loss = 0;
        double oldLoss = loss;
        double bestLoss = -1;
        printf("negative = %d\n", negative);
    
        
        threadSize = (int) trainPaths.size() / threadNum;
        for (int loop = 0; loop < loopNum; loop ++)
        {
            lp = loop;
            random_shuffle(shuffleIndex.begin(), shuffleIndex.end());
            printf("loop = %d %lf %lf %d %d %d\n", loop, alpha, loss, num, numSmall, numLarge);
            fprintf(fout, "loop = %d %lf %lf %d %d %d\n", loop, alpha, loss, num, numSmall, numLarge);
            fflush(fout);
//            pthread_t p[threadNum];
//            for(int i = 0; i < threadNum + 1; i ++){
//                //printf("%d\n", i);
//                pthread_create(&p[i], NULL, TrainEmbedding, (void *)i );
//            }
//            for  (int i = 0; i < threadNum + 1; i ++)
//                pthread_join(p[i], NULL);
            for (int i = 0; i < trainPaths.size(); i ++)
            {
                trainedWordNum ++;
                //0: h|r,t; 1:r|h,t; 2:t|h,r
                UpdateEmbedding(shuffleIndex[i]);
            }
            /*
             word_vector = update_vector;
             theta_u = update_theta_u;
             
             int num = 0;
             for (int j = 0; j < nodeNum; j ++)
             for (int k = 0; k < dim; k ++)
             if (word_vector[j * dim + k] != update_vector[j * dim + k] || theta_u[j * dim + k] != update_theta_u[j * dim + k])
             num ++;
             printf("%d\n", num);
             */
            oldLoss = loss;
            loss = CountLoss();
            if (bestLoss == -1 || loss > bestLoss)
            {
                for (int i = 0; i < nodeNum; i++)
                {
                    for (int j = 0; j < dim; j++)
                    {
                        best_vector[i * dim + j] = word_vector[i * dim + j];
                        best_theta_u[i * dim + j] = theta_u[i * dim + j];
                    }
                }
                bestW = w;
                bestLoss = loss;
            }
            
//            for (int i = 0; i < nodeNum; i ++)
//            {
//                printf("% lf", bestW[i]);
//            }
//            printf("\n");
            

            if (loop % modNum == 0)
            {
                double accuracy = TripleClassification::Test(0, "", dataSource, dataSourcePath);
//                for (int i = 0; i < nodeNum * maxPathLen; i ++)
//                {
//                    if (bestW[i] != 0)
//                        printf("%d %lf\t", i, bestW[i]);
//                }
//                printf("\n");

                //double best_accuracy = TripleClassification::Test(1, "", dataSource, dataSourcePath);
                printf("Triple Classification: accuracy = %lf\n", accuracy);
                fprintf(fout, "Triple Classification: accuracy = %lf\n", accuracy);
                fflush(fout);
            }
            if (fabs(oldLoss - loss) < 0.00001) break;
        }
        printf("loop = %d %lf %lf %d %d %d\n", loopNum, alpha, loss, num, numSmall, numLarge);
        fprintf(fout, "loop = %d %lf %lf %d %d %d\n", loopNum, alpha, loss, num, numSmall, numLarge);
        for (int i = 0; i < nodeNum; i ++)
        {
            for (int j = 0; j < dim; j ++)
                fprintf(fembedding, "%lf ", best_vector[i * dim + j]);
            fprintf(fembedding, "\n");
        }
        for (int i = 0; i < nodeNum; i ++)
        {
            for (int j = 0; j < dim; j ++)
                fprintf(fembedding, "%lf ", best_theta_u[i * dim + j]);
            fprintf(fembedding, "\n");
        }
        
        
        if (attentionLabel == 1)
        {
            for (int i = 0; i < (relationNum + entityNum) * maxPathLen; i ++)
            {
                fprintf(fembedding, "%lf ", bestW[i]);
            }
        }
        fprintf(fembedding, "\n");
        fclose(fout);
        fclose(fembedding);
    }
}



