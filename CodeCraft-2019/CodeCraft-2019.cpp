#include "config.h"
#include "car.h"
#include "cross.h"
#include "road.h"

using namespace std;

ulong runTimeCount = 0;
road roadnull(-1, 0, 0, 0, 0, 0, 0);
map<long, cross> crosslist;
map<long, road> roadlist;
vector<car> carlist;
int crossToRoadDistance = 15;   //需前方道路有crossToRoadDistance空位才进入路口


void carinit(string carfilename);
void roadinit(string roadfilename);
void crossinit(string crossfilename);
void dispatchmain();
void answerWrite(string answerfilename);

int main(int argc, char *argv[])
{
    std::cout << "Begin" << std::endl;
	
	if(argc < 5){
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}
	
	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string answerPath(argv[4]);
	
	std::cout << "carPath is " << carPath << std::endl;
	std::cout << "roadPath is " << roadPath << std::endl;
	std::cout << "crossPath is " << crossPath << std::endl;
	std::cout << "answerPath is " << answerPath << std::endl;
	
	//道路、路口、车辆初始化
	roadinit(roadPath);
	crossinit(crossPath);
	carinit(carPath);

	//放置车辆置各路口
	for(auto c =carlist.begin();c!=carlist.end();c++)
	{
		crosslist[c->from].carlist.push_back(&(*c));
	}

	//各路口待出发车辆排序 1.预计出发时间 2.速度（降序）3.id   应从back取数
	for(auto cr=crosslist.begin();cr!=crosslist.end();cr++)
	{
		sort(cr->second.carlist.begin(), cr->second.carlist.end(), [](car* c1, car* c2)
		{
			if (c1->plantime != c2->plantime)
				return c1->plantime > c2->plantime;
			else if (c1->speed != c2->speed)
				return c1->speed < c2->speed;
			else
				return c1->id > c2->id;
		});
	}
	//所有车辆初始路径计算
	pathinit();  //计算最短路径变量初始化
	carPathInit();   //所有车辆初始路径计算

	//时间片循环计算
	dispatchmain();

	//结果输出
	answerWrite(answerPath);
	
	return 0;
}

void carinit(string carfilename)
{
	char temp;
	long id;
	long from;          //出发路口
	long to;            //目的路口
	uint speed;          //最高速度
	ulong plantime;      //计划出发时间

	ifstream in(carfilename.data());
	if (!in.is_open())
	{
		cout << "打开car失败！！！" << endl;
		exit(0);
	}
	
	while (!in.eof())
	{
		in >> temp;
		while (temp == ' ' && !in.eof())temp = in.get();  //过滤空格
		if (temp != '#')
		{
			while (temp != '(' && !in.eof())temp = in.get();  //定位到内容
			in >> id; 
			while (in.get() != ',' && !in.eof());
			in >> from;
			while (in.get() != ',' && !in.eof());
			in >> to;
			while (in.get() != ',' && !in.eof());
			in >> speed;
			while (in.get() != ',' && !in.eof());
			in >> plantime;

			carlist.push_back(car(id, from, to, speed, plantime));
			//crosslist[from].carlist.push_back(&carlist.back());
		}

		while (temp != '\n' && !in.eof())temp = in.get();  //换行
	}
}

void roadinit(string roadfilename)
{
	char temp;
	long id;
	uint length;    //长度
	uint speed;     //限速
	uint channel;   //车道数
	long from;      //起点路口id
	long to;        //终点路口id
	bool isDuplex;  //是否双向

	ifstream in(roadfilename.data());
	if (!in.is_open())
	{
		cout << "打开road失败！！！" << endl;
		exit(0);
	}

	roadlist[-1] = roadnull;
	while (!in.eof())
	{
		in >> temp;
		while (temp == ' ' && !in.eof())temp = in.get();  //过滤空格
		if (temp != '#')
		{
			while (temp != '(' && !in.eof())temp = in.get();  //定位到内容
			in >> id;
			while (in.get() != ',' && !in.eof());
			in >> length;
			while (in.get() != ',' && !in.eof());
			in >> speed;
			while (in.get() != ',' && !in.eof());
			in >> channel;
			while (in.get() != ',' && !in.eof());
			in >> from;
			while (in.get() != ',' && !in.eof());
			in >> to;
			while (in.get() != ',' && !in.eof());
			in >> isDuplex;
			
			roadlist[id] = road(id, length, speed, channel, from, to, isDuplex);
		}

		while (in.get() != '\n' && !in.eof());  //换行
	}
}

