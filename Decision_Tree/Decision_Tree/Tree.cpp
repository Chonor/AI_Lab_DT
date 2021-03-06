#include "Tree.h"
#define MAX 100
DATA::DATA() {
	T = F = 0;
	H = 0.0;
}
DATA::DATA(vector<vector<double> >Data, vector<double>label) {
	this->Data = Data;
	this->label = label;
	T = F = 0;
	H = 0.0;
	Count();
}
void DATA::Count() { //统计数据集
	for (int i = 0; i < Data[0].size(); i++) {
		map < double, T_F > m;
		cnt.push_back(m);
	}
	for (int i = 0; i < label.size(); i++) {
		if (label[i] == 1)T++;//总统计标签
		else F++;
		vector<double>vtmp = Data[i];
		for (int j = 0; j < vtmp.size(); j++) {
			if (cnt[j].count(vtmp[j]) > 0) {  //统计各个特征值的各个值标签
				if (label[i] == 1)cnt[j][vtmp[j]].T++;
				else cnt[j][vtmp[j]].F++;
			}
			else {
				if (label[i] == 1) {
					cnt[j][vtmp[j]].T = 1;
					cnt[j][vtmp[j]].F = 0;
				}
				else {
					cnt[j][vtmp[j]].T = 0;
					cnt[j][vtmp[j]].F = 1;
				}
			}
		}
	}
	double part_t = -((double)T / label.size())*log2((double)T / label.size());
	double part_f = -((double)F / label.size())*log2((double)F / label.size());
	if (T == 0)part_t = 0;  //处理log0;
	if (F == 0)part_f = 0;
	H = part_t + part_f;//计算经验熵
}
Tree::Tree()
{
	root = new Tree_Node;
	root->info = -1;
	root->label = 0;
	now = root;
	choose = 0;
}

Tree::Tree(int choose, DATA Train, DATA Test)
{
	root = new Tree_Node;
	root->info = -1;
	root->label = 0;
	now = root;
	this->choose = choose;
	this->Train = Train;
	this->Test = Test;
	this->count = Train.Data[0].size();
	this->Train.Count();
	Count_All_label();
}
Tree::~Tree()
{

}
vector<double> Tree::ID3(DATA Data) {
	vector<double> Gain;
	for (int i = 0; i < Data.Data[0].size(); i++) {//遍历所有特征值
		double sum = 0;
		map < double, T_F > ::iterator iter;
		for (iter = Data.cnt[i].begin(); iter != Data.cnt[i].end(); iter++) { //遍历所有值计算条件熵
			T_F tmps = iter->second;   //分成两部分计算
			double part_t = -((double)(tmps.T) / (tmps.T + tmps.F))*log2((double)(tmps.T) / (tmps.T + tmps.F));
			double part_f = -((double)(tmps.F) / (tmps.T + tmps.F))*log2((double)(tmps.F) / (tmps.T + tmps.F));
			if (tmps.T == 0)part_t = 0;  //处理log0;
			if (tmps.F == 0)part_f = 0;//处理log0;
			sum += ((double)(tmps.T + tmps.F) / Data.Data.size())*(part_t + part_f);
		}
		Gain.push_back(Data.H - sum);//计算信息增益
	}
	return Gain;
}
vector<double> Tree::C45(DATA Data) {
	vector<double> Gain = ID3(Data); //先获得信息增益
	vector<double>GainRatio;
	for (int i = 0; i < Data.Data[0].size(); i++) {//遍历所有特征值
		double sum = 0;
		map<double, T_F>::iterator iter;
		for (iter = Data.cnt[i].begin(); iter != Data.cnt[i].end(); iter++) { //求SplitInfo
			T_F tmps = iter->second;
			int tmp = tmps.T + tmps.F;
			if (tmp == 0) sum += 0;//处理log0
			else sum += -((double)(tmps.T + tmps.F) / Data.Data.size())*log2((double)(tmps.T + tmps.F) / Data.Data.size());
		}
		GainRatio.push_back(Gain[i] / sum);//计算信息增益率
	}
	return GainRatio;
}
vector<double> Tree::Gini(DATA Data) {
	//计算原始Gini指数
	double Original_Gini = 1 - ((double)Data.T / (Data.T + Data.F))*((double)Data.T / (Data.T + Data.F)) -
		((double)Data.F / (Data.T + Data.F))*((double)Data.F / (Data.T + Data.F));
	vector<double>Gini;
	for (int i = 0; i < Data.Data[0].size(); i++) {//遍历所有特征值
		double sum = 0;
		map<double, T_F>::iterator iter;
		for (iter = Data.cnt[i].begin(); iter != Data.cnt[i].end(); iter++) { //计算Gini
			T_F tmps = iter->second;
			sum += ((double)(tmps.T + tmps.F) / Data.Data.size())*
				(1 - ((double)tmps.T / (tmps.T + tmps.F))*((double)tmps.T / (tmps.T + tmps.F))
					- ((double)tmps.F / (tmps.T + tmps.F))*((double)tmps.F / (tmps.T + tmps.F)));
		}
		Gini.push_back(Original_Gini - sum); //计算差值
	}
	return Gini;
}
void Tree::Count_All_label() {
	for (int i = 0; i < Train.cnt.size(); i++) {
		map<double, T_F>::iterator iter;
		vector<double>tmps;
		for (iter = Train.cnt[i].begin(); iter != Train.cnt[i].end(); iter++)
			tmps.push_back(iter->first);
		All_label.push_back(tmps);
	}
}

