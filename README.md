GAKE:Graph Aware Knoweldge Embedding
==========
Knowledge Graph Embedding projects triples in a given knowledge graph to d-dimensional vectors. We provide the source code and datasets of the COLING 2016 paper: "GAKE:Graph Aware Knowledge Embedding".


DataSets
=========
We provide FB15K in data folder. The data is originally released by the paper "Translating Embeddings for Modeling Multi-relational Data". [[Download]](ttps://everest.hds.utc.fr/doku.php?id=en:transe)

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
For training, you need to type "./main" in the corresponding folder.

The output of the model will be saved in folder result/.

Cite
=========
If you use the code, pleasee cite the following paper:
[Feng et al. 2016] Jun Feng, Minlie Huang, Yang Yang, and Xiaoyan Zhu. GAKE: Graph Aware Knowledge Embedding. In COLING2016. [[pdf]](https://aclweb.org/anthology/C/C16/C16-1062.pdf)

Reference
=========
[Borders et al. 2013] Antoine Bordes, Nicolas Usunier, Alberto Garcia-Duran. Translating Embedding for Modeling Multi-Relational Data.

https://code.google.com/archive/p/word2vec/
