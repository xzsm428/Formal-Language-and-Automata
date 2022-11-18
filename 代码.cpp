#include <fstream>
#include <iostream>
#include<string>
#include<set>
#include<algorithm>
using namespace std;
struct state
{
	set<int> To0, To1, Toe;//用下标区分当前状态，三个集合分别存放读入0/1/ε后跳转到的状态
};
state eNFA[100];
state NFA[100];
struct DFA_state
{
	set<int> include;
	state To;
	bool isEnd=0;//是终止状态为1,不是为0
}DFA[100];
set<int> tmp_;
set<int> start, End;//存开始状态和NFA终止状态
int n = 0;//状态数
void Read(string s, int n, int kind)
{
	int i = 0;
	while (i < s.size())
	{
		if (s[i] == 'q')
		{
			switch (kind)
			{
			case 0://读入0
				eNFA[n].To0.insert(int(s[i + 1]) - '0');
				break;
			case 1://读入1
				eNFA[n].To1.insert(int(s[i + 1]) - '0');
				break;
			case 2://读入ε
				eNFA[n].Toe.insert(int(s[i + 1]) - '0');
				break;
			}
			i += 2;
		}
		else i++;
	}
}
void traverse(int q)
{
	for (set<int>::iterator it = eNFA[q].Toe.begin(); it != eNFA[q].Toe.end(); it++)
	{
		tmp_.insert(*it);
		traverse(*it);
	}
}
set<int> ε_CLOSURE(int q)//求状态q的ε_CLOSURE
{
	set<int> C;
	tmp_.insert(q);
	for (set<int>::iterator it = eNFA[q].Toe.begin(); it != eNFA[q].Toe.end(); it++)
	{
		tmp_.insert(*it);
		traverse(*it);
	}
	C = tmp_;
	tmp_.clear();
	return C;
}
void  Extended_transferFunc(int q,int kind)//扩展转移函数
{
	set<int> C = ε_CLOSURE(q);//求当前状态的ε - CLOSURE
	set<int> tmp;
	for (set<int>::iterator it = C.begin(); it != C.end(); it++)
	{
		switch (kind)
		{
		case 0://读入0
		{
			for (set<int>::iterator i = eNFA[*it].To0.begin(); i != eNFA[*it].To0.end(); i++)
				tmp.insert(*i);
			break;
		}
		case 1://读入1
		{
			for (set<int>::iterator i = eNFA[*it].To1.begin(); i != eNFA[*it].To1.end(); i++)
				tmp.insert(*i);
			break;
		}
		}
	}
	C.clear();
	for (set<int>::iterator it = tmp.begin(); it != tmp.end(); it++)
	{
		set<int> C1 = ε_CLOSURE(*it);
		set_union(C1.begin(), C1.end(), C.begin(), C.end(), inserter(C, C.begin()));
	}
	if (kind) NFA[q].To1 = C;
	else NFA[q].To0 = C;
}
int transferFunc(int q,int kind)
{
	int judge = 0;//返回值为1表明状态集合中有终止态，0没有
	if (kind) 
		for (set<int>::iterator it = NFA[q].To1.begin(); it != NFA[q].To1.end(); it++)
		{
			tmp_.insert(*it);
			if (End.count(*it)) judge = 1;
		}
	else 
		for (set<int>::iterator it = NFA[q].To0.begin(); it != NFA[q].To0.end(); it++)
		{
			tmp_.insert(*it);
			if (End.count(*it)) judge = 1;
		}
	return judge;
}
void ToNFA()
{
	int a, b;
	string s;
	string s0, s1, se;
	ifstream in("input.txt");
	in >> a >> b >> s;//读第一行
	while (in >> s >> s0 >> s1 >> se)
	{
		n++;
		if (s[0] == '#') start.insert(int(s[2]) - '0');
		else if (s[0] == '*') End.insert(int(s[2]) - '0');
		Read(s0, n, 0);
		Read(s1, n, 1);
		Read(se, n, 2);
	}
	in.close();
	for (set<int>::iterator it = start.begin(); it != start.end(); it++)//求NFA的F
	{
		set<int> C=ε_CLOSURE(*it);
		set<int> tmp;
		set_intersection(C.begin(), C.end(), End.begin(), End.end(), inserter(tmp, tmp.begin()));//求F交ε_CLOSURE(q0)
		if (tmp.size())//若F与ε - CLOSURE(q0)的交集不为空
			set_union(start.begin(), start.end(), End.begin(), End.end(), inserter(End, End.begin()));//FU{q0}存入End
	}
	for (int i = 1; i <= n; i++)
		for (int j = 0; j < 2; j++) Extended_transferFunc(i, j);
}
void ToDFA()
{
	n = -1;//DFA状态数
	set<set<int>> State;//判断状态是否已存在
	for (set<int>::iterator it = start.begin(); it != start.end(); it++)//存开始状态
	{
		n++;
		DFA[n].include.insert(*it);
		if (End.count(*it)) DFA[n].isEnd = 1;//开始状态和结束状态相同
		State.insert(DFA[n].include);
	}
	int index = 0;//index用于遍历
	for (set<set<int>>::iterator it = State.begin(); it != State.end(); it++)//遍历状态集合
	{
		int s = 0;//s用于记录本轮新加入的状态数
		for (int i = 0; i <= 1; i++)//读入0，1
		{
			int judge = 0;
			for (set<int>::iterator it_ = (*it).begin(); it_ != (*it).end(); it_++)//遍历状态集合中的每个状态
			{
				if (!judge) judge = transferFunc(*it_, i);//返回值为1表明状态集合中有终止态，0没有
				else transferFunc(*it_, i);//若judge为1,则现有状态集合已为终止态，judge不再改变
				if (i) DFA[index].To.To1 = tmp_;//读入1跳到的状态
				else DFA[index].To.To0 = tmp_;//读入0跳到的状态
				if (!State.count(tmp_) && !tmp_.empty())//该状态为新状态(不为空且不在已有状态里）
				{
					s++;
					DFA[n + s].include = tmp_;//DFA的新状态
					DFA[n + s].isEnd = judge;//1为终止态，0不是
					State.insert(tmp_);
				}
			}
			tmp_.clear();
		}
		index += 1;
		n += s;
	}
}
void Out()
{
	ofstream out("output.txt");
	out << "0 1"<<endl;
	out << '#';
	for (int i = 0; i <= n; i++)
	{
		string s = "";
		if (DFA[i].isEnd) out << '*';
		out << "{";
		for (set<int>::iterator it = DFA[i].include.begin(); it != DFA[i].include.end(); it++)//输出状态
			s += 'q' + to_string(*it) + ',';
		s.erase(s.size() - 1);//删除最后一个逗号
		out << s;
		out << "} ";
		s.clear();
		out << "{";
		for (set<int>::iterator it = DFA[i].To.To0.begin(); it != DFA[i].To.To0.end(); it++)//输出0转移
			s += 'q' + to_string(*it) + ',';
		if (s.size()) s.erase(s.size() - 1);
		out << s;
		out << "} ";
		s.clear();
		out << "{";
		for (set<int>::iterator it = DFA[i].To.To1.begin(); it != DFA[i].To.To1.end(); it++)//输出1转移
			s += 'q' + to_string(*it) + ',';
		if (s.size()) s.erase(s.size() - 1);
		out << s;
		out << "}" << endl;
	}
	out.close();

}
int main()
{
	ToNFA();
	ToDFA();
	Out();
	return 0;
}
