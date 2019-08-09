#ifndef __CROSS_H_
#define __CROSS_H_

#include "config.h"
#include "car.h"
#include "road.h"

#define PLANTIMEJUMP 10
extern int crossToRoadDistance;

struct crossroad     //路口道路信息
{
	long id;   //路口道路id
	road * roadincro;  //路口道路指针
	int isforward;   //该路口是否是该道路的到达路口（to）
	crossroad* opporoad;  //对面路口，对应直行
	crossroad* leftroad;  //左方，逆时针方向路口
	crossroad* righroad;  //右方，顺时针方向路口

	std::deque<car*> * getcarForw();   //获取该道路前向（驶向该路口方向）车辆列表
	std::deque<car*> * getcarReve();   //获取该道路逆向（驶离该路口方向）车辆列表
	std::deque<car*>* getFirstCar();   //获取该道路第一优先级车所在车道
	int croadInsert(std::deque<car*>* carlane, crossroad* targetroad);       //本路口某车道车辆驶入目标路口道路 0：无调度 1：有调度
	int croadUpdate();    //处理该道路过马路车辆 0：处理结束无等待车辆，1：有等待车辆
};

class cross  //路口信息
{
private:
	//void cross::updateRoad(crossroad* croad, std::deque<car*> *carlists);
	int carToRoad(int  vacancies);  //车辆出库 0：车库无车  1：上路失败  2：成功上路    输入：需前方道路有vacancies空位才进行出路口，即出路口后该车道仍至少有空位vacancies-1
public://#(id, roadId, roadId, roadId, roadId)
	long id;     //路口id
	//long roadid1;
	//road * road1;
	//long roadid2;
	//road * road2;
	//long roadid3;
	//road * road3;
	//long roadid4;
	//road * road4;

	crossroad* crossroad1;
	crossroad* crossroad2;
	crossroad* crossroad3;
	crossroad* crossroad4;

	std::vector<crossroad*> crossroads;   //按道路id升序排列的各路口道路，包括道路为空（id=-1）的道路
	std::vector<car*> carlist;    //该路口的待出发车辆

public:
	cross(){}
	cross(long i_id, long i_roadid1, long i_roadid2, long i_roadid3, long i_roadid4, road* i_road1, road* i_road2, road* i_road3, road* i_road4)
	//	:id(i_id), roadid1(i_roadid1), roadid2(i_roadid2), roadid3(i_roadid3), roadid4(i_roadid4), road1(i_road1), road2(i_road2), road3(i_road3), road4(i_road4) 
	{
		id = i_id;

		crossroad1 = nullptr;
		crossroad2 = nullptr;
		crossroad3 = nullptr;
		crossroad4 = nullptr;
		crossroad1 = new crossroad();
		crossroad2 = new crossroad();
		crossroad3 = new crossroad();
		crossroad4 = new crossroad();

		crossroads.reserve(4);
		crossroad1->id = i_roadid1;
		crossroad1->roadincro = i_road1;
		crossroad1->isforward = i_id == i_road1->to;
		crossroad1->leftroad = crossroad2;
		crossroad1->opporoad = crossroad3;
		crossroad1->righroad = crossroad4;
		crossroads.push_back(crossroad1);

		crossroad2->id = i_roadid2;
		crossroad2->roadincro = i_road2;
		crossroad2->isforward = i_id == i_road2->to;
		crossroad2->leftroad = crossroad3;
		crossroad2->opporoad = crossroad4;
		crossroad2->righroad = crossroad1;
		crossroads.push_back(crossroad2);

		crossroad3->id = i_roadid3;
		crossroad3->roadincro = i_road3;
		crossroad3->isforward = i_id == i_road3->to;
		crossroad3->leftroad = crossroad4;
		crossroad3->opporoad = crossroad1;
		crossroad3->righroad = crossroad2;
		crossroads.push_back(crossroad3);

		crossroad4->id = i_roadid4;
		crossroad4->roadincro = i_road4;
		crossroad4->isforward = i_id == i_road4->to;
		crossroad4->leftroad = crossroad1;
		crossroad4->opporoad = crossroad2;
		crossroad4->righroad = crossroad3;
		crossroads.push_back(crossroad4);

		std::sort(crossroads.begin(), crossroads.end(), [](crossroad* r1, crossroad* r2)
		{
			return r1->id < r2->id;
		});

	}

	~cross(){  //map会调用拷贝构造函数
		//if(nullptr!= crossroad1)
		//	delete crossroad1;
		//if (nullptr != crossroad2)
		//	delete crossroad2;
		//if (nullptr != crossroad3)
		//	delete crossroad3;
		//if (nullptr != crossroad4)
		//	delete crossroad4;
	}

	int updateCross();
};


#endif
