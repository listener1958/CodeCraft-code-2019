#include "road.h"



void road::carlistUpdate(bool isFirst,std::deque<car*>& list)
{
	if (0 == list.size())
		return;

	uint distemp = 0;     //前车行走距离
	carstate st = wait;   //前车状态
	uint sp = 0;  //当前车辆可行驶最大车速
	for (auto it=list.begin();it!=list.end();it++)
	{
		sp = this->speed > (*it)->speed ? (*it)->speed : this->speed;
		(*it)->distance += distemp;
		if ((!isFirst) && stop == (*it)->state)
		{
			for (auto it1 = it; it1 != list.end(); it1++)
			{
				(*it1)->state = stop;   //后边车辆状态均更新为停止
			}
			return;          //当同一时间片第二次更新时，已停止车辆不能前进
		}

		if ((*it)->distance >= sp)
		{
			(*it)->distance -= sp;
			(*it)->location += sp;
			(*it)->state = stop;
			distemp = sp;
		}
		else if (stop == st)
		{
			distemp = (*it)->distance;
			(*it)->location += distemp;
			(*it)->distance = 0;
			(*it)->state = stop;
		}
		else
		{
			(*it)->state = wait;
		}
		st = (*it)->state;
	}
}

int road::carInsert(car * incar, bool crossIsTo)
{
	//该道路驶入车辆 0：成功，1：失败，车等待状态，2：失败，车在前一道路可至终止状态（此路由于限速或停止状态车辆拥堵，不能进入）或无可进入道路

	std::deque<car*> *carlists = nullptr;
	uint s = incar->speed < this->speed ? incar->speed : this->speed;   //获取下一道路可行驶最大车速
	if (s <= incar->distance)
		return 2;
	s -= incar->distance;   //可在此道路行进距离

	if (!crossIsTo)
	{
		carlists = this->carlistsForw;
	}
	else if (this->isDuplex == true)
	{
		carlists = this->carlistsReve;
	}
	if (nullptr == carlists)
		return 2;

	for (int i = 0; i < this->channel; i++)
	{
		if (carlists[i].size() == 0)  //道路空
		{
			incar->distance = this->length - s;
			incar->location = s;
			incar->locid = this->id;  //修改车辆位置道路id
			incar->channel = i; if (crossIsTo)incar->channel = -incar->channel;  //修改车辆位置车道
			incar->state = stop;
			carlists[i].push_back(incar);
			return 0;
		}
		else if (s < carlists[i].back()->location)  //最后一辆车后有空位,且足够
		{
			incar->distance = carlists[i].back()->location - s - 1;
			incar->location = s;
			incar->locid = this->id;  //修改车辆位置道路id
			incar->channel = i; if (crossIsTo)incar->channel = -incar->channel;  //修改车辆位置车道
			incar->state = stop;
			carlists[i].push_back(incar);
			return 0;
		}
		else if (carlists[i].back()->state == stop)  //距末尾车不足够远且前车停止
		{
			if (carlists[i].back()->location > 1)  //前车后有空位
			{
				incar->distance = 0;
				incar->location = carlists[i].back()->location - 1;
				incar->locid = this->id;  //修改车辆位置道路id
				incar->channel = i; if (crossIsTo)incar->channel = -incar->channel;  //修改车辆位置车道
				incar->state = stop;
				carlists[i].push_back(incar);
				return 0;
			}
		}
		else   //距末尾车不足够远且前车等待
		{
			return 1;
		}
	}

	return 2;   //各车道均为满状态
}

void road::carStateUpdate(bool isForward, bool isFirst)
{
	if (isForward)
	{
		for (int i = 0; i < channel; i++)
		{
			carlistUpdate(isFirst, carlistsForw[i]);
		}
	}
	else
	{
		for (int i = 0; i < channel; i++)
		{
			carlistUpdate(isFirst, carlistsReve[i]);
		}
	}


}
