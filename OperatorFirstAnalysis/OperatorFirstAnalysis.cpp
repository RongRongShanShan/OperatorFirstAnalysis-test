#include<iostream>
#include<String>
#include<Vector>
#include<Map>
#include<Set>
#include <algorithm>
#include <iomanip>
using namespace std;

//输入文法储存
vector<string> G;
//开始符号
char start;
//非终结符列表
set<char> vN;
//终结符列表
set<char> vT;
//文法关系映射  key:文法左部  value:所有产生式右部
map<char, vector<string>> GMap;
//FIRSTVT集 和 LASTVT集
map<char, set<char>> FIRSTVT, LASTVT;

//算符优先分析表
typedef struct OperatorFirstTable {
	//终结符和#
	vector<char> ter;
	//表内容 = < > @
	vector<vector<char>> table;

	/*获取某个终结符(包含#)对应的索引*/
	int getTerIdx(char t) {
		int i = 0;
		for (; i < ter.size(); ++i) {
			if (ter[i] == t) {
				return i;
			}
		}
		return -1; //ter包含所有vT，不会返回-1
	}

	/*构建分析表*/
	void createAnalysisTable() {
		//填充ter
		for (char t : vT) {
			ter.push_back(t);
		}
		ter.push_back('#');
		//初始化table (初始全设为@表示没有优先关系)
		int tsize = ter.size();
		table.resize(tsize);
		for (int i = 0; i < tsize; ++i) {
			for (int j = 0; j < tsize; ++j) {
				table[i].push_back('@');
			}
		}
		//分析填充table
		for (char x : vN) {
			for (string right : GMap[x]) {
				int len = right.size();
				//考虑产生式本身字符的优先级
				for (int i = 0; i < len; ++i) {
					//1. 如果存在A->..aBb..或A->..ab..,则a=b
					//2. 如果存在A->..aB..则对于b∈FIRSTVT(B)有a<b
					if (vT.find(right[i]) != vT.end()) { // ..a..
						int a = getTerIdx(right[i]);
						//..ab..  ->  a=b
						if (i + 1 < len && vT.find(right[i + 1]) != vT.end()) {
							int b = getTerIdx(right[i + 1]);
							table[a][b] = '=';
						}
						//..aB..  ->  b∈FIRSTVT(B)，a<b
						else if (i + 1 < len && vN.find(right[i + 1]) != vN.end()) {
							for (char ch : FIRSTVT[right[i + 1]]) {
								int b = getTerIdx(ch);
								table[a][b] = '<';
							}
							//..aBb..  ->  a=b
							if (i + 2 < len && vT.find(right[i + 2]) != vT.end()) {
								int b = getTerIdx(right[i + 2]);
								table[a][b] = '=';
							}
						}
					}
					//3. 如果存在A->..Bb..,则对于a∈LASTVT(B)有a>b
					else { // ..B..
						if (i + 1 < len && vT.find(right[i + 1]) != vT.end()) { //..Bb..
							int b = getTerIdx(right[i + 1]);
							for (char ch : LASTVT[right[i]]) {
								int a = getTerIdx(ch);
								table[a][b] = '>';
							}
						}
					}
				}
			}
		}
		//扩充：#E# ， E为开始符号
		//  对于a∈FIRSTVT(E),#<a
		for (char ch : FIRSTVT[start]) {
			table[getTerIdx('#')][getTerIdx(ch)] = '<';
		}
		//  对于a∈LASTVT(E),a>#
		for (char ch : LASTVT[start]) {
			table[getTerIdx(ch)][getTerIdx('#')] = '>';
		}
		//#=#
		table[table.size() - 1][table.size() - 1] = '=';
	}

	/*展示分析表*/
	void showTable() {
		cout << "算符优先分析表:" << endl;
		cout << "    ";
		for (int i = 0; i < ter.size(); ++i) {
			cout << setw(6) << ter[i];
		}
		cout << endl;
		for (int i = 0; i < ter.size(); ++i) {
			cout << "  " << setw(2) << ter[i];
			for (int j = 0; j < ter.size(); ++j) {
				cout << setw(6) << table[i][j];
			}
			cout << endl;
		}
		cout << endl;
	}

	/*判断输入字符串是否符合文法*/
	void analyseInputString(string input) {
		//步骤
		int step = 1;
		//使用string充当分析栈
		string stack = "#";
		//剩余串处理 (如果末尾没有#就补上)
		if (input.at(input.size() - 1) != '#') {
			input += "#";
		}
		//表头
		cout << "   " << setw(5) << "步骤" << setw(14) << "分析栈" << setw(6) << "关系"
			<< setw(14) << "剩余串" << setw(14) << "动作\n";
		while (true) {
			cout << "   " << setw(5) << step << setw(14) << stack;
			//如果剩余串只剩#且栈中终结符只剩#，规约成功
			if (input == "#") {
				bool just = true;
				for (int i = 1; i < stack.size(); ++i) {
					if (vT.find(stack.at(i)) != vT.end()) {
						just = false;
						break;
					}
				}
				if (just) {
					cout << setw(6) << "="
						<< setw(14) << "#" << setw(14) << "接受\n";
					return;
				}
			}
			//结合剩余串首位考虑优先关系
			char priority = '@';
			for (int i = stack.size() - 1; i > -1; --i) {
				int a = stack.at(i);
				if (a == '#' || vT.find(a) != vT.end()) {
					priority = table[getTerIdx(a)][getTerIdx(input.at(0))];
					break;
				}
			}
			//如果栈尾与串首优先关系为@(空),无法继续进行
			if (priority == '@') {
				cout << setw(6) << "无"
					<< setw(14) << input << setw(14) << "错误\n";
				return;
			}
			//打印栈尾与串首的优先关系
			cout << setw(6) << priority << setw(14) << input;
			//根据优先关系分析接下来的动作
			if (priority != '>') {  //当前优先关系为<或=,移进
				stack += input.at(0);
				input.erase(input.begin());
				cout << setw(14) << "移进\n";
			}
			else {  //当前优先关系为>,需检查栈中是否存在可规约串(最左素短语)
				//最后一个终结符的位置
				int e = stack.size() - 1;
				for (; e > -1; --e) {
					if (vT.find(stack.at(e)) != vT.end()) {
						break;
					}
				}
				//如果最后一个终结符不是最栈前面的#
				if (e != 0) {
					//找是否存在a(b-1)<a(b)=...=e
					bool flag = false;
					int b = e;  // b一定在终结符位置
					for (; b > -1; --b) {
						//a(b-1)与a(b)的优先关系
						char p = '@';  //因为已经入栈，所以优先关系不可能为@
						//找第b-1个终结符
						int i = b - 1;
						for (; i > -1; --i) {
							//不要忘记考虑#
							if (stack.at(i) == '#' || vT.find(stack.at(i)) != vT.end()) {
								//a(b-1)与a(b)的优先关系
								p = table[getTerIdx(stack.at(i))][getTerIdx(stack.at(b))];
								break;
							}
						}
						if (p == '=') {
							b = i;
						}
						else if (p == '<') {
							flag = true;
							break;
						}
						else {
							break;
						}
					}
					//找到了满足条件的b (存在可规约串)
					if (flag) {
						//取出从b到e的终结符及其前后非终结符形成的式子
						string str = "";
						if (b - 1 > 0 && vN.find(stack.at(b - 1)) != vN.end()) {
							str = stack.substr(b - 1);
							stack = stack.substr(0, b - 1);
						}
						else {
							str = stack.substr(b);
							stack = stack.substr(0, b);
						}
						//在文法中查找与式子格式相近的产生式规约
						for (char x : vN) {
							//是否完成规约
							bool flag = false;
							for (string right : GMap[x]) {
								//格式相近 -> 长度相同，相同位置的终结符相同，非终结符位置相同
								//  算符优先跳跃性很强，只要相同位置的种类一致且终结符完全相同即可
								if (right.size() == str.size()) {
									flag = true;  //可能能规约
									for (int i = 0; i < str.size(); ++i) {
										if (vT.find(str.at(i)) != vT.end()) {
											//终结符不一致
											if (str.at(i) != right.at(i)) {
												flag = false;
												break;
											}
										}
										else {
											//不同时为非终结符
											if (vN.find(right.at(i)) == vN.end()) {
												flag = false;
												break;
											}
										}
									}
									//满足条件，进行规约
									if (flag) {
										stack += x;
										cout << setw(14) << "规约\n";
										break;
									}
								}
							}
							if (flag) {
								break; //已经完成规约
							}
						}
					}
				}
				else {
					//其他情况不能找到可规约串的情况都移进
					stack += input.at(0);
					input.erase(input.begin());
					cout << setw(14) << "移进\n";
				}
			}
			++step;
		}
	}

};
OperatorFirstTable operatorFirstTable;

