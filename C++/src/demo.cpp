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
#include <assert.h>
using namespace std;

int MAP_SIZE = 1000;

enum SOIL_TYPE {
    SOIL,
    PAVEMENT,
    GRASS,
    MAD,
    SHALLOW_WATER,
    DEEP_WATER,
    OBSTACLE
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
    double slope;
    double action;
    double elevation;
    int soilType;
    int safetyType;
};

vector<vector<Point>> elevation_map;
unordered_map<bool, unordered_map<int, bool>> visited;
priority_queue<pair<int, int>> pathQueue;

vector<vector<double>> Cost;
vector<vector<int>> Path;
vector<vector<int>> NI;
vector<int> NNI;
vector<int> R;
unordered_map<int, int> validPoint;

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


//DP cost equation
pair<double, int> findMin(int j, int i){
    double min_cost = numeric_limits<double>::max();
    double current_cost = min_cost;
    int minPoint = j;
    for (int k = 0; k < NNI[j]; k++){
        int point = NI[j][k];
        current_cost = elevation_map[transferToPair(point).first][transferToPair(point).second].weight +
                    Cost[validPoint[point]][i] + R[j];
        if (min_cost > current_cost){
            min_cost = current_cost;
            minPoint = point;
        }
    }
    return make_pair(min_cost, minPoint);
}

void deleteNI(int x, int deleteNum){
//    cout << "Delete (x, deleteNUM): " << x << " " << deleteNum << "\n";
//    for (int i = 0; i < 8; i++) {
//        cout << NI[x][i] << " ";
//    }
//    cout << "\n";
    
    for (int i = 0; i < NNI[x]; i++) {
        if (NI[x][i] == deleteNum) {
            for (int j = i + 1; j < NNI[x]; j++) {
                NI[x][j - 1] = NI[x][j];
            }
            NI[x][NNI[x] - 1] = -1;
            break;
        }
    }
    NNI[x]--;
    
//    for (int i = 0; i < 8; i++) {
//        cout << NI[x][i] << " ";
//    }
//    cout << "\n";
}

void updateDeleteNum(int deleteX){
    pair<int, int> deletePoint = transferToPair(deleteX);
    elevation_map[deletePoint.first][deletePoint.second].slope = -1;
    NNI[deleteX] = 0;
}

