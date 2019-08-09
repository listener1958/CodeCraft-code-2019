#ifndef __ROAD_H_
#define __ROAD_H_

#include "config.h"
#include "car.h"

//enum uord { Unidir, dual };  //车道数  单向，双向

class road
{
private:
	void carlistUpdate(bool isFirst, std::deque<car*> &list);

public:
	//(id, length, speed, channel, from, to, isDuplex)
	long id;
	uint length;    //长度
	uint speed;     //限速
	uint channel;   //车道数
	long from;      //起点路口id
	//cross& fromptr; //起点路口
	long to;        //终点路口id
	//cross& toptr;   //终点路口
	bool isDuplex;  //是否双向
	std::deque<car*> *carlistsForw;  //正向行驶，路上车辆列表,自from至to
	std::deque<car*> *carlistsReve;  //正向行驶，路上车辆列表，若单向道，则为空 自to至from
	//car* firstcar;  //该道路首辆车

public:
	road() {
		carlistsForw = nullptr;
		carlistsReve = nullptr;
	}
	road(long i_id,uint i_length,uint i_speed,uint i_channel,long i_from,long i_to,bool i_isDuplex):
		 id(i_id),  length(i_length), speed(i_speed),  channel(i_channel),  from(i_from), to(i_to), isDuplex(i_isDuplex)
	{
		carlistsForw = nullptr;
		carlistsReve = nullptr;

		carlistsForw = new std::deque<car*>[i_channel];
		if(isDuplex)
			carlistsReve = new std::deque<car*>[i_channel];

	}
	~road() {
		/*if (nullptr != carlistsForw)
			delete[]carlistsForw;
		if (nullptr != carlistsReve)
			delete[]carlistsReve;
			
		carlistsForw = nullptr;
		carlistsReve = nullptr;*/
	}

	int carInsert(car * incar,bool crossIsTo);       //该道路驶入车辆 0：成功，1：失败，车等待状态，2：失败，车在前一道路可至终止状态（此路由于限速或停止状态车辆拥堵，不能进入）
	void carStateUpdate(bool isForward, bool isFirst); //更新道路车辆状态isForward:此次为前向道路车辆更新 isFirst：是否一时间片内首次调用
};


#endif
