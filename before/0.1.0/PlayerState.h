#ifndef _PLAYERSTATE_H_
#define _PLAYERSTATE_H_
#include<iostream>
class PlayerState
{
public:
	unsigned int  userid;
	std::string username;
	float _Playpos_x;//x
	float _Playpos_y;//高度
	float _Playpos_z;
	float _Playpos_v;//方向

};

#endif
