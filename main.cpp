#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <vector>

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

#define LOG(X) std::cout << #X << " " << X << "\n"

#define LOG_RESULT(X)  \
LOG(X.value);           \
LOG(X.code);           \
LOG(X.points);         \
LOG(X.point_without_accuracity); \
LOG(X.point_with_min); \
LOG(X.point_with_max); \


using Function = double (*)(double, double);


void print_line(){
    std::cout << "------------------------\n";
}

struct Result{
    double value;
    int code;
    size_t points;
    size_t point_without_accuracity;
    size_t point_with_min;
    size_t point_with_max;
};

void step_h(size_t& count_points, size_t& point_with_min, size_t& point_with_max, double& h, double hMin, double hMax){
    count_points++;
    if (h == hMin){
        point_with_min++;
    }
    else if (h == hMax){
        point_with_max++;
    }
}

template <size_t rank>
void next_h(double cur_eps, double eps, double& h, double hMax, double hMin, size_t& count_points_without_accuracity){
    h = pow(eps/cur_eps, rank + 1) * h;
    h = std::min(h, hMax);
    h = std::max(h, hMin);
    if (cur_eps > eps){
        count_points_without_accuracity++;
    }
}

template <Function f>
void step_x_y(double h, double& x, double& y, double& cur_eps){
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
}

template <Function f>
void step_x_y_3(double h, double& x, double& y, double& cur_eps){
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
}

template <Function f>
Result second_rang(double A, double B, double hMin, double hMax, double C, double yC, double eps) {
    double cur_eps = 9999999;
    double h = (B - A) / 10;
    h = std::max(h, hMin);
    h = std::min(h, hMax);
    size_t count_points{};
    size_t count_points_without_accuracity{};
    size_t point_with_min{};
    size_t point_with_max{};
    double x = C;
    double y = yC;
    step_h(count_points,point_with_min, point_with_max, h, hMin, hMax);
    step_x_y<f>(h, x, y, cur_eps);
    //second and next step
    while (x < B) {
        step_h(count_points,point_with_min, point_with_max, h, hMin, hMax);
        step_x_y<f>(h, x, y, cur_eps);
        next_h<2>(cur_eps, eps, h, hMax, hMin, count_points_without_accuracity);
    }
    LOG(count_points);
    LOG(count_points_without_accuracity);
    print_line();
    return {.value = y,
            .code = ((double)count_points/count_points_without_accuracity  < 0.5),
            .points = count_points,
            .point_without_accuracity = count_points_without_accuracity,
            .point_with_min = point_with_min,
            .point_with_max = point_with_max};
}


template <Function f>
Result third_rang(double A, double B, double hMin, double hMax, double C, double yC, double eps){
    double cur_eps = 9999999;
    double h = (B-A)/ 10;
    h = std::max(h, hMin);
    h = std::min(h, hMax);
    size_t count_points{};
    size_t count_points_without_accuracity{};
    size_t point_with_min{};
    size_t point_with_max{};
    double x = C;
    double y = yC;
    step_h(count_points,point_with_min, point_with_max, h, hMin, hMax);
    step_x_y_3<f>(h, x, y, cur_eps);
    while (x < B) {
        step_h(count_points,point_with_min, point_with_max, h, hMin, hMax);
        step_x_y_3<f>(h, x, y, cur_eps);
        next_h<3>(cur_eps, eps, h, hMax, hMin, count_points_without_accuracity);
    }
    print_line();
    return {.value = y,
            .code = ((double)count_points/count_points_without_accuracity  < 0.5),
            .points = count_points,
            .point_without_accuracity = count_points_without_accuracity,
            .point_with_min = point_with_min,
            .point_with_max = point_with_max};
}

int main() {
    std::ifstream file("in.txt");
    auto&& [A, B, C, yC, hMin, hMax, eps] = input(file);
    auto res2 = second_rang<Fun1>(A, B, hMin, hMax, C, yC, eps);
    auto res3 = third_rang<Fun1>(A, B, hMin, hMax, C, yC, eps);
    LOG_RESULT(res2);
    print_line();
    LOG_RESULT(res3);
}
