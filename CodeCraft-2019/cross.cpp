#include "cross.h"

int cross::carToRoad(int  vacancies)
{
	//0：车库无车  1：上路失败  2：成功上路    输入：需前方道路有vacancies空位才进行出路口
	
	if (this->carlist.empty())
		return 0;   //车库无车

	car* c = this->carlist.back();
	if (c->plantime > runTimeCount)
		return 1;  //未到上路时间
	//if (c->from == c->locid)
	if (c->nextroad == -1)
		c->nextRoadCal();

	std::vector<crossroad*>::iterator it=std::find_if(this->crossroads.begin(),this->crossroads.end(),[c](crossroad* cc){    //查找车辆道路
		return cc->id == c->nextroad;
	});
	if (it == this->crossroads.end())
	{
		std::cout << "出库车辆道路id有误！！！" << std::endl;
		return 0;
	}

	if (vacancies < 0)vacancies = c->speed;  //为负则前方至少有车速个空位，即可最大速行驶
	else if (vacancies < 1)vacancies = 1;  //至少有一个空位才可能出库成功
	auto cl = (*it)->getcarReve();
	int speed = (*it)->roadincro->speed > c->speed ? c->speed : (*it)->roadincro->speed;
	if (nullptr == cl)
	{
		std::cout << "出库车辆道路id有误！！！" << std::endl;
		return 0;
	}
	for (int i = 0; i < (*it)->roadincro->channel; i++)
	{
		if (cl[i].empty() ||   //车道空
			cl[i].back()->location > speed ||         //车道空位充足
			(cl[i].back()->location > 1 && cl[i].back()->state == stop))   //车道空位不足但前车停止状态
		{   //若出库，车辆将进入此道路
			if (cl[i].empty() || cl[i].back()->location > vacancies)  //道路空位数满足要求
			{
				int temp = (*it)->roadincro->carInsert(c, (*it)->isforward);
				if (temp == 0)  //上路成功
				{
					this->carlist.pop_back();  //车辆出库
					if (c->nextroad == (*it)->id)
						c->nextRoadCal();   //上路成功，更新下一道路
					c->acttime = runTimeCount;
					return 2;
				}
			}
			else
			{
				c->plantime = runTimeCount + PLANTIMEJUMP;
				return 1;
			}
		}
	}

	return 1;
}

int cross::updateCross()
{
	//假设每辆车nextroad已知

	//返回0：路口各道路与路口车库无车辆需调度  1：所有车已停止状态  2：未调度车辆，仍有等待车辆  3：有调度车辆

	//循环调用路口每条道路，返回0：各车道无车辆  1：所有车已停止状态  2：无调度，但有等待   3：有调度
	//循环终止条件：所有道路 0/1/2
	//循环终止后，若为0/1考虑调度路口车库车辆
	int result = -1;
	int resulttemp = -1;
	int temp;

	while (1)
	{
		resulttemp = -1;
		for (int i = 0; i < 4; i++)
		{
			if (-1 != crossroads[i]->id)
			{
				temp = crossroads[i]->croadUpdate();
				if (temp > result)
					result = temp;
				if (temp > resulttemp)
					resulttemp = temp;
					
			}
		}
		if (resulttemp < 3)
			break;   //无车辆调度，停止循环
	}
	
	if (result > 1)
		return result;

	//车库车辆上道 0：车库无车  1：上路失败  2：成功上路
	temp = 2;
	while (temp == 2)
	{
		temp = carToRoad(crossToRoadDistance);
	}

	if (0 == temp && 0 == result)
		return 0;
	else
		return 1;
}

std::deque<car*>* crossroad::getcarForw()  //获取道路驶向该路口方向道路
{
	if (-1 == this->id)
		return nullptr;

	if (this->isforward)   //该路口位于该道路前向出口
		return this->roadincro->carlistsForw;  //必有此路
	else if (roadincro->isDuplex == 1)   //该路口位于该道路反向入口
		return this->roadincro->carlistsReve;  //需双向道路才会有此路

	return nullptr;
}

std::deque<car*>* crossroad::getcarReve()
{
	if (-1 == this->id)
		return nullptr;

	if (this->isforward)   //该路口位于该道路前向出口
	{
		if(roadincro->isDuplex == 1)
			return this->roadincro->carlistsReve;  //需双向道路才会有此路
	}
	else                                  //该路口位于该道路反向入口
		return this->roadincro->carlistsForw;

	return nullptr;
}

std::deque<car*>* crossroad::getFirstCar()   //返回该道路第一优先级车辆所在车道
{
	//返回：nullptr：道路无车  车道第一辆为停止：该道路有车但均停止状态  否则：车道第一辆为第一优先级车辆

	//取各车道最前方车辆
	//距道路末端最短为第一优先级
	//若相同，车道id小为第一优先级

	std::deque<car*>* carlist = getcarForw();
	if (nullptr == carlist)
		return nullptr;   //无此道路
	uint distance1 = UINT16_MAX;
	std::deque<car*>* result = nullptr;

	for (int i = 0; i < roadincro->channel; i++)
	{
		if (carlist[i].size() > 0)    //车道有车
		{
			if (carlist[i][0]->state == wait)   //车道第一辆车为等待状态
			{
				if (carlist[i][0]->distance < distance1)  //距终点距离较少
				{
					distance1 = carlist[i][0]->distance;
					result = &carlist[i];
				}
			}
			else if (nullptr == result)   //车道第一辆车为停止状态且未识别到优先车辆
			{
				result = &carlist[i];
			}
		}
	}

	return result;
}