void updateSlope(int x, int deleteX){
    pair<int, int> point = transferToPair(x);
    pair<int, int> deletePoint = transferToPair(deleteX);

//    cout << "Slope = " << elevation_map[point.first][point.second].slope << " " << NNI[x] << " ";
    double newSlope;
    if (NNI[x] == 1) {
        newSlope = 100000;
        deleteNI(x, deleteX);
    } else if (NNI[x] == 0){
//        cout << "Slope = " << elevation_map[point.first][point.second].slope << " " << NNI[x] << "x  = " << x << "deleteX = " << deleteX <<"\n";
        newSlope = 0;
    } else {
        newSlope  = (elevation_map[point.first][point.second].slope * NNI[x] -
           abs(elevation_map[deletePoint.first][deletePoint.second].elevation
               - elevation_map[point.first][point.second].elevation)) / (NNI[x] - 1);
        deleteNI(x, deleteX);
    }
    
//    cout << "New Slope = " << newSlope << "\n";
    elevation_map[point.first][point.second].slope = newSlope;
    elevation_map[point.first][point.second].weight = newSlope;
    
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
    
    cerr << "Start Initialization\n";
    elevation_map.reserve(MAP_SIZE);
    
    long trash;
    double value;
    double formerValue = 0;
    double maxWeight = 10000;
    
    ifstream file;
    file.open("MichiganLake", ifstream::in);
    
//    vector<vector<int>> NI;
//    NI.resize(MAP_SIZE * MAP_SIZE);
////    NI[0] =
//    for (int i = 1; i < MAP_SIZE * MAP_SIZE - 1; i++){
//        NI[i].resize(4);
//    }
    
    //DP Times
    int times = 700;
    int divideNum = 5;
    
    
    cerr << "Start Read data\n";
    //Reading data
    for (int i = 0; i < MAP_SIZE; i++){
        vector<Point> current_x;
        current_x.reserve(MAP_SIZE);
        for (int j = 0; j < MAP_SIZE; j++) {
            file >> trash >> trash >> value;
            
            Point thisPoint;
            thisPoint.elevation = value;
            
            current_x.push_back(thisPoint);
            formerValue = value;
        }
        elevation_map.push_back(current_x);
    }
    file.close();
    
    cerr << "Start Set Slope\n";
    //set slope
    double elevation = elevation_map[0][0].elevation;
    elevation_map[0][0].slope = (abs(elevation - elevation_map[0][1].elevation)
                                 + abs(elevation - elevation_map[1][1].elevation)
                                 + abs(elevation - elevation_map[1][0].elevation))/3;
    elevation_map[0][0].weight = elevation_map[0][0].slope;
    
    elevation = elevation_map[0][MAP_SIZE - 1].elevation;
    elevation_map[0][MAP_SIZE - 1].slope = (abs(elevation - elevation_map[0][MAP_SIZE - 2].elevation)
                                            + abs(elevation - elevation_map[1][MAP_SIZE - 1].elevation)
                                            + abs(elevation - elevation_map[1][MAP_SIZE - 2].elevation))/3;
    elevation_map[0][MAP_SIZE - 1].weight = elevation_map[0][MAP_SIZE - 1].slope;
    
    elevation = elevation_map[MAP_SIZE - 1][MAP_SIZE - 1].elevation;
    elevation_map[MAP_SIZE - 1][MAP_SIZE - 1].slope = (abs(elevation - elevation_map[MAP_SIZE - 1][MAP_SIZE - 2].elevation)
                                            + abs(elevation - elevation_map[MAP_SIZE - 2][MAP_SIZE - 1].elevation)
                                            + abs(elevation - elevation_map[MAP_SIZE - 2][MAP_SIZE - 2].elevation))/3;
    elevation_map[MAP_SIZE - 1][MAP_SIZE - 1].weight = elevation_map[MAP_SIZE - 1][MAP_SIZE - 1].slope;
    
    elevation = elevation_map[MAP_SIZE - 1][0].elevation;
    elevation_map[MAP_SIZE - 1][0].slope = (abs(elevation - elevation_map[MAP_SIZE - 1][0].elevation)
                                            + abs(elevation - elevation_map[MAP_SIZE - 2][1].elevation)
                                            + abs(elevation - elevation_map[MAP_SIZE - 1][1].elevation))/3;
    elevation_map[MAP_SIZE - 1][0].weight = elevation_map[MAP_SIZE - 1][0].slope;
    
    for (int j = 1; j < MAP_SIZE - 1; j++){
        elevation = elevation_map[0][j].elevation;
        elevation_map[0][j].slope = (abs(elevation - elevation_map[0][j + 1].elevation)
                                     + abs(elevation - elevation_map[0][j - 1].elevation)
                                     + abs(elevation - elevation_map[1][j + 1].elevation)
                                     + abs(elevation - elevation_map[1][j - 1].elevation)
                                     + abs(elevation - elevation_map[1][j].elevation))/5;
        elevation_map[0][j].weight = elevation_map[0][j].slope;
        
        elevation = elevation_map[j][0].elevation;
        elevation_map[j][0].slope = (abs(elevation - elevation_map[j - 1][0].elevation)
                                     + abs(elevation - elevation_map[j + 1][0].elevation)
                                     + abs(elevation - elevation_map[j - 1][1].elevation)
                                     + abs(elevation - elevation_map[j + 1][1].elevation)
                                     + abs(elevation - elevation_map[j][1].elevation))/5;
        elevation_map[j][0].weight = elevation_map[j][0].slope;
        
        elevation = elevation_map[j][MAP_SIZE - 1].elevation;
        elevation_map[j][MAP_SIZE - 1].slope = (abs(elevation - elevation_map[j - 1][MAP_SIZE - 1].elevation)
                                     + abs(elevation - elevation_map[j + 1][MAP_SIZE - 1].elevation)
                                     + abs(elevation - elevation_map[j - 1][MAP_SIZE - 2].elevation)
                                     + abs(elevation - elevation_map[j + 1][MAP_SIZE - 2].elevation)
                                     + abs(elevation - elevation_map[j][MAP_SIZE - 2].elevation))/5;
        elevation_map[j][MAP_SIZE - 1].weight = elevation_map[j][MAP_SIZE - 1].slope;
        
        elevation = elevation_map[MAP_SIZE - 1][j].elevation;
        elevation_map[MAP_SIZE - 1][j].slope = (abs(elevation - elevation_map[MAP_SIZE - 1][j + 1].elevation)
                                     + abs(elevation - elevation_map[MAP_SIZE - 1][j - 1].elevation)
                                     + abs(elevation - elevation_map[MAP_SIZE - 2][j + 1].elevation)
                                     + abs(elevation - elevation_map[MAP_SIZE - 2][j - 1].elevation)
                                     + abs(elevation - elevation_map[MAP_SIZE - 2][j].elevation))/5;
        elevation_map[MAP_SIZE - 1][j].weight = elevation_map[MAP_SIZE - 1][j].slope;
    }
    for (int i = 1; i < MAP_SIZE - 1; i += 5){
        for (int j = 1; j < MAP_SIZE - 1; j += 5){
            double elevation = elevation_map[i][j].elevation;
            elevation_map[i][j].slope = (abs(elevation - elevation_map[i - 1][j].elevation)
                                         + abs(elevation - elevation_map[i + 1][j].elevation)
                                         + abs(elevation - elevation_map[i][j + 1].elevation)
                                         + abs(elevation - elevation_map[i][j - 1].elevation)
                                         + abs(elevation - elevation_map[i + 1][j + 1].elevation)
                                         + abs(elevation - elevation_map[i - 1][j - 1].elevation)
                                         + abs(elevation - elevation_map[i - 1][j + 1].elevation)
                                         + abs(elevation - elevation_map[i + 1][j - 1].elevation))/8;
            elevation_map[i][j].weight = elevation_map[i][j].slope;
        }
    }
    
    //200 * 200
    pair<int, int> startPoint = make_pair(43 * 5, 65 * 5);
    pair<int, int> startPoint2 = make_pair(12 * 5, 150 * 5);
    pair<int, int> startPoint3 = make_pair(198 * 5, 8 * 5);
    pair<int, int> endPoint = make_pair(98 * 5, 76 * 5);
    
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
        NI[i].resize(8);
        for (int j = 0; j < 8; j++){
            file >> NI[i][j];
        }
    }
    file.close();
    
    int EndPoint = transferToNum(endPoint);
    NI[EndPoint][0] = EndPoint;
    NNI[EndPoint] = 1;
    R[EndPoint] = -10000;
    
    cerr << "Start Slope Threshold\n";
    //Set slope threshold (weather)
    double Slope_Threshold = 0.35;
    
    for (int k = 0; k < 7; k++) {
        for (int i = 0; i < MAP_SIZE; i++){
            for (int j = 0; j < MAP_SIZE; j++){
                if (elevation_map[i][j].slope > Slope_Threshold) {
                    int point = transferToNum(i, j);
                    for (int k = 0; k < NNI[point]; k++){
                        updateSlope(NI[point][k], point);
                    }
                    updateDeleteNum(point);
                }//if
            }//for j
        }//for i
    }//for k

    cerr << "Start Set Action Cost\n";
    //All Valid Points
    int validPointNum = 0;
    double averageSlope = 0;
    double averageAction = 0;
    validPoint.reserve(MAP_SIZE * MAP_SIZE);
    vector<int> validPointVector;
    validPointVector.reserve(MAP_SIZE * MAP_SIZE);
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++) {
        if (NNI[i] != 0) {
            validPoint.emplace(i, validPointNum);
            validPointVector.push_back(i);
            validPointNum++;
            pair<int, int> point = transferToPair(i);
            double currentSlope = elevation_map[point.first][point.second].slope;
            averageSlope += currentSlope;
            double action = 0;
            for (int j = 0; j < NNI[i]; j++) {
                action += abs(currentSlope - elevation_map[transferToPair(NNI[j]).first][transferToPair(NNI[j]).second].slope);
            }
            action /= NNI[i];
            elevation_map[point.first][point.second].action = action;
            averageAction += action;
        }
    }
    averageSlope /= (validPointNum + 1);
    averageAction /= (validPointNum + 1);
    double a = averageSlope / averageAction;
    a++;
    a = 1/a;
    double b = 1 - a;
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++) {
        if (NNI[i] != 0) {
//            pair<int, int> point = transferToPair(i);
            Point point = elevation_map[transferToPair(i).first][transferToPair(i).second];
            point.weight = a * point.slope + b * point.action;
        }
    }
    
    //Set action cost
    
    
