// Aditya Vasantharao, pd. 3

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <ctime>
#include <utility>
#include <algorithm>
#include <vector>
#include <cmath>
#include <climits>
#include <cfloat>
#include <iomanip>
#include <list>
#include <iterator>

using namespace std;

class Point {
    private:
        double x;
        double y;

    public:
        double getX() {
            return x;
        }

        double getY() {
            return y;
        }

        Point(double first, double second) : x(first), y(second) {}
        Point(const Point &other) : x(other.x), y(other.y) {}
        Point() : x(0.0), y(0.0) {}

        double distance(Point other) {
            return sqrt(pow(other.x - x, 2) + pow(other.y - y, 2));
        }    
};

class PointPair {
    private:
        Point first;
        Point second;
        double distance;

    public:
        PointPair(Point p1, Point p2, double d) : first(p1), second(p2), distance(d) {}

        Point getFirst() {
            return first;
        }

        Point getSecond() {
            return second;
        }

        double getDistance() {
            return distance;
        }
};

// set pixel to be 1 (black pixel)
void color_pixel(int **pixels, int x, int y, int height, int width, bool red = false) {
    if(x < 0 || x > width - 1 || y < 0 || y > height - 1);
    else {
        if(red) {
            pixels[y][x] = 2;
        }
        else {
            pixels[y][x] = 1; // reverse the order because of arrays
        }
    }
}

// scales point to canvas dimensions
void color_pixel(int **pixels, Point p, int height, int width, bool red = false) {
    int x = static_cast<int>(p.getX() * width);
    int y = static_cast<int>(p.getY() * height);

    if(x < 0 || x > width - 1 || y < 0 || y > height - 1);
    else {
        if(red) {
            pixels[y][x] = 2;
        }
        else {
            pixels[y][x] = 1; // reverse the order because of arrays
        }
    }
}

// write the board to a ppm file
void write_board(int **pixels, int height, int width, string idx = "") {
    ofstream outfile;
    outfile.open("output" + idx + ".ppm");

    // header
    outfile << "P3 " << width << " " << height << " " << 255 << endl;

    // content
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(pixels[i][j] == 0) {
                // ppm files require one output for r, g, and b
                outfile << "255 255 255 ";
            }
            else if(pixels[i][j] == 1) { // 1 represents a black pixel
                outfile << "0 0 0 ";
            }
            else if(pixels[i][j] == 2) { // 2 represents a red pixel
                outfile << "255 0 0 ";
            }
        }
        outfile << endl;
    } 

    outfile.close();
}

class Line {
    private:
        Point first;
        Point second;
        double length;
        double slope;

        int canvas_height;
        int canvas_width;

        double threshold = 1e-10;

    public:
        double getLength() {
            return first.distance(second);
        }

        Point getFirst() {
            return first;
        }
        
        Point getSecond() {
            return second;
        }

        double getSlope() {
            return slope;
        }

        // draw a line between any two given points
        void draw_line(int **pixels) {
            // both points intersect with top right bottom or left of screen
            // check x = canvas_width, if 0 <= y && y <= canvas_height we're good
            // else plug in y = 0, solve for x
            Point scaled_first;
            Point scaled_second;

            // check 0 slope
            if((-threshold <= slope) && (slope <= threshold)) {
                // cout << "0" << endl;
                scaled_first = Point(static_cast<double>(canvas_width), first.getY() * canvas_height);
                scaled_second = Point(-static_cast<double>(canvas_width), first.getY() * canvas_height);
            }
            // check infinite slope
            else if(100000.0 <= abs(slope)) {
                // cout << "1" << endl;
                scaled_first = Point(first.getX() * canvas_width, static_cast<double>(canvas_height));
                scaled_second = Point(first.getX() * canvas_width, -static_cast<double>(canvas_height));
            }
            else {
                // check right side
                double right_y = slope * (canvas_width - first.getX() * canvas_width) + first.getY() * canvas_height;
                if((0 <= right_y) && (right_y <= canvas_height)) {
                    // hits the right side of the screen
                    scaled_first = Point(canvas_width, right_y);
                }
                // check top
                double top_x = (canvas_height - first.getY() * canvas_height) / slope + first.getX() * canvas_width;
                if((0 <= top_x) && (top_x <= canvas_height)) {
                    // hits the top of the screen
                    if((scaled_first.getX() == 0.0) && (scaled_first.getY() == 0.0)) {
                        scaled_first = Point(top_x, canvas_height);
                    }
                    else {
                        scaled_second = Point(top_x, canvas_height);
                    }
                }
                // check left
                double left_y = slope * (0 - first.getX() * canvas_width) + first.getY() * canvas_height;
                if((0 <= left_y) && (left_y <= canvas_height)) {
                    // hits the left side of the screen
                    if((scaled_first.getX() == 0.0) && (scaled_first.getY() == 0.0)) {
                        scaled_first = Point(0, left_y); 
                    }
                    else if((scaled_second.getX() == 0.0) && (scaled_second.getY() == 0.0)){
                        scaled_second = Point(0, left_y); 
                    }
                }
                // check bottom
                double bottom_x = (0 - first.getY() * canvas_height) / slope + first.getX() * canvas_width;
                if((0 <= bottom_x) && (bottom_x <= canvas_height) && ((scaled_second.getX() == 0.0) && (scaled_second.getY() == 0.0))) {
                    // hits the top of the screen
                    scaled_second = Point(bottom_x, 0);
                }
            }

            int dx = scaled_second.getX() - scaled_first.getX(); // x2 - x1
            int dy = scaled_second.getY() - scaled_first.getY(); // y2 - y1
            int error = abs(dy) - abs(dx);

            if(error < 0 && scaled_second.getX() <= scaled_first.getX()) { // reverse the points
                _draw_line_bresenham(pixels, scaled_second, scaled_first, canvas_height, canvas_width);
            }
            else if(error > 0 && scaled_second.getY() <= scaled_first.getY()) {
                _draw_line_bresenham(pixels, scaled_second, scaled_first, canvas_height, canvas_width);
            }
            else {
                _draw_line_bresenham(pixels, scaled_first, scaled_second, canvas_height, canvas_width);
            }
        }