/*清除字符串中的空格*/
string removeSpaces(const string& s) {
	string tmp(s);
	tmp.erase(std::remove(tmp.begin(), tmp.end(), ' '), tmp.end());
	return tmp;
}

/*处理输入的文法
完成符号分类(vN|vT)和文法转化(string->map<char, vector<string>>)*/
void handleGrammarForms() {
	start = G[0][0];
	vN.insert(start);
	for (int i = 0; i < G.size(); ++i) {
		//产生式的左右部分分别处理
		string str = G[i];
		//处理文法左边 (前面清除了空格,切割直接取索引,不需要find("->"))
		char gLeft = removeSpaces(str.substr(0, 1))[0];
		vN.insert(gLeft);
		//初始化文法产生式映射GMap
		if (GMap.find(gLeft) == GMap.end()) {
			vector<string> gRight;
			GMap[gLeft] = gRight;
		}
		//处理文法右边
		string rightStr = str.substr(3);
		string grammerRight;
		for (int i = 0; i < rightStr.length(); ++i) {
			char ch = rightStr[i];
			if (ch == '|') {
				//右边根据 | 分割成不同的产生结果
				GMap[gLeft].push_back(grammerRight);
				string().swap(grammerRight);
			}
			else {
				//将字符分类
				if ('A' <= ch && ch <= 'Z') {
					vN.insert(ch);
				}
				else if (ch != '$') { //ε没有放在终结符列表中
					vT.insert(ch);
				}
				//放入产生结果中
				grammerRight += ch;
			}
		}
		GMap[gLeft].push_back(grammerRight);
	}
}

