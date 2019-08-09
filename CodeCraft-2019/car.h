#ifndef __CAR_H_
#define __CAR_H_

#include "config.h"


enum carstate{wait,stop};      //运行状态（等待，终止）

class car
{
private:

public:
//#(id,from,to,speed,planTime)
	long id;
	long from;          //出发路口
	long to;            //目的路口
	uint speed;          //最高速度
	ulong plantime;      //计划出发时间

	ulong acttime;       //实际出发时间
	uint distance;       //与前车距离,表示该车可行进距离
	uint location;       //在道路上位置，进入道路后为该道路长度，行驶至道路最前端值为1
	carstate state;      //运行状态（等待，终止）
/***********************************用于死锁解锁*****************************************************************************/
	long locid;  //当前位置id  标记车辆位于哪条道路上或车辆位于路口尚未出库
	int channel;  //若已出库，当前道路车道  正表示前向车道，负表示反向车道
/***********************************用于死锁解锁******************************************************************************/
	long nextroad;      //下一路口
	std::vector<long> path;  //运行路径

public:
	car() {}
	car(long i_id, long i_from, long i_to, uint i_speed, ulong i_plantime) :
		id(i_id), from(i_from), to(i_to), speed(i_speed), plantime(i_plantime), acttime(0), distance(0), state(stop), nextroad(-1),locid(from) {};

	~car();

	int nextRoadCal();   //更新nextroad 变量  0：nextroad已是终点值-2  1：更新成功
};



#endif