        // note: the lines cannot be parallel
        Point find_intersection(Line other) {
            if((other.slope - threshold <= slope) && (slope <= other.slope + threshold)) {
                return Point(0.0, 0.0); // no intersection between parallel lines
            }
            else if ((100000.0 <= abs(slope)) && ((-threshold <= other.slope) && (other.slope <= threshold))) { // x = ___ and y = ___
                return Point(first.getX(), other.first.getY());
            } 
            else if (((-threshold <= slope) && (slope <= threshold)) && (100000.0 <= abs(other.slope))) { // y = ___ and x = ___ 
                return Point(other.first.getX(), first.getY());
            }
            else if(100000.0 <= abs(slope)) {
                return Point(first.getX(), other.slope * (first.getX() - other.first.getX()) + other.first.getY());
            }
            else if((-threshold <= slope) && (slope <= threshold)) {
                return Point((first.getY() - other.first.getY()) / other.slope + other.first.getX(), first.getY()); 
            }
            else if(((-threshold <= other.slope) && (other.slope <= threshold)) || (100000.0 <= abs(other.slope))) {
                return other.find_intersection(getCopy());
            }
            else {
                double x = (other.slope * other.first.getX() - slope * first.getX() + first.getY() - other.first.getY()) / (other.slope - slope);
                double y = slope * (x - first.getX()) + first.getY();
                return Point(x, y);
            }
        }

        // returns a copy of this line
        Line getCopy() {
            return Line(first, second, canvas_height, canvas_width);
        }

        Line make_perpendicular(Point other) {
            double final_slope;
            if(slope == DBL_MAX) {
                final_slope = 0;
            }
            else if (slope == 0) {
                final_slope = DBL_MAX;    
            }
            else {
                final_slope = -(1.0 / slope);
            }

            return Line(other, final_slope, canvas_height, canvas_width);
        }

        // point-point definition
        Line(Point p1, Point p2, int c_height, int c_width) : first(p1), second(p2), length(p1.distance(p2)), canvas_height(c_height), canvas_width(c_width) {
            // slope: (y2 - y1) / (x2 - x1)
            if(p1.getX() == p2.getX()) {
                slope = DBL_MAX; // infinite slope
            }
            else if(p1.getY() == p2.getY()) {
                slope = 0;
            }
            else {
                slope = (p2.getY() - p1.getY()) / (p2.getX() - p1.getX());
            }
        }

        // point-slope form
        Line(Point p1, double m, int c_height, int c_width) : first(p1), slope(m), second(Point(p1.getX() + 1, p1.getY() + m)), canvas_height(c_height), canvas_width(c_width) {}

