#include <iostream>
#include <sstream>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <cstdlib>
#include <getopt.h>
#include <iomanip>
#include <queue>
#include <deque>
#include <set>
#include <cmath>
#include <stack>
#include <fstream>
using namespace std;

int MAP_SIZE = 1000;

enum SOIL_TYPE {
    SOIL,
    PAVEMENT,
    GRASS,
    MAD,
    SHALLOW_WATER,
    DEEP_WATER
} soil_type;

enum SAFETY {
    SAFE,
    RISK,
    UNSAFE
}safety;


struct Weather_Type {
    int type;
    int intensity;
};


struct Point{
    double weight;
    double slope_x;
    double slope_y;
    double elevation;
    int soilType;
    int safetyType;
};

vector<vector<Point>> elevation_map;
unordered_map<bool, unordered_map<int, bool>> visited;
priority_queue<pair<int, int>> pathQueue;

pair<int, int> CompareFunction(int first_x, int first_y, int second_x, int second_y){
    if (!(visited.find(first_x) != visited.end() && visited[first_x].find(first_y) != visited[first_x].end())){
        if (elevation_map[first_x][first_y].safetyType == elevation_map[second_x][second_y].safetyType){
            if (elevation_map[first_x][first_y].weight < elevation_map[second_x][second_y].weight){
                return make_pair(first_x, first_y);
            }
        }
        else if (elevation_map[first_x][first_y].safetyType < elevation_map[second_x][second_y].safetyType){
            return make_pair(first_x, first_y);
        }
    }
    return make_pair(second_x, second_y);
}

class comp {
public:
    bool operator() (const pair<int, int> & A, const pair<int, int> & B){
        if (!(visited.find(A.first) != visited.end() &&
              visited[A.first].find(A.second) != visited[A.first].end())){
            if (elevation_map[A.first][A.second].safetyType ==
                elevation_map[B.first][B.second].safetyType){
                if (elevation_map[A.first][A.second].weight <
                    elevation_map[B.first][B.second].weight){
                    return true;
                }
            }
            else if (elevation_map[A.first][A.second].safetyType <
                     elevation_map[B.first][B.second].safetyType){
                return true;
            }
        }
        return false;
    }
};

//void print_graph_weight(vector<vector<Point>> & graph){
//    cout <<"Print the weight of the graph\n";
//    for (auto i : graph){
//        for (auto j : i){
//            cout << j.weight <<" ";
//        }
//        cout <<"\n";
//    }
//}

enum WEATHER_TYPE {
    SU,//sunshine
    SN,//snow
    RA,//rain
    BR,//mist
    DZ,
    IC,
    UP,
    FZ,
    SH,
    TS,
    PL,
    FG,
    HZ,
    FU,
    SA,
    WEATHER_TYPE_COUNT
} weather_type;

string weather_type_strings[WEATHER_TYPE_COUNT] = {
    "SU", "SN", "RA", "BR", "DZ", "IC", "UP", "FZ", "SH", "TS", "PL", "FG", "HZ", "FU", "SA"
};

enum WEATHER_INTENSITY {
    LIGHT,
    MODERATE,
    HEAVY,
    WEATHER_INTENSITY_COUNT
};

int get_weather_type(string weather_string){
    for (int i = 0; i < WEATHER_TYPE_COUNT; i++){
        if (weather_string.find(weather_type_strings[i]) != std::string::npos){
            return i;
        }
    }
    return 6;
}

int get_weather_intensity(string weather_string){
    if (weather_string.find("+")) return 2;
    else if (weather_string.find("-")) return 0;
    else return 1;
}

int transferToNum(pair<int, int> point){
    return MAP_SIZE * point.first + point.second;
}

int transferToNum(int A, int B){
    return MAP_SIZE * A + B;
}

pair<int, int> transferToPair(int Num){
    return make_pair(Num / MAP_SIZE, Num % MAP_SIZE);
}

vector<vector<double>> Cost;
vector<vector<int>> Path;
vector<vector<int>> NI;
vector<int> NNI;
vector<int> R;

pair<double, int> findMin(int j, int i){
    double min_cost = numeric_limits<double>::max();
    double current_cost = min_cost;
    int minPoint = j;
    for (int k = 0; k < NNI[j]; k++){
        pair<int, int> current_point = transferToPair(NI[j][k]);
        current_cost = elevation_map[current_point.first][current_point.second].weight +
                    Cost[NI[j][k]][i] + R[j] + 1;
        if (min_cost > current_cost){
            min_cost = current_cost;
            minPoint = NI[j][k];
        }
    }
    return make_pair(min_cost, minPoint);
}

/*  1. Mountain
    <CenterLat>44.2062527
    <CenterLon>-119.5812443
    <ULLat>44.2126250
    <ULLon>-119.5875305
    <URLat>44.2125222
    <URLon>-119.5747582
    <LRLat>44.1998805
    <LRLon>-119.5749582
    <LLLat>44.1999833
    <LLLon>-119.5877249
 
 
    2. Land
    <CenterLat>41.1003777
    <CenterLon>-86.4307332
    <ULLat>41.1072000
    <ULLon>-86.4399110
    <URLat>41.1073222
    <URLon>-86.4217110
    <LRLat>41.0935555
    <LRLon>-86.4215554
    <LLLat>41.0934333
    <LLLon>-86.4397499
 */
