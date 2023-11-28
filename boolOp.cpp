// This is clone from https://github.com/Espade/Weiler-Atherton-Clipping.git
// but there are several bugs that need to be fixed.

#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#define Size 400
using namespace std;
namespace py = pybind11;

struct Point {
	int x, y;
    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}
};
typedef struct IntersectionPoint {
	int pointFlag;
	int index0, index1;
	Point p;
	bool inFlag;
	int dis;
}IP;

class Pg {
public:
	vector<Point> pts;
	Pg(void);
	~Pg(void);
};
Pg::Pg(void) {}
Pg::~Pg(void) {}

bool isPointInsidePg(Point p, Pg& py) {
	int cnt = 0, size = py.pts.size();
	for (int i = 0; i < size; i++) {
		Point p1 = py.pts[i];
		Point p2 = py.pts[(i + 1) % size];
		if (p1.y == p2.y) continue;
		if (p.y < min(p1.y, p2.y)) continue;
		if (p.y >= max(p1.y, p2.y)) continue;
		double x = (double)(p.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;
		if (x > p.x) cnt++;
	}
	return (cnt % 2 == 1);
}
int cross(Point& p0, Point& p1, Point& p2) {
	return ((p2.x - p0.x) * (p1.y - p0.y) - (p1.x - p0.x) * (p2.y - p0.y));
}
bool onSegment(Point& p0, Point& p1, Point& p2) {
	int minx = min(p0.x, p1.x), maxx = max(p0.x, p1.x);
	int miny = min(p0.y, p1.y), maxy = max(p0.y, p1.y);
	if (p2.x >= minx && p2.x <= maxx && p2.y >= miny && p2.y <= maxy) return true;
	return false;
}
bool segmentsIntersect(Point& p1, Point& p2, Point& p3, Point& p4) {
	int d1 = cross(p3, p4, p1);
	int d2 = cross(p3, p4, p2);
	int d3 = cross(p1, p2, p3);
	int d4 = cross(p1, p2, p4);
	if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
		((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
		return true;
	if (d1 == 0 && onSegment(p3, p4, p1)) return true;
	if (d2 == 0 && onSegment(p3, p4, p2)) return true;
	if (d3 == 0 && onSegment(p1, p2, p3)) return true;
	if (d4 == 0 && onSegment(p1, p2, p4)) return true;
	return false;
}
Point getintersectPoint(Point p1, Point p2, Point p3, Point p4) {
	Point p;
	int b1 = (p2.y - p1.y) * p1.x + (p1.x - p2.x) * p1.y;
	int b2 = (p4.y - p3.y) * p3.x + (p3.x - p4.x) * p3.y;
	int D = (p2.x - p1.x) * (p4.y - p3.y) - (p4.x - p3.x) * (p2.y - p1.y);
	int D1 = b2 * (p2.x - p1.x) - b1 * (p4.x - p3.x);
	int D2 = b2 * (p2.y - p1.y) - b1 * (p4.y - p3.y);
	p.x = D1 / D;
	p.y = D2 / D;
	return p;
}
void generateIntersectPoints(Pg& pyclip, Pg& py, list<IP>& iplist) {
	int clipSize = pyclip.pts.size(), pySize = py.pts.size();
	for (int i = 0; i < clipSize; i++) {
		Point p1 = pyclip.pts[i];
		Point p2 = pyclip.pts[(i + 1) % clipSize];
		for (int j = 0; j < pySize; j++) {
			Point p3 = py.pts[j];
			Point p4 = py.pts[(j + 1) % pySize];
			if (segmentsIntersect(p1, p2, p3, p4)) {
				IP ip;
				ip.index0 = j;
				ip.index1 = i;
				ip.p = getintersectPoint(p1, p2, p3, p4);
				iplist.push_back(ip);
			}
		}
	}
}
int getDistance(Point& p1, Point& p2) {
	return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}
bool distanceComparator(IP& ip1, IP& ip2) {
	return ip1.dis < ip2.dis;
}
void generateList(Pg& py, list<IP>& iplist, list<IP>& comlist, int index) {
	int size = py.pts.size();
	list<IP>::iterator it;

	for (int i = 0; i < size; i++) {
		Point p1 = py.pts[i];
		IP ip;
		ip.pointFlag = 0;
		ip.p = p1;
		comlist.push_back(ip);
		list<IP> oneSeg;
		for (it = iplist.begin(); it != iplist.end(); it++) {
			if ((index == 0 && i == it->index0) ||
				(index == 1 && i == it->index1)) {
				it->dis = getDistance(it->p, p1);
				it->pointFlag = 1;
				oneSeg.push_back(*it);
			}
		}
		oneSeg.sort(distanceComparator);
		for (it = oneSeg.begin(); it != oneSeg.end(); it++)
			comlist.push_back(*it);
	}
}
void getPgPointInOut(list<IP>& Pglist, Pg& pyclip) {
	bool inFlag;
	list<IP>::iterator it;
	for (it = Pglist.begin(); it != Pglist.end(); it++) {
		if (it->pointFlag == 0) {
			if (isPointInsidePg(it->p, pyclip)) inFlag = true;
			else inFlag = false;
		}
		else {
			inFlag = !inFlag;
			it->inFlag = inFlag;
		}
	}
}
bool operator==(Point& p1, Point& p2) {
	return p1.x == p2.x && p1.y == p2.y;
}
void getClipPointInOut(list<IP>& cliplist, list<IP>& Pglist) {
	list<IP>::iterator it, it1;
	for (it = cliplist.begin(); it != cliplist.end(); it++) {
		if (it->pointFlag == 0) continue;
		for (it1 = Pglist.begin(); it1 != Pglist.end(); it1++) {
			if (it1->pointFlag == 0) continue;
			if (it->p == it1->p) it->inFlag = it1->inFlag;
		}
	}
}
Pg generateClipArea(list<IP>& Pglist, list<IP>& cliplist) {
	list<IP>::iterator it, it1;
	Pg py;

	for (it = Pglist.begin(); it != Pglist.end(); it++)
		if (it->pointFlag == 1 && it->inFlag) break;
	py.pts.clear();

	while (true) {
		if (it == Pglist.end()) break;
		py.pts.push_back(it->p);
		for (; it != Pglist.end(); it++) {
			if (it->pointFlag == 1 && !it->inFlag) break;
			py.pts.push_back(it->p);
		}
		for (it1 = cliplist.begin(); it1 != cliplist.end(); it1++)
			if (it1->p == it->p) break;

		for (; it1 != cliplist.end(); it1++) {
			if (it1->pointFlag == 1 && it1->inFlag) break;
			py.pts.push_back(it1->p);
		}
		if (py.pts[0] == it1->p) {
			for (int i = 0; i < py.pts.size(); i++)
                cout << py.pts[i].x << " " << py.pts[i].y <<endl;
			for (; it != Pglist.end(); it++)
				if (it->pointFlag == 1 && it->inFlag) break;
			continue;
		}
		for (; it != Pglist.end(); it++)
			if (it->p == it1->p) break;
	}
	return py;
}

Pg weilerAtherton(Pg& pyclip, Pg& py) {
	list<IP> iplist, Pglist, cliplist;
	Pg pg;
	generateIntersectPoints(pyclip, py, iplist);

	generateList(py, iplist, Pglist, 0);
	generateList(pyclip, iplist, cliplist, 1);

	getPgPointInOut(Pglist, pyclip);

	getClipPointInOut(cliplist, Pglist);

	pg = generateClipArea(Pglist, cliplist);

	for (int i = 0; i < pg.pts.size(); i++)
        cout << pg.pts[i].x << " " << pg.pts[i].y <<endl;
	return pg;
}

void GenerateRandomSimplePg(Pg &G, int M)
{
	Point P;
	G.pts.clear();
	for (int i = 0; i < M; ++i)
	{
		bool flag;
		do {
			P.x = rand() % Size;
			P.y = rand() % Size;
			flag = true;
			for (int j = 1; j < i - 1; ++j)
				if (segmentsIntersect(G.pts[j - 1], G.pts[j], G.pts[i - 1], P)) {
					flag = false;
					break;
				}
			if (flag && i == M - 1) {
				for (int j = 2; j < i; ++j)
					if (segmentsIntersect(G.pts[j - 1], G.pts[j], P, G.pts[0])) {
						flag = false;
						break;
					}
			}
		} while (!flag);
		G.pts.push_back(P);
	}
}

PYBIND11_MODULE(boolOp, m) {
    py::class_<Point>(m, "Point")
        .def(py::init<>())
		.def(py::init<int, int>())
        .def_readwrite("x", &Point::x)
        .def_readwrite("y", &Point::y);

    py::class_<Pg>(m, "Pg")
        .def(py::init<>())
        .def_readwrite("pts", &Pg::pts);

	m.def("andOperation", &weilerAtherton, "and operation");
}