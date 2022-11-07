# OperatorFirstAnalysis-test
编译原理实验-算符优先分析

#### 实验要求
一个交互式面向对象的算符优先分析程序基本功能是：<br>
（1）	输入文法规则<br>
（2）	对文法进行转换<br>
（3）	生成每个非终结符的FirstVT和LastVT<br>
（4）	生成算符优先分析表<br>
（5）	再输入文法符号串<br>
（6）	生成移进规约步骤<br>

#### 测试用例
E->E+T<br>
E->T<br>
T->T*F<br>
T->F<br>
F->(E)<br>
F->i<br>