void crossinit(string crossfilename)
{
	char temp;
	long id;
	long roadid1;
	long roadid2;
	long roadid3;
	long roadid4;
	

	ifstream in(crossfilename.data());
	if (!in.is_open())
	{
		cout << "打开cross失败！！！" << endl;
		exit(0);
	}

	while (!in.eof())
	{
		in >> temp;
		while (temp == ' ' && !in.eof())temp = in.get();  //过滤空格
		if (temp != '#')
		{
			while (temp != '(' && !in.eof())temp = in.get();  //定位到内容
			in >> id;
			while (in.get() != ',' && !in.eof());
			in >> roadid1;
			while (in.get() != ',' && !in.eof());
			in >> roadid2;
			while (in.get() != ',' && !in.eof());
			in >> roadid3;
			while (in.get() != ',' && !in.eof());
			in >> roadid4;

			//crosslist.push_back(cross(id, roadid1, roadid2, roadid3, roadid4, roadlist[roadid1], roadlist[roadid2], roadlist[roadid3], roadlist[roadid4]));
			crosslist.insert(map<long, cross>::value_type(id,cross(id, roadid1, roadid2, roadid3, roadid4, &roadlist[roadid1], &roadlist[roadid2], &roadlist[roadid3], &roadlist[roadid4])));
		}

		while (in.get() != '\n' && !in.eof());  //换行
	}
}

void dispatchmain()
{
	int k;

	while (++runTimeCount)   //时间片累加
	{
		cout << runTimeCount << "   " << endl;

		for(auto ro=roadlist.begin();ro!=roadlist.end();ro++)
		{
			ro->second.carStateUpdate(true, true);
			if (ro->second.isDuplex)
				ro->second.carStateUpdate(false, true);
		}

		//返回0：路口各道路与路口车库无车辆需调度  1：所有车已停止状态  2：未调度车辆，仍有等待车辆  3：有调度车辆
		int result = -1;
		int resulttemp = -1;
		int temp;
		//pair<long, cross>* crdie;
		cross* crdie;  //死锁路口

		while (1)
		{
			resulttemp = -1;
			for(auto cr=crosslist.begin();cr!=crosslist.end();cr++)
			{
				temp = cr->second.updateCross();
				
				if (temp > result)
					result = temp;
				if (temp > resulttemp)
				{
					resulttemp = temp;
					crdie = &(cr->second);
				}
			}

			if (resulttemp < 3)
				break;   //无车辆调度，停止循环
		}
		if (resulttemp == 2)
		{
			//死锁
			std::cout<<"死锁！！！"<<std::endl;
			exit(0);
			k = 0;
		}

		//各路口待出发车辆排序 1.预计出发时间 2.速度（降序）3.id   应从back取数
		
		//for (auto cr = crosslist.begin(); cr != crosslist.end(); cr++)
		//{
		//	auto cartime = find_if(cr->second.carlist.begin(), cr->second.carlist.end(), [](car* c1) {return c1->plantime > time; });
		//	if (cartime == cr->second.carlist.end())continue;   //未找到，转下一路口
		//	if (cartime == cr->second.carlist.begin())continue;   //无出发时间较小车辆，转下一路口
		//	sort(cr->second.carlist.begin(), cartime, [](car* c1, car* c2)
		//	{
		//		if (c1->speed != c2->speed)
		//			return c1->speed < c2->speed;
		//		else
		//			return c1->id > c2->id;
		//	});
		//}

		if (result == 0)
			break;   //调度结束
	}

}

void answerWrite(string answerfilename)
{
	char* s = "#(carId,StartTime,RoadId...)";

	ofstream out(answerfilename.data());   //测试文件不为空结果
	if (!out.is_open())
	{
		cout << "打开answer失败！！！" << endl;
		exit(0);
	}
	out << s << endl;
	
	for(auto c=carlist.begin();c!=carlist.end();c++)
	{
		out << '(' << c->id <<','<< c->acttime;
		for(auto r=c->path.begin();r!=c->path.end();r++)
		{
			out << ',' << *r;
		}
		out << ')' << endl;
	}
	out.close();
}
