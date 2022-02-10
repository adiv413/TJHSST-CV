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

void histeresis(vector<vector<Pixel>> &pixels, int i, int j) {
    if(pixels[i][j].getR() == 128) {
        if(i + 1 < pixels.size()) {
            if(pixels[i + 1][j].getR() == 255) {
                pixels[i][j] = Pixel(255, 255, 255);
                histeresis(pixels, i + 1, j);
            }
        }
        if(i - 1 >= 0) {
            if(pixels[i - 1][j].getR() == 255) {
                pixels[i][j] = Pixel(255, 255, 255);
                histeresis(pixels, i - 1, j);
            }
        }
        if(j + 1 < pixels[0].size()) {
            if(pixels[i][j + 1].getR() == 255) {
                pixels[i][j] = Pixel(255, 255, 255);
                histeresis(pixels, i, j + 1);
            }
        }
        if(j - 1 >= 0) {
            if(pixels[i][j - 1].getR() == 255) {
                pixels[i][j] = Pixel(255, 255, 255);
                histeresis(pixels, i, j - 1);
            }
        }
        if(i + 1 < pixels.size() && j + 1 < pixels[0].size()) {
            if(pixels[i + 1][j + 1].getR() == 255) {
                pixels[i][j] = Pixel(255, 255, 255);
                histeresis(pixels, i + 1, j + 1);
            }
        }
        if(i - 1 >= 0 && j - 1 >= 0) {
            if(pixels[i - 1][j - 1].getR() == 255) {
                pixels[i][j] = Pixel(255, 255, 255);
                histeresis(pixels, i - 1, j - 1);
            }
        }
        if(i + 1 < pixels.size() && j - 1 >= 0) {
            if(pixels[i + 1][j - 1].getR() == 255) {
                pixels[i][j] = Pixel(255, 255, 255);
                histeresis(pixels, i + 1, j - 1);
            }
        }
        if(i - 1 >= 0 && j + 1 < pixels[0].size()) {
            if(pixels[i - 1][j + 1].getR() == 255) {
                pixels[i][j] = Pixel(255, 255, 255);
                histeresis(pixels, i - 1, j + 1);
            }
        }
    }
}

void part1() {
    // read in the ppm file
    vector<vector<Pixel>> pixels = readPPM("image.ppm");

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

    // output grayscaled image to imageg.ppm
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

    // add zero padding=1 in preperation for convulution
    pixels.insert(pixels.begin(), vector<Pixel>(width, Pixel(0, 0, 0)));
    pixels.push_back(vector<Pixel>(width, Pixel(0, 0, 0)));

    for(int i = 0; i < pixels.size(); i++) {
        pixels[i].insert(pixels[i].begin(), Pixel(0, 0, 0));
        pixels[i].push_back(Pixel(0, 0, 0));
    }

    // x convolution
    
    vector<vector<Pixel>> x_edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    vector<vector<int>> x_kernel = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};

    for(int i = 1; i < height + 1; i++) {
        for(int j = 1; j < width + 1; j++) {
            int value = 0;

            for(int k = 0; k <= 2; k++) {
                for(int l = 0; l <= 2; l++) {
                    value += pixels[i + (k - 1)][j + (l - 1)].getR() * x_kernel[k][l]; // we can use getR because it's all the same after grayscaling
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
                    value += pixels[i + (k - 1)][j + (l - 1)].getR() * y_kernel[k][l]; // we can use getR because it's all the same after grayscaling
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

    // thresholding
    int threshold = 200;

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() > threshold) {
                edges[i][j] = Pixel(255, 255, 255);
            } else {
                edges[i][j] = Pixel(0, 0, 0);
            }
        }
    }

    ofstream out2("imagem.ppm");
    out2 << "P3" << endl;
    out2 << width << " " << height << endl;
    out2 << "1" << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() == 0) {
                out2 << 0 << " " << 0 << " " << 0 << " ";
            }
            else {
                out2 << 1 << " " << 1 << " " << 1 << " ";
            }
        }
        out2 << endl;
    }
    out2.close();
}

void part2() {
    // read in the ppm file
    vector<vector<Pixel>> pixels = readPPM("image.ppm");

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

    // add zero padding=1 in preperation for convulution
    pixels.insert(pixels.begin(), vector<Pixel>(width, Pixel(0, 0, 0)));
    pixels.push_back(vector<Pixel>(width, Pixel(0, 0, 0)));

    for(int i = 0; i < pixels.size(); i++) {
        pixels[i].insert(pixels[i].begin(), Pixel(0, 0, 0));
        pixels[i].push_back(Pixel(0, 0, 0));
    }

    // x convolution
    
    vector<vector<Pixel>> x_edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    vector<vector<int>> x_kernel = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};

    for(int i = 1; i < height + 1; i++) {
        for(int j = 1; j < width + 1; j++) {
            int value = 0;

            for(int k = 0; k <= 2; k++) {
                for(int l = 0; l <= 2; l++) {
                    value += pixels[i + (k - 1)][j + (l - 1)].getR() * x_kernel[k][l]; // we can use getR because it's all the same after grayscaling
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
                    value += pixels[i + (k - 1)][j + (l - 1)].getR() * y_kernel[k][l]; // we can use getR because it's all the same after grayscaling
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

    // thresholding
    int threshold1 = 100;
    int threshold2 = 200;

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

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            histeresis(edges, i, j);
        }
    }
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() == 128) {
                edges[i][j] = Pixel(0, 0, 0);
            }
        }
    }

    // output edge detections to imagem.ppm
    ofstream out2("image1.ppm");
    out2 << "P3" << endl;
    out2 << width << " " << height << endl;
    out2 << "1" << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() == 0) {
                out2 << 0 << " " << 0 << " " << 0 << " ";
            }
            else {
                out2 << 1 << " " << 1 << " " << 1 << " ";
            }
        }
        out2 << endl;
    }
    out2.close();
}

