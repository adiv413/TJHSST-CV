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

using namespace std;

double total_time = 0;

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
        PointPair(Point p1, Point p2) : first(p1), second(p2), distance(p1.distance(p2)) {}

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
    outfile.open("points" + idx + ".ppm");

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
        // cout << contents << endl;
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

PointPair brute_force(list<Point> points) {
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

    return minimum;
}

string part1() {
    srand((unsigned) time(0));

    // temporary, used for drawing only
    int height = 800;
    int width = 800; 
    int num_points = 400000;
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










    // long double curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // PointPair minimum = brute_force(points);
    // long double curr_time1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    stringstream outfile_text;

    // outfile_text << std::fixed << std::showpoint;
    // outfile_text << setprecision(23);

    // // write to file
    // outfile_text << "Brute Force: " << endl;
    // outfile_text << "Distance: " << minimum.getDistance() << ", Point 1: (" << minimum.getFirst().getX() << ", " << minimum.getFirst().getY() << "), Point 2: (" << minimum.getSecond().getX() << ", " << minimum.getSecond().getY() << ")" << endl;
    // outfile_text << "Time: " << (curr_time1 - curr_time) / 1000.0 << " seconds" << endl;

    // for(Point p : points) {
    //     Circle pt_circle(p, 3, height, width);
    //     pt_circle.draw_circle(pixels);
    // }

    // Circle best_point_first(minimum.getFirst(), 3, height, width, true);
    // Circle best_point_second(minimum.getSecond(), 3, height, width, true);

    // Circle best_point_first_radius2(minimum.getFirst(), 2, height, width, true);
    // Circle best_point_second_radius2(minimum.getSecond(), 2, height, width, true);

    // best_point_first.draw_circle(pixels);
    // best_point_second.draw_circle(pixels);

    // best_point_first_radius2.draw_circle(pixels);
    // best_point_second_radius2.draw_circle(pixels);

    // write_board(pixels, height, width);


















    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // cleanup

    for(int i = 0; i < height; i++) {
        delete[] pixels[i];
    }

    delete[] pixels;
    cout << outfile_text.str() << endl;
    return outfile_text.str();
}

bool compare_point_by_x(Point x, Point y) {
    return x.getX() < y.getX();
}

bool compare_point_by_y(Point x, Point y) {
    return x.getY() < y.getY();
}

// @param left: index of the left bound for the split
// @param right: index of the right bound for the split

PointPair recur(int left, int right, vector<Point>& points) {
    if(right - left == 2) {
        list<Point> new_points = {points[left], points[left + 1], points[right]};
        PointPair minimum = brute_force(new_points);

        return minimum;
    }

    if(right - left == 1) {
        return PointPair(points[left], points[right]);
    }

    int center = (left + right) / 2;

    PointPair pp1 = recur(left, center, points);
    PointPair pp2 = recur(center, right, points);

    // long double start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    PointPair absolute_min = pp1;

    if(pp1.getDistance() > pp2.getDistance()) {
        absolute_min = pp2;
    }

    // check band of length absolute_min.getDistance()

    vector<Point> band_points;

    double distance = absolute_min.getDistance();   

    for(int i = center; i >= left; i--) {
        if(abs(points[i].getX() - points[center].getX()) < distance) {
            band_points.push_back(points[i]);
        }
        else {
            break;
        }
    }

    for(int i = center + 1; i <= right; i++) {
        if(abs(points[i].getX() - points[center].getX()) < distance) {
            band_points.push_back(points[i]);
        }
        else {
            break;
        }
    }


    if(band_points.size() > 1) {
        PointPair band_minimum(band_points.front(), band_points.back(), band_points.front().distance(band_points.back()));
        int count = 1;

        for(vector<Point>::iterator i = band_points.begin(); i != band_points.end(); ++i, count++) {
            vector<Point>::iterator j = band_points.begin();
            advance(j, count);

            for(; j != band_points.end(); ++j) {
                double dist = i->distance(*j);
                
                if(dist < band_minimum.getDistance()) {
                    band_minimum = PointPair(*i, *j, dist);
                }
            }
        }
        
        if(band_minimum.getDistance() < absolute_min.getDistance()) {
            return band_minimum;
        }
        else {
            return absolute_min;
        }
    }
    else {
        return absolute_min;
    }
}

