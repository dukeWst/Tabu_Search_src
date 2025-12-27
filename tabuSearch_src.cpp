#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <climits>
#include <list>
#include <random>
#include <iomanip>

using namespace std;

// Cau truc dai dien cho mot buoc di chuyen (Hoan doi 2 thanh pho tai vi tri i va j)
struct Move {
    int index1;
    int index2;

    bool operator==(const Move& other) const {
        return (index1 == other.index1 && index2 == other.index2) ||
               (index1 == other.index2 && index2 == other.index1);
    }
};

class TabuSearchTSP {
private:
    int numCities;
    vector<vector<int>> distanceMatrix; // Ma tran khoang cach
    int tabuTenure; // Thoi han cam (so vong lap nam trong danh sach Tabu)
    int maxIterations; // Dieu kien dung
    list<Move> tabuList; // Danh sach Tabu

public:
    TabuSearchTSP(int cities, int tenure, int maxIter) 
        : numCities(cities), tabuTenure(tenure), maxIterations(maxIter) {
        generateDistanceMatrix();
    }

    // Tao ma tran khoang cach ngau nhien (hoac hardcode theo de bai)
    void generateDistanceMatrix() {
        distanceMatrix.resize(numCities, vector<int>(numCities));
        // Vi du ma tran doi xung don gian cho 5 thanh pho (A, B, C, D, E)
        // De demo, ta dung du lieu gia lap ngau nhien nhung co dinh seed de ket qua giong nhau
        mt19937 rng(42); 
        uniform_int_distribution<int> dist(10, 100);

        for (int i = 0; i < numCities; ++i) {
            for (int j = i + 1; j < numCities; ++j) {
                int d = dist(rng);
                distanceMatrix[i][j] = d;
                distanceMatrix[j][i] = d;
            }
            distanceMatrix[i][i] = 0;
        }
    }

    // Tinh tong chi phi (do dai quang duong) cua mot hanh trinh
    int calculateCost(const vector<int>& solution) {
        int cost = 0;
        for (size_t i = 0; i < solution.size() - 1; ++i) {
            cost += distanceMatrix[solution[i]][solution[i+1]];
        }
        // Cong khoang cach tu diem cuoi ve diem dau
        cost += distanceMatrix[solution.back()][solution[0]];
        return cost;
    }

    // Kiem tra xem mot nuoc di co bi cam khong
    bool isTabu(const Move& move) {
        for (const auto& m : tabuList) {
            if (m == move) return true;
        }
        return false;
    }

    // Them nuoc di vao danh sach Tabu va xoa cai cu nhat neu day
    void updateTabuList(const Move& move) {
        tabuList.push_back(move);
        if (tabuList.size() > tabuTenure) {
            tabuList.pop_front();
        }
    }

    // Ham chinh thuc hien giai thuat
    void solve() {
        [cite_start]// 1. Khoi tao: Tao nghiem ban dau ngau nhien [cite: 12]
        vector<int> currentSolution(numCities);
        for(int i=0; i<numCities; ++i) currentSolution[i] = i;
        
        // Tron ngau nhien de co diem bat dau (tru thanh pho 0 de co dinh diem xuat phat cho de nhin)
        shuffle(currentSolution.begin() + 1, currentSolution.end(), mt19937(random_device{}()));

        vector<int> bestSolution = currentSolution;
        int currentCost = calculateCost(currentSolution);
        int bestCost = currentCost;

        cout << "--- BAT DAU TABU SEARCH ---" << endl;
        cout << "Nghiem ban dau: " << bestCost << endl;

        [cite_start]// Vong lap chinh [cite: 16]
        for (int iter = 0; iter < maxIterations; ++iter) {
            
            // Tim lan can tot nhat
            Move bestMove = {-1, -1};
            int bestNeighborCost = INT_MAX;
            vector<int> bestNeighborSol;

            // Duyet qua tat ca cac lan can bang cach hoan doi 2 thanh pho (Swap) 
            // Khong doi cho thanh pho dau tien (index 0) de giu diem xuat phat co dinh
            for (int i = 1; i < numCities; ++i) {
                for (int j = i + 1; j < numCities; ++j) {
                    
                    // Tao lan can
                    vector<int> neighborSol = currentSolution;
                    swap(neighborSol[i], neighborSol[j]);
                    
                    int neighborCost = calculateCost(neighborSol);
                    Move currentMove = {i, j};

                    // Kiem tra Tabu va Tieu chi Aspiration 
                    bool is_tabu = isTabu(currentMove);
                    bool aspiration = (neighborCost < bestCost); // Neu tot hon ky luc toan cuc -> Pha vo Tabu

                    // Chon nghiem tot nhat trong dam lan can (cho phep chon neu khong bi Tabu HOAC thoa man Aspiration)
                    if (!is_tabu || aspiration) {
                        if (neighborCost < bestNeighborCost) {
                            bestNeighborCost = neighborCost;
                            bestMove = currentMove;
                            bestNeighborSol = neighborSol;
                        }
                    }
                }
            }

            // Neu tim duoc nuoc di hop le
            if (bestMove.index1 != -1) {
                currentSolution = bestNeighborSol;
                currentCost = bestNeighborCost;
                
                [cite_start]// Cap nhat Tabu List [cite: 15]
                updateTabuList(bestMove);

                // Cap nhat ket qua tot nhat toan cuc
                if (currentCost < bestCost) {
                    bestCost = currentCost;
                    bestSolution = currentSolution;
                    cout << "Iter " << iter << ": Tim thay ky luc moi! Chi phi = " << bestCost << " (Aspiration)" << endl;
                }
            } else {
                // Truong hop hiem: tat ca lan can deu bi Tabu va khong thoa man Aspiration
                cout << "Iter " << iter << ": Khong tim thay nuoc di hop le." << endl;
            }
        }

        cout << "\n--- KET QUA CUOI CUNG ---" << endl;
        cout << "Chi phi thap nhat tim duoc: " << bestCost << endl;
        cout << "Hanh trinh: ";
        for (int city : bestSolution) {
            cout << city << " -> ";
        }
        cout << bestSolution[0] << endl; // Quay ve diem dau
    }
};

int main() {
    int numCities = 10;    // So thanh pho
    int tabuTenure = 5;    // Kich thuoc danh sach Tabu
    int maxIter = 100;     [cite_start]// So vong lap toi da [cite: 10]

    TabuSearchTSP tsp(numCities, tabuTenure, maxIter);
    tsp.solve();

    return 0;
}