/*打印当前文法*/
void showGMap() {
	for (char x : vN) {
		string s = "";
		s += x;
		s.append("->");
		auto right = GMap[x];
		for (int i = 0; i < right.size(); ++i) {
			s.append(right[i]);
			if (i != right.size() - 1) {
				s.append("|");
			}
		}
		cout << "  " << s << endl;
	}
	cout << endl;
}

/*求FIRSTVT集*/
void getFIRSTVT() {
	//初始化FIRSTVT
	for (char x : vN) {
		set<char> value;
		FIRSTVT[x] = value;
	}
	//A->a...或A->Ba... => a∈FIRSTVT(A)
	//产生式的第一个终结符a属于FIRSTVT(A)
	//这部分不会不断增大FIRSTVT,单独执行一次即可
	for (char x : vN) {
		for (string& right : GMap[x]) {
			for (char ch : right) {
				if (vT.find(ch) != vT.end()) {
					FIRSTVT[x].insert(ch);
					break; //只要第一个
				}
			}
		}
	}
	//A->B...且a∈FIRSTVT(B) => a∈FIRSTVT(A)
	//当产生式首位B为非终结符时，FIRSTVT(B)属于FIRSTVT(A)
	//执行到不能再增大
	int num = 0;
	do {
		num = 0;
		for (char x : vN) {
			for (string& right : GMap[x]) {
				if (vN.find(right[0]) != vN.end()) {
					for (char c : FIRSTVT[right[0]]) {
						if (FIRSTVT[x].insert(c).second) {
							++num; //插入成功说明FIRSTVT在增大
						}
					}
				}
			}
		}
	} while (num > 0);
}

/*展示FIRSTVT*/
void showFIRSTVT() {
	cout << "FIRSTVT：" << endl;
	for (auto iterator : FIRSTVT) {
		cout << "  FIRSTVT(" << iterator.first << ")={";
		for (auto it = iterator.second.begin(); it != iterator.second.end(); ) {
			string s = "";
			s += *it;
			cout << s;
			if (++it != iterator.second.end()) {
				cout << ",";
			}
		}
		cout << "}" << endl;
	}
	cout << endl;
}

/*求LASTVT集*/
void getLASTVT() {
	//初始化
	for (char x : vN) {
		set<char> value;
		LASTVT[x] = value;
	}
	//A->...a或A->...aB => a∈LASTVT(A)
	//产生式的最后一个终结符a属于LASTVT(A)
	for (char x : vN) {
		for (string& right : GMap[x]) {
			for (int i = right.size() - 1; i > -1; --i) {
				if (vT.find(right[i]) != vT.end()) {
					LASTVT[x].insert(right[i]);
					break; //只要最后一个
				}
			}
		}
	}
	//A->...B且a∈LASTVT(B) => a∈LASTVT(A)
	//当产生式末尾B为非终结符时，LASTVT(B)属于LASTVT(A)
	//执行到不能再增大
	int num = 0;
	do {
		num = 0;
		for (char x : vN) {
			for (string& right : GMap[x]) {
				char lastChar = right[right.size() - 1];
				if (vN.find(lastChar) != vN.end()) {
					for (char c : LASTVT[lastChar]) {
						if (LASTVT[x].insert(c).second) {
							++num;
						}
					}
				}
			}
		}
	} while (num > 0);
}

/*展示LASTVT*/
void showLASTVT() {
	cout << "LASTVT：" << endl;
	for (auto iterator : LASTVT) {
		cout << "  LASTVT(" << iterator.first << ")={";
		for (auto it = iterator.second.begin(); it != iterator.second.end(); ) {
			string s = "";
			s += *it;
			cout << s;
			if (++it != iterator.second.end()) {
				cout << ",";
			}
		}
		cout << "}" << endl;
	}
	cout << endl;
}

int main() {
	cout << "请逐行输入文法(输入#停止输入，$表示空串)" << endl;
	string str;
	while (cin >> str) {
		if (str == "#") break;
		//清除空格后放在G中
		G.push_back(removeSpaces(str));
	}
	cout << "\n";
	//符号分类和文法产生式转换为map
	handleGrammarForms();
	//求FIRSTVT集和LASTVT集
	getFIRSTVT();
	showFIRSTVT();
	getLASTVT();
	showLASTVT();
	//构建算符优先关系矩阵
	operatorFirstTable.createAnalysisTable();
	operatorFirstTable.showTable();
	//输入文法符号串
	cout << "输入字符串用于检查：(末尾加上#)\n";
	cin >> str;
	//判断是否符合文法
	operatorFirstTable.analyseInputString(str);
}