// @param left: index of the left bound for the split
// @param right: index of the right bound for the split

PointPair recur_complete(int left, int right, vector<Point>& points) {
    if(right - left == 2) {
        list<Point> new_points = {points[left], points[left + 1], points[right]};
        PointPair minimum = brute_force(new_points);

        return minimum;
    }

    if(right - left == 1) {
        return PointPair(points[left], points[right]);
    }

    int center = (left + right) / 2;

    PointPair pp1 = recur_complete(left, center, points);
    PointPair pp2 = recur_complete(center, right, points);

    PointPair absolute_min = pp1;

    if(pp1.getDistance() > pp2.getDistance()) {
        absolute_min = pp2;
    }

    // check band of length absolute_min.getDistance()

    vector<Point> band_points;

    double distance = absolute_min.getDistance();   

    for(int i = center; i >= left; i--) {
        if(abs(points[i].getX() - points[center].getX()) < distance) {
            band_points.push_back(points[i]);
        }
        else {
            break;
        }
    }

    for(int i = center + 1; i <= right; i++) {
        if(abs(points[i].getX() - points[center].getX()) < distance) {
            band_points.push_back(points[i]);
        }
        else {
            break;
        }
    }

    if(band_points.size() > 1) {
        sort(band_points.begin(), band_points.end(), compare_point_by_y);
        PointPair band_minimum(band_points.front(), band_points.back(), band_points.front().distance(band_points.back()));
        int count = 1;

        for(vector<Point>::iterator i = band_points.begin(); i != band_points.end(); ++i, count++) {
            vector<Point>::iterator j = band_points.begin();
            advance(j, count);

            for(int inner_count = 1; j != band_points.end() && inner_count <= 15; ++j, inner_count++) {
                double dist = i->distance(*j);
                
                if(dist < band_minimum.getDistance()) {
                    band_minimum = PointPair(*i, *j, dist);
                }
            }
        }
        
        if(band_minimum.getDistance() < absolute_min.getDistance()) {
            return band_minimum;
        }
        else {
            return absolute_min;
        }
    }
    else {
        return absolute_min;
    }
}

string part2() {
    vector<Point> points = read_file();
    cout << std::fixed << std::showpoint;
    cout << setprecision(23);

    long double curr_time2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    sort(points.begin(), points.end(), compare_point_by_x);

    PointPair recur_minimum = recur(0, points.size() - 1, points);
    long double curr_time3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
     
    stringstream outfile;

    outfile << std::fixed << std::showpoint;
    outfile << setprecision(23);

    // write to file
    outfile << "Intermediate Recursive: " << endl;
    outfile << "Distance: " << recur_minimum.getDistance() << ", Point 1: (" << recur_minimum.getFirst().getX() << ", " << recur_minimum.getFirst().getY() << "), Point 2: (" << recur_minimum.getSecond().getX() << ", " << recur_minimum.getSecond().getY() << ")" << endl;
    outfile << "Time: " << (curr_time3 - curr_time2) / 1000.0 << " seconds" << endl;

    cout << outfile.str() << endl;
    return outfile.str();
}

string part3() {
    vector<Point> points = read_file();
    cout << std::fixed << std::showpoint;
    cout << setprecision(23);

    long double curr_time2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    sort(points.begin(), points.end(), compare_point_by_x);

    PointPair recur_minimum = recur_complete(0, points.size() - 1, points);
    long double curr_time3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
     
    stringstream outfile;

    outfile << std::fixed << std::showpoint;
    outfile << setprecision(23);

    // write to file
    outfile << "Complete Recursive: " << endl;
    outfile << "Distance: " << recur_minimum.getDistance() << ", Point 1: (" << recur_minimum.getFirst().getX() << ", " << recur_minimum.getFirst().getY() << "), Point 2: (" << recur_minimum.getSecond().getX() << ", " << recur_minimum.getSecond().getY() << ")" << endl;
    outfile << "Time: " << (curr_time3 - curr_time2) / 1000.0 << " seconds" << endl;

    cout << outfile.str() << endl;
    return outfile.str();
}

int main() {
    string part1_output = part1();
    string part2_output = part2();
    string part3_output = part3();

    ofstream outfile;
    outfile.open("results.txt");

    // outfile << part1_output << endl;
    outfile << part2_output << endl;
    outfile << part3_output << endl;

    outfile.close();
}