//    file.open("MountainViewLandType", ifstream::in);
//    int soilType;
//    for (int i = 0; i < 1000; i++){
//        for (int j = 0; j < 1000; j++){
//            file >> soilType;
//            if (soilType == 0) {
//                elevation_map[i][j].soilType = SOIL;
//            } else if (soilType == 1) {
//                elevation_map[i][j].soilType = PAVEMENT;
//            } else if (soilType == 2) {
//                elevation_map[i][j].soilType = OBSTACLE;
//            } else {
//                elevation_map[i][j].soilType = SHALLOW_WATER;
//            }
//        }
//    }
//    file.close();
    
    cerr << "   validPointNum = " << validPointNum << "\n";
    Cost.resize(validPointNum);
    Path.resize(validPointNum);
    for (int i = 0; i < validPointNum; i++){
        Cost[i].resize(times + 1);
        Path[i].resize(times + 1);
    }
    
    cerr << "Start DP\n";
    for (int i = 0; i < times; i++){
        cerr << fixed << setprecision(2) <<(double) i / times * 100 << "%";
        for (int j = 0; j < validPointNum; j++){
            pair<double, int> minPoint = findMin(validPointVector[j], i);
            Cost[j][i+1] = minPoint.first;
            Path[j][i] = validPoint[minPoint.second];
        }
        cerr << "\r";
    }
    
    
    cerr << "Start Generate Path\n";
    //First Path
    
    int finalPoint = INT_MAX;
    int formerPoint = validPoint[transferToNum(startPoint)];
    cout << startPoint.first << " " << startPoint.second << "\n";
    bool safety = true;
    for (int i = 0; i < 1000; i++){
        finalPoint = Path[formerPoint][times - 1];
        pair<int, int> thisPoint = transferToPair(validPointVector[finalPoint]);
        cout << thisPoint.first << " " << thisPoint.second << "\n";
        formerPoint = finalPoint;
//        if (elevation_map[thisPoint.first][thisPoint.second].weight == maxWeight) {
//            safety = false;
//        }
    }
