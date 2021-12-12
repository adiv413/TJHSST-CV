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
#include <chrono>
#include <sstream>
#include <unordered_map>
#include <stack>

using namespace std;

enum class Color {
    WHITE,
    BLACK,
    RED,
    GREEN,
    BLUE
};

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

        bool operator==(const Point &other) const {
            return (x == other.x && y == other.y);
        }

        double distance(Point other) {
            return sqrt(pow(other.x - x, 2) + pow(other.y - y, 2));
        }    
};

vector<Point> convex_hull;

// set pixel to be 1 (black pixel)
void color_pixel(int **pixels, int x, int y, int height, int width, Color color = Color::BLACK) {
    if(x < 0 || x > width - 1 || y < 0 || y > height - 1);
    else {
        pixels[y][x] = (int) color;// reverse the order because of arrays
    }
}

// scales point to canvas dimensions
void color_pixel(int **pixels, Point p, int height, int width, Color color = Color::BLACK) {
    int x = static_cast<int>(p.getX() * width);
    int y = static_cast<int>(p.getY() * height);

    if(x < 0 || x > width - 1 || y < 0 || y > height - 1);
    else {
        pixels[y][x] = (int) color;// reverse the order because of arrays
    }
}

// write the board to a ppm file
void write_board(int **pixels, int height, int width, string filename, string idx = "") {
    ofstream outfile;
    outfile.open(filename + idx + ".ppm");

    // header
    outfile << "P3 " << width << " " << height << " " << 255 << endl;

    // content
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(pixels[i][j] == (int) Color::WHITE) {
                // ppm files require one output for r, g, and b
                outfile << "255 255 255 ";
            }
            else if(pixels[i][j] == (int) Color::BLACK) { // 1 represents a black pixel
                outfile << "0 0 0 ";
            }
            else if(pixels[i][j] == (int) Color::RED) { // 2 represents a red pixel
                outfile << "255 0 0 ";
            }
            else if(pixels[i][j] == (int) Color::GREEN) { 
                outfile << "0 255 0 ";
            }
            else if(pixels[i][j] == (int) Color::BLUE) { 
                outfile << "0 0 255 ";
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
            Point scaled_first((int) (first.getX() * canvas_width), (int) (first.getY() * canvas_height));
            Point scaled_second((int) (second.getX() * canvas_width), (int) (second.getY() * canvas_height));

            // check 0 slope
            // if((-threshold <= slope) && (slope <= threshold)) {
            //     scaled_first = Point(static_cast<double>(canvas_width), first.getY() * canvas_height);
            //     scaled_second = Point(-static_cast<double>(canvas_width), first.getY() * canvas_height);
            // }
            // // check infinite slope
            // else if(100000.0 <= abs(slope)) {
            //     scaled_first = Point(first.getX() * canvas_width, static_cast<double>(canvas_height));
            //     scaled_second = Point(first.getX() * canvas_width, -static_cast<double>(canvas_height));
            // }
            // else {
            //     // check right side
            //     double right_y = slope * (canvas_width - first.getX() * canvas_width) + first.getY() * canvas_height;
            //     if((0 <= right_y) && (right_y <= canvas_height)) {
            //         // hits the right side of the screen
            //         scaled_first = Point(canvas_width, right_y);
            //     }
            //     // check top
            //     double top_x = (canvas_height - first.getY() * canvas_height) / slope + first.getX() * canvas_width;
            //     if((0 <= top_x) && (top_x <= canvas_height)) {
            //         // hits the top of the screen
            //         if((scaled_first.getX() == 0.0) && (scaled_first.getY() == 0.0)) {
            //             scaled_first = Point(top_x, canvas_height);
            //         }
            //         else {
            //             scaled_second = Point(top_x, canvas_height);
            //         }
            //     }
            //     // check left
            //     double left_y = slope * (0 - first.getX() * canvas_width) + first.getY() * canvas_height;
            //     if((0 <= left_y) && (left_y <= canvas_height)) {
            //         // hits the left side of the screen
            //         if((scaled_first.getX() == 0.0) && (scaled_first.getY() == 0.0)) {
            //             scaled_first = Point(0, left_y); 
            //         }
            //         else if((scaled_second.getX() == 0.0) && (scaled_second.getY() == 0.0)){
            //             scaled_second = Point(0, left_y); 
            //         }
            //     }
            //     // check bottom
            //     double bottom_x = (0 - first.getY() * canvas_height) / slope + first.getX() * canvas_width;
            //     if((0 <= bottom_x) && (bottom_x <= canvas_height) && ((scaled_second.getX() == 0.0) && (scaled_second.getY() == 0.0))) {
            //         // hits the top of the screen
            //         scaled_second = Point(bottom_x, 0);
            //     }
            // }

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

class Circle {
    private:
        Point center;
        double radius;
        int canvas_height;
        int canvas_width;
        Color color;

    public:

        Circle(Point p1, double r, int c_h, int c_w, Color c = Color::BLACK) : center(Point(p1.getX() * c_w, p1.getY() * c_h)), radius(r), canvas_height(c_h), canvas_width(c_w), color(c) {}

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

                color_pixel(pixels, x + center.getX(), y + center.getY(), canvas_height, canvas_width, color);
                color_pixel(pixels, x + center.getX(), -y + center.getY(), canvas_height, canvas_width, color);
                color_pixel(pixels, -x + center.getX(), y + center.getY(), canvas_height, canvas_width, color);
                color_pixel(pixels, -x + center.getX(), -y + center.getY(), canvas_height, canvas_width, color);
                color_pixel(pixels, y + center.getX(), x + center.getY(), canvas_height, canvas_width, color);
                color_pixel(pixels, y + center.getX(), -x + center.getY(), canvas_height, canvas_width, color);
                color_pixel(pixels, -y + center.getX(), x + center.getY(), canvas_height, canvas_width, color);
                color_pixel(pixels, -y + center.getX(), -x + center.getY(), canvas_height, canvas_width, color);

                y2_new -= (2 * x) - 3;
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

vector<Point> read_file() {
    ifstream file;
    file.open("points.txt");
    string contents;
    
    vector<Point> points;

    while(getline(file, contents)) {
        int space = 0;

        for(int i = 0; i < contents.length() - 1; i++) {
            if(string(2, contents[i]) == "  ") {
                space = i;
                break;
            }
        }

        if(space == 0) {
            continue;
        }

    
        string x_str = contents.substr(0, space);
        string y_str = contents.substr(space + 2);
        
        double x = stod(x_str);
        double y = stod(y_str);

        points.push_back(Point(x, y));

    }

    return points;
}

void generate_points(int num_points) {
    srand((unsigned) time(0));
    list<Point> points;   

    for(int i = 0; i < num_points; i++) {
        points.push_back(Point(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX));
    }

    write_file(points, "points.txt");
}

bool is_right_of_line(Point l1, Point l2, Point p){
     return ((l2.getX() - l1.getX()) * (p.getY() - l1.getY()) - (l2.getY() - l1.getY()) * (p.getX() - l1.getX())) < 0;
}

bool is_point_in_triangle(Point a, Point b, Point c, Point p, int height, int width) {
    vector<Point> points = {a, b, c, p};
    double error = 0.000000000000001;

    // patterns to check triangles such that each point is not in the triangle formed by the other 3
    vector<vector<int>> patterns = {
        {0, 1, 2, 3}
    };

    // check each triangle, check if point is inside

    for(vector<int> pattern : patterns) {
        // check that the area of the smaller triangles, compare to bigger triangle

        vector<vector<int>> triangle_point_patterns = {
            {pattern[0], pattern[1]}, 
            {pattern[0], pattern[2]},
            {pattern[1], pattern[2]},  
        };
        int test_point = pattern[3];
        double area_sum = 0.0;

        for(vector<int> triangle_pattern : triangle_point_patterns) {
            area_sum += Triangle(points[triangle_pattern[0]], points[triangle_pattern[1]], points[test_point], height, width).get_area();
        }

        double big_triangle_area = Triangle(points[pattern[0]], points[pattern[1]], points[pattern[2]], height, width).get_area();

        // if sum of areas of smaller triangles = area of big triangle, point is inside

        if(big_triangle_area - error <= area_sum && area_sum <= big_triangle_area + error) {
            // point lies inside the triangle
            return true;

        }
        
        return false;
    }

    return false;
}

void find_hull(vector<Point> points, Point p, Point q, int height, int width, int **pixels) {
    if(points.size() == 0) {
        return;
    }
    if(points.size() == 1) {
        // insert point between p and q
        int index = 0;
        for(int i = 0; i < convex_hull.size(); i++) {
            if(convex_hull[i] == p) {
                index = i;
                break;
            }
        }

        int index2 = index;
        for(int i = index; i < convex_hull.size(); i++) {
            if(convex_hull[i] == q) {
                index2 = i;
                break;
            }
        }
        convex_hull.insert(convex_hull.begin() + max(index, index2), points[0]);

        return;
    }

    Line pq(p, q, height, width);

    double farthest_distance = 0.0;
    Point farthest_point = points[0];

    for(Point p1 : points) {
        Line l1 = pq.make_perpendicular(p1);
        Point intersection = l1.find_intersection(pq);
        double dist = p1.distance(intersection);

        if(dist > farthest_distance) {
            farthest_distance = dist;
            farthest_point = p1;
        }
    }

    // insert farthest_point between p and q

    int index = 0;
    for(int i = 0; i < convex_hull.size(); i++) {
        if(convex_hull[i] == p) {
            index = i;
            break;
        }
    }

    int index2 = index;
    for(int i = index; i < convex_hull.size(); i++) {
        if(convex_hull[i] == q) {
            index2 = i;
            break;
        }
    }
    
    convex_hull.insert(convex_hull.begin() + max(index, index2), farthest_point);
    vector<Point> triangle_points = {p, q, farthest_point};

    vector<Point> s1;
    vector<Point> s2;

    for(Point p1 : points) {
        // if p1 is not p, q, or farthest_point
        if(find(triangle_points.begin(), triangle_points.end(), p1) == triangle_points.end()) {
            if(!is_point_in_triangle(p, q, farthest_point, p1, height, width)) {
                if(is_right_of_line(p, farthest_point, p1)) {
                    s1.push_back(p1);
                }
                if(is_right_of_line(farthest_point, q, p1)) {
                    s2.push_back(p1);
                }
            }
        }
    }

    find_hull(s1, p, farthest_point, height, width, pixels);
    find_hull(s2, farthest_point, q, height, width, pixels);
}

void quick_hull(vector<Point> points) {
    int height = 400;
    int width = 400; 

    int **pixels;
    pixels = new int*[height];
    
    for(int i = 0; i < height; i++) {
        pixels[i] = new int[width];

        // initialize all values in pixel array to default value of 0 (white pixel)
        for(int j = 0; j < width; j++) {
            pixels[i][j] = 0;
        }
    }

    // find the topmost and bottommost point in the list
    Point top = points[0];
    Point bottom = points[0];

    for(int i = 1; i < points.size(); i++) {
        if(points[i].getY() > top.getY()) {
            top = points[i];
        }
        else if(points[i].getY() < bottom.getY()) {
            bottom = points[i];
        }
    }

    // add the topmost and bottommost points to the convex hull
    convex_hull.push_back(bottom);
    convex_hull.push_back(top);
    convex_hull.push_back(bottom);

    // remove the topmost and bottommost points from the list
    points.erase(std::remove(points.begin(), points.end(), top), points.end());
    points.erase(std::remove(points.begin(), points.end(), bottom), points.end());

    Circle c1(top, 2, height, width, Color::BLUE);
    c1.draw_circle(pixels);
    Circle c2(bottom, 2, height, width, Color::BLUE);
    c2.draw_circle(pixels);

    vector<Point> s1;
    vector<Point> s2;

    for(Point p : points) {
        if(is_right_of_line(bottom, top, p)) {
            s1.push_back(p);
        }
        else {
            s2.push_back(p);
        }
        Circle c(p, 2, height, width, Color::BLUE);
        c.draw_circle(pixels);
    }

    find_hull(s1, bottom, top, height, width, pixels);
    find_hull(s2, top, bottom, height, width, pixels);

    for(int i = 0; i < convex_hull.size(); i++) {
        Point p = convex_hull[i];
        if(i != 0) {
            Line l(convex_hull[i-1], convex_hull[i], height, width);
            l.draw_line(pixels);
        }
        
        Circle c(p, 4, height, width, Color::RED);
        c.draw_circle(pixels);
    }


    write_board(pixels, height, width, "quickhull");

    for(int i = 0; i < height; i++) {
        delete[] pixels[i];
    }

    delete[] pixels;

}

void part1() {
    generate_points(60);
    vector<Point> points = read_file();
    quick_hull(points);
}

void graham_scan(vector<Point> points) {
    int height = 400;
    int width = 400; 

    int **pixels;
    pixels = new int*[height];
    
    for(int i = 0; i < height; i++) {
        pixels[i] = new int[width];

        // initialize all values in pixel array to default value of 0 (white pixel)
        for(int j = 0; j < width; j++) {
            pixels[i][j] = 0;
        }
    }

    Point p = points[0];
    for(Point p1 : points) {
        Circle c1(p1, 2, height, width, Color::BLUE);
        c1.draw_circle(pixels);

        if(p1.getY() < p.getY()) {
            p = p1;
        }
        if(p1.getY() == p.getY() && p1.getX() < p.getX()) {
            p = p1;
        }
    }

    // sorting function must be a lambda in order to use p as a variable in sort function
    // sort points by polar angle
    sort(points.begin(), points.end(), [&p](Point p1, Point p2) -> bool {
        double angle1 = atan2(p1.getY() - p.getY(), p1.getX() - p.getX());
        double angle2 = atan2(p2.getY() - p.getY(), p2.getX() - p.getX());
        return angle1 < angle2;
    });

    stack<Point> s;
    s.push(points[0]);
    s.push(points[1]);

    for(int i = 2; i < points.size(); i++) {
        Point current = points[i];
        while(s.size() >= 2) {
            Point tp = s.top();
            s.pop();
            Point prev = s.top();
            s.push(tp);

            if(is_right_of_line(prev, tp, current)) {
                s.pop();
            }
            else {
                break;
            }
        }
        s.push(current);
    }

    Point prev = s.top();
    Point tp = prev;
    s.pop();

    Circle c1(prev, 2, height, width, Color::RED);
    c1.draw_circle(pixels);

    while(!s.empty()) {
        Point p1 = s.top();
        s.pop();

        Line l(prev, p1, height, width);
        l.draw_line(pixels);

        Circle c1(p1, 2, height, width, Color::RED);
        c1.draw_circle(pixels);

        prev = p1;
    }

    Line l1(prev, tp, height, width);
    l1.draw_line(pixels);

    write_board(pixels, height, width, "grahamscan");

    for(int i = 0; i < height; i++) {
        delete[] pixels[i];
    }

    delete[] pixels;
}

void part2() {
    vector<Point> points = read_file();
    graham_scan(points);
}

int main() {
    part2();
}