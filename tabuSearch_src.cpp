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
    int tabuTenure; // Thoi han cam
    int maxIterations; // Dieu kien dung
    list<Move> tabuList; // Danh sach Tabu

public:
    TabuSearchTSP(int cities, int tenure, int maxIter) 
        : numCities(cities), tabuTenure(tenure), maxIterations(maxIter) {
        generateDistanceMatrix();
    }

    // Tao ma tran khoang cach gia lap
    void generateDistanceMatrix() {
        distanceMatrix.resize(numCities, vector<int>(numCities));
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

    // Tinh tong chi phi
    int calculateCost(const vector<int>& solution) {
        int cost = 0;
        for (size_t i = 0; i < solution.size() - 1; ++i) {
            cost += distanceMatrix[solution[i]][solution[i+1]];
        }
        cost += distanceMatrix[solution.back()][solution[0]];
        return cost;
    }

    bool isTabu(const Move& move) {
        for (const auto& m : tabuList) {
            if (m == move) return true;
        }
        return false;
    }

    void updateTabuList(const Move& move) {
        tabuList.push_back(move);
        if (tabuList.size() > (size_t)tabuTenure) {
            tabuList.pop_front();
        }
    }

    void solve() {
        // 1. Khoi tao: Tao nghiem ban dau
        vector<int> currentSolution(numCities);
        for(int i=0; i<numCities; ++i) currentSolution[i] = i;
        
        // Giu nguyen thanh pho 0, chi xao tron cac thanh pho con lai
        shuffle(currentSolution.begin() + 1, currentSolution.end(), mt19937(random_device{}()));

        vector<int> bestSolution = currentSolution;
        int currentCost = calculateCost(currentSolution);
        int bestCost = currentCost;

        cout << "--- BAT DAU TABU SEARCH ---" << endl;
        cout << "Nghiem ban dau: " << bestCost << endl;

        // Vong lap chinh
        for (int iter = 0; iter < maxIterations; ++iter) {
            
            Move bestMove = {-1, -1};
            int bestNeighborCost = INT_MAX;
            vector<int> bestNeighborSol;

            // Duyet qua cac lang gieng (Swap)
            for (int i = 1; i < numCities; ++i) {
                for (int j = i + 1; j < numCities; ++j) {
                    
                    vector<int> neighborSol = currentSolution;
                    swap(neighborSol[i], neighborSol[j]);
                    
                    int neighborCost = calculateCost(neighborSol);
                    Move currentMove = {i, j};

                    bool is_tabu = isTabu(currentMove);
                    // Aspiration: Neu tot hon Best Global thi chap nhan ke ca khi bi Tabu
                    bool aspiration = (neighborCost < bestCost); 

                    if (!is_tabu || aspiration) {
                        if (neighborCost < bestNeighborCost) {
                            bestNeighborCost = neighborCost;
                            bestMove = currentMove;
                            bestNeighborSol = neighborSol;
                        }
                    }
                }
            }

            // Xu ly ket qua tim duoc
            if (bestMove.index1 != -1) {
                currentSolution = bestNeighborSol;
                currentCost = bestNeighborCost;
                
                updateTabuList(bestMove);

                if (currentCost < bestCost) {
                    bestCost = currentCost;
                    bestSolution = currentSolution;
                    cout << "Iter " << iter << ": Tim thay ky luc moi! Chi phi = " << bestCost << endl;
                }
            } else {
                // SUA LOI: Thoat vong lap neu khong con nuoc di nao kha thi
                cout << "Iter " << iter << ": Be tac (Tat ca nuoc di deu bi Tabu). Dung thuat toan." << endl;
                break; 
            }
        }

        cout << "\n--- KET QUA CUOI CUNG ---" << endl;
        cout << "Chi phi thap nhat: " << bestCost << endl;
        cout << "Hanh trinh: ";
        for (int city : bestSolution) {
            cout << city << " -> ";
        }
        cout << bestSolution[0] << endl; 
    }
};

int main() {
    int numCities = 10;    
    int tabuTenure = 5;    
    int maxIter = 100;      

    TabuSearchTSP tsp(numCities, tabuTenure, maxIter);
    tsp.solve();

    return 0;
}
