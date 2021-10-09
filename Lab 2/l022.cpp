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

// set pixel to be 1 (black pixel)
void color_pixel(int **pixels, int x, int y, int height, int width) {
    if(x < 0 || x > width - 1 || y < 0 || y > height - 1);
    else {
        pixels[y][x] = 1; // reverse the order because of arrays
    }
}

// scales point to canvas dimensions
void color_pixel(int **pixels, Point p, int height, int width) {
    int x = static_cast<int>(p.getX() * width);
    int y = static_cast<int>(p.getY() * height);

    if(x < 0 || x > width - 1 || y < 0 || y > height - 1);
    else {
        pixels[y][x] = 1; // reverse the order because of arrays
    }
}

// write the board to a ppm file
void write_board(int **pixels, int height, int width, string idx) {
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

            if(pixels[i][j] == 1) { // 1 represents a black pixel
                outfile << "0 0 0 ";
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
                cout << "0" << endl;
                scaled_first = Point(static_cast<double>(canvas_width), first.getY() * canvas_height);
                scaled_second = Point(-static_cast<double>(canvas_width), first.getY() * canvas_height);
            }
            // check infinite slope
            else if(100000.0 <= abs(slope)) {
                cout << "1" << endl;
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

// write all points to points.txt
void write_file(vector<Point> points) {
    ofstream outfile;
    outfile.open("points.txt");

    outfile << std::fixed << std::showpoint;
    outfile << std::setprecision(17);

    for(int i = 0; i < points.size(); i++) {
        Point p = points[i];
        outfile << "("  << p.getX() << "," << p.getY() << ")";
        
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
    while((p1.getX() == p2.getX() && p2.getX() == p3.getX()) || (p1.getY() == p2.getY() && p2.getY() == p3.getY())) {
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

vector<Point> read_file() {
    ifstream file;
    file.open("points.txt");

    string contents;
    getline(file, contents);
    contents += " "; // makes it easier to parse the points

    cout << contents << endl;

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

vector<Point> get_smallest_square(vector<Point> points, int height, int width) {
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

    // choose the first and second elements of points as the opposite points


    for(int idx = 0; idx < 3; idx++) {
        Point a;
        Point b;
        Point c;
        Point d;

        // choose ab to start
        if(idx == 0) {
            a = points[0];
            b = points[1];
            c = points[2];
            d = points[3];
        }
        // choose ac to start
        else if(idx == 1) {
            a = points[0];
            b = points[2];
            c = points[1];
            d = points[3];
        }
        // choose ad to start
        else {
            a = points[0];
            b = points[3];
            c = points[2];
            d = points[1];
        }
    

        Line ab = Line(a, b, height, width);

        // draw line perpendicular to a through c
        Line ab_perp = ab.make_perpendicular(c);
        
        // pick point e on line ab_perp such that ce is the same length as ab

        double distance = a.distance(b);
        double delta_x;
        double delta_y;

        if(ab_perp.getSlope() == 0) {
            delta_x = distance;
            delta_y = 0;
        }
        else if(ab_perp.getSlope() == DBL_MAX) {
            delta_x = 0;
            delta_y = distance;
        }
        else {
            double vx = ab_perp.getSecond().getX() - ab_perp.getFirst().getX();
            double vy = ab_perp.getSecond().getY() - ab_perp.getFirst().getY();

            double ux = vx / ab_perp.getLength();
            double uy = vy / ab_perp.getLength();

            delta_x = ux * distance;
            delta_y = uy * distance;

        }

        // two squares: one with c + dx,dy, one with c - dx,dy

        for(int i = 0; i < 2; i++) {
            Point e;

            if(i == 0) {
                e = Point(c.getX() + delta_x, c.getY() + delta_y);
            }
            else {
                e = Point(c.getX() - delta_x, c.getY() - delta_y);
            }

            Line ce(c, e, height, width);
            Line de(d, e, height, width);

            Line af = de.make_perpendicular(a);
            Line bg = de.make_perpendicular(b);
            Line ch = af.make_perpendicular(c);

            // from here, the square is FHC1G
            Point f = af.find_intersection(de);
            Point h = af.find_intersection(ch);
            Point c1 = bg.find_intersection(ch);
            Point g = bg.find_intersection(de);

            double side_length = f.distance(h);
            double area = side_length * side_length;

            cout << "area for board " << idx * 2 + i << ": " << area << endl;

            // ab.draw_line(pixels);
            // ab_perp.draw_line(pixels);
            de.draw_line(pixels);
            af.draw_line(pixels);
            bg.draw_line(pixels);
            ch.draw_line(pixels);

            color_pixel(pixels, a, height, width);
            color_pixel(pixels, b, height, width);
            color_pixel(pixels, c, height, width);
            color_pixel(pixels, d, height, width);
            color_pixel(pixels, e, height, width);

            write_board(pixels, height, width, to_string(idx * 2 + i));

            // erase the board
            for(int i = 0; i < height; i++) {
                for(int j = 0; j < width; j++) {
                    pixels[i][j] = 0;
                }
            }
        }
    }
    
    vector<Point> ret;
    return ret;
}

void part2() {
    vector<Point> points = read_file();
    cout << setprecision(17);

    for(auto i : points) {
        cout << i.getX() << " " << i.getY() << endl;
    }

    get_smallest_square(points, 100, 100);
}

int main() {
    part1();
    part2();
}