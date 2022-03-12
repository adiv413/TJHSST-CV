// Aditya Vasantharao, pd. 4

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
    PURPLE,
    BLUE,
    GREEN,
    YELLOW
};

int width = 0;
int height = 0;

class HashFunction {
public:
    size_t operator()(const pair<int, int>& p) const
    {
        int x = p.first;
        int y = p.second;

        return abs((x + 2 * y * y) * (5 * x + 3 * y));
    }
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

        double distance(Point other) {
            return sqrt(pow(other.x - x, 2) + pow(other.y - y, 2));
        }    
};

// set pixel to be 1 (black pixel)
void cast_vote(int **pixels, int x, int y, int height, int width) {
    if(x < 0 || x > height - 1 || y < 0 || y > width - 1);
    else {
        pixels[x][y] += 1;
    }
}

// write the board to a ppm file
void write_board(int **pixels, int height, int width, string filename) {
    ofstream outfile;
    outfile.open(filename + ".ppm");

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
            Point scaled_first(-1.0, -1.0);
            Point scaled_second(-1.0, -1.0);

            // check 0 slope
            if((-threshold <= slope) && (slope <= threshold)) {
                scaled_first = Point(static_cast<double>(canvas_width), first.getY());
                scaled_second = Point(-static_cast<double>(canvas_width), first.getY());
            }
            // check infinite slope
            else if(100000.0 <= abs(slope)) {
                scaled_first = Point(first.getX(), static_cast<double>(canvas_height));
                scaled_second = Point(first.getX(), -static_cast<double>(canvas_height));
            }
            else {
                // check right side
                double right_y = slope * (canvas_height - first.getX()) + first.getY();
                if((0 <= right_y) && (right_y <= canvas_height)) {
                    // hits the right side of the screen
                    scaled_first = Point(canvas_height, right_y);
                }
                // check top
                double top_x = (canvas_width - first.getY()) / slope + first.getX();
                if((0 <= top_x) && (top_x <= canvas_width)) {
                    // hits the top of the screen
                    if((scaled_first.getX() == -1.0) && (scaled_first.getY() == -1.0)) {
                        scaled_first = Point(top_x, canvas_width);
                    }
                    else {
                        scaled_second = Point(top_x, canvas_height);
                    }
                }
                // check left
                double left_y = slope * (0 - first.getX()) + first.getY();
                if((0 <= left_y) && (left_y <= canvas_height)) {
                    // hits the left side of the screen
                    if((scaled_first.getX() == -1.0) && (scaled_first.getY() == -1.0)) {
                        scaled_first = Point(0, left_y); 
                    }
                    else if((scaled_second.getX() == -1.0) && (scaled_second.getY() == -1.0)){
                        scaled_second = Point(0, left_y); 
                    }
                }
                // check bottom
                double bottom_x = (0 - first.getY()) / slope + first.getX();
                if((0 <= bottom_x) && (bottom_x <= canvas_width) && ((scaled_second.getX() == -1.0) && (scaled_second.getY() == -1.0))) {
                    // hits the bottom of the screen
                    scaled_second = Point(bottom_x, 0);
                }
            }

            if(scaled_first.getX() == -1.0 || scaled_first.getY() == -1.0 || scaled_second.getX() == -1.0 || scaled_second.getY() == -1.0);
            else {
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
                        cast_vote(pixels, i, j, canvas_height, canvas_width);

                        if(error >= 0) {
                            j -= 1;
                            error -= dx;
                        }
                        error += -dy;
                    }
                }
                else { // normal case x driven
                    for(int i = p1.getX(); i <= p2.getX(); i++) {
                        cast_vote(pixels, i, j, canvas_height, canvas_width);

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
                        cast_vote(pixels, i, j, canvas_height, canvas_width);

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
                        cast_vote(pixels, i, j, canvas_height, canvas_width);

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

class Pixel {
    private:
        int r;
        int g;
        int b;

    public:
        int getR() { 
            return r; 
        }

        int getG() { 
            return g; 
        }

        int getB() { 
            return b; 
        }

        void setR(int r) { 
            this->r = r; 
        }

        void setG(int g) { 
            this->g = g; 
        }

        void setB(int b) { 
            this->b = b; 
        }

        Pixel(int r, int g, int b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }
};

// set pixel to be 1 (black pixel)
void color_pixel(vector<vector<Pixel>> &pixels, int x, int y, int height, int width, Color color) {
    if(x < 0 || x > height - 1 || y < 0 || y > width - 1);
    else {
        if(color == Color::WHITE) {
            pixels[x][y] = Pixel(255, 255, 255);
        }
        else if(color == Color::RED) {
            pixels[x][y] = Pixel(255, 0, 0);
        }
        else if(color == Color::BLUE) {
            pixels[x][y] = Pixel(0, 0, 255);
        }
        else if(color == Color::GREEN) {
            pixels[x][y] = Pixel(0, 255, 0);
        }
        else if(color == Color::PURPLE) {
            pixels[x][y] = Pixel(255, 0, 255);
        }
        else if(color == Color::YELLOW) {
            pixels[x][y] = Pixel(255, 255, 0);
        }
        else {
            pixels[x][y] = Pixel(0, 0, 0); // default to black
        }
    }
}

void vote_circle(vector<vector<Pixel>> &edges, unordered_map<pair<int, int>, vector<int>, HashFunction> &circle_votes, int x, int y, int height, int width, int radius, int center_x, int center_y) {
    if(x < 0 || x > height - 1 || y < 0 || y > width - 1);
    else if(edges[x][y].getR() != 0) {
        circle_votes[make_pair(center_x,center_y)][radius] += 1;
    }
}

class Circle {
    private:
        Point center;
        double radius;
        int canvas_height;
        int canvas_width;

    public:

        Circle(Point p1, double r, int c_h, int c_w) : center(p1), radius(r), canvas_height(c_h), canvas_width(c_w) {}

        void draw_circle(vector<vector<Pixel>> &pixels, Color color) {
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

        void vote(vector<vector<Pixel>> &edges, unordered_map<pair<int, int>, vector<int>, HashFunction> &circle_votes) {
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

                vote_circle(edges, circle_votes, x + center.getX(), y + center.getY(), canvas_height, canvas_width, radius, center.getX(), center.getY());
                vote_circle(edges, circle_votes, x + center.getX(), -y + center.getY(), canvas_height, canvas_width, radius, center.getX(), center.getY());
                vote_circle(edges, circle_votes, -x + center.getX(), y + center.getY(), canvas_height, canvas_width, radius, center.getX(), center.getY());
                vote_circle(edges, circle_votes, -x + center.getX(), -y + center.getY(), canvas_height, canvas_width, radius, center.getX(), center.getY());
                vote_circle(edges, circle_votes, y + center.getX(), x + center.getY(), canvas_height, canvas_width, radius, center.getX(), center.getY());
                vote_circle(edges, circle_votes, y + center.getX(), -x + center.getY(), canvas_height, canvas_width, radius, center.getX(), center.getY());
                vote_circle(edges, circle_votes, -y + center.getX(), x + center.getY(), canvas_height, canvas_width, radius, center.getX(), center.getY());
                vote_circle(edges, circle_votes, -y + center.getX(), -x + center.getY(), canvas_height, canvas_width, radius, center.getX(), center.getY());

                y2_new -= (2 * x) - 3;
            }
        }
};

vector<vector<Pixel>> readPPM(string infile) {
    ifstream inp(infile);
    string temp1;
    inp >> temp1; // useless information (file type, we don't need that)
    
    inp >> width >> height; // width and height of image

    int temp2;
    inp >> temp2; // more useless information (max color will always be 255)


    vector<vector<Pixel>> pixels(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    // fill pixels with all pixels in the ppm

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int r, g, b;
            inp >> r >> g >> b;
            pixels[i][j] = Pixel(r, g, b);
        }
    }

    return pixels;
}

void hysteresis(vector<vector<Pixel>> &pixels, int i, int j, vector<vector<int>> &visited) {
    if(pixels[i][j].getR() != 0 && visited[i][j] != 1) {
        if(i + 1 < pixels.size()) {
            visited[i][j] = 1;
            hysteresis(pixels, i + 1, j, visited);
        }
        if(i - 1 >= 0) {
            visited[i][j] = 1;
            hysteresis(pixels, i - 1, j, visited);
        }
        if(j + 1 < pixels[0].size()) {
            visited[i][j] = 1;
            hysteresis(pixels, i, j + 1, visited);
        }
        if(j - 1 >= 0) {
            visited[i][j] = 1;
            hysteresis(pixels, i, j - 1, visited);
        }
        if(i + 1 < pixels.size() && j + 1 < pixels[0].size()) {
            visited[i][j] = 1;
            hysteresis(pixels, i + 1, j + 1, visited);
        }
        if(i - 1 >= 0 && j - 1 >= 0) {
            visited[i][j] = 1;
            hysteresis(pixels, i - 1, j - 1, visited);
        }
        if(i + 1 < pixels.size() && j - 1 >= 0) {
            visited[i][j] = 1;
            hysteresis(pixels, i + 1, j - 1, visited);
        }
        if(i - 1 >= 0 && j + 1 < pixels[0].size()) {
            visited[i][j] = 1;
            hysteresis(pixels, i - 1, j + 1, visited);
        }
    }
}

void part1(int threshold1 = 55, int threshold2 = 120, string filename = "coins_easy.ppm", int center_threshold = 15, int circle_threshold = 15) {
    const long double pi = 3.14159265358979323846;

    // read in the ppm file
    vector<vector<Pixel>> pixels = readPPM(filename);

    // grayscale pixels
    for(int i = 0; i < pixels.size(); i++) {
        for(int j = 0; j < pixels[i].size(); j++) {
            int r = pixels[i][j].getR();
            int g = pixels[i][j].getG();
            int b = pixels[i][j].getB();
            int gray = (r + g + b) / 3;
            pixels[i][j] = Pixel(gray, gray, gray);
        }
    }

    // add zero padding=1 in preperation for convolution
    pixels.insert(pixels.begin(), vector<Pixel>(width, Pixel(0, 0, 0)));
    pixels.push_back(vector<Pixel>(width, Pixel(0, 0, 0)));

    for(int i = 0; i < pixels.size(); i++) {
        pixels[i].insert(pixels[i].begin(), Pixel(0, 0, 0));
        pixels[i].push_back(Pixel(0, 0, 0));
    }

    // gaussian blur
    vector<vector<Pixel>> blurred_pixels(height, vector<Pixel>(width, Pixel(0, 0, 0)));

    vector<vector<double>> kernel = {
        {1/16.0, 2/16.0, 1/16.0},
        {2/16.0, 4/16.0, 2/16.0},
        {1/16.0, 2/16.0, 1/16.0}
    };

    for(int i = 1; i < height + 1; i++) {
        for(int j = 1; j < width + 1; j++) {
            int value = 0;

            for(int k = 0; k <= 2; k++) {
                for(int l = 0; l <= 2; l++) {
                    value += pixels[i + (k - 1)][j + (l - 1)].getR() * kernel[k][l]; // we can use getR because it's all the same after grayscaling
                }
            }

            blurred_pixels[i - 1][j - 1] = Pixel(value, value, value);
        }
    }

    // add padding for next convolution

    blurred_pixels.insert(blurred_pixels.begin(), vector<Pixel>(width, Pixel(0, 0, 0)));
    blurred_pixels.push_back(vector<Pixel>(width, Pixel(0, 0, 0)));

    for(int i = 0; i < blurred_pixels.size(); i++) {
        blurred_pixels[i].insert(blurred_pixels[i].begin(), Pixel(0, 0, 0));
        blurred_pixels[i].push_back(Pixel(0, 0, 0));
    }

    int maxblur = 0;
    for(int i = 0; i < blurred_pixels.size(); i++) {
        for(int j = 0; j < blurred_pixels[i].size(); j++) {
            if(blurred_pixels[i][j].getR() > maxblur) {
                maxblur = blurred_pixels[i][j].getR();
            }
        }
    }

    // x convolution
    
    vector<vector<Pixel>> x_edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    vector<vector<int>> x_kernel = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

    for(int i = 1; i < height + 1; i++) {
        for(int j = 1; j < width + 1; j++) {
            int value = 0;

            for(int k = 0; k <= 2; k++) {
                for(int l = 0; l <= 2; l++) {
                    value += blurred_pixels[i + (k - 1)][j + (l - 1)].getR() * x_kernel[k][l]; // we can use getR because it's all the same after grayscaling
                }
            }

            x_edges[i - 1][j - 1] = Pixel(value, value, value);
        }
    }

    // y convolution
    vector<vector<Pixel>> y_edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    vector<vector<int>> y_kernel = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};

    for(int i = 1; i < height + 1; i++) {
        for(int j = 1; j < width + 1; j++) {
            int value = 0;

            for(int k = 0; k <= 2; k++) {
                for(int l = 0; l <= 2; l++) {
                    value += blurred_pixels[i + (k - 1)][j + (l - 1)].getR() * y_kernel[k][l]; // we can use getR because it's all the same after grayscaling
                }
            }

            y_edges[i - 1][j - 1] = Pixel(value, value, value);
        }
    }

    // combine x and y edges
    vector<vector<Pixel>> edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            int value = sqrt(x_edges[i][j].getR() * x_edges[i][j].getR() + y_edges[i][j].getR() * y_edges[i][j].getR());
            edges[i][j] = Pixel(value, value, value);
        }
    }

    // find angles from x and y edges

    vector<vector<int>> angles(height, vector<int>(width, 0));

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            double angle = atan2(y_edges[i][j].getR(), x_edges[i][j].getR()) * 180 / pi;
            if(angle <= -157.5) {
                angles[i][j] = 0;
            }
            else if(-157.5 <= angle && angle <= -112.5) {
                angles[i][j] = 45;
            }
            else if(-112.5 <= angle && angle <= -67.5) {
                angles[i][j] = 90;
            }
            else if(-67.5 <= angle && angle <= -22.5) {
                angles[i][j] = 135;
            }
            else if(-22.5 <= angle && angle <= 22.5) {
                angles[i][j] = 0;
            }
            else if(22.5 <= angle && angle <= 67.5) {
                angles[i][j] = 45;
            }
            else if(67.5 <= angle && angle <= 112.5) {
                angles[i][j] = 90;
            }
            else if(112.5 <= angle && angle <= 157.5) {
                angles[i][j] = 135;
            }
            else if(angle >= 157.5) {
                angles[i][j] = 0;
            }
        }
    }

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(angles[i][j] == 0) {
                int max = 0;

                if(j > 0) {
                    if(edges[i][j - 1].getR() > max) {
                        max = edges[i][j - 1].getR();
                    }
                }
                if(j < width - 1) {
                    if(edges[i][j + 1].getR() > max) {
                        max = edges[i][j + 1].getR();
                    }
                }

                if(edges[i][j].getR() < max) {
                    edges[i][j] = Pixel(0, 0, 0);
                }
            }
            else if(angles[i][j] == 45) {
                int max = 0;

                if(i < height - 1 && j > 0) {
                    if(edges[i + 1][j - 1].getR() > max) {
                        max = edges[i + 1][j - 1].getR();
                    }
                }
                if(i > 0 && j < width - 1) {
                    if(edges[i - 1][j + 1].getR() > max) {
                        max = edges[i - 1][j + 1].getR();
                    }
                }

                if(edges[i][j].getR() < max) {
                    edges[i][j] = Pixel(0, 0, 0);
                }
            }
            else if(angles[i][j] == 90) {
                int max = 0;

                if(i > 0) {
                    if(edges[i - 1][j].getR() > max) {
                        max = edges[i - 1][j].getR();
                    }
                }
                if(i < height - 1) {
                    if(edges[i + 1][j].getR() > max) {
                        max = edges[i + 1][j].getR();
                    }
                }

                if(edges[i][j].getR() < max) {
                    edges[i][j] = Pixel(0, 0, 0);
                }
            }
            else if(angles[i][j] == 135) {
                int max = 0;

                if(i < height - 1 && j < width - 1) {
                    if(edges[i + 1][j + 1].getR() > max) {
                        max = edges[i + 1][j + 1].getR();
                    }
                }
                if(i > 0 && j > 0) {
                    if(edges[i - 1][j - 1].getR() > max) {
                        max = edges[i - 1][j - 1].getR();
                    }
                }

                if(edges[i][j].getR() < max) {
                    edges[i][j] = Pixel(0, 0, 0);
                }
            }
        }
    }

    // apply thresholding and hysteresis

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() > threshold2) {
                edges[i][j] = Pixel(255, 255, 255);
            } 
            else if(edges[i][j].getR() > threshold1) {
                edges[i][j] = Pixel(128, 128, 128);
            }
            else {
                edges[i][j] = Pixel(0, 0, 0);
            }
        }
    }

    // vector<vector<int>> visited(height, vector<int>(width, 0));
    // vector<vector<Pixel>> new_edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));


    vector<vector<int>> visited(height, vector<int>(width, 0));

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() == 255) {
                hysteresis(edges, i, j, visited);
            }
        }
    }

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(visited[i][j] == 1) {
                edges[i][j] = Pixel(255, 255, 255);
            }
            else {
                edges[i][j] = Pixel(0, 0, 0);
            }
        }
    }

    for(int j = 0; j < width; j++) {
        edges[0][j] = Pixel(0, 0, 0);
        edges[height - 1][j] = Pixel(0, 0, 0);
        edges[1][j] = Pixel(0, 0, 0);
        edges[height - 2][j] = Pixel(0, 0, 0);

        x_edges[0][j] = Pixel(0, 0, 0);
        x_edges[height - 1][j] = Pixel(0, 0, 0);
        x_edges[1][j] = Pixel(0, 0, 0);
        x_edges[height - 2][j] = Pixel(0, 0, 0);

        y_edges[0][j] = Pixel(0, 0, 0);
        y_edges[height - 1][j] = Pixel(0, 0, 0);
        y_edges[1][j] = Pixel(0, 0, 0);
        y_edges[height - 2][j] = Pixel(0, 0, 0);
    }

    for(int i = 0; i < edges.size(); i++) {
        edges[i][0] = Pixel(0, 0, 0);
        edges[i][width - 1] = Pixel(0, 0, 0);
        edges[i][1] = Pixel(0, 0, 0);
        edges[i][width - 2] = Pixel(0, 0, 0);

        x_edges[i][0] = Pixel(0, 0, 0);
        x_edges[i][width - 1] = Pixel(0, 0, 0);
        x_edges[i][1] = Pixel(0, 0, 0);
        x_edges[i][width - 2] = Pixel(0, 0, 0);

        y_edges[i][0] = Pixel(0, 0, 0);
        y_edges[i][width - 1] = Pixel(0, 0, 0);
        y_edges[i][1] = Pixel(0, 0, 0);
        y_edges[i][width - 2] = Pixel(0, 0, 0);
    }

    // output final edge detections to imagef.ppm
    ofstream out4("imagef.ppm");
    out4 << "P3" << endl;
    out4 << width << " " << height << endl;
    out4 << "1" << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() == 0) {
                out4 << 0 << " " << 0 << " " << 0 << " ";
            }
            else {
                out4 << 1 << " " << 1 << " " << 1 << " ";
            }
        }
        out4 << endl;
    }
    out4.close();

    int **votes;
    votes = new int*[height];
    
    for(int i = 0; i < height; i++) {
        votes[i] = new int[width];

        // initialize all values in pixel array to default value of 0 (white pixel)
        for(int j = 0; j < width; j++) {
            votes[i][j] = 0;
        }
    }

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() != 0) { 
                double angle = atan2(y_edges[i][j].getR(), x_edges[i][j].getR()) * 180 / pi;
                double slope = sin(angle * pi / 180) / cos(angle * pi / 180);

                Line l(Point(i, j), slope, height, width);
                Line x = l.make_perpendicular(Point(i, j));
                
                
                x.draw_line(votes);
            }
        }
    }

    for(int j = 0; j < width; j++) {
        votes[0][j] = 0;
        votes[height - 1][j] = 0;
        votes[1][j] = 0;
        votes[height - 2][j] = 0;
    }

    for(int i = 0; i < height; i++) {
        votes[i][0] = 0;
        votes[i][width - 1] = 0;
        votes[i][1] = 0;
        votes[i][width - 2] = 0;
    }

    // show the lines perpendicular to circle when drawing final voting results
    int max = 0;

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(votes[i][j] > max) {
                max = votes[i][j];
            }
        }
    }

    ofstream out5("imagev.ppm");
    out5 << "P3" << endl;
    out5 << width << " " << height << endl;
    out5 << max << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            out5 << votes[i][j] << " " << votes[i][j] << " " << votes[i][j] << " ";
        }
        out5 << endl;
    }
    out5.close();

    // threshold votes

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(votes[i][j] < center_threshold) {
                votes[i][j] = 0;
            }
        }
    }

    // threshold votes by local max

    int radius_min = 70;
    int radius_max = 135; // 175 for coins3.ppm

    int local_max_grid_size = 2 * (4 * radius_max / 5);

    for(int i = 0; i < height; i += local_max_grid_size) {
        for(int j = 0; j < width; j += local_max_grid_size) {
            int local_max_votes = 0;
            int local_max_k = 0;
            int local_max_l = 0;

            for(int k = i; k < min(height, i + local_max_grid_size); k++) { // if local max grid size is not divisible, it will miss the ends
                for(int l = j; l < min(width, j + local_max_grid_size); l++) {
                    if(votes[k][l] > local_max_votes) {
                        local_max_votes = votes[k][l];
                        local_max_k = k;
                        local_max_l = l;
                    }
                }
            }

            for(int k = i; k < min(height, i + local_max_grid_size); k++) {
                for(int l = j; l < min(width, j + local_max_grid_size); l++) {
                    if(k != local_max_k || l != local_max_l) {
                        votes[k][l] = 0;
                    }
                }
            }
        }
    }    

    // vote on circles for each candidate radius

    vector<vector<Pixel>> output = readPPM(filename);
    unordered_map<pair<int, int>, vector<int>, HashFunction> circle_votes;

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(votes[i][j] != 0) {
                circle_votes[make_pair(i, j)] = vector<int>(radius_max + 1);
                
                for(int e = 1; e < 5; e++) {
                    Circle c(Point(i, j), e, height, width);
                    c.draw_circle(output, Color::BLACK);
                }
            }
        }
    }

    // draw candidate centers

    ofstream out6("imageCC.ppm");
    out6 << "P3" << endl;
    out6 << width << " " << height << endl;
    out6 << 255 << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            out6 << output[i][j].getR() << " " << output[i][j].getG() << " " << output[i][j].getB() << " ";
        }
        out6 << endl;
    }
    out6.close();

    // free memory for unused vectors

    vector<vector<Pixel>>().swap(pixels);
    vector<vector<Pixel>>().swap(output);
    vector<vector<Pixel>>().swap(blurred_pixels);
    vector<vector<Pixel>>().swap(x_edges);
    vector<vector<Pixel>>().swap(y_edges);
    vector<vector<int>>().swap(x_kernel);
    vector<vector<int>>().swap(y_kernel);
    vector<vector<int>>().swap(angles);
    vector<vector<int>>().swap(visited);

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(votes[i][j] != 0) {
                for(int r = radius_min; r < radius_max; r++) {
                    Circle c(Point(i, j), r, height, width);
                    c.vote(edges, circle_votes);
                }
            }
        }
    }

    // find final centers by simple thresholding at each candidate radius

    unordered_map<pair<int, int>, int, HashFunction> final_centers;

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(votes[i][j] != 0) {
                int max_r_votes = 0;
                int max_r = 1;

                for(int r = radius_min; r < radius_max; r++) {
                    if(circle_votes[make_pair(i, j)][r] >= max_r_votes) {
                        max_r_votes = circle_votes[make_pair(i, j)][r];
                        max_r = r;
                    }
                }

                if(max_r_votes > circle_threshold) {
                    final_centers[make_pair(i, j)] = max_r;
                }
            }
        }
    }

    // threshold final centers again via local max

    vector<pair<int, int>> to_delete;

    for(auto p : final_centers) {
        int i = p.first.first;
        int j = p.first.second;
        int r = p.second;

        int local_max_votes = 0;
        int local_max_k = 0;
        int local_max_l = 0;

        int i_begin = std::max(0, i - local_max_grid_size / 2);
        int i_end = std::min(i + local_max_grid_size / 2, height);

        int j_begin = std::max(0, j - local_max_grid_size / 2);
        int j_end = std::min(j + local_max_grid_size / 2, width);

        for(int k = i_begin; k < i_end; k++) { // if local max grid size is not divisible, it will miss the ends
            for(int l = j_begin; l < j_end; l++) {
                if(final_centers.count(make_pair(k, l)) != 0) {
                    to_delete.push_back(make_pair(k, l));
                    int curr_circle_votes = circle_votes[make_pair(k, l)][final_centers[make_pair(k, l)]];
                    
                    if(curr_circle_votes > local_max_votes) {
                        local_max_votes = curr_circle_votes;
                        local_max_k = k;
                        local_max_l = l;
                    }
                }
            }
        }

        to_delete.erase(std::remove(to_delete.begin(), to_delete.end(), make_pair(local_max_k, local_max_l)), to_delete.end());
    }

    for(pair<int, int> p : to_delete) {
        final_centers.erase(p);
    }

    // find coin type and draw final centers

    vector<vector<Pixel>> raw_pixel_reading = readPPM(filename);
    vector<vector<Pixel>> output2 = readPPM(filename);
    
    int num_pennies = 0;
    int num_dimes = 0;
    int num_nickels = 0;
    int num_quarters = 0;
    int num_silver_dollars = 0;

    for(auto p : final_centers) {
        int i = p.first.first;
        int j = p.first.second;
        int r = p.second;

        if(!(i < 600 && j < 150)) {
            Pixel pix = raw_pixel_reading[i][j];
            int avg = (pix.getR() + pix.getG() + pix.getB()) / 3;
            
            if(pix.getR() >= pix.getG() + static_cast<int>(avg / 6)) { // penny, based on color
                for(int e = -3; e < 5; e++) {
                    Circle c(Point(i, j), std::max(1, r + e), height, width);
                    c.draw_circle(output2, Color::RED);
                }
                num_pennies++;
            }
            else if(std::abs(pix.getR() - avg) <= 5 && std::abs(pix.getG() - avg) <= 5 && std::abs(pix.getB() - avg) <= 5); // not a coin, so skip
            else {
                if(150 <= r && r <= 175) { // silver dollar
                    for(int e = -3; e < 5; e++) {
                        Circle c(Point(i, j), std::max(radius_min, r + e), height, width);
                        c.draw_circle(output2, Color::YELLOW);
                    }
                    num_silver_dollars++;
                }
                else if(109 <= r && r <= 133) { // quarter
                    for(int e = -3; e < 5; e++) {
                        Circle c(Point(i, j), std::max(radius_min, r + e), height, width);
                        c.draw_circle(output2, Color::GREEN);
                    }
                    num_quarters++;
                }
                else if(93 <= r && r < 109) { // nickel
                    for(int e = -3; e < 5; e++) {
                        Circle c(Point(i, j), std::max(radius_min, r + e), height, width);
                        c.draw_circle(output2, Color::PURPLE);
                    }
                    num_nickels++;
                }
                else if(80 <= r && r < 93) { // penny
                    for(int e = -3; e < 5; e++) {
                        Circle c(Point(i, j), std::max(radius_min, r + e), height, width);
                        c.draw_circle(output2, Color::RED);
                    }
                    num_pennies++;
                }
                else if(70 <= r && r < 80) { // dime
                    for(int e = -3; e < 5; e++) {
                        Circle c(Point(i, j), std::max(radius_min, r + e), height, width);
                        c.draw_circle(output2, Color::BLUE);
                    }
                    num_dimes++;
                }
            }

            cout << i << " " << j << " " << r << " " << circle_votes[make_pair(i, j)][r] << endl;
            cout << output2[i][j].getR() << " " << output2[i][j].getG() << " " << output2[i][j].getB() << endl;
            cout << endl;
        }
    }

    cout << num_pennies << " Pennies" << endl;
    cout << num_nickels << " Nickels" << endl;
    cout << num_dimes << " Dimes" << endl;
    cout << num_quarters << " Quarters" << endl;
    cout << num_silver_dollars << " Silver Dollars" << endl;

    cout << "Total sum: $" << 0.01 * num_pennies + 0.05 * num_nickels + 0.1 * num_dimes + 0.25 * num_quarters + 0.5 * num_silver_dollars << endl;

    ofstream out7("coins.ppm");
    out7 << "P3" << endl;
    out7 << width << " " << height << endl;
    out7 << 255 << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            out7 << output2[i][j].getR() << " " << output2[i][j].getG() << " " << output2[i][j].getB() << " ";
        }
        out7 << endl;
    }
    out7.close();
    

    for(int i = 0; i < height; i++) {
        delete[] votes[i];
    }

    delete[] votes;

}

int main(int argc, char** argv) {
    if(argc > 10) {
        part1(stoi(argv[2]), stoi(argv[4]), argv[6], stoi(argv[8]), stoi(argv[10]));
    }
    else if(argc > 8) {
        part1(stoi(argv[2]), stoi(argv[4]), argv[6], stoi(argv[8]));
    }
    else if(argc > 6) { 
        part1(stoi(argv[2]), stoi(argv[4]), argv[6]);
    }
    else if(argc > 4) {
        part1(stoi(argv[2]), stoi(argv[4]));
    }
    else if(argc > 2) {
        part1(stoi(argv[2]));
    }
    else {
        part1();
    }
}