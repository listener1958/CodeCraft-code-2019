#include "car.h"


car::~car()
{
}

int car::nextRoadCal()
{
	if (-1 == nextroad)
	{
		nextroad = path[0];
		return 1;
	}
	else if (-2 == nextroad)
	{
		return 0;
	}

	std::vector<long>::iterator it = std::find(path.begin(), path.end(), nextroad);
	if (it == path.end())
	{
		std::cout << "车辆路径更新错误！！！" << std::endl;
	}
	else if ((it+1) == path.end())
	{
		nextroad = -2;
	}
	else
		nextroad = *(++it);

	return 1;
}

