#include "config.h"
#include "car.h"
#include "cross.h"
#include "road.h"

#define FLT_MAX          3.402823466e+38F        // max value

struct crossPoint {     //邻接表法最短路径，节点路径、与源点距离、是否计算标志
	long path;        //节点路径
	long roadpath;    //道路路径
	float dis;        //与源点距离
	bool mark;        //是否作为生长点标志
};

extern std::vector<car> carlist;
extern std::map<long, cross> crosslist;
std::map<long, crossPoint> cPoint;

void pathinit()
{
	for(auto cl:crosslist)
	{
		cPoint[cl.first] = crossPoint();
	}
}

void carPathInit()
{
	uint speed;     //待规划路径车辆速度
	long start;     //待规划路径车辆起点
	car* carptr;    //当前计算路径车辆
	long crosspath; //车辆路口路径
	long roadpath;
	std::vector<car*> carlistptr;  //车辆列表
	std::queue<car*> cartemp;   //一起进行路径计算车辆
	std::stack<long> carpath;   //车辆路径中间变量

	for (auto c = carlist.begin(); c != carlist.end(); c++)
	{
		carlistptr.push_back(&(*c));
	}
	//排序优先级 from  speed  id
	std::sort(carlistptr.begin(), carlistptr.end(), [](car* c1, car* c2) {
		if (c1->from != c2->from)
			return c1->from < c2->from;
		else if (c1->speed != c2->speed)
			return c1->speed > c2->speed;
		else
			return c1->id < c2->id;
	});

	speed = carlistptr[0]->speed;
	start = carlistptr[0]->from;

	for(auto c:carlistptr)
	{
		if (c->from == start && c->speed == speed)
		{
			cartemp.push(c);
		}
		else
		{
			shortpath_dijkstra(speed, start);
			while (!cartemp.empty())
			{
				carptr = cartemp.front();
				cartemp.pop();

				crosspath = carptr->to;
				roadpath = cPoint[crosspath].roadpath;
				while (roadpath != -1)   //车辆道路路径入栈
				{
					carpath.push(roadpath);
					crosspath = cPoint[crosspath].path;
					roadpath = cPoint[crosspath].roadpath;
				}
				while (!carpath.empty())   //车辆道路路径出栈
				{
					carptr->path.push_back(carpath.top());
					carpath.pop();
				}
			}
			speed = c->speed;
			start = c->from;
			cartemp.push(c);
		}
	}
	//处理最后一波车辆路径
	shortpath_dijkstra(speed, start);
	while (!cartemp.empty())
	{
		carptr = cartemp.front();
		cartemp.pop();

		crosspath = carptr->to;
		roadpath = cPoint[crosspath].roadpath;
		while (roadpath != -1)   //车辆道路路径入栈
		{
			carpath.push(roadpath);
			crosspath = cPoint[crosspath].path;
			roadpath = cPoint[crosspath].roadpath;
		}
		while (!carpath.empty())   //车辆道路路径出栈
		{
			carptr->path.push_back(carpath.top());
			carpath.pop();
		}
	}
}

void shortpath_dijkstra(int speed, long s)     //n顶点数，s源点号 
{
	long nextcrossid;
	long minpath;   //最短路径对应节点
	float mindis;   //当前最短路径
	float distime;
	crossroad *croad;

	for (auto cl = cPoint.begin(); cl != cPoint.end(); cl++)   //设置待定路径初值 
	{
		cl->second.dis = FLT_MAX;
		cl->second.mark = false;
		cl->second.path = crosslist[s].id;
		cl->second.roadpath = -1;
	}
	cPoint[s].dis = 0; cPoint[s].mark = 1;  //设置s为源点 
	minpath = s;
	mindis = 0;
	while (mindis < FLT_MAX)
	{
		cPoint[s].mark = 1;
		for (int i = 0; i < 4; i++)  //初始化与初始节点s相邻的节点最短路径
		{
			croad = crosslist[s].crossroads[i];
			if (-1 != croad->id)   //存在此道路
			{
				if (1 == croad->roadincro->isDuplex ||
					0 == croad->isforward)             //存在A->B的有向路
				{
					if (croad->isforward)              //取B节点处路口id
						nextcrossid = croad->roadincro->from;
					else
						nextcrossid = croad->roadincro->to;
					if (false == cPoint[nextcrossid].mark)
					{
						distime = croad->roadincro->speed > speed ? speed : croad->roadincro->speed;   //取道路与车辆较小速度
						distime = (float)croad->roadincro->length / distime;    //计算道路行驶时长，即cost
						distime = cPoint[s].dis + distime;    //计算若A->B 累积cost
						if (cPoint[nextcrossid].dis > distime)
						{
							cPoint[nextcrossid].dis = distime;  //更新最小路径长
							cPoint[nextcrossid].path = s;
							cPoint[nextcrossid].roadpath = croad->id;
						}

					}
				}
			}
		}

		mindis = FLT_MAX;
		for(auto cl:cPoint)   //设置待定路径初值 
		{
			if (mindis > cl.second.dis && cl.second.mark == false)   //从mark为0的节点中，查找距源点最近的点，获取新的生长点
			{
				mindis = cl.second.dis;
				minpath = cl.first;
			}
		}
		s = minpath;  //更新最新生长点
	}
}
