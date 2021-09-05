// Aditya Vasantharao, pd. 3

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include <utility>
#include <algorithm>
#include <vector>
#include <cmath>
#include <climits>
#include <cfloat>

using namespace std;

// set pixel to be 1 (black pixel)
void color_pixel(int **pixels, int x, int y, int height=INT_MAX, int width=INT_MAX) {
    if(x < 0 || x > width || y < 0 || y > height);
    else {
        pixels[y][x] = 1; // reverse the order because of arrays
    }
}

// write the board to a ppm file
void write_board(int **pixels, int height, int width) {
    ofstream outfile;
    outfile.open("output.ppm");

    // header
    outfile << "P3 " << width << " " << height << " " << 255 << endl;

    // content
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(pixels[i][j] == 0) {
                // ppm files require one output for r, g, and b
                outfile << "255 255 255 ";
            }

            if(pixels[i][j] == 1) { // 1 represents a black pixel
                outfile << "0 0 0 ";
            }
        }
        outfile << endl;
    } 

    outfile.close();
}

class Point {
    public:
        double x;
        double y;

        Point(double first, double second) : x(first), y(second) {}
        Point(const Point &other) : x(other.x), y(other.y) {}
        Point() : x(0.0), y(0.0) {}

        double distance(Point other) {
            return sqrt(pow(other.x - x, 2) + pow(other.y - y, 2));
        }
};

class Line {
    public:
        Point first;
        Point second;
        double length;
        double slope;

        // draw a line between any two given points
        void draw_line(int **pixels) {
            int dx = second.x - first.x; // x2 - x1
            int dy = second.y - first.y; // y2 - y1
            int error = abs(dy) - abs(dx);

            if(error < 0 && second.x <= first.x) { // reverse the points
                _draw_line_bresenham(pixels, second, first);
            }
            else if(error > 0 && second.y <= first.y) {
                _draw_line_bresenham(pixels, second, first);
            }
            else {
                _draw_line_bresenham(pixels, first, second);
            }
        }

        // note: the lines cannot be parallel
        Point find_intersection(Line other) {
            if(slope == other.slope) {
                return Point(0.0, 0.0); // no intersection between parallel lines
            }
            else if (slope == DBL_MAX && other.slope == 0) { // x = ___ and y = ___
                return Point(first.x, other.first.y);
            } 
            else if (slope == 0 && other.slope == DBL_MAX) { // y = ___ and x = ___ 
                return Point(other.first.x, first.y);
            }
            else if(slope == DBL_MAX) {
                return Point(first.x, other.slope * (first.x - other.first.x) + other.first.y);
            }
            else if(slope == 0) {
                return Point((first.y - other.first.y) / other.slope + other.first.x, first.y); 
            }
            else if(other.slope == 0 || other.slope == DBL_MAX) {
                return other.find_intersection(getCopy());
            }
            else {
                double x = (other.slope * other.first.x - slope * first.x + first.y - other.first.y) / (other.slope - slope);
                double y = slope * (x - first.x) + first.y;
                return Point(x, y);
            }
        }

        // returns a copy of this line
        Line getCopy() {
            return Line(first, second);
        }

        Line(Point p1, Point p2) : first(p1), second(p2), length(p1.distance(p2)) {
            // slope: (y2 - y1) / (x2 - x1)
            if(p1.x == p2.x) {
                slope = DBL_MAX; // infinite slope
            }
            else if(p1.y == p2.y) {
                slope = 0;
            }
            else {
                slope = (p2.y - p1.y) / (p2.x - p1.x);
            }
        }

        Line(Point p1, double m) : first(p1), slope(m), second(Point(p1.x + 1, p1.y + slope)) {}