vector<double> Tree::Decision(bool trees) {
	vector<double>label;
	for (int i = 0; i < Test.Data.size(); i++) { //遍历测试or验证集
		Tree_Node *check = new Tree_Node;
		check = root;
		bool flag = 1;
		while (check->choose != -1) { //当前节点有子节点
			flag = 1;
			double Min = 9999999;
			int pos;
			for (int j = 0; j < check->child.size(); j++) { //当前节点的所有子节点
				if (check->child[j]->info == Test.Data[i][check->choose]) {//存在特征值匹配的子节点
					check = check->child[j];						//节点下移
					flag = 0;
					break;
				}
				else{//不再存在，就近原则，选择一个最接近当前值的子节点
					if (fabs(check->child[j]->info - Test.Data[i][check->choose]) < Min) {
						Min = fabs(check->child[j]->info - Test.Data[i][check->choose]);
						pos = j;
					}
				}
			}
			if (flag&&check->label == 0) {//承接上面的不存在
				check = check->child[pos];//节点下移
			}
			else if (check->label != 0)break;//叶子结点直接跳出循环
		}
		if(trees)label.push_back((double)check->data.T / (check->data.F+ check->data.T)); //存储判决结果 森林
		else label.push_back(check->data.T >= check->data.F ? 1 : -1);//树
	}
	return label;
}

void Tree::Build_Tree() {
	root->data = Train;
	now->data = Train;
	Build_Tree(Train);
}
//#define MIN 5
void Tree::Build_Tree(DATA data) {
	if (count == 0 ||  data.Data.size()==data.T|| data.Data.size() == data.F) { //终止递归条件
		if (data.T > data.F)now->label = 1;
		else now->label = -1;
		return;
	}
	/*if (data.Data.size() < MIN) {//使用阈值终止递归
		if (data.T > data.F)now->label = 1;
		else now->label = -1;
		return;
	}*/
	vector<DATA>tmp_Data;
	vector<double> Gain;
	int Max = 0;
	if (choose == 0)//方法选择
		Gain = ID3(data);
	else if (choose == 1)
		Gain = C45(data);
	else
		Gain = Gini(data);
	//选择特征值
	for (int i = 1; i < Gain.size(); i++) 
		if (Gain[Max] < Gain[i])Max = i;
	
	//选择节点没有没有降低熵时
	if (Gain[Max] <= 0) {
		now->choose = -1;
		if (data.T >= data.F)now->label = 1;
		else now->label = -1;
		return;
	}
	now->choose = Max;
	//进行数据集分割
	map<double, T_F>::iterator iter;
	map<double, int>find;
	int i = 0;
	vector<double>index;
	for (iter = data.cnt[Max].begin(); iter != data.cnt[Max].end(); iter++) {
		find[iter->first] = i++;							//map索引
		index.push_back(iter->first);
	}
	for (int i = 0; i < find.size(); i++) {
		DATA d;
		tmp_Data.push_back(d);
	}
	for (int i = 0; i < data.Data.size(); i++) {			//分割数据集
		int pos = find[data.Data[i][Max]];
		data.Data[i][Max] = 0;								//当前特征值列置0
		tmp_Data[pos].Data.push_back(data.Data[i]);
		tmp_Data[pos].label.push_back(data.label[i]);
	}
	for (int i = 0; i < All_label[Max].size(); i++) {		//当前特征值的所有值
		if (find.count(All_label[Max][i]) > 0) {			//如果其子数据集非空
			count--;										//递归层数-1
			tmp_Data[find[All_label[Max][i]]].Count();		//统计子数据集
			Tree_Node *next = new Tree_Node;				//新建子节点
			now->child.push_back(next);						//放入当前节点子节点集
			next->pre = now;								//设置子节点属性
			next->info = index[find[All_label[Max][i]]];	
			next->choose = -1;
			next->label = 0;
			next->data = tmp_Data[find[All_label[Max][i]]];
			now = next;										//递归
			Build_Tree(tmp_Data[find[All_label[Max][i]]]);
			count++;										//回溯
			now = now->pre;
		}
		else {												//特征值的值对应子数据集空
			Tree_Node *next = new Tree_Node;				//新建子节点但不递归
			now->child.push_back(next);	
			next->pre = now;
			next->info = All_label[Max][i];
			next->choose = -1;
			next->label = data.T>=data.F? 1:-1;				//父节点中最多的标签
		}
	}
}
void Tree::Print_Tree() { //打印树
	for (int i = Train.Data[0].size(); i > count; i--)
		cout << '\t';
	cout  << "val:" << now->info << "\tcol:" << now->choose <<"\tlabel:"<<now->label << endl;
	for (int i = 0; i < now->child.size(); i++) {
		now=now->child[i];
		count--;
		Print_Tree();
		count++;
		now = now->pre;
	}
}

