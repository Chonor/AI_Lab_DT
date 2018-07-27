#include"Tree.h"

#define ID3_ 0
#define C4_5 1
#define GINI 2
#define BUILD_TREE 0
#define BUILD_FOREST 1
int main() {
	Decision_Tree tree(10, BUILD_FOREST, GINI, "train.csv", "test.csv");
	//tree.Verify();
	tree.Tests();
}