// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<cstdio>
#include <fstream>
#include <string>
#include<vector>
#include<WinSock2.h>
#include<Windows.h>
#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include "MyAddition.h"
#include "Seller.h"
#include "Buyer.h"
#include "Clothes.h"
#include "Foods.h"
#include "Books.h"
#include "ShoppingCart.h"
const int PORT = 8000;
#define MaxClient 10
#define MaxBufSize 1024

char SendBuf[MaxBufSize] = {}, RecvBuf[MaxBufSize]={};

typedef struct sendgoods {//用来传送商品数据的结构体
	double value;//价格
	int quantity;//存量
	int seller;
	char name[101];//名字
	char introduction[200];//介绍
}sendgoods;

typedef struct SendCart {//用来传送购物车的结构体
	int type;//商品种类
	int id;//商品在当前种类里的id
	int num;//数量
	char name[101];
}SendCart;

int idm;
int state=-1, idtype=0;
//state是当前状态，-1为未登录，其他数字为登录者的编号
//idtype为登陆者的类型，buyer为1，seller为0
std::vector < Buyer > buyer;//buyer idtype 1
std::vector < Seller > seller;//seller idtype 0
std::vector < Books > bo;
std::vector < Foods > fo;
std::vector < Clothes > clo;
std::vector < ShoppingCart > cart;
std::vector < ShoppingCart > oform;
double formval = 0;