Forest::Forest()
{
}
Forest::Forest(int choose, DATA Train, DATA Test)
{
	this->choose = choose;
	this->Train = Train;
	this->Test = Test;
	this->count = Train.Data[0].size();
	this->use_col = (int)sqrt(count);
}
void Forest::Build_Forest() {
	int a[MAX], b[MAX];												//产生特征值组合
	for (int i = 0; i < count; i++)
		a[i]=i;
	combine(a, count, use_col, b, use_col);							//产生特征值组合
	int num =  rand() % (cols.size() / 3) + (2 * cols.size() / 3);		//随机森林大小
	//int num = cols.size();
	if (num > 1000)num = 1000;										//控制最大
	for (int i = 0; i < num; i++) {
		int rand_col = rand() % cols.size();						//随机选择特征值组合
		DATA tmp_train, tmp_test;
		for (int j = 0; j < Train.Data.size(); j++) {				//随机有放回的抽取训练集
			int pos = rand() % Train.Data.size();
			vector<double>values;
			for (int k = 0; k < cols[rand_col].size(); k++)			//根据特征值组合抽取特征值
				values.push_back(Train.Data[pos][cols[rand_col][k]]);
			tmp_train.Data.push_back(values);
			tmp_train.label.push_back(Train.label[pos]);
		}
		for (int j = 0; j < Test.Data.size(); j++) {				//复制一遍测试集
			vector<double>values;
			for (int k = 0; k < cols[rand_col].size(); k++)
				values.push_back(Test.Data[j][cols[rand_col][k]]);	//根据特征值组合抽取特征值
			tmp_test.Data.push_back(values);
			tmp_test.label.push_back(Test.label[j]);
		}
		Tree t(choose, tmp_train, tmp_test);						//训练一颗树
		t.Build_Tree();
		trees.push_back(t);											//放入森林中
	}
}
vector<double> Forest::Decision() {
	vector<vector<double> >tmp;
	vector<double>ans;
	for (int i = 0; i < trees.size(); i++) 
		tmp.push_back(trees[i].Decision(1));
	for (int i = 0; i < Test.Data.size(); i++) {
		int T=0, F = 0;
		double sum = 0;
		for (int j = 0; j < tmp.size(); j++) {
			sum+=tmp[j][i];
		}
		ans.push_back(sum>=(double)tmp.size() / 2 ? 1 : -1);//根据概率决定
	}
	return ans;
}

void Forest::combine(int a[], int n, int m, int b[], int M) {//组合数计算
	for (int i = n; i >= m; i--) {
		b[m - 1] = i - 1;
		if (m > 1)
			combine(a, i - 1, m - 1, b, M);
		else {
			vector<int>tmp;
			for (int j = M - 1; j >= 0; j--)
				tmp.push_back(a[b[j]]);
			cols.push_back(tmp);
		}
	}
}

Forest::~Forest()
{

}

Decision_Tree::Decision_Tree()
{
	K = 5;
	build = 0;
}
Decision_Tree::Decision_Tree(int K,int build,int choose,string train_src,string test_src)
{
	this->K = K;
	this->build = build;
	this->choose = choose;
	Train = Data_In(train_src,1);
	Test = Data_In(test_src,0);
}
Decision_Tree::~Decision_Tree()
{
}