    private:
        // bresenham algorithm to draw a line between two given points
        // note: the points must be in increasing order if x1 < x2 and y1 < y2
        void _draw_line_bresenham(int **pixels, Point p1, Point p2) {
            int dx = p2.x - p1.x; // x2 - x1
            int dy = p2.y - p1.y; // y2 - y1
            int error = abs(dy) - abs(dx);  // negative: x driven, positive: y driven

            if(error < 0) { // x-driven
                int j = p1.y; // y1

                if(dy < 0) { // going down
                    for(int i = p1.x; i <= p2.x; i++) {
                        color_pixel(pixels, i, j);

                        if(error >= 0) {
                            j -= 1;
                            error -= dx;
                        }
                        error += -dy;
                    }
                }
                else { // normal case x driven
                    for(int i = p1.x; i <= p2.x; i++) {
                        color_pixel(pixels, i, j);

                        if(error >= 0) {
                            j += 1;
                            error -= dx;
                        }
                        error += dy;
                    }
                }
            }
            else { // y-driven
                if(dx < 0) { // going left
                    int i = p1.x;

                    for(int j = p1.y; j <= p2.y; j++) {
                        color_pixel(pixels, i, j);

                        if(error <= 0) {
                            i -= 1;
                            error += dy;
                        }

                        error -= -dx;
                    }
                }
                else { // normal case y driven
                    int i = p1.x;

                    for(int j = p1.y; j <= p2.y; j++) {
                        color_pixel(pixels, i, j);

                        if(error <= 0) {
                            i += 1;
                            error += dy;
                        }

                        error -= dx;
                    }
                }
            }
        }
};

class Triangle {
    public: 
        vector<Line> lines;

        // semiperimeter
        double s;

        double incircle_radius;
        double circumcircle_radius;

        Point incircle_center;
        Point circumcircle_center;

        // lengths of the lines
        double a;
        double b;
        double c;

        Triangle(Point p1, Point p2, Point p3, int **pixels) {
            lines.push_back(Line(p1, p2));
            lines.push_back(Line(p2, p3));
            lines.push_back(Line(p3, p1));

            a = lines[0].length;
            b = lines[1].length;
            c = lines[2].length;

            s = 0.5 * (a + b + c);
            incircle_radius = sqrt((s - a) * (s - b) * (s - c) / s);
            circumcircle_radius = a * b * c / (4 * incircle_radius * s);

            // set incircle and circumcircle radii here

            // circumcircle: find the intersection of the perpendicular bisectors of any two of the triangles sides
            Point midpoint1((lines[0].second.x + lines[0].first.x) / 2, (lines[0].second.y + lines[0].first.y) / 2);
            Point midpoint2((lines[1].second.x + lines[1].first.x) / 2, (lines[1].second.y + lines[1].first.y) / 2);

            double slope1;
            if(lines[0].slope == 0) {
                slope1 = DBL_MAX;
            }
            else if(lines[0].slope == DBL_MAX) {
                slope1 = 0;
            } 
            else {
                slope1 = - (1 / lines[0].slope);
            }

            double slope2;
            if(lines[1].slope == 0) {
                slope2 = DBL_MAX;
            }
            else if(lines[1].slope == DBL_MAX) {
                slope2 = 0;
            } 
            else {
                slope2 = - (1 / lines[1].slope);
            }

            circumcircle_center = Line(midpoint1, slope1).find_intersection(Line(midpoint2, slope2));

            Point test = Line(midpoint1, slope1).find_intersection(Line(midpoint2, slope2));

            color_pixel(pixels, test.x, test.y);
        }

        void draw_triangle(int **pixels) {
            for(Line line : lines) {
                line.draw_line(pixels);
            }
        }
};

class Circle {
    public:
        Point center;
        double radius;
        int canvas_height;
        int canvas_width;

        Circle(Point p1, double r, int c_h, int c_w) : center(p1), radius(r), canvas_height(c_h), canvas_width(c_w) {}

