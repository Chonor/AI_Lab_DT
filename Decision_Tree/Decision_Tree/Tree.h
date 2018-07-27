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
	void Count();											//���ݼ�����

	vector<vector<double> >Data;							//�洢���ݼ�
	vector<double>label;
	vector<map<double, T_F>>cnt;							//����
	int T, F;												//�ܵı�ǩ���
	double H;												//��
};
struct Tree_Node
{
	double info;											//�����ֵ֧
	int choose;												//��ǰ�ڵ�ѡ�������
	double label;											//��ǩ
	DATA data;												//��ǰ�ڵ����ݼ�
	Tree_Node *pre;											//���ڵ�
	vector<Tree_Node *>child;								//�ӽڵ㼯
};

class Tree
{
public:
	Tree();
	Tree(int choose, DATA Train,DATA Test);
	~Tree();
	void Build_Tree();										//�ݹ齨��
	void Build_Tree(DATA data);
	void Print_Tree();										//���ļ����ķ�ʽ��ӡ��
	vector<double> Decision(bool trees);					//�о�
private:
	Tree_Node* root;										//�̶��ĸ��ڵ㱣����
	Tree_Node* now;											//�ݹ��ýڵ�
	DATA Train,Test;										//���ݼ�
	int choose;												//ȷ������								
	int count;												//�趨�ݹ����
	vector<double> ID3(DATA Data);							//����ѡ��
	vector<double> C45(DATA Data);
	vector<double> Gini(DATA Data);
	vector<vector<double> >All_label;						//ÿ������ֵ������ֵ
	void Count_All_label();									
};

class Forest
{
public:
	Forest();				
	Forest(int choose, DATA Train, DATA Test);
	void Build_Forest();									//ɭ�ִ���
	vector<double> Decision();								//ɭ�־���
	~Forest();

private:
	int choose;												//ID3 C4.5 Gini
	int count;												
	int use_col;											//��ѡ������ֵ�ĸ���
	vector<Tree>trees;										//�����
	DATA Train, Test;
	vector<vector<int> >cols;								//�������ֵ���
	void combine(int a[], int n, int m, int b[], int M);	//��������ֵ���
};
class Decision_Tree											//�ܿ���
{
public:
	Decision_Tree();
	Decision_Tree(int K, int build, int choose, string train_src, string test_src);
	DATA Data_In(string src, bool random);					//��������
	void Verify();											//K�۽�����֤
	void Tests();											//����
	~Decision_Tree();
private:
	int K,build,choose;										//K�۵�K������orɭ�֡��ڵ�ѡ�񷽷�
	vector<DATA>K_data;										//K�����ݴ��
	vector<DATA>K_Train;
	vector<DATA>K_Test;
	DATA Train, Test;										//���������
	void K_Fold();											//�ָ�K������
	double Cmp_label(vector<double>lable, vector<double>ans);//����ȶ�
};


#endif // !TREE_H

