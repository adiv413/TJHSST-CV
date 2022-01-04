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

int width = 0;
int height = 0;

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

vector<vector<Pixel>> readPPM(string infile) {
    ifstream inp(infile);
    string temp1;
    inp >> temp1; // useless information (file type, we don't need that)
    
    inp >> width >> height; // width and height of image

    int temp2;
    inp >> temp2; // more useless information (max color will always be 255)


    vector<vector<Pixel>> pixels(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    cout << width << " " << height<< endl;
    // fill pixels with all pixels in the ppm

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // cout << i << " " << j << endl;
            int r, g, b;
            inp >> r >> g >> b;
            pixels[i][j] = Pixel(r, g, b);
        }
    }

    cout << pixels.size() << " " << pixels[0].size() << endl;

    return pixels;
}

int main() {
    // read in the ppm file
    vector<vector<Pixel>> pixels = readPPM("image.ppm");

    // print out pixels
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            cout << pixels[i][j].getR() << " " << pixels[i][j].getG() << " " << pixels[i][j].getB() << " ";
        }
        cout << endl;
    }
    

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

    // output pixels to imageg.ppm
    ofstream out("imageg.ppm");
    out << "P3" << endl;
    out << width << " " << height << endl;
    out << "255" << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            out << pixels[i][j].getR() << " " << pixels[i][j].getG() << " " << pixels[i][j].getB() << " ";
        }
        out << endl;
    }
    out.close();
    
    

    // implement canny edge detection with a sobel filter and gaussian blur on pixels
    // use a 3x3 sobel filter
    // use a gaussian blur with a sigma of 1.0
    // use a threshold of 0.5
    // use a non-maximum suppression
    // use a hysteresis threshold of 0.5
    // write the output to a new ppm file
    
}