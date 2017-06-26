GAKE:Graph Aware Knoweldge Embedding
==========
Knowledge Graph Embedding projects triples in a given knowledge graph to d-dimensional vectors. We provide the source code and datasets of the COLING 2016 paper: "GAKE:Graph Aware Knowledge Embedding".


DataSets
=========
We provide FB15K in data folder. The data is originally released by the paper "Translating Embeddings for Modeling Multi-relational Data". [[Download]](https://everest.hds.utc.fr/doku.php?id=en:transe)

The data format is:
+ train.txt: training data, format(entity1, relation, entity2)
+ valid.txt: validation data, same format as training data
+ test.txt: test data, same format as training data
+ entity_to_id.txt: all entities and corresponding ids, one per line
+ relation_to_id.txt: all relations and corresponding ids, one per line

Code
=========
We refer to the implement code of CBOW model published by Google.[[code]](https://code.google.com/archive/p/word2vec/)

Compile
=========
Just type "make" in the corresponding folder.

Train
========
For training, you need to type "./main [dim] [window] [alpha] [loopNum] [attentionLabel] [pathContextNum] [edgeContextNum] [edgeNum] [pathRate] [edgeRate]" in the corresponding folder.

The output of the model will be saved in folder result/.

Parameter Setting:

+dim: the dimension of embedding vectors
+window: the length of path context
+alpha: learning rate
+loopNum: training iteration number
+attentionLabel: use the attention mechanism or not
+pathContextNum: path context number
+edgeContextNum: edge context number
+edgeNum: the number of chosen edges for each entity
+pathRate: the prestige of path context
+edgeRate: the prestige of edge context

Cite
=========
If you use the code, pleasee cite the following paper:
[Feng et al. 2016] Jun Feng, Minlie Huang, Yang Yang, and Xiaoyan Zhu. GAKE: Graph Aware Knowledge Embedding. In COLING2016. [[pdf]](https://aclweb.org/anthology/C/C16/C16-1062.pdf)

Reference
=========
[1] [Borders et al. 2013] Antoine Bordes, Nicolas Usunier, Alberto Garcia-Duran. Translating Embedding for Modeling Multi-Relational Data.

[2] https://code.google.com/archive/p/word2vec/
