#pragma once
#include "Goods.h"

#include <string>
#include<fstream>
#include<iostream>
class Books : public Goods {
public:
	double getPrice() {
		return value * BooksOff;
	}
	Books(double val = 0, int qua = 0, int id = 0, std::string na = "", std::string intro = "") {
		seller = id; value = val; quantity = qua;
		name = na; introduction = intro;
		frozen = 0;
	}
	void PrintData() {
		std::cout << "��Ʒ����" << name << std::endl;
		std::cout << "���ܣ�" << introduction << std::endl;
		std::cout << "ʣ��������" << quantity << std::endl;
		std::cout << "�۸�" << getPrice() << std::endl;
		std::cout << "���ͣ��鱾" << std::endl;
	}
};