int crossroad::croadInsert(std::deque<car*>* carlane, crossroad * targetroad)
{
	//本路口某车道车辆驶入目标路口道路 0：无调度 1：有调度

	int temp;
	uint distemp;  //过马路车辆与路口距离存储
	car* carf;

	carf = (*carlane)[0];
	distemp = carf->distance;
	temp = targetroad->roadincro->carInsert(carf, targetroad->isforward);  //对面道路插入车辆，0：成功  1：等待  2：停止（停在原路口）
	if (0 == temp)
	{
		if (carf->nextroad == targetroad->id)
			carf->nextRoadCal();   //上路成功，更新下一道路
		carlane->pop_front();   //车辆成功入下一路口，此路口车辆弹出
		if (carlane->size() > 0)  //第一优先级车辆后边有车
		{
			(*carlane)[0]->distance += (distemp + 1);  //此道路上与前车距离为前车可行驶距离+1
		}
		this->roadincro->carStateUpdate(this->isforward, false);  //更新此道路车辆状态

		return 1;
	}
	else if (1 == temp)
	{
		return 0;  //车辆等待，后续车辆也依次等待,无调度
	}
	else
	{
		if (carlane->size() > 1)  //第一优先级车辆后边有车
			(*carlane)[1]->distance += carf->distance;  //更新后边车辆与前车距离
		carf->location += carf->distance;
		carf->distance = 0;
		carf->state = stop;
		this->roadincro->carStateUpdate(this->isforward, false);  //更新此道路车辆状态

		return 1;
	}

	return 0;  //正常不会执行
}

int crossroad::croadUpdate()
{
	//返回0：各车道无车辆  1：所有车已停止状态  2：无调度，但有等待   3：有调度
	//循环取该道路第一优先级车辆
	//若直行，则直行，若不可行则退出
	//若左转，取右侧车是否直行，若无则左转，若有则退出
	//若右转，取右侧车是否直行，对面车是否左转，若有则退出
	//退出代码，第一次循环退出 0/1/2 否则 0/1/3

	//std::deque<car*>* carlistfrom = getcarForw();  //车辆驶出道路
	int temp;
	int result = 2;
	std::deque<car*>* carlane;  //第一优先级车辆所在道路
	std::deque<car*>* carlantemp;  //其他方向道路第一优先级车辆所在道路
	car* carf;

	carlane = getFirstCar();
	if (nullptr == carlane)   //道路无车
		return 0;
	else if (stop == (*carlane)[0]->state)  //各车道车辆均为停止状态
		return 1;

	while (nullptr != carlane && wait == (*carlane)[0]->state)
	{
		carf = (*carlane)[0];
		if (-2 == carf->nextroad)   //路口为终点
		{
			if (carlane->size() > 1)  //第一优先级车辆后边有车
				(*carlane)[1]->distance += (carf->distance+1);  //更新后边车辆与前车距离
			carlane->pop_front();
			this->roadincro->carStateUpdate(this->isforward, false);  //更新此道路车辆状态
			result = 3;    //有调度
		}
		else if (carf->nextroad == this->opporoad->id)  //直行
		{
			temp = croadInsert(carlane, this->opporoad);
			if (temp == 1)
				result = 3;    //有调度
			else
				break;  //无调度，车辆等待，后续车辆随之等待
		}
		else if (carf->nextroad == this->leftroad->id)  //左转
		{
			carlantemp = this->righroad->getFirstCar();
			if (nullptr != carlantemp && carlantemp->front()->state == wait &&
				(carlantemp->front()->nextroad == this->leftroad->id || carlantemp->front()->nextroad == -2))
				break;     //其他方向有高优先级车辆待行

			temp = croadInsert(carlane, this->leftroad);
			if (temp == 1)
				result = 3;    //有调度
			else
				break;  //无调度，车辆等待，后续车辆随之等待
		}
		else if (carf->nextroad == this->righroad->id)  //右转
		{
			carlantemp = this->leftroad->getFirstCar();
			if (nullptr != carlantemp && carlantemp->front()->state == wait &&
				(carlantemp->front()->nextroad == this->righroad->id || carlantemp->front()->nextroad == -2))   //检测直行
				break;     //其他方向有高优先级车辆待行
			carlantemp = this->opporoad->getFirstCar();
			if (nullptr != carlantemp && carlantemp->front()->state == wait &&
				carlantemp->front()->nextroad == this->righroad->id)   //检测左转
				break;     //其他方向有高优先级车辆待行

			temp = croadInsert(carlane, this->righroad);
			if (temp == 1)
				result = 3;    //有调度
			else
				break;  //无调度，车辆等待，后续车辆随之等待
		}
		else
		{
			std::cout << "转弯车辆道路id有误！！！" << std::endl;
			return 3;
		}
		carlane = getFirstCar();
	}

	return result;
}
