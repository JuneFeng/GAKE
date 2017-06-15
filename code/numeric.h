//
//  numeric.h
//  NetworkEmbedding
//
//  Created by Feng Jun on 16/1/2.
//  Copyright (c) 2016年 Feng Jun. All rights reserved.
//

#ifndef __NetworkEmbedding__numeric__
#define __NetworkEmbedding__numeric__

#include <stdio.h>
#include "common.h"

#endif /* defined(__NetworkEmbedding__numeric__) */

bool SortFunction(NodeInfo a, NodeInfo b);
void InitExpTable();
void CreateBinaryTree();
double CountTmpScore(int bestOrNot, int type, int centerNode, int cX, int cY, int label);
int GetLen(int pathType, int type, int index);
void GetEmbedding();
