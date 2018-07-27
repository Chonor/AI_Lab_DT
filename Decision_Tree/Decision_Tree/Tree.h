#ifndef TREE_H
#define TREE_H
#include<iostream>
#include<string>
#include<cstring>
#include<cmath>
#include<map>
#include<vector>
#include<fstream>
#include<sstream>
#include<random>
#include <ctime>
#include <algorithm>
using namespace std;

struct T_F
{
	int T;
	int F;
};
class DATA
{
public:
	DATA();
	DATA(vector<vector<double> >Data, vector<double>label);
	void Count();											//数据集计数

	vector<vector<double> >Data;							//存储数据集
	vector<double>label;
	vector<map<double, T_F>>cnt;							//索引
	int T, F;												//总的标签情况
	double H;												//熵
};
struct Tree_Node
{
	double info;											//储存分支值
	int choose;												//当前节点选择的特征
	double label;											//标签
	DATA data;												//当前节点数据集
	Tree_Node *pre;											//父节点
	vector<Tree_Node *>child;								//子节点集
};

class Tree
{
public:
	Tree();
	Tree(int choose, DATA Train,DATA Test);
	~Tree();
	void Build_Tree();										//递归建树
	void Build_Tree(DATA data);
	void Print_Tree();										//以文件树的方式打印树
	vector<double> Decision(bool trees);					//判决
private:
	Tree_Node* root;										//固定的根节点保留用
	Tree_Node* now;											//递归用节点
	DATA Train,Test;										//数据集
	int choose;												//确定方法								
	int count;												//设定递归层数
	vector<double> ID3(DATA Data);							//三种选择
	vector<double> C45(DATA Data);
	vector<double> Gini(DATA Data);
	vector<vector<double> >All_label;						//每个特征值的所有值
	void Count_All_label();									
};

class Forest
{
public:
	Forest();				
	Forest(int choose, DATA Train, DATA Test);
	void Build_Forest();									//森林创建
	vector<double> Decision();								//森林决策
	~Forest();

private:
	int choose;												//ID3 C4.5 Gini
	int count;												
	int use_col;											//挑选的特征值的个数
	vector<Tree>trees;										//存放树
	DATA Train, Test;
	vector<vector<int> >cols;								//存放特征值组合
	void combine(int a[], int n, int m, int b[], int M);	//计算特征值组合
};
class Decision_Tree											//总控类
{
public:
	Decision_Tree();
	Decision_Tree(int K, int build, int choose, string train_src, string test_src);
	DATA Data_In(string src, bool random);					//数据输入
	void Verify();											//K折交叉验证
	void Tests();											//测试
	~Decision_Tree();
private:
	int K,build,choose;										//K折的K、建树or森林、节点选择方法
	vector<DATA>K_data;										//K折数据存放
	vector<DATA>K_Train;
	vector<DATA>K_Test;
	DATA Train, Test;										//存放总数据
	void K_Fold();											//分割K折数据
	double Cmp_label(vector<double>lable, vector<double>ans);//结果比对
};


#endif // !TREE_H

