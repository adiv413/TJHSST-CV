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
#include <iomanip>

using namespace std;

// set pixel to be 1 (black pixel)
void color_pixel(int **pixels, int x, int y, int height, int width) {
    if(x < 0 || x > width - 1 || y < 0 || y > height - 1);
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

        int canvas_height;
        int canvas_width;

        // draw a line between any two given points
        void draw_line(int **pixels) {
            Point scaled_first((int) (first.x * canvas_width), (int) (first.y * canvas_height));
            Point scaled_second((int) (second.x * canvas_width), (int) (second.y * canvas_height));

            int dx = scaled_second.x - scaled_first.x; // x2 - x1
            int dy = scaled_second.y - scaled_first.y; // y2 - y1
            int error = abs(dy) - abs(dx);

            if(error < 0 && scaled_second.x <= scaled_first.x) { // reverse the points
                _draw_line_bresenham(pixels, scaled_second, scaled_first, canvas_height, canvas_width);
            }
            else if(error > 0 && scaled_second.y <= scaled_first.y) {
                _draw_line_bresenham(pixels, scaled_second, scaled_first, canvas_height, canvas_width);
            }
            else {
                _draw_line_bresenham(pixels, scaled_first, scaled_second, canvas_height, canvas_width);
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
            return Line(first, second, canvas_height, canvas_width);
        }

        // point-point definition
        Line(Point p1, Point p2, int c_height, int c_width) : first(p1), second(p2), length(p1.distance(p2)), canvas_height(c_height), canvas_width(c_width) {
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

        // point-slope form
        Line(Point p1, double m, int c_height, int c_width) : first(p1), slope(m), second(Point(p1.x + 1, p1.y + slope)), canvas_height(c_height), canvas_width(c_width) {}

    private:
        // bresenham algorithm to draw a line between two given points
        // note: the points must be in increasing order if x1 < x2 and y1 < y2
        void _draw_line_bresenham(int **pixels, Point p1, Point p2, int canvas_height, int canvas_width) {
            int dx = p2.x - p1.x; // x2 - x1
            int dy = p2.y - p1.y; // y2 - y1
            int error = abs(dy) - abs(dx);  // negative: x driven, positive: y driven

            if(error < 0) { // x-driven
                int j = p1.y; // y1

                if(dy < 0) { // going down
                    for(int i = p1.x; i <= p2.x; i++) {
                        color_pixel(pixels, i, j, canvas_height, canvas_width);

                        if(error >= 0) {
                            j -= 1;
                            error -= dx;
                        }
                        error += -dy;
                    }
                }
                else { // normal case x driven
                    for(int i = p1.x; i <= p2.x; i++) {
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
                    int i = p1.x;

                    for(int j = p1.y; j <= p2.y; j++) {
                        color_pixel(pixels, i, j, canvas_height, canvas_width);

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
    public: 
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

        // all calculations done in [0,1]
        Triangle(Point p1, Point p2, Point p3, int c_height, int c_width) {
            canvas_height = c_height;
            canvas_width = c_width;

            lines.push_back(Line(p1, p2, canvas_height, canvas_width));
            lines.push_back(Line(p2, p3, canvas_height, canvas_width));
            lines.push_back(Line(p3, p1, canvas_height, canvas_width));

            a = lines[0].length;
            b = lines[1].length;
            c = lines[2].length;

            s = 0.5 * (a + b + c);

            incircle_radius = sqrt((s - a) * (s - b) * (s - c) / s);
            circumcircle_radius = a * b * c / (4 * incircle_radius * s);

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
                slope1 = -(1 / lines[0].slope);
            }

            double slope2;
            if(lines[1].slope == 0) {
                slope2 = DBL_MAX;
            }
            else if(lines[1].slope == DBL_MAX) {
                slope2 = 0;
            } 
            else {
                slope2 = -(1 / lines[1].slope);
            }

            circumcircle_center = Line(midpoint1, slope1, canvas_height, canvas_width).find_intersection(Line(midpoint2, slope2, canvas_height, canvas_width));

            // incircle
            // formula from https://artofproblemsolving.com/wiki/index.php/Incircle#:~:text=For%20a%20triangle%2C%20the%20center,the%20intersection%20of%20angle%20bisectors.

            double incenter_x = (p1.x * b + p2.x * c + p3.x * a) / (a + b + c);
            double incenter_y = (p1.y * b + p2.y * c + p3.y * a) / (a + b + c);
            incircle_center = Point(incenter_x, incenter_y);
            
            // centroid

            double centroid_x = (p1.x + p2.x + p3.x) / 3;
            double centroid_y = (p1.y + p2.y + p3.y) / 3;
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

// write all points to points.txt
void write_file(vector<Point> points) {
    ofstream outfile;
    outfile.open("points.txt");

    outfile << std::fixed << std::showpoint;
    outfile << std::setprecision(17);

    for(int i = 0; i < points.size(); i++) {
        Point p = points[i];
        outfile << "("  << p.x << "," << p.y << ")";
        
        if(i != points.size() - 1) {
            outfile << " , ";
        }
    }
    outfile.close();
}

void part1() {
    srand((unsigned) time(0));

    // temporary, used for drawing only
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

    // randomly generate points in [0,1]

    Point p1(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX);
    Point p2(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX);
    Point p3(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX);

    // check that points are not collinear
    while((p1.x == p2.x && p2.x == p3.x) || (p1.y == p2.y && p2.y == p3.y)) {
        p3 = Point(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX);
    }

    // point to check

    Point p4(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX);

    vector<Point> points = {p1, p2, p3, p4};
    double error = 0.000000000000001;

    // patterns to check triangles such that each point is not in the triangle formed by the other 3
    vector<vector<int>> patterns = {
        {0, 1, 2, 3}, 
        {0, 1, 3, 2}, 
        {0, 2, 3, 1}, 
        {1, 2, 3, 0}
    };

    bool still_running = true;

    while(still_running) {
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
                still_running = true;
                break;

            }
            else {
                // break out of the loop
                still_running = false;
            }

            // test point on the line before submitting 
        }

        if(still_running) {
            points[3] = Point(((double) rand()) / RAND_MAX, ((double) rand()) / RAND_MAX);
        }

    }

    write_file(points);

    // cleanup

    for(int i = 0; i < height; i++) {
        delete[] pixels[i];
    }

    delete[] pixels;
}

int main() {
    part1();
}