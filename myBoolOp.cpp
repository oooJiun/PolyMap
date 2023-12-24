#include <iostream>
#include <cmath>
#include <bits/stdc++.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <chrono>
using namespace std;
namespace py = pybind11;
const int inf = 1e9;
const double eps = 1e-5;

struct Point2d {
    double x, y;
    bool operator < (const Point2d &rhs) const {
        if (x==rhs.x) return y < rhs.y;
        return x < rhs.x;
    }
};
struct Line{
    Point2d start;
    Point2d end;
};
inline Point2d Vector(Point2d a, Point2d b) {
    return{ b.x - a.x, b.y - a.y };
}
double dis2(Point2d a, Point2d b) {
    return (b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y);
}
double cross(Point2d A, Point2d B, Point2d P) {
    Point2d AB = Vector(A,B);
    Point2d AP = Vector(A,P);
    return AB.x*AP.y - AB.y*AP.x;
}
double dot(Point2d A, Point2d B, Point2d P) {
    Point2d AB = Vector(A,B);
    Point2d AP = Vector(A,P);
    return AB.x*AP.x + AB.y*AP.y;
}
int dir(Point2d A, Point2d B, Point2d P) {
    if (cross(A, B, P) > 0)  return -1;
    else if (cross(A, B, P)<0) return 1;
    else if (dot(A, B, P) < 0) return -2;
    else if (dot(A, B, P) >= 0)
    {
        if (dis2(A, B) < dis2(A, P)) return 2;
        else return 0;
    }
    return 0;
}
double disLine(Point2d A, Point2d B, Point2d P) {
    return fabs(cross(A, B, P)) / sqrt(dis2(A, B));
}

Point2d intersection(Line u, Line v) {
    Point2d A1 = u.start;
    Point2d A2 = u.end;
    Point2d B1 = v.start;
    Point2d B2 = v.end;
    if (dir(A1, A2, B1)*dir(A1, A2, B2) <= 0 && dir(B1, B2, A1)*dir(B1, B2, A2) <= 0) {
        double t = disLine(A1, A2, B1) / (disLine(A1, A2, B1) + disLine(A1, A2, B2));
        Point2d B1B2 = Vector(B1, B2);
        Point2d inter = { B1.x + B1B2.x*t, B1.y + B1B2.y*t };
        return {inter.x, inter.y};
    } else {
        return {-inf, -inf};
    }
}

vector<Line> p1, p2;
vector<Point2d> new1, new2;
double dis(Point2d point1, Point2d point2) {
    return sqrt((point1.x-point2.x)*(point1.x-point2.x) + (point1.y-point2.y)*(point1.y-point2.y));
}
map<int, bool> vis1, vis2;
map<int, int> pos1, pos2;

void clear() {
    p1.clear();
    p2.clear();
    new1.clear();
    new2.clear();
    vis1.clear();
    vis2.clear();
    pos1.clear();
    pos2.clear();
}

bool isPointInsidePoly(Point2d P,const vector<Point2d>& polyVertices) {
    std::size_t vertCount = polyVertices.size();
    if (vertCount < 2)
        return false;
    Point2d tmp = P;
    for (int l = 0; l < 2; l++) {
        for (int r = 0; r < 2; r++) {
            P = tmp;
            if (l % 2) P.x += 0.001;
            else P.x -= 0.001;
            if (r % 2) P.y += 0.001;
            else P.y -= 0.001;
            bool inside = false;
            for (unsigned i = 1; i <= vertCount; ++i) {
                const Point2d &A = polyVertices[i - 1];
                const Point2d &B = polyVertices[i % vertCount];
                if ((B.y <= P.y && P.y < A.y) || (A.y <= P.y && P.y < B.y)) {
                    double t = (P.x - B.x) * (A.y - B.y) - (A.x - B.x) * (P.y - B.y);
                    if (A.y < B.y)
                        t = -t;
                    if (t < 0)
                        inside = !inside;
                }
            }
            if (inside) return inside;
        }
    }
    return false;
}
void line(vector<Point2d>poly1, vector<Point2d>poly2) {
    int len1 = poly1.size();
    for (int i = 0; i < len1; i++) {
        p1.push_back({{poly1[i].x, poly1[i].y}, {poly1[(i+1)%len1].x, poly1[(i+1)%len1].y}});
    }
    int len2 = poly2.size();
    for (int i = 0; i < len2; i++) {
        p2.push_back({{poly2[i].x, poly2[i].y}, {poly2[(i+1)%len2].x, poly2[(i+1)%len2].y}});
    }
}
void getIntersections(vector<Point2d>poly1, vector<Point2d>poly2) {
    int len1 = poly1.size();
    for (int i = 0; i < len1; i++) {
        new1.push_back(poly1[i]);
        vector<Point2d> tmp;
        for (auto it2 : p2) {
            Point2d p = intersection({{poly1[i].x, poly1[i].y},{poly1[(i+1)%len1].x, poly1[(i+1)%len1].y}}, it2);
            if (p.x != -inf && p.y != -inf) tmp.push_back({p.x, p.y});
        }
        sort(tmp.begin(), tmp.end(), [&](Point2d p1, Point2d p2){
            return dis(p1, poly1[i]) < dis(p2, poly1[i]);
        });
        for (auto it : tmp) new1.push_back(it);
    }

    int len2 = poly2.size();
    for (int i = 0; i < len2; i++) {
        new2.push_back(poly2[i]);
        vector<Point2d> tmp;
        for (auto it2 : p1) {
            Point2d p = intersection({{poly2[i].x, poly2[i].y},{poly2[(i+1)%len2].x, poly2[(i+1)%len2].y}}, it2);
            if (p.x != -inf && p.y != -inf) tmp.push_back({p.x, p.y});
        }
        sort(tmp.begin(), tmp.end(), [&](Point2d p1, Point2d p2){
            return dis(p1, poly2[i]) < dis(p2, poly2[i]);
        });
        for (auto it : tmp) new2.push_back(it);
    }
    for (int i = 0; i < new1.size(); i++) {//映射关系，给定eps为误差范围
        for (int j = 0; j < new2.size(); j++) {
            if (fabs(new1[i].x-new2[j].x)<eps&&fabs(new1[i].y-new2[j].y)<eps) {
                pos1[i] = j;
                pos2[j] = i;
            }
        }
    }
}