int main(int argc, const char * argv[]) {
    ios_base::sync_with_stdio(false);
    
    
    double ELEVATION_SAFE_RISK_BOUNDARY = 0.5;
    double ELEVATION_RISK_UNSAFE_BOUNDARY = 1;
    
    
    elevation_map.reserve(MAP_SIZE);
    
    long trash;
    double value;
    double formerValue = 0;
    
//    vector<long> x;
//    vector<long> y;
//    vector<double> z;
//    x.reserve(1000000);
//    y.reserve(1000000);
//    z.reserve(1000000);
    
    ifstream file;
    file.open("MountainView", ifstream::in);
    
//    vector<vector<int>> NI;
//    NI.resize(MAP_SIZE * MAP_SIZE);
////    NI[0] =
//    for (int i = 1; i < MAP_SIZE * MAP_SIZE - 1; i++){
//        NI[i].resize(4);
//    }
    
    int times = 500;
    Cost.resize(MAP_SIZE * MAP_SIZE);
    Path.resize(MAP_SIZE * MAP_SIZE);
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++){
        Cost[i].resize(times + 1);
        Path[i].resize(times + 1);
    }
    
    
    
    //Done data
    for (int i = 0; i < MAP_SIZE; i++){
        vector<Point> current_x;
        current_x.reserve(MAP_SIZE);
        
        for (int j = 0; j < MAP_SIZE; j++) {
//            cin >> trash; x.push_back(trash);
//            cin >> trash; y.push_back(trash);
//            cin >> value; z.push_back(value);
            file >> trash >> trash >> value;
            
            Point thisPoint;
            thisPoint.elevation = value;
            if (j == 0) {
                thisPoint.slope_x = 0;
            } else {
                thisPoint.slope_x = (value - formerValue);
            }
            if (i == 0) {
                thisPoint.slope_y = 0;
            } else {
                thisPoint.slope_y = (value - elevation_map[i-1][j].elevation);
            }
            thisPoint.weight = (thisPoint.slope_x + thisPoint.slope_y)/2;
            if (abs(thisPoint.weight) < ELEVATION_SAFE_RISK_BOUNDARY) thisPoint.safetyType = SAFE;
            else if (abs(thisPoint.weight) > ELEVATION_RISK_UNSAFE_BOUNDARY) thisPoint.safetyType = UNSAFE;
            else thisPoint.safetyType = RISK;
            
            current_x.push_back(thisPoint);
            formerValue = value;
        }
        elevation_map.push_back(current_x);
    }
    file.close();
    pair<int, int> startPoint = make_pair(123, 543);
    pair<int, int> endPoint = make_pair(56, 780);
    
    NI.resize(MAP_SIZE * MAP_SIZE);
    NNI.resize(MAP_SIZE * MAP_SIZE);
    R.resize(MAP_SIZE * MAP_SIZE);
    file.open("NNI", ifstream::in);
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++){
        file >> NNI[i];
    }
    file.close();
    file.open("NI", ifstream::in);
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++){
        NI[i].resize(4);
        for (int j = 0; j < 4; j++){
            file >> NI[i][j];
        }
    }
    file.close();
    
    int EndPoint = transferToNum(endPoint);
    NI[EndPoint][0] = EndPoint;
    NNI[EndPoint] = 1;
    R[EndPoint] = -10000;
    
    for (int i = 0; i < times; i++){
        for (int j = 0; j < MAP_SIZE * MAP_SIZE; j++){
            pair<double, int> minPoint = findMin(j, i);
//            if (j == EndPoint){
//                cout << minPoint.first << " " << minPoint.second << "\n";
//            }
            Cost[j][i+1] = minPoint.first;
            Path[j][i] = minPoint.second;
        }
    }
    
    int finalPoint = INT_MAX;
    int formerPoint = transferToNum(startPoint);
    cout << startPoint.first << " " << startPoint.second << "\n";
//    while (formerPoint != transferToNum(endPoint)) {
    for (int i = 0; i < 1000; i++){
        finalPoint = Path[formerPoint][times - 1];
        pair<int, int> thisPoint = transferToPair(finalPoint);
        cout << thisPoint.first << " " << thisPoint.second << "\n";
        formerPoint = finalPoint;
    }
    
//    }
    
//    cout << "Please input your start point: ";
//    cin >> startPoint.first;
//    cin >> startPoint.second;
//    cout << "Please input your end point: ";
//    cin >> endPoint.first;
//    cin >> endPoint.second;

    
    