void part3(int threshold1 = 100, int threshold2 = 200, string filename = "image.ppm") {
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

    // output grayscaled image to imageg.ppm
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

    // add zero padding=1 in preperation for convulution
    pixels.insert(pixels.begin(), vector<Pixel>(width, Pixel(0, 0, 0)));
    pixels.push_back(vector<Pixel>(width, Pixel(0, 0, 0)));

    for(int i = 0; i < pixels.size(); i++) {
        pixels[i].insert(pixels[i].begin(), Pixel(0, 0, 0));
        pixels[i].push_back(Pixel(0, 0, 0));
    }

    // x convolution
    
    vector<vector<Pixel>> x_edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));
    vector<vector<int>> x_kernel = {{1, 0, -1}, {2, 0, -2}, {1, 0, -1}};

    for(int i = 1; i < height + 1; i++) {
        for(int j = 1; j < width + 1; j++) {
            int value = 0;

            for(int k = 0; k <= 2; k++) {
                for(int l = 0; l <= 2; l++) {
                    value += pixels[i + (k - 1)][j + (l - 1)].getR() * x_kernel[k][l]; // we can use getR because it's all the same after grayscaling
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
                    value += pixels[i + (k - 1)][j + (l - 1)].getR() * y_kernel[k][l]; // we can use getR because it's all the same after grayscaling
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

    // thresholding ONLY

    vector<vector<Pixel>> threshold_only_edges(height, vector<Pixel>(width, Pixel(0, 0, 0)));

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() > threshold2) {
                threshold_only_edges[i][j] = Pixel(255, 255, 255);
            } 
            else if(edges[i][j].getR() > threshold1) {
                threshold_only_edges[i][j] = Pixel(128, 128, 128);
            }
            else {
                threshold_only_edges[i][j] = Pixel(0, 0, 0);
            }
        }
    }

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            histeresis(threshold_only_edges, i, j);
        }
    }
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(threshold_only_edges[i][j].getR() == 128) {
                threshold_only_edges[i][j] = Pixel(0, 0, 0);
            }
        }
    }

    // output threshold only edge detections to image1.ppm
    ofstream out2("image1.ppm");
    out2 << "P3" << endl;
    out2 << width << " " << height << endl;
    out2 << "1" << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(threshold_only_edges[i][j].getR() == 0) {
                out2 << 0 << " " << 0 << " " << 0 << " ";
            }
            else {
                out2 << 1 << " " << 1 << " " << 1 << " ";
            }
        }
        out2 << endl;
    }
    out2.close();

    // find angles from x and y edges

    vector<vector<int>> angles(height, vector<int>(width, 0));

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            double angle = atan2(y_edges[i][j].getR(), x_edges[i][j].getR()) * 180 / pi;
            if(-180.0 <= angle && angle >= -157.5) {
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
            else {
                angles[i][j] = 0;
            }
        }
    }

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(angles[i][j] == 0) {
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
            else if(angles[i][j] == 45) {
                int max = 0;

                if(i > 0 && j > 0) {
                    if(edges[i - 1][j - 1].getR() > max) {
                        max = edges[i - 1][j - 1].getR();
                    }
                }
                if(i < height - 1 && j < width - 1) {
                    if(edges[i + 1][j + 1].getR() > max) {
                        max = edges[i + 1][j + 1].getR();
                    }
                }

                if(edges[i][j].getR() < max) {
                    edges[i][j] = Pixel(0, 0, 0);
                }
            }
            else if(angles[i][j] == 90) {
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
            else if(angles[i][j] == 135) {
                int max = 0;

                if(i > 0 && j < width - 1) {
                    if(edges[i - 1][j + 1].getR() > max) {
                        max = edges[i - 1][j + 1].getR();
                    }
                }
                if(i < height - 1 && j > 0) {
                    if(edges[i + 1][j - 1].getR() > max) {
                        max = edges[i + 1][j - 1].getR();
                    }
                }

                if(edges[i][j].getR() < max) {
                    edges[i][j] = Pixel(0, 0, 0);
                }
            }
        }
    }


    // output edge detections to image2.ppm
    ofstream out3("image2.ppm");
    out3 << "P3" << endl;
    out3 << width << " " << height << endl;
    out3 << "1" << endl;
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() == 0) {
                out3 << 0 << " " << 0 << " " << 0 << " ";
            }
            else {
                out3 << 1 << " " << 1 << " " << 1 << " ";
            }
        }
        out3 << endl;
    }
    out3.close();

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

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            histeresis(edges, i, j);
        }
    }
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(edges[i][j].getR() == 128) {
                edges[i][j] = Pixel(0, 0, 0);
            }
        }
    }

    // output threshold only edge detections to image1.ppm
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

}

int main(int argc, char** argv) {
    // part1();
    // part2();

    if(argc > 6) { 
        part3(stoi(argv[2]), stoi(argv[4]), argv[6]);
    }
    else if(argc > 4) {
        part3(stoi(argv[2]), stoi(argv[4]));
    }
    else if(argc > 2) {
        part3(stoi(argv[2]));
    }
    else {
        part3();
    }
}