//    if (safety) {
//        cerr << "1: This path is safe.\n";
//    } else {
//        cerr << "1: Can not reach with safe path.\n";
//    }
    
    //Second Path
    
    formerPoint = validPoint[transferToNum(startPoint2)];
    cout << startPoint2.first << " " << startPoint2.second << "\n";
    safety = true;
    for (int i = 0; i < 1000; i++){
        finalPoint = Path[formerPoint][times - 1];
        pair<int, int> thisPoint = transferToPair(validPointVector[finalPoint]);
        cout << thisPoint.first << " " << thisPoint.second << "\n";
        formerPoint = finalPoint;
//        if (elevation_map[thisPoint.first][thisPoint.second].weight == maxWeight) {
//            safety = false;
//        }
    }
//    if (safety) {
//        cerr << "2: This path is safe.\n";
//    } else {
//        cerr << "2: Can not reach with safe path 2.\n";
//    }
    
    //Third Path
    
    formerPoint = validPoint[transferToNum(startPoint3)];
    cout << startPoint3.first << " " << startPoint3.second << "\n";
    safety = true;
    for (int i = 0; i < 1000; i++){
        finalPoint = Path[formerPoint][times - 1];
        pair<int, int> thisPoint = transferToPair(validPointVector[finalPoint]);
        cout << thisPoint.first << " " << thisPoint.second << "\n";
        formerPoint = finalPoint;
//        if (elevation_map[thisPoint.first][thisPoint.second].weight == maxWeight) {
//            safety = false;
//        }
    }