//    int x_diff = endPoint.first - startPoint.first;
//    int y_diff = endPoint.second - startPoint.second;
//    int dx = 1; int dy = 1;
//    if (x_diff < 0) dx = -1;
//    else if (x_diff == 0) dx = 0;
//    if (y_diff < 0) dy = -1;
//    else if (y_diff == 0) dy = 0;
//    
//    
//    pair<int, int> currentPoint;
//    currentPoint = startPoint;
//    
//    
//    cout << currentPoint.first << " " << currentPoint.second << "\n";
////    int yyyy = 0;
//    
//    while((currentPoint.first != endPoint.first) || (currentPoint.second != endPoint.second)){
////        cout <<"current: "<<  currentPoint.first << " " << currentPoint.second <<endl;
//        visited[currentPoint.first][currentPoint.second] = true;
//        pair<int, int> nextPoint = currentPoint;
//        
//        if (currentPoint.first == endPoint.first) dx = 0;
//        else if (currentPoint.first < endPoint.first) dx = 1;
//        else dx = -1;
//        if (currentPoint.second == endPoint.second) dy = 0;
//        else if (currentPoint.second < endPoint.second) dy = 1;
//        else dy = -1;
//        cout <<"dx = " << dx << " dy = " << dy << endl;
//        
//        if (dx == 0) nextPoint.second += dy;
//        if (dy == 0) nextPoint.first += dx;
//        
////        int curx;
////        int cury;
//        if (dy != 0 && dx != 0){
//            nextPoint.second += dy;
//            nextPoint = CompareFunction(currentPoint.first + dx, currentPoint.second, nextPoint.first, nextPoint.second);
//            nextPoint = CompareFunction(currentPoint.first + dx, currentPoint.second + dy, nextPoint.first, nextPoint.second);
//        }
//        else if (dx == 0){
//            nextPoint = CompareFunction(currentPoint.first + 1, currentPoint.second, nextPoint.first, nextPoint.second);
//            nextPoint = CompareFunction(currentPoint.first - 1, currentPoint.second, nextPoint.first, nextPoint.second);
//        }
//        else if (dy == 0){
//            nextPoint = CompareFunction(currentPoint.first, currentPoint.second + 1, nextPoint.first, nextPoint.second);
//            nextPoint = CompareFunction(currentPoint.first, currentPoint.second - 1, nextPoint.first, nextPoint.second);
//        }
//        
//        currentPoint = nextPoint;
////        if (currentPoint.first == nextPoint.first && currentPoint.second == nextPoint.second){
////            nextPoint = make_pair(currentPoint.first + dx, currentPoint.second + dy);
////        } else {
////            currentPoint = nextPoint;
////        }
//        
//        cout << nextPoint.first << " " << nextPoint.second << "\n";
////        yyyy++;
////        if (yyyy > 10){
////            break;
////        }
//        
//    }
    
    cout << "Finished\n";
    
//    //Weather Map:  May Need parse ","
//    string weather_type;
//    long WEATHER_SIZE = 12000;
//    vector<Weather_Type> weather;
//    weather.reserve(WEATHER_SIZE);
//    int formerIntensity = 0;
//    int formerType = 0;
//    while(cin >> weather_type){
//        int type = get_weather_type(weather_type);
//        int intensity = get_weather_intensity(weather_type);
//        if (type != UP) {
//            formerType = type;
//            formerIntensity = intensity;
//        }
//        else {
//            type = formerType;
//            intensity = formerIntensity;
//        }
//        Weather_Type this_type;
//        this_type.type = type;
//        this_type.intensity = intensity;
//        weather.push_back(move(this_type));
//    }
    
//    Gnuplot g1("lines");
//    cout << "window 5: splot with hidden3d" << endl;
//    g1.set_isosamples(25).set_hidden3d();
//    g1.plot_xyz(x, y, z);
    
    
    return 0;
}


//    //Mountain
//    while(cin >> x){
//        cin >> y;
//        cin >> value;
//        x = (x/2 - 26)/974*1000;
//        y = (y/2 - 50)/950*1000;
//        if (x < 0) x = 0;
//        if (y < 0) y = 0;
//        if (map[(int) (x)][(int) (y)] != 0.0){
//            map[(int) (x)][(int) (y)] = (map[(int) (x)][(int) (y)] + value) / 2;
//        }
//        else{
//            map[(int) (x)][(int) (y)] = value;
//        }
//    }

//Land
//    while(cin >> x){
//        cin >> y;
//        cin >> value;
//        x = ((x/5) - 10)/991*1000;
//        y = ((y/5) - 4)/996*1000;
//        if (x < 0) x = 0;
//        if (y < 0) y = 0;
//        if (x > 999) x = 999;
//        if (y > 999) y = 999;
//        if (map[(int) (x)][(int) (y)] != 0.0){
//            map[(int) (x)][(int) (y)] = (map[(int) (x)][(int) (y)] + value) / 2;
//        }
//        else{
//            map[(int) (x)][(int) (y)] = value;
//        }
//    }
//
//    for (int i = 0; i < 1000; i++){
//        for (int j = 0; j < 1000; j++){
//            if (map[i][j] == 0){
//                if (i == 0 || i == 999 ){
//                    map[i][j] = (map[i][j-1] + map[i][j+1])/2;
//                }
//                else{
//                    map[i][j] = (map[i][j-1] + map[i][j+1] + map[i-1][j] + map[i+1][j])/4;
//                }
//            }
//
//
//            cout << i << " " << j << " " << map[i][j] << "\n";
//        }
//    }
//