vector<Point2d> AND(vector<Point2d> poly1, vector<Point2d> poly2) {
    clear();
    vector<Point2d> now;
    Point2d tmp;
    tmp.x = -1, tmp.y = -1;
    line(poly1, poly2);
    getIntersections(poly1, poly2);
    int len1 = new1.size();    
    int len2 = new2.size();
    for (int i = 0; i < new1.size(); i++) {
        if (vis1[i]) continue;
        int ch = 1, nowpos = i;
        while (1) {
            if (ch == 1) {
                if (isPointInsidePoly(new1[nowpos], poly2)) now.push_back(new1[nowpos]);
                if (vis1[nowpos]) {
                    now.push_back(tmp);
                    break;
                }
                vis1[nowpos] = true;
                if (isPointInsidePoly(new1[nowpos], poly2) && !isPointInsidePoly(new1[(nowpos+1)%len1], poly2)) {
                    ch = 2;
                    nowpos = pos1[nowpos];
                    nowpos = (nowpos + 1) % len2;
                } else {
                    nowpos = (nowpos + 1) % len1;
                }
            } else {
                if (isPointInsidePoly(new2[nowpos], poly1)) now.push_back(new2[nowpos]);
                if (vis2[nowpos]) {
                    break;
                }
                vis2[nowpos] = true;
                if (isPointInsidePoly(new2[nowpos], poly1) && !isPointInsidePoly(new2[(nowpos+1)%len2], poly1)) {
                    ch = 1;
                    nowpos = pos2[nowpos];
                    nowpos = (nowpos + 1) % len1;
                } else {
                    nowpos = (nowpos + 1) % len2;
                }
            }
        }
    }
    return now;
}

vector<Point2d> OR(vector<Point2d> poly1, vector<Point2d> poly2) {
    clear();
    vector<Point2d> now;
    Point2d tmp;
    tmp.x = -1, tmp.y = -1;
    line(poly1, poly2);
    getIntersections(poly1, poly2);
    int len1 = new1.size();    
    int len2 = new2.size();
    if(len1 == poly1.size()) {
        if(isPointInsidePoly(new1[0], new2)) {
            for (int j = 0; j < new2.size(); j++) {
                return new2;
            }
        } else if(isPointInsidePoly(new2[0], new1)) {
            for (int j = 0; j < new1.size(); j++) {
                return new1;
            }
        } else {
            for (int j = 0; j < new1.size(); j++) {
                now.push_back(new1[j]);
            }
            now.push_back(tmp);
            for (int j = 0; j < new2.size(); j++) {
                now.push_back(new2[j]);
            }
        }
        return now;
    } 
    for (int i = 0; i < new1.size(); i++) {
        if (vis1[i]) continue;
        int ch = 1, nowpos = i;
        while (1) {
            if (ch == 1) {
                now.push_back(new1[nowpos]);
                if (vis1[nowpos]) {
                    return now;
                    // now.push_back(tmp);
                    // break;
                }
                vis1[nowpos] = true;
                if (isPointInsidePoly(new1[nowpos], poly2)) {
                    ch = 2;
                    nowpos = pos1[nowpos];
                    nowpos = (nowpos + 1) % len2;
                } else {
                    nowpos = (nowpos + 1) % len1;
                }
            } else {
                if (isPointInsidePoly(new2[nowpos], poly1)) now.push_back(new2[nowpos]);
                if (!isPointInsidePoly(new2[nowpos], poly1)) now.push_back(new2[nowpos]);
                if (vis2[nowpos]) {
                    return now;
                }
                vis2[nowpos] = true;
                if (isPointInsidePoly(new2[nowpos], poly1)) {
                    ch = 1;
                    nowpos = pos2[nowpos];
                    nowpos = (nowpos + 1) % len1;
                } else {
                    nowpos = (nowpos + 1) % len2;
                }
            }
        }
    }
    return now;
}

// int main(int argc,char** argv) {
//     vector<Point2d>poly1, poly2;
//     poly1.push_back({-5, -5});
//     poly1.push_back({-5, 5});
//     poly1.push_back({5, 5});
//     poly1.push_back({5, -5});

//     poly2.push_back({-7, 0});
//     poly2.push_back({0, 7});
//     poly2.push_back({7, 0});
//     poly2.push_back({0, -7});
//     AND(poly1, poly2);
// }

PYBIND11_MODULE(myBoolOp, m) {
    py::class_<Point2d>(m, "Point")
        .def(py::init<>())
		.def(py::init<double, double>())
        .def_readwrite("x", &Point2d::x)
        .def_readwrite("y", &Point2d::y);

	m.def("andOperation", &AND, "and operation");
    m.def("orOperation", &OR, "or operation");
}