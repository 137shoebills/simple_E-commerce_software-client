#pragma once

#include <string>
#include<fstream>
#include<iostream>
#include "MyAddition.h"
class User {
protected:
	std::string name;//����С�ڵ���20������ĸ�������
	std::string password;//���ȴ��ڵ���8С�ڵ���20
	int state;//1�����¼��0����δ��¼
	double money;
	int id;
public:
	virtual int getUserType()=0;
	bool ModifyPassword(std::string s) {
		if (s.size() >= 8 && s.size() <= 20) {
			password = s;
			std::cout << "�޸ĳɹ�,���빲" << s.size() << "λ" << std::endl;
			return 1;
		}else if (s.size() < 8) {
			std::cout << "�������,�޸�ʧ��" << std::endl;
			return 0;
		}else {
			std::cout << "�������,�޸�ʧ��" << std::endl;
			return 0;
		}
	}
	std::string getname() {
		return name;
	}
	bool login(std::string s) {
		if (s == password) {
			state = 1;
			std::cout << "��½�ɹ�" << std::endl;
			return 1;
		}
		else {
			std::cout << "����������ز���" << std::endl;
			return 0;
		}
	}
	void logout() {
		state = 0;
	}
	std::string outname() {
		return name;
	}
	void OutData(std::ofstream &ofs) {
		ofs << name<< std::endl;
		ofs << password << std::endl;
		ofs << money << std::endl;
		ofs << id << std::endl;
		ofs << std::endl;
	}
	double GetMon() { return money; }
	void InMon(double mon) { money += mon; }
};