int MyLogin(void* arg) {//登录
	SOCKET* ClientSocket = (SOCKET*)arg;
	std::string a;
	if (state != -1) {
		std::cout << "已登录，请勿重复登陆" << std::endl;
		return 1;
	}
	std::cout << "请输入用户名" << std::endl;
	a = GetS();
	int k = 0;
	k = send(*ClientSocket, a.c_str(), strlen(a.c_str())*sizeof(char)+1, 0);
	if (k <= 0) return 0;
	int f=0;
	k = recv(*ClientSocket, (char*)&f, sizeof(int), 0);
	if (k <= 0) return 0;
	if (f != -1) {
		std::cout << "请输入密码" << std::endl;
		a = GetS();
		k = send(*ClientSocket, a.c_str(), strlen(a.c_str()) * sizeof(char) + 1, 0);
		if (k <= 0)return 0;
		k = recv(*ClientSocket,(char*)&state, sizeof(int), 0);
		if (state == -1) {
			std::cout << "密码错误，登陆失败" << std::endl;
		}
		else {
			std::cout << "登陆成功" << std::endl;
		}
	}
	else {
		std::cout << "用户名不存在" << std::endl;
	}return 1;
}
void MySignUp(void *arg) {//注册
	SOCKET* ClientSocket = (SOCKET*)arg;
	std::string a, b;
	int k;
	for (;;) {
		std::cout << "请输入注册的用户名" << std::endl;
		a = GetS();
		int fff = 0;
		if (a.size() > 20) {
			std::cout << "用户名过长，请重新输入" << std::endl;
		}
		else{
			k = send(*ClientSocket, a.c_str(), strlen(a.c_str()) * sizeof(char) + 1, 0);
			if (k <= 0) return;
			k = recv(*ClientSocket, (char*)&fff, sizeof(int), 0);
			if (k <= 0) return;
			if (fff == 1)std::cout << "用户名已存在，请重新输入" << std::endl;
			else break;
		}
	}
	for (;;) {
		std::cout << "请输入注册的密码" << std::endl;
		b = GetS();
		if (b.size() < 8 || b.size() > 20) {
			std::cout << "密码过长或过短，请重新输入" << std::endl;
		}
		else break;
	}
	k = send(*ClientSocket, b.c_str(), strlen(b.c_str()) * sizeof(char) + 1, 0);
	if (k <= 0) return;
	int t = 0;
	for (;;) {
		std::cout << "请输入注册类型 1（买家）或0（商家）" << std::endl;
		t = GetInt();
		if (t != 0 && t != 1) {
			std::cout << "类型只能为1（买家）或0（商家）" << std::endl;
		}
		else break;
	}
	k = send(*ClientSocket, (char*)&t, sizeof(int), 0);
	if (k <= 0) return;
	
	if (t) {
		std::cout << "注册买家成功" << std::endl;
	}
	else {
		std::cout << "注册商家成功" << std::endl;
	}
}
bool MakeCP(std::string a,std::string b) {//在b字符串中查找子序列a
	int** f;
	f = new int *[100];
	for (int i = 0; i < 100; ++i) {
		f[i] = new int[100];
		for (int j = 0; j < 100; ++j)f[i][j] = 0;
	}
	std::string aa = '0'+ a;
	std::string bb = '0' + b;
	for (int i = 0;i<=b.size(); ++i) f[i][0] = 1;
	for (int i = 1;i<bb.size(); ++i) {
		for (int j = 1; j < aa.size(); ++j) {
			if(bb[i] == aa[j]&&f[i-1][j-1]) {
				f[i][j] = 1;
			}
			if (f[i-1][j])f[i][j] = 1;
		}
	}
	int xx = f[b.size()][a.size()];
	for (int i = 0; i < 100; ++i) {
		delete [] f[i];
	}
	delete [] f;
	return xx;
}
void ModPass(void *arg) {//修改密码
	SOCKET* ClientSocket = (SOCKET*)arg;
	if (state == -1) {
		std::cout << "未登录" << std::endl;
		return;
	}
	int k;
	int ff = 0;
	std::cout << "请输入修改的密码：" << std::endl;
	std::string s=GetS();
	k = send(*ClientSocket, s.c_str(), strlen(s.c_str()) * sizeof(char) + 1, 0);
	if (k <= 0) return;
	k = recv(*ClientSocket, (char*)&ff, sizeof(int), 0);
	if (k <= 0) return;
	if (ff) {
		std::cout << "修改成功" << std::endl;
	}
}
void LogOut() {
	if (state != -1) {
		if (idtype == 0) {
			seller[state].logout();
		}
		else {
			buyer[state].logout();
		}
	}
	state = -1;
	std::cout << "已退出登录" << std::endl;
}
void ChangeMoney(void *arg) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	if (state == -1) {
		std::cout << "未登录" << std::endl;
		return;
	}
	int k;
	if (idtype == 0) {
		double val=0;
		k = recv(*ClientSocket, (char*)&val, sizeof(double), 0);
		if (k <= 0) return;
		std::cout << "当前余额：" << val << std::endl;
		std::cout << "请输入你要充值的数额:" << std::endl;
		double zz;
		zz = GetDouble();
		k = send(*ClientSocket, (char*)&zz, sizeof(double), 0);
		if (k <= 0) return;
		std::cout << "充值完成" << std::endl;
	}
	else if (idtype == 1) {
		double val = 0;
		k = recv(*ClientSocket, (char*)&val, sizeof(double), 0);
		if (k <= 0) return;
		std::cout << "当前余额：" << val << std::endl;
		std::cout << "请输入你要充值的数额:" << std::endl;
		double zz;
		zz = GetDouble();
		k = send(*ClientSocket, (char*)&zz, sizeof(double), 0);
		if (k <= 0) return;
		std::cout << "充值完成" << std::endl;
	}
}
sendgoods GetSendGoods(double val, int qua,int sel, std::string name, std::string intro) {//将信息转化成sendgoods

	sendgoods aa; 
	aa.value = val; aa.quantity = qua;
	aa.seller = sel;
	for (int i = 0; i < name.size(); ++i) {
		aa.name[i] = name[i];
	}aa.name[name.size()] = '\0';
	for (int i = 0; i < intro.size(); ++i) {
		aa.introduction[i] = intro[i];
	}aa.introduction[intro.size()] = '\0';
	return aa;
}
void AddGoods(void *arg) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	if (state < 0) {
		std::cout << "请登录" << std::endl;
		return;
	}
	if (idtype != 0) {
		std::cout << "你不是商家" << std::endl;
		return;
	}
	int tt, qua;
	double val;
	std::string name, intro;
	for (;;) {
		std::cout << "请输入商品种类：书本为1，衣物为2，食品为3" << std::endl;
		tt = GetInt();
		if (tt == 1 || tt == 2 || tt == 3)break;
	}
	int k;
	k = send(*ClientSocket, (char*)&tt, sizeof(int), 0);
	if (k <= 0) return;
	std::cout << "请输入商品价格(小数)" << std::endl;
	val = GetDouble();
	std::cout << "请输入商品数量(整数)" << std::endl;
	qua = GetInt();
	std::cout << "请输入商品名称" << std::endl;
	name = GetS();
	std::cout << "请输入商品介绍" << std::endl;
	intro = GetS();
	sendgoods aa = GetSendGoods(val,qua,state,name,intro);
	if (tt == 1) {
		k = send(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0) return;
	}
	else if (tt == 2) {
		k = send(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0) return;
	}
	else {
		k = send(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0) return;
	}
	std::cout << "添加完成" << std::endl;
}
void PutInGoods(void* arg) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	bo.clear();
	clo.clear();
	fo.clear();
	int k, tt=0;
	k = recv(*ClientSocket, (char*)&tt, sizeof(int), 0);
	if (k <= 0) return;
	//std::cout << tt << std::endl;
	for (int i = 0; i < tt; ++i) {
		sendgoods aa;
		k = recv(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0) return;
		Books zz(aa.value, aa.quantity, aa.seller, aa.name, aa.introduction);
		bo.push_back(zz);
	}
	k = recv(*ClientSocket, (char*)&tt, sizeof(int), 0);
	for (int i = 0; i < tt; ++i) {
		sendgoods aa;
		k = recv(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0) return;
		Clothes zz(aa.value, aa.quantity, aa.seller, aa.name, aa.introduction);
		clo.push_back(zz);
	}
	k = recv(*ClientSocket, (char*)&tt, sizeof(int), 0);
	for (int i = 0; i < tt; ++i) {
		sendgoods aa;
		k = recv(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0) return;
		Foods zz(aa.value, aa.quantity, aa.seller, aa.name, aa.introduction);
		fo.push_back(zz);
	}
}
void ShowGoods(void *arg) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(arg);
	for (int i = 0; i < bo.size(); ++i) {
		bo[i].PrintData();
		std::cout << std::endl;
	}for (int i = 0; i < clo.size(); ++i) {
		clo[i].PrintData();
		std::cout << std::endl;
	}for (int i = 0; i < fo.size(); ++i) {
		fo[i].PrintData();
		std::cout << std::endl;
	}
	std::cout << "以上为全部商品" << std::endl;
}
void SearchGoods(void *arg) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(arg);
	std::string a;
	std::cout << "请输入所查询的名字" << std::endl;
	a = GetS();
	int totgoods=0;
	for (int i = 0; i < bo.size(); ++i) {
		if (MakeCP(a, bo[i].BackName())) {
			bo[i].PrintData();
			++totgoods;
			std::cout << std::endl;
		}
	}for (int i = 0; i < clo.size(); ++i) {
		if (MakeCP(a, clo[i].BackName())) {
			clo[i].PrintData();
			++totgoods;
			std::cout << std::endl;
		}
	}for (int i = 0; i < fo.size(); ++i) {
		if (MakeCP(a, fo[i].BackName())) {
			fo[i].PrintData();
			++totgoods;
			std::cout << std::endl;
		}
	}
	std::cout << "共查询到" << totgoods << "个商品" << std::endl;
	std::cout << "查询完毕" << std::endl;
}
void ChangeGoods(void* arg) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(arg);
	if (state < 0) {
		std::cout << "请登录" << std::endl;
		return;
	}
	if (idtype != 0) {
		std::cout << "你不是商家,不能更改商品信息" << std::endl;
		return;
	}
	int tt, qua, f = 0;
	double val;
	std::string name, intro, a;
	for (;;) {
		std::cout << "请输入商品种类：书本为1，衣物为2，食品为3" << std::endl;
		tt = GetInt();
		if (tt == 1 || tt == 2 || tt == 3)break;
	}
	std::cout << "请输入所更改货品的名字:" << std::endl;
	a = GetS();
	std::cout << "请输入货品更改后的价格:" << std::endl;
	val = GetDouble();
	std::cout << "请输入货品更改后的数量:" << std::endl;
	qua=GetInt();
	int k = 0;
	k = send(*ClientSocket, (char*)&tt, sizeof(tt), 0);
	if (k <= 0) return;
	k = send(*ClientSocket, a.c_str(), strlen(a.c_str()) * sizeof(char) + 1, 0);
	if (k <= 0) return;
	k = send(*ClientSocket, (char*)&val, sizeof(val), 0);
	if (k <= 0) return;
	k = send(*ClientSocket, (char*)&qua, sizeof(qua), 0);
	if (k <= 0) return;
	if (tt == 1) {
		for (int i = 0; i < bo.size(); ++i) {
			if (a == bo[i].BackName()) {
				f = 1;
				if (bo[i].BackSeller() == state) {
					bo[i].ChangeVal(val);
					bo[i].ChangeQua(qua); 
					std::cout << "已更新" << std::endl;
				}
				else std::cout << "不具备权限" << std::endl;
			}
		}
	}
	if (tt == 2) {
		for (int i = 0; i < clo.size(); ++i) {
			if (a == clo[i].BackName()) {
				f = 1;
				if (clo[i].BackSeller() == state) {
					clo[i].ChangeVal(val);
					clo[i].ChangeQua(qua);
					std::cout << "已更新" << std::endl;
				}
				else std::cout << "不具备权限" << std::endl;
			}
		}
	}
	if (tt == 3) {
		for (int i = 0; i < fo.size(); ++i) {
			if (a == fo[i].BackName()) {
				f = 1;
				if (fo[i].BackSeller() == state) {
					fo[i].ChangeVal(val);
					fo[i].ChangeQua(qua);
					std::cout << "已更新" << std::endl;
				}
				else std::cout << "不具备权限" << std::endl;
			}
		}
	}
	if (!f) std::cout << "名字查不到" << std::endl;
	else {
		std::cout << "更改完成" << std::endl;
	}
}
void InterFace() {
	std::string ch;
	std::ifstream InterFile("InterFace.txt");
	while (getline(InterFile, ch)) {
		std::cout << ch << std::endl;
	}
	std::cout << " 又鸽了咕 电商交易平台" << std::endl;
	std::cout << std::endl;
	std::cout << "Design by: 杨澜" << std::endl;
	std::cout << std::endl;
	std::cout << "输入 /help 查看命令行操作方式" << std::endl;
	std::cout << std::endl;
}
void InsertCart(std::string name, int tp, int id, int num) {//加入购物车的具体操作
	int ff = 0;
	std::vector<ShoppingCart>::iterator itr = cart.begin();
	for (int i = 0;i<cart.size(); ++i,++itr) {
		if (cart[i].name==name) {
			ff = 1;
			cart[i].num += num;
			int fff = 0;
			if (tp == 1) {
				if (cart[i].num <= 0) {
					fff = -1;
				}
				else if (cart[i].num > bo[id].BackQua()) {
					cart[i].num = bo[id].BackQua();
					fff = 1;
				}
			}
			if (tp == 2) {
				if (cart[i].num <= 0) {
					fff = -1;
				}
				else if (cart[i].num > clo[id].BackQua()) {
					cart[i].num = clo[id].BackQua();
					fff = 1;
				}
			}
			if (tp == 3) {
				if (cart[i].num <= 0) {
					fff = -1;
				}
				else if (cart[i].num > fo[id].BackQua()) {
					cart[i].num = fo[id].BackQua();
					fff = 1;
				}
			}
			if (fff == 1) {
				std::cout << "购买数量大于存量，当前购买数量更新为物品存量" << std::endl;
			}
			else if (fff == -1) {
				itr=cart.erase(itr);
				std::cout << "购买数量小于0，物品从购物车删除" << std::endl;
			}
			break;
		}
	}if (!ff) {
		if (num > 0) {
			int fff = 0;
			if (tp == 1) {
				if (num > bo[id].BackQua()) {
					num = bo[id].BackQua();
					fff = 1;
				}
			}
			if (tp == 2) {
				if (num > clo[id].BackQua()) {
					num = clo[id].BackQua();
					fff = 1;
				}
			}
			if (tp == 3) {
				if (num > fo[id].BackQua()) {
					num = fo[id].BackQua();
					fff = 1;
				}
			}
			if (fff == 1) {
				std::cout << "购买数量大于存量，当前购买数量更新为物品存量" << std::endl;
			}
			ShoppingCart a(name, tp, id, num);
			cart.push_back(a);
		}
		else {
			std::cout << "商品未存在购物车中" << std::endl;
		}
	}
}
void AddCart(void *arg) {//添加入购物车
	if (state == -1) {
		std::cout << "未登录" << std::endl;
		return;
	}
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(ClientSocket);
	for (;;) {
		int goodst = 0;
		std::cout << "请输入加入购物车的商品类型：书本为1，衣物为2，食品为3，退出当前模式为4。" << std::endl;
		goodst = GetInt();
		while (goodst != 1 && goodst != 2 && goodst != 3&&goodst!=4) {
			std::cout << "请重新输入商品类型：书本为1，衣物为2，食品为3，退出当前模式为4。" << std::endl;
			goodst = GetInt();
		}
		if (goodst == 4)break;
		std::cout << "请输入加入购物车的商品名称" << std::endl;
		std::string a;
		a = GetS();
		std::cout << "请输入加入购物车的商品数量" << std::endl;
		int num;
		num = GetInt();
		int ff = 0;
		if(goodst==1){
			for (int i = 0; i < bo.size(); ++i) {
				if (a == bo[i].BackName()) {
					ff = 1;
					InsertCart(bo[i].BackName(),goodst,i,num);
				}
			}
		}
		if (goodst == 2) {
			for (int i = 0; i < clo.size(); ++i) {
				if (a == clo[i].BackName()) {
					ff = 1;
					InsertCart(clo[i].BackName(), goodst, i, num);
				}
			}
		}
		if (goodst == 3) {
			for (int i = 0; i < fo.size(); ++i) {
				if (a == fo[i].BackName()) {
					ff = 1;
					InsertCart(fo[i].BackName(), goodst, i, num);
				}
			}
		}
		if (!ff) {
			std::cout << "找不到该名称的商品" << std::endl;
		}
	}
}
void ShowCart(void *arg) {
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(ClientSocket);
	for (int i = 0; i < cart.size(); ++i) {
		std::cout << "购买数量：" << cart[i].num << std::endl;
		if (cart[i].type == 1) {
			bo[cart[i].id].PrintData();
			std::cout << std::endl;
		}
		else if (cart[i].type == 2) {
			clo[cart[i].id].PrintData();
			std::cout << std::endl;
		}
		else {
			fo[cart[i].id].PrintData();
			std::cout << std::endl;
		}
	}
	if (cart.size() == 0)std::cout << "购物车为空" << std::endl;
}
void MakeForm(void *arg) {//将购物车转化为订单
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(ClientSocket);
	if (state == -1) {
		std::cout << "未登录" << std::endl;
		return;
	}
	int k = 0;
	int ff = 0;
	if (oform.size() != 0) {
		ff = 1;
		k = send(*ClientSocket, (char*)&ff, sizeof(ff), 0);
		if (k <= 0)return;
	}
	else {
		k = send(*ClientSocket, (char*)&ff, sizeof(ff), 0);
		if (k <= 0)return;
		int n=cart.size();
		k = send(*ClientSocket, (char*)&n, sizeof(n), 0);
		if (k <= 0)return;
		formval = 0; 
		for (int i = 0; i < n; ++i) {
			SendCart aa;
			aa.id = cart[i].id;
			aa.num = cart[i].num;
			aa.type = cart[i].type;
			for (int j = 0; j < cart[i].name.size(); ++j)aa.name[j] = cart[i].name[j];
			aa.name[cart[i].name.size()] = '\0';
			k = send(*ClientSocket, (char*)&aa, sizeof(aa), 0);
			if (k <= 0)return;
		}
		for (int i = 0; i < cart.size(); ++i) {
			if (cart[i].type == 1) {
				formval += bo[cart[i].id].getPrice() * cart[i].num;
			}
			else if (cart[i].type == 2) {
				formval += clo[cart[i].id].getPrice() * cart[i].num;
			}
			else {
				formval += fo[cart[i].id].getPrice() * cart[i].num;
			}
		}
		int fff = 0;
		k = recv(*ClientSocket, (char*)&fff, sizeof(fff), 0);
		if (k <= 0)return;
		if (fff == 1)ff = 1;
	}
	if (ff == 1) { 
		std::cout << "生成订单失败， 当前有支付未完成 或 购物车中存在商品被冻结 。" << std::endl;
		return; 
	}
	oform = cart;
	cart.clear();
	std::cout << "生成订单成功" << std::endl;
	std::cout << "订单需支付" << formval <<"元"<< std::endl;
}
void PayForm(void* arg) {//支付订单
	SOCKET* ClientSocket = (SOCKET*)arg;
	PutInGoods(ClientSocket);
	int k =0;
	if (state == -1) {
		std::cout << "未登录" << std::endl;
		return;
	}
	int n=oform.size();
	k = send(*ClientSocket, (char*)&n, sizeof(n), 0);
	if (k <= 0)return;

	if ( n==0 ) {
		std::cout << "当前无订单" << std::endl;
		return;
	}
	k = send(*ClientSocket, (char*)&formval, sizeof(formval), 0);
	if (k <= 0)return;
	double mon=0;
	if (idtype == 0) {
		k = recv(*ClientSocket, (char*)&mon, sizeof(mon), 0);
		if (k <= 0)return;
		if (mon < formval) {
			std::cout << "余额不足，支付失败" << std::endl;
			return;
		}
	}
	else if (idtype == 1) {
		k = recv(*ClientSocket, (char*)&mon, sizeof(mon), 0);
		if (k <= 0)return;
		if (mon < formval) {
			std::cout << "余额不足，支付失败" << std::endl;
			return;
		}
	}
	for (int i = 0; i < n; ++i) {
		SendCart aa;
		aa.id = oform[i].id;
		aa.num = oform[i].num;
		aa.type = oform[i].type;
		for (int j = 0; j < oform[i].name.size(); ++j)aa.name[j] = oform[i].name[j];
		aa.name[oform[i].name.size()] = '\0';
		std::cout << aa.name << std::endl;
		k = send(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0)return;
	}
	formval = 0;
	std::cout << "支付完成" << std::endl;
}
void ClearForm(void* arg) {//清除订单
	SOCKET* ClientSocket = (SOCKET*)arg;
	int n = oform.size(),k=0;
	k = send(*ClientSocket, (char*)&n, sizeof(n), 0);
	if (k <= 0)return;
	for (int i = 0; i < n; ++i) {
		SendCart aa;
		aa.id = oform[i].id;
		aa.num = oform[i].num;
		aa.type = oform[i].type;
		for (int j = 0; j < oform[i].name.size(); ++j)aa.name[j] = oform[i].name[j];
		aa.name[oform[i].name.size()] = '\0';
		std::cout << aa.name << std::endl;
		k = send(*ClientSocket, (char*)&aa, sizeof(aa), 0);
		if (k <= 0)return;
	}
	formval = 0;
	std::cout << "订单清除完成" << std::endl;
}
int SendCmd(void * arg,std::string cmd) {//向服务器发送命令行
	SOCKET* ClientSocket = (SOCKET*)arg;
	int k = send(*ClientSocket, cmd.c_str(), strlen(cmd.c_str()) * sizeof(char) + 1, 0);
	if (k < 0) {
		std::cout << "向服务器发送消息失败！" << std::endl;
		return 0;
	}
	return 1;
}
void GetUsersFromServer(void* arg) {
	SOCKET* ClientSocket = (SOCKET*)arg;

}
int main() {
	InterFace();
	std::cout << "-------------客户端--------------" << std::endl;
	//初始化
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	//创建客户端套接字
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSocket == SOCKET_ERROR) {
		std::cout << "套接字创建失败！" << std::endl;
		return 0;
	}
	else { std::cout << "套接字创建成功！" << std::endl; }
	//绑定套接字
	sockaddr_in socketAddr;
	memset(&socketAddr,0,sizeof(SOCKADDR));
	socketAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &socketAddr.sin_addr.S_un.S_addr);
	socketAddr.sin_port = htons(PORT);
	//服务器监听
	int k = connect(ClientSocket, (SOCKADDR*)&socketAddr, sizeof(SOCKADDR));
	if (k == SOCKET_ERROR) {
		std::cout << "连接失败！" << std::endl;
		return 0;
	}
	else {
		std::cout << "连接成功！" << std::endl;
	}
	for(;;) {
		std::cout << "请输入命令进行操作，输入/help查看命令介绍" << std::endl;
		std::string cmd;
		cmd = GetS();
		if (cmd == "/login") {//登录
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			//Sleep(500);
			//k = recv(ClientSocket, RecvBuf, sizeof(RecvBuf), 0);
			k=MyLogin(&ClientSocket);
			if(!k)return 0;
		}
		else if (cmd == "/modpass") {//修改密码
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			ModPass(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/signup") {//注册
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			MySignUp(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/logout") {//登出
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			cart.clear();
			ClearForm(&ClientSocket);
			LogOut();
			std::cout << std::endl;
		}
		else if (cmd == "/money") {//查看当前钱数和充值
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			ChangeMoney(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/addgoods") {//添加商品
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			AddGoods(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/show") {//展示所有商品
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			ShowGoods(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/search") {//查找商品
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			SearchGoods(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/changegoods") {//改变商品数据
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			ChangeGoods(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/addincart") {
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			AddCart(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/showcart") {
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			ShowCart(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/makeform") {
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			MakeForm(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/payform") {
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			PayForm(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/clearform") {
			k = SendCmd(&ClientSocket, cmd);
			if (!k)break;
			ClearForm(&ClientSocket);
			std::cout << std::endl;
		}
		else if (cmd == "/help") {
			std::cout << "/login 是登录" << std::endl;
			std::cout << "/signup 是注册" << std::endl;
			std::cout << "/modpass 是修改密码" << std::endl;
			std::cout << "/logout 是登出" << std::endl;
			std::cout << "/search 是查找商品" << std::endl;
			std::cout << "/show 是展示所有商品" << std::endl;
			std::cout << "/changegoods 是商户改变一个商品数据" << std::endl;
			std::cout << "/addgoods 是商户增加商品" << std::endl;
			std::cout << "/money 是显示当前账号存款和更改存款" << std::endl;
			std::cout << "/addincart 在购物车中添加商品" << std::endl;
			std::cout << "/showcart 展示购物车中商品" << std::endl;
			std::cout << "/makeform 将购物车转化为订单" << std::endl;
			std::cout << "/payform 给订单付款" << std::endl;
			std::cout << "/clearform 取消订单" << std::endl;
			std::cout << std::endl;
		}
		else {
			std::cout << "命令不能识别" << std::endl;
			std::cout << "输入/help查看命令介绍" << std::endl;
			std::cout << std::endl;
		}
	}
	std::cout << "与服务器连接中止" << std::endl;
	closesocket(ClientSocket);
	WSACleanup();
	system("pause");
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
