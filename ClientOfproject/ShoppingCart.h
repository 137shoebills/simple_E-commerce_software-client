#pragma once
#include <string>
class ShoppingCart{
public:
	int type;//��Ʒ����
	int id;//��Ʒ�ڵ�ǰ�������id
	int num;//����
	std::string name;
	ShoppingCart(std::string na="",int tt = 0, int ii = 0, int nn = 0) {
		name = na;
		type = tt;
		id = ii;
		num = nn;
	}
};