        void draw_circle(int **pixels) {
            int x, y, xmax, y2, y2_new, ty;
            xmax = (int) (radius * 0.70710678); // maximum x at radius/sqrt(2) + x0
            y = radius;
            y2 = y * y;
            ty = (2 * y) - 1; y2_new = y2;
            for (x = 0; x <= xmax; x++) {
                if ((y2 - y2_new) >= ty) {
                    y2 -= ty;
                    y -= 1;
                    ty -= 2;
                }

                color_pixel(pixels, x + center.x, y + center.y, canvas_height, canvas_width);
                color_pixel(pixels, x + center.x, -y + center.y, canvas_height, canvas_width);
                color_pixel(pixels, -x + center.x, y + center.y, canvas_height, canvas_width);
                color_pixel(pixels, -x + center.x, -y + center.y, canvas_height, canvas_width);
                color_pixel(pixels, y + center.x, x + center.y, canvas_height, canvas_width);
                color_pixel(pixels, y + center.x, -x + center.y, canvas_height, canvas_width);
                color_pixel(pixels, -y + center.x, x + center.y, canvas_height, canvas_width);
                color_pixel(pixels, -y + center.x, -x + center.y, canvas_height, canvas_width);

                y2_new -= (2 * x) - 3;
            }
        }
};

int main() {
    srand((unsigned) time(0));
    int height = 100;
    int width = 100;    

    // allocate memory for pixel array
    // cleaner to keep the data type as int** and dynamically allocate
    // rather than making it fixed width and passing around int[][size] in functions

    int **pixels;
    pixels = new int*[height];
    
    for(int i = 0; i < height; i++) {
        pixels[i] = new int[width];

        // initialize all values in pixel array to default value of 0 (white pixel)
        for(int j = 0; j < width; j++) {
            pixels[i][j] = 0;
        }
    }

    Point p1(rand() % height, rand() % width);
    Point p2(rand() % height, rand() % width);
    Point p3(rand() % height, rand() % width);

    // check that points are not collinear
    while((p1.x == p2.x && p2.x == p3.x) || (p1.y == p2.y && p2.y == p3.y)) {
        p3 = Point(rand() % height, rand() % width);
    }

    cout << p1.x << " " << p1.y << endl;
    cout << p2.x << " " << p2.y << endl;
    cout << p3.x << " " << p3.y << endl;

    Triangle t(p1, p2, p3, pixels);

    cout << t.circumcircle_center.x << " " << t.circumcircle_center.y << endl;
    
    t.draw_triangle(pixels);
    Circle c(Point(50, 50), t.incircle_radius, height, width);
    c.draw_circle(pixels);

    Circle c1(t.circumcircle_center, t.circumcircle_radius, height, width);
    c1.draw_circle(pixels);
    write_board(pixels, height, width);

    // cleanup

    for(int i = 0; i < height; i++) {
        delete[] pixels[i];
    }

    delete[] pixels;

}




// Test Cases:

// Lines:

// pair<Point, Point> points[] = {
//     pair<Point, Point>{Point(9, 2), Point(1, 2)},
//     pair<Point, Point>{Point(1, 2), Point(9, 2)},
//     pair<Point, Point>{Point(0, 0), Point(9, 2)},
//     pair<Point, Point>{Point(9, 2), Point(0, 0)},        
//     pair<Point, Point>{Point(0, 9), Point(9, 2)},
//     pair<Point, Point>{Point(9, 2), Point(0, 9)},
//     pair<Point, Point>{Point(2, 2), Point(2, 9)},
//     pair<Point, Point>{Point(2, 9), Point(2, 2)},
//     pair<Point, Point>{Point(2, 2), Point(6, 9)},
//     pair<Point, Point>{Point(2, 2), Point(6, 9)},
//     pair<Point, Point>{Point(8, 2), Point(2, 9)},
//     pair<Point, Point>{Point(8, 2), Point(2, 9)}
// };

// for(auto point_pair : points) {
//     Line l(point_pair.first, point_pair.second);
//     l.draw_line(pixels);
//     cout << endl << endl;

//     for(int i = 0; i < height; i++) {
//         // initialize all values in pixel array to default value of 0 (white pixel)
//         for(int j = 0; j < width; j++) {
//             cout << pixels[i][j] << " ";
//         }
//         cout << endl;
//     }

//     for(int i = 0; i < height; i++) {
//         // initialize all values in pixel array to default value of 0 (white pixel)
//         for(int j = 0; j < width; j++) {
//             pixels[i][j] = 0;
//         }
//     }
// }