#include "ros/ros.h"
#include "msgs/dest.h"
#include "msgs/route.h"
#include <cmath>

const float dists[10][2] = {{0, 0}, {185, -175}, {15, 320}, {70, -160}, {185, -155}, {295, 220}, {-278, 60}, {483, 0}, {-60, -265}, {-335, 105}};
float coord[10][10][3];

void calcCoords(const float dists[], float coord);

class Next_dest
{
public:
	Next_dest();

private:
	ros::NodeHandle n;
		ros::Publisher dest_pub;
		ros::Subscriber route_sub;
	void routeCallBack(const next_dest::route::ConstPtr& route);
};

Next_dest::Next_dest()
{
	dest_pub = n.advertise<next_dest::dest>("/dest", 5);
	route_sub = n.subscribe<next_dest::route>("/route", 5, &next_dest::routeCallBack, this);
}

void calcCoords(const float dists[], float coord[]);
{
	for (int i = 0; i < 10; ++i)
	{
		coord[i][i+1][0] = dists[i+1][0];
		coord[i][i+1][1] = dists[i+1][1];
		coord[i][i][0] = 0;
		coord[i][i][1] = 0;
	}

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			if (j < i)
			{
				coord[i][j][0] = -coord[j][i][0];
				coord[j][i][1] = -coord[j][i][1];
			}
			else if (j - i > 1)
			{
				for (int k = i; k < j; ++k)
				{
					coord[i][j][0] += coord[k][k + 1][0];
					coord[i][j][1] += coord[k][k + 1][1];
				}
			}
		}
	}

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			coord[i][j][2] = sqrtf(coord[i][j][0] * coord[i][j][0] + coord[i][j][1] * coord[i][j][1]);
			coord[i][j][0] /= coord[i][j][2];
			coord[i][j][1] /= coord[i][j][2];
		}
	}
}

void Next_dest::routeCallBack(const next_dest::route::ConstPtr& route)
{
	Next_dest::dest dest;

	dest.x = coord[current][next][0];
	dest.y = coord[current][next][1];
	dest.dist = coord[current][next][2];
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "Calculate_the_orientation_and_distance_to_the_next_target");
	calcCoords(dists[], coord[]);
	Next_dest next_dest;
	ros::spin();
}
