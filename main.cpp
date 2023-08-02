#include <iostream>
#include <fstream>
#include <cmath>
#include <map>

double Fun1(double x, double y){
    return tan(x) + tan(y);
}

#define map_value(X) {#X, &X}

#define checker(X) if(!X) {std::string error = #X; \
error += " is missing\n";                          \
throw std::runtime_error(error);                   \
}

auto input(std::ifstream& in){
    std::optional<double> A, B, C, yC, hMin, hMax, eps;
    std::map<std::string, std::optional<double>*> map{
            map_value(A),
            map_value(B),
            map_value(C),
            map_value(yC),
            map_value(hMin),
            map_value(hMax),
            map_value(eps)
    };
    std::string temp;
    while (std::getline(in, temp)){
        if (temp.empty()){
            continue;
        }
        size_t index = std::find(temp.begin(), temp.end(), '=') - temp.begin();
        std::string key = temp.substr(0, index);
        std::string value = temp.substr(index + 1, temp.size());
        double d_value = std::stod(value);
        auto it = map.find(key);
        if (it == map.end()){
            continue;
        }
        *(it->second) = d_value;
    }
    checker(A);
    checker(B);
    checker(C);
    checker(yC);
    checker(hMin);
    checker(hMax);
    checker(eps);
    return std::make_tuple(*A, *B, *C, *yC, *hMin, *hMax, *eps);
}

using value = decltype(input(std::declval<std::ifstream&>()));

#define LOG(X) std::cout << #X << " " << X << "\n"


void print_line(){
    std::cout << "------------------------\n";
}

double second_rang(double A, double B, double hMin, double hMax, double C, double yC, double eps, std::function<double(double, double)> f) {
    double cur_eps = 9999999;
    double h = (B - A) / 10;
    if (h < hMin) {
        h = hMin;
    } else if (h > hMax) {
        h = hMax;
    }
    size_t count_points = 0;
    double x = C;
    double y = yC;
    while (cur_eps > eps && x < B) {
        double k1 = h * f(x, y);
        double k2 = h * f(x + h, y + k1);
        double prev_y = y;
        double prev_x = x;
        y = y + 0.5 * (k1 + k2);
        x = x + h;
        LOG(prev_y);
        LOG(prev_x);
        LOG(y);
        LOG(x);
        cur_eps = abs(y - prev_y);
        count_points++;
    }
    size_t count_points_without_accuracity = count_points - 1;
    LOG(count_points);
    LOG(count_points_without_accuracity);
    print_line();
    return y;
}

double third_rang(double A, double B, double hMin, double hMax, double C, double yC, double eps, std::function<double(double, double)> f){
    double cur_eps = 9999999;
    double h = (B-A)/ 10;
    if (h < hMin){
        h = hMin;
    }
    else if (h > hMax){
        h = hMax;
    }
    size_t count_points = 0;
    double x = C;
    double y = yC;
    while (cur_eps > eps && x < B) {
        double prev_y = y;
        double prev_x = x;
        double k1 = h * f(x, y);
        double k2 = h * f(x + h / 2, y + k1 / 2);
        double k3 = h * f(x + h, y - k1 + 2 * k2);
        y = y + (k1 + 4 * k2 + k3) / 6;
        x = x + h;
        LOG(prev_y);
        LOG(prev_x);
        LOG(y);
        LOG(x);
        cur_eps = abs(y - prev_y);
        count_points++;
    }
    print_line();
    return y;
}

int main() {
    std::ifstream file("in.txt");
    std::function<double(double, double)> f = Fun1;
    auto&& [A, B, C, yC, hMin, hMax, eps] = input(file);
    double y2 = second_rang(A, B, hMin, hMax, C, yC, eps, f);
    double y3 = third_rang(A, B, hMin, hMax, C, yC, eps, f);
    LOG(y2);
    LOG(y3);
}
