#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <deque>
#include <queue>
#include <stack>
#include <string>


typedef unsigned int uint;
typedef unsigned long ulong;
extern ulong runTimeCount;

//struct crossPoint {     //邻接表法最短路径，节点路径、与源点距离、是否计算标志
//	long path;        //节点路径
//	long roadpath;    //道路路径
//	float dis;        //与源点距离
//	bool mark;        //是否作为生长点标志
//};

void pathinit();  //计算最短路径变量初始化
void shortpath_dijkstra(int speed, long s);  //计算车速speed，初始路口s时最短路径
void carPathInit();   //所有车辆初始路径计算




#endif
