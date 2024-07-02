#include <iostream>
#include <vector>
#include <queue>
#include <bitset>
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
    bitset<100> chosed;
    KnapSack() {}
    KnapSack(int weight, int n, vector<int>& values, vector<int>& costs) {
        this->weight = weight;
        this->n = n;
        this->values = values;
        this->costs = costs;
        this->chosed.reset();
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
        tape.resize(knapsack.n + 1);
        for (int i = 0; i <= knapsack.n; i++) {
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

    virtual void printTape() = 0;

    void JudgeChoded() {
        int currW = knapsack.weight;
        for (int i = knapsack.n; i > 0; i--) {
            if (tape[i][currW] != tape[i - 1][currW]) {
                knapsack.chosed.set(i, 1);
                currW -= knapsack.costs[i];
            }
        }
    }

    void showChosed() {
        for (int i = 1; i <= knapsack.n; i++) {
            if (knapsack.chosed[i]) {
                cout << "选中第" << i << "个物品" << endl;
            }
        }
        cout << "总价值为：" << tape[knapsack.n][knapsack.weight] << endl;
    }
};

class DP : public TuringMachine {
public:
    DP(const KnapSack& knapsack) : TuringMachine(knapsack) {}
    void step() {
        switch (currentState) {
        case START:
        {
            tape[1][0] = 0;
            head++;
            currentState = CONTINUE;
            break;
        }
        case CONTINUE:
        {
            static int currentTape = 1;
            if (currentTape == 1) {
                if (head >= knapsack.costs[1]) {
                    tape[1][head] = knapsack.values[1];
                }
                else {
                    tape[1][head] = 0;
                }
                currentState = CONTINUE;
                head++;
                if (head == knapsack.weight + 1) {
                    currentTape++;
                    head = 0;
                }
                break;
            }
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
                if (currentTape == knapsack.n + 1) {
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

    void printTape() {
        for (int i = 1; i <= knapsack.n; i++) {
            for (int j = 0; j <= knapsack.weight; j++) {
                cout << tape[i][j] << " ";
            }
            cout << endl;
        }
    }
};

struct Node {
    int currentValue;
    int currentWeight;
    int level;
    Node* parent;
    bool selected;
    Node() {}
    Node(int currentValue, int currentWeight, int level, Node* parent, bool selected) {
        this->currentValue = currentValue;
        this->currentWeight = currentWeight;
        this->level = level;
        this->parent = parent;
        this->selected = selected;
    }
    Node(const Node& node) {
        this->currentValue = node.currentValue;
        this->currentWeight = node.currentWeight;
        this->level = node.level;
        this->parent = node.parent;
        this->selected = node.selected;
    }
};

Node* bestValue = nullptr;
queue<Node*> q;

class BranchAndBound : public TuringMachine {
public:
    BranchAndBound(const KnapSack& knapsack) : TuringMachine(knapsack) {
        Node* root = new Node(0, 0, 0, nullptr, false);
        q.push(root);
    }

    void step() override {
        if (q.empty()) {
            currentState = HALT;
            return;
        }
        Node* node = q.front();
        q.pop();

        int nextLevel = node->level + 1;
        if (nextLevel > knapsack.n) {
            if (bestValue == nullptr || node->currentValue > bestValue->currentValue) {
                delete bestValue; // 防止内存泄漏
                bestValue = new Node(*node); // 更新最佳值节点
            }
            delete node; // 处理完毕，释放当前节点内存
            return;
        }

        // 尝试包含下一个物品
        if (node->currentWeight + knapsack.costs[nextLevel] <= knapsack.weight) {
            Node* leftChild = new Node(
                node->currentValue + knapsack.values[nextLevel],
                node->currentWeight + knapsack.costs[nextLevel],
                nextLevel,
                node,
                true
            );
            q.push(leftChild);
        }

        // 尝试不包含下一个物品
        Node* rightChild = new Node(
            node->currentValue,
            node->currentWeight,
            nextLevel,
            node,
            false
        );
        q.push(rightChild);
        queue<Node*> temp(q);
        while (!temp.empty()) {
            cout << "(" << temp.front()->currentValue << " " << temp.front()->currentWeight << ") ";
            temp.pop();
        }
        cout << endl;
    }

    void printTape() override {
        if (bestValue == nullptr) {
            cout << "没有找到最优解。" << endl;
            return;
        }
        cout << "最优解路径：" << endl;
        Node* node = bestValue;
        while (node->parent != nullptr) { // 根节点没有选择，所以不打印
            if (node->selected) {
                cout << "选中第" << node->level << "个物品，价值：" << knapsack.values[node->level] << "，重量：" << knapsack.costs[node->level] << endl;
            }
            node = node->parent;
        }
        cout << "总价值为：" << bestValue->currentValue << endl;
    }
};

int main() {
    cout << "请输入背包容积：";
    int weight;
    cin >> weight;
    cout << "请输入物品数量：";
    int n;
    cin >> n;
    vector<int> values(n + 1);
    vector<int> costs(n + 1);
    cout << "请输入物品价值：";
    for (int i = 1; i <= n; i++) {
        cin >> values[i];
    }
    cout << "请输入物品重量：";
    for (int i = 1; i <= n; i++) {
        cin >> costs[i];
    }
    KnapSack knapSack(weight, n, values, costs);
    DP dp(knapSack);
    dp.run();
    dp.JudgeChoded();
    dp.printTape();
    dp.showChosed();
    // BranchAndBound bb(knapSack);
    // bb.run();
    // bb.printTape();
    return 0;
}