DATA Decision_Tree::Data_In(string src,bool random) {
	srand((unsigned)time(NULL));
	DATA tmp_data;
	vector<string>str;
	vector<vector<double> >Data;
	vector<double>label;
	ifstream in;
	in.open(src);
	string tmp;
	while (getline(in, tmp))str.push_back(tmp);
	if(random)random_shuffle(str.begin(), str.end());//打乱
	for(int s=0;s<str.size();s++){
		tmp = str[s];
		vector<double>vtmp;
		string stmp = "";
		for (int i = 0; i < tmp.length(); i++) {//输入没什么说的
			if (tmp[i] != ',')stmp += tmp[i];
			else {
				stringstream ss(stmp);
				double a;
				ss >> a;
				vtmp.push_back(a);
				stmp = "";
			}
		}
		Data.push_back(vtmp);
		stringstream ss(stmp);
		double a;
		ss >> a;
		label.push_back(a);
	}

	tmp_data.Data = Data;
	tmp_data.label = label;
	/*tmp_data.Count();
	for (int i = 0; i < tmp_data.cnt.size(); i++) {
		if (tmp_data.cnt[i].size() > 10) {//此处设定连续值多于10进行处理
			int div = log2(tmp_data.cnt[i].size());
			for (int j = 0; j < tmp_data.Data.size(); j++)
				tmp_data.Data[j][i] /= div;
		}
	}*/
	return tmp_data;
}
void Decision_Tree::K_Fold() {
	for (int i = 0; i < K; i++) {//拆分k个数据集
		DATA tmp_data;
		for (int j = 0; j < Train.Data.size() / K; j++) {
			tmp_data.Data.push_back(Train.Data[i*(Train.Data.size() / K) + j]);
			tmp_data.label.push_back(Train.label[i*(Train.Data.size() / K) + j]);
		}
		if (i == K - 1) {
			for (int j = 0; j <  Train.Data.size() % K; j++) {
				tmp_data.Data.push_back(Train.Data[Train.Data.size() - 1 - j]);
				tmp_data.label.push_back(Train.label[Train.Data.size() - 1 - j]);
			}
		}
		K_data.push_back(tmp_data);
	}
	for (int i = 0; i < K; i++) {//k个数据集重组
		DATA tmp_train, tmp_val;
		for (int j = 0; j < K_data.size(); j++) {
			if (j != i) {
				for (int k = 0; k < K_data[j].Data.size(); k++) {
					tmp_train.Data.push_back(K_data[j].Data[k]);
					tmp_train.label.push_back(K_data[j].label[k]);
				}
			}
			else {
				for (int k = 0; k < K_data[j].Data.size(); k++) {
					tmp_val.Data.push_back(K_data[j].Data[k]);
					tmp_val.label.push_back(K_data[j].label[k]);
				}
			}
		}
		K_Train.push_back(tmp_train);
		K_Test.push_back(tmp_val);
	}
}
void Decision_Tree::Verify() {
	K_Fold();	//把数据集分成K份
	double sum = 0;
	if (build == 0)cout << "Build Tree.use:";
	else cout << "Build Forest.use:";
	if (choose == 0)cout << "ID3\n";
	else if (choose == 1)cout << "C4.5\n";
	else cout << "Gini\n";
	cout << "use " << K << " fold\n";
	for (int i = 0; i < K; i++) {										//重复交叉验证K次
		if (build == 0) {												//进行决策树的建立
			Tree t(choose, K_Train[i], K_Test[i]);						
			t.Build_Tree();												
			double tmp = Cmp_label(K_Test[i].label, t.Decision(0));		//判决
			cout <<i<<" fold:"<<tmp << endl;
			sum+=tmp;													//求和
		}
		else {
			Forest f(choose, K_Train[i], K_Test[i]);
			f.Build_Forest();
			double tmp = Cmp_label(K_Test[i].label, f.Decision());
			cout << i << " fold:" << tmp << endl;
			sum += tmp;
		}
	}
	cout <<"平均准确率："<<sum / K << endl;
}
double Decision_Tree::Cmp_label(vector<double>lable, vector<double>ans) {
	int cnt = 0;
	for (int i = 0; i < lable.size(); i++) 
		if (lable[i] == ans[i])cnt++;
	return (double)cnt/ lable.size();
}
void Decision_Tree::Tests(){
	if (build == 0)cout << "Build Tree.use:";
	else cout << "Build Forest.use:";
	if (choose == 0)cout << "ID3\n";
	else if (choose == 1)cout << "C4.5\n";
	else cout << "Gini\n";
	vector<double>ans;
	K_Fold();
	if (build == 0) {												//进行决策树的建立
		Tree t(choose, Train, Test);
		t.Build_Tree();
		//t.Print_Tree();
		ans=t.Decision(0);
	}
	else {
		Forest f(choose, Train, Test);
		f.Build_Forest();
		ans=f.Decision();
	}
	ofstream out;
	out.open("ans.txt");
	for (int i = 0; i < ans.size(); i++) {
		out << ans[i] << endl;
	}
	out.close();
}