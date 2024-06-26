#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

enum State {
    START,
    CONTINUE,
    HALT
};

struct KnapSack {
    int weight;
    int n;
    vector<int> values;
    vector<int> costs;
    KnapSack() {}
    KnapSack(int weight, int n, vector<int>& values, vector<int>& costs) {
        this->weight = weight;
        this->n = n;
        this->values = values;
        this->costs = costs;
    }
};

class TuringMachine {
protected:
    vector<vector<int>> tape;
    unsigned int head;
    State currentState;
    KnapSack knapsack;

public:
    TuringMachine() {
        head = 0;
        currentState = START;
    }

    TuringMachine(const KnapSack& knapsack) {
        this->knapsack = knapsack;
        tape.resize(knapsack.n);
        for (int i = 0; i < knapsack.n; i++) {
            tape[i].resize(knapsack.weight + 1);
        }
        head = 0;
        currentState = START;
    }

    void run() {
        while (currentState != HALT) {
            step();
        }
    }

    virtual void step() = 0;

    void printTape() {
        for (auto& x : tape) {
            for (auto& y : x) {
                cout << y << "  ";
            }
            cout << endl;
        }
    };
};

class DP : public TuringMachine {
public:
    DP(const KnapSack& knapsack) : TuringMachine(knapsack) {}
    void step() {
        switch (currentState) {
        case START:
        {
            tape[0][0] = 0;
            for (int i = 1; i <= knapsack.weight; i++) {
                if (i >= knapsack.costs[0]) {
                    tape[0][i] = knapsack.values[0];
                }
                else {
                    tape[0][i] = 0;
                }
            }
            currentState = CONTINUE;
            break;
        }
        case CONTINUE:
        {
            static int currentTape = 1;
            int currentValue = tape[currentTape - 1][head];
            if (head < knapsack.costs[currentTape]) {
                tape[currentTape][head] = currentValue;
            }
            else {
                int nextValue = tape[currentTape - 1][head - knapsack.costs[currentTape]] + knapsack.values[currentTape];
                tape[currentTape][head] = max(currentValue, nextValue);
            }
            head++;
            if (head == knapsack.weight + 1) {
                currentTape++;
                if (currentTape == knapsack.n) {
                    currentState = HALT;
                    return;
                }
                head = 0;
            }
            break;
        }
        case HALT:
            break;
        }
    }
};

int main() {
    cout << "请输入背包容积：";
    int weight;
    cin >> weight;
    cout << "请输入物品数量：";
    int n;
    cin >> n;
    vector<int> values(n);
    vector<int> costs(n);
    cout << "请输入物品价值：";
    for (int i = 0; i < n; i++) {
        cin >> values[i];
    }
    cout << "请输入物品重量：";
    for (int i = 0; i < n; i++) {
        cin >> costs[i];
    }
    KnapSack knapSack(weight, n, values, costs);
    DP dp(knapSack);
    dp.run();
    dp.printTape();
    return 0;
}