    private:
        // bresenham algorithm to draw a line between two given points
        // note: the points must be in increasing order if x1 < x2 and y1 < y2
        void _draw_line_bresenham(int **pixels, Point p1, Point p2, int canvas_height, int canvas_width) {
            int dx = p2.getX() - p1.getX(); // x2 - x1
            int dy = p2.getY() - p1.getY(); // y2 - y1
            int error = abs(dy) - abs(dx);  // negative: x driven, positive: y driven

            if(error < 0) { // x-driven
                int j = p1.getY(); // y1

                if(dy < 0) { // going down
                    for(int i = p1.getX(); i <= p2.getX(); i++) {
                        color_pixel(pixels, i, j, canvas_height, canvas_width);

                        if(error >= 0) {
                            j -= 1;
                            error -= dx;
                        }
                        error += -dy;
                    }
                }
                else { // normal case x driven
                    for(int i = p1.getX(); i <= p2.getX(); i++) {
                        color_pixel(pixels, i, j, canvas_height, canvas_width);

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
                    int i = p1.getX();

                    for(int j = p1.getY(); j <= p2.getY(); j++) {
                        color_pixel(pixels, i, j, canvas_height, canvas_width);

                        if(error <= 0) {
                            i -= 1;
                            error += dy;
                        }

                        error -= -dx;
                    }
                }
                else { // normal case y driven
                    int i = p1.getX();

                    for(int j = p1.getY(); j <= p2.getY(); j++) {
                        color_pixel(pixels, i, j, canvas_height, canvas_width);

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

class Circle {
    private:
        Point center;
        double radius;
        int canvas_height;
        int canvas_width;
        bool red;

    public:

        Circle(Point p1, double r, int c_h, int c_w, bool color_red = false) : center(Point(p1.getX() * c_w, p1.getY() * c_h)), radius(r), canvas_height(c_h), canvas_width(c_w), red(color_red) {}

        void draw_circle(int **pixels) {
            int x, y, xmax, y2, y2_new, ty;
            xmax = (int) (radius * 0.70710678); // maximum x at radius/sqrt(2) + x0
            y = radius;
            y2 = y * y;
            ty = (2 * y) - 1; y2_new = y2;
            
            for (x = 0; x <= xmax + 1; x++) {
                if ((y2 - y2_new) >= ty) {
                    y2 -= ty;
                    y -= 1;
                    ty -= 2;
                }

                color_pixel(pixels, x + center.getX(), y + center.getY(), canvas_height, canvas_width, red);
                color_pixel(pixels, x + center.getX(), -y + center.getY(), canvas_height, canvas_width, red);
                color_pixel(pixels, -x + center.getX(), y + center.getY(), canvas_height, canvas_width, red);
                color_pixel(pixels, -x + center.getX(), -y + center.getY(), canvas_height, canvas_width, red);
                color_pixel(pixels, y + center.getX(), x + center.getY(), canvas_height, canvas_width, red);
                color_pixel(pixels, y + center.getX(), -x + center.getY(), canvas_height, canvas_width, red);
                color_pixel(pixels, -y + center.getX(), x + center.getY(), canvas_height, canvas_width, red);
                color_pixel(pixels, -y + center.getX(), -x + center.getY(), canvas_height, canvas_width, red);

                y2_new -= (2 * x) - 3;
            }
        }
};

class Triangle {
    private:
        vector<Line> lines;

        // semiperimeter
        double s;

        double incircle_radius;
        double circumcircle_radius;

        Point incircle_center;
        Point circumcircle_center;
        Point centroid;

        int canvas_height;
        int canvas_width;

        // lengths of the lines
        double a;
        double b;
        double c;

    public: 
        // all calculations done in [0,1]
        Triangle(Point p1, Point p2, Point p3, int c_height, int c_width) {
            canvas_height = c_height;
            canvas_width = c_width;

            lines.push_back(Line(p1, p2, canvas_height, canvas_width));
            lines.push_back(Line(p2, p3, canvas_height, canvas_width));
            lines.push_back(Line(p3, p1, canvas_height, canvas_width));

            a = lines[0].getLength();
            b = lines[1].getLength();
            c = lines[2].getLength();

            s = 0.5 * (a + b + c);

            incircle_radius = sqrt((s - a) * (s - b) * (s - c) / s);
            circumcircle_radius = a * b * c / (4 * incircle_radius * s);

            // circumcircle: find the intersection of the perpendicular bisectors of any two of the triangles sides
            Point midpoint1((lines[0].getSecond().getX() + lines[0].getFirst().getX()) / 2, (lines[0].getSecond().getY() + lines[0].getFirst().getY()) / 2);
            Point midpoint2((lines[1].getSecond().getX() + lines[1].getFirst().getX()) / 2, (lines[1].getSecond().getY() + lines[1].getFirst().getY()) / 2);

            double slope1;
            if(lines[0].getSlope() == 0) {
                slope1 = DBL_MAX;
            }
            else if(lines[0].getSlope() == DBL_MAX) {
                slope1 = 0;
            } 
            else {
                slope1 = -(1 / lines[0].getSlope());
            }

            double slope2;
            if(lines[1].getSlope() == 0) {
                slope2 = DBL_MAX;
            }
            else if(lines[1].getSlope() == DBL_MAX) {
                slope2 = 0;
            } 
            else {
                slope2 = -(1 / lines[1].getSlope());
            }

            circumcircle_center = Line(midpoint1, slope1, canvas_height, canvas_width).find_intersection(Line(midpoint2, slope2, canvas_height, canvas_width));

            // incircle
            // formula from https://artofproblemsolving.com/wiki/index.php/Incircle#:~:text=For%20a%20triangle%2C%20the%20center,the%20intersection%20of%20angle%20bisectors.

            double incenter_x = (p1.getX() * b + p2.getX() * c + p3.getX() * a) / (a + b + c);
            double incenter_y = (p1.getY() * b + p2.getY() * c + p3.getY() * a) / (a + b + c);
            incircle_center = Point(incenter_x, incenter_y);
            
            // centroid

            double centroid_x = (p1.getX() + p2.getX() + p3.getX()) / 3;
            double centroid_y = (p1.getY() + p2.getY() + p3.getY()) / 3;
            centroid = Point(centroid_x, centroid_y);
        }

        double get_area() {
            return sqrt(s * (s - a) * (s - b) * (s - c));
        }

        void draw_triangle(int **pixels) {
            for(Line line : lines) {
                line.draw_line(pixels);
            }
        }
};

// write all points to file
void write_file(list<Point> points, string filename) {
    ofstream outfile;
    outfile.open(filename); // open in overwrite mode

    outfile << std::fixed << std::showpoint;
    outfile << std::setprecision(23);

    int count = 0;

    for(std::list<Point>::iterator it = points.begin(); it != points.end(); ++it, count++) {
        outfile << it->getX() << "  " << it->getY();
        
        if(count != points.size() - 1) {
            outfile << endl;
        }
    }
    outfile.close();
}

void write_file_with_area(vector<Point> points, double area, string filename) {
    ofstream outfile;
    outfile.open(filename, std::ios_base::app); // open in append mode

    outfile << std::fixed << std::showpoint;
    outfile << std::setprecision(23);

    for(int i = 0; i < points.size(); i++) {
        Point p = points[i];
        outfile << "("  << p.getX() << "," << p.getY() << ")";
        
        if(i != points.size() - 1) {
            outfile << " , ";
        }
    }
    outfile << " Area = " << area << endl;
    outfile.close();
}

void part1() {
    srand((unsigned) time(0));

    // temporary, used for drawing only
    int height = 800;
    int width = 800; 
    int num_points = 60;
    list<Point> points;   

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

    for(int i = 0; i < num_points; i++) {
        points.push_back(Point(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX));
    }

    write_file(points, "points.txt");

    // find minimum distance between points using brute-force algorithm

    PointPair minimum(points.front(), points.back(), points.front().distance(points.back()));
    int count = 1;

    for(list<Point>::iterator i = points.begin(); i != points.end(); ++i, count++) {
        list<Point>::iterator j = points.begin();
        advance(j, count);

        for(; j != points.end(); ++j) {
            double dist = i->distance(*j);
            
            if(dist < minimum.getDistance()) {
                minimum = PointPair(*i, *j, dist);
            }
        }
    }

    for(Point p : points) {
        Circle pt_circle(p, 3, height, width);
        pt_circle.draw_circle(pixels);
    }

    Circle best_point_first(minimum.getFirst(), 3, height, width, true);
    Circle best_point_second(minimum.getSecond(), 3, height, width, true);

    Circle best_point_first_radius2(minimum.getFirst(), 2, height, width, true);
    Circle best_point_second_radius2(minimum.getSecond(), 2, height, width, true);

    best_point_first.draw_circle(pixels);
    best_point_second.draw_circle(pixels);

    best_point_first_radius2.draw_circle(pixels);
    best_point_second_radius2.draw_circle(pixels);

    write_board(pixels, height, width);

    // cleanup

    for(int i = 0; i < height; i++) {
        delete[] pixels[i];
    }

    delete[] pixels;
}

vector<Point> read_file() {
    ifstream file;
    file.open("points.txt");

    string contents;
    getline(file, contents);
    contents += " "; // makes it easier to parse the points

    // cout << contents << endl;

    vector<int> spaces;
    for(int i = 0; i < contents.length(); i++) {
        if(string(1, contents[i]) == " ") {
            spaces.push_back(i);
        }
    }

    int prev = 0;
    vector<Point> points;

    for(int i : spaces) {
        string point_str = contents.substr(prev, i - prev);
        prev = i;

        if(point_str.find("(") == string::npos) {
            continue;
        }
        
        int left = point_str.find("(");
        int center = point_str.find(",");
        int right = point_str.find(")");

        double x = stod(point_str.substr(left + 1, center - left - 1));
        double y = stod(point_str.substr(center + 1, right - center - 1));

        points.push_back(Point(x, y));
    }

    return points;
}

int main() {
    part1();
}