//    if (safety) {
//        cerr << "3: This path is safe.\n";
//    } else {
//        cerr << "3: Can not reach with safe path.\n";
//    }
    
    cerr << "Finished\n";
    
    return 0;
}

//    }

//                    if (i == 0) {
//                        if (j == 0) {
//                            updateSlope(1, point);
//                            updateSlope(1001, point);
//                            updateSlope(1000, point);
//                        } else if (j == MAP_SIZE - 1) {
//                            updateSlope(transferToNum(i + 1, j), point);
//                            updateSlope(transferToNum(i + 1, j - 1), point);
//                            updateSlope(transferToNum(i, j - 1), point);
//                        } else {
//                            updateSlope(transferToNum(i, j + 1), point);
//                            updateSlope(transferToNum(i, j - 1), point);
//                            updateSlope(transferToNum(i + 1, j + 1), point);
//                            updateSlope(transferToNum(i + 1, j - 1), point);
//                            updateSlope(transferToNum(i + 1, j), point);
//                        }
//                    } else if (i == MAP_SIZE - 1) {
//                        if (j == 0) {
//                            updateSlope(transferToNum(i, j + 1), point);
//                            updateSlope(transferToNum(i - 1, j + 1), point);
//                            updateSlope(transferToNum(i - 1, j), point);
//                        } else if (j == MAP_SIZE - 1) {
//                            updateSlope(transferToNum(i - 1, j), point);
//                            updateSlope(transferToNum(i - 1, j - 1), point);
//                            updateSlope(transferToNum(i, j - 1), point);
//                        } else {
//                            updateSlope(transferToNum(i, j + 1), point);
//                            updateSlope(transferToNum(i, j - 1), point);
//                            updateSlope(transferToNum(i - 1, j + 1), point);
//                            updateSlope(transferToNum(i - 1, j - 1), point);
//                            updateSlope(transferToNum(i - 1, j), point);
//                        }
//                    } else {
//                        updateSlope(transferToNum(i, j + 1), point);
//                        updateSlope(transferToNum(i, j - 1), point);
//                        updateSlope(transferToNum(i + 1, j + 1), point);
//                        updateSlope(transferToNum(i + 1, j - 1), point);
//                        updateSlope(transferToNum(i + 1, j), point);
//                        updateSlope(transferToNum(i - 1, j), point);
//                        updateSlope(transferToNum(i - 1, j + 1), point);
//                        updateSlope(transferToNum(i - 1, j - 1), point);
//                    }


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
//INWater
//    double x;
//    double y;
//    double value;
//    vector<vector<double>> map;
//    map.resize(1000);
//    for (int i = 0; i < 1000; i++) {
//        map[i].resize(1000);
//    }
//
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
//            cout << i << " " << j << " " << map[i][j] << "\n";
//        }
//    }
//
//    return 0;

