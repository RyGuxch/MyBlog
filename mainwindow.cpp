#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPixmap>
#include <QTime>
#include <QPushButton>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    paused(false),
    infoDisplay(nullptr)  // 初始化为 nullptr
{
    ui->setupUi(this);
    // 设置随机数生成器的种子
    srand(QTime::currentTime().msec());

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    ui->graphicsView->setBackgroundBrush(Qt::lightGray);

    // 在 UI 界面中找到 QPlainTextEdit 组件
    infoDisplay = ui->plainTextEdit;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateSimulation);
    timer->start(2000);  // 设置定时器间隔（毫秒）

    // 连接暂停按钮的点击事件到槽函数
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::togglePause);

}

MainWindow::~MainWindow()
{
    delete ui;
}

Map::Map() {
    // 随机生成一些草地和障碍物

}

// 获取指定位置的地形
Terrain Map::getTerrain(int x, int y) {
    return terrain[x][y];
}

// 设置指定位置的地形
void Map::setTerrain(int x, int y, Terrain t) {
    terrain[x][y] = t;
}

Weather::Weather() {
    // 随机生成雨天或晴天
    int r = rand() % 2; // 生成0或1的随机数
    if (r == 0) { // 50%的概率生成雨天
        current = RAINY;
    } else { // 50%的概率生成晴天
        current = SUNNY;
    }
}

// 获取当前的天气
WEATHER Weather::getCurrent() {
    return current;
}

// 改变当前的天气
void Weather::change() {
    // 随机改变雨天或晴天
    int r = rand() % 2; // 生成0或1的随机数
    if (r == 0) { // 50%的概率改变为雨天
        current = RAINY;
    } else { // 50%的概率改变为晴天
        current = SUNNY;
    }
}

void Disaster::occur(Map &map) {
    // 随机生成一些新的障碍物
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            int r = rand() % 100; // 生成0到99之间的随机数
            if (r < 10) { // 10%的概率生成障碍物
                map.setTerrain(i, j, OBSTACLE);
            }
        }
    }
}

Grass::Grass(int x, int y) {
    // 随机生成数量
    amount = rand() % 10 + 1; // 生成1到10之间的随机数
    // 设置位置
    this->x = x;
    this->y = y;
}

// 获取数量
int Grass::getAmount() {
    return amount;
}

// 设置数量
void Grass::setAmount(int amount) {
    this->amount = amount;
}

// 获取位置
void Grass::getPosition(int &x, int &y) {
    x = this->x;
    y = this->y;
}

Obstacle::Obstacle(int x, int y) {
    //设置位置
    this->x = x;
    this->y = y;
}

void Obstacle::getPosition(int &x, int &y) {
    x = this->x;
    y = this->y;
}

Animal::Animal(AnimalType type, Gender gender, int x, int y) {
    // 设置动物的种类
    this->type = type;
    // 设置性别
    this->gender = gender;
    //设置年龄
    age = 0;
    // 设置年龄状态为幼年
    ageState = YOUNG;
    // 设置健康状态为80
    health = 100;
    // 设置位置
    this->x = x;
    this->y = y;
    //设置饱食度
    satiety = 100;
}

// 获取动物的种类
AnimalType Animal::getType() {
    return type;
}

// 获取性别
Gender Animal::getGender() {
    return gender;
}

//获取年龄

int Animal::getAge() {
    return age;
}
// 获取年龄状态
AgeState Animal::getAgeState() {
    return ageState;
}

// 设置年龄状态
void Animal::setAgeState(AgeState ageState) {
    this->ageState = ageState;
}

// 获取健康状态
int Animal::getHealth() {
    return health;
}

// 设置健康状态
void Animal::setHealth(int health) {
    this->health = health;
}

// 获取位置
void Animal::getPosition(int &x, int &y) {
    x = this->x;
    y = this->y;
}

// 设置位置
void Animal::setPosition(int x, int y) {
    this->x = x;
    this->y = y;
}


// 判断动物是否可以繁衍
bool Animal::canBreed() {
    return ageState == ADULT;
}

Sheep::Sheep(Gender gender, int x, int y):Animal::Animal(SHEEP,gender,x,y){
    // 调用父类的构造函数
    //设置视野范围为10
    sight = 4;
}

// 判断羊是否死亡
bool Sheep::isDead() {
    return health <= 0 || age > 30 || satiety <= 0;
}

// 判断羊是否可以捕食另一种动物
bool Sheep::canEat(Animal *other) {
    // 羊不能捕食任何动物
    return false;
}

// 羊的行为，包括移动、吃草、繁衍等
void Sheep::act(Map &map, Weather &weather, Disaster &disaster, vector<Grass*> &grasses, vector<Animal*> &animals) {
    // 实现羊的行为逻辑
    // 如果羊已经死亡，直接返回
    if (isDead()) {
        return;
    }

    //年龄增长
    age += 1;
    //饱食度下降
    satiety -= 6;

    // 如果天气是雨天，羊的健康状态下降10点
    if (weather.getCurrent() == RAINY) {
        health -= 10;
    }

    // 如果羊的年龄大于4，羊的年龄状态变为成年
    if (age > 10) {
        ageState = ADULT;
    }

    // 如果羊的年龄大于10，羊的年龄状态变为成年
    if (age > 25) {
        ageState = OLD;
        health -= 5;    //额外健康衰减
    }

    // 如果羊的年龄大于15，羊死亡
    if(age > 30) {

    }

    // 遍历动物的向量，判断是否有狼在附近
    bool hasWolf = false; // 标记是否有狼在附近
    // 寻找视野范围内的最近的狼，使用广度优先搜索算法
    int targetX = -1, targetY = -1; // 目标狼的位置
    int minDistance = INT_MAX; // 最小距离
    bool visited[MAP_HEIGHT][MAP_WIDTH]; // 记录已经访问过的位置
    memset(visited, false, sizeof(visited)); // 初始化为false
    queue<pair<int, int> > q; // 使用队列来存储搜索过程中的位置
    q.push(make_pair(x, y)); // 将当前位置入队
    visited[x][y] = true; // 标记当前位置已访问
    while (!q.empty()) { // 当队列不为空时，继续搜索
        // 取出队首的位置
        int curX = q.front().first;
        int curY = q.front().second;
        q.pop();
        // 遍历该位置的上下左右四个相邻位置
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                // 如果是对角线方向，跳过
                if (abs(i) + abs(j) != 1) {
                    continue;
                }
                // 计算相邻位置的坐标
                int nextX = curX + i;
                int nextY = curY + j;
                // 如果坐标越界，跳过
                if (nextX < 0 || nextX >= MAP_HEIGHT || nextY < 0 || nextY >= MAP_WIDTH) {
                    continue;
                }
                // 如果该位置已经访问过，跳过
                if (visited[nextX][nextY]) {
                    continue;
                }
                // 如果该位置是障碍物，跳过
                if (map.getTerrain(nextX, nextY) == OBSTACLE) {
                    continue;
                }
                // 如果该位置到当前位置的距离大于视野范围，跳过
                if (sqrt((nextX - x)*(nextX - x) + (nextY - y)*(nextY - y)) > sight) {
                    continue;
                }
                // 遍历动物的向量，判断是否有狼在该位置
                for (int k = 0; k < animals.size(); k++) {
                    // 获取动物的种类和位置
                    AnimalType type = animals[k]->getType();
                    int animalX, animalY;
                    animals[k]->getPosition(animalX, animalY);
                    // 如果动物是狼，并且位置和该位置相同，判断是否是最近的狼
                    if (type == WOLF && animalX == nextX && animalY == nextY) {
                        // 计算该位置到当前位置的距离
                        int distance = sqrt((nextX - x)*(nextX - x) + (nextY - y)*(nextY - y));
                        // 如果距离小于最小距离，更新目标羊的位置和最小距离
                        if (distance < minDistance) {
                            targetX = nextX;
                            targetY = nextY;
                            minDistance = distance;
                            hasWolf = true;
                        }
                    }
                }
                // 将该位置入队
                q.push(make_pair(nextX, nextY));
                // 标记该位置已访问
                visited[nextX][nextY] = true;
            }
        }
    }

    // 如果找到了目标狼，远离目标狼移动一步
    if (targetX != -1 && targetY != -1) {
        // 计算目标羊的方向
        int dirX = targetX - x;
        int dirY = targetY - y;
        // 如果方向不为0，将其变为1或-1
        if (dirX != 0) {
            dirX = dirX / abs(dirX);
        }
        if (dirY != 0) {
            dirY = dirY / abs(dirY);
        }
        // 计算移动后的位置
        int newX = x - dirX;
        int newY = y - dirY;
        // 设置新的位置
        if (newX >= 0 && newX < MAP_HEIGHT && newY >= 0 && newY < MAP_WIDTH && map.getTerrain(newX, newY) != OBSTACLE)
            setPosition(newX, newY);
    }

    // 如果没狼在附近，搜索草地或繁衍
    if (!hasWolf) {
        if(age < 16 || Offspring.size() || satiety < 30) {
            // 寻找最近的草地，使用深度优先搜索算法
            int targetX = -1, targetY = -1; // 目标草地的位置
            int minDistance = INT_MAX; // 最小距离
            bool visited[MAP_HEIGHT][MAP_WIDTH]; // 记录已经访问过的位置
            memset(visited, false, sizeof(visited)); // 初始化为false
            stack<pair<int, int> > s; // 使用栈来存储搜索过程中的位置
            s.push(make_pair(x, y)); // 将当前位置压入栈
            visited[x][y] = true; // 标记当前位置已访问
            while (!s.empty()) { // 当栈不为空时，继续搜索
                // 取出栈顶的位置
                int curX = s.top().first;
                int curY = s.top().second;
                s.pop();
                // 遍历该位置的上下左右四个相邻位置
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        // 如果是对角线方向，跳过
                        if (abs(i) + abs(j) != 1) {
                            continue;
                        }
                        // 计算相邻位置的坐标
                        int nextX = curX + i;
                        int nextY = curY + j;
                        // 如果坐标越界，跳过
                        if (nextX < 0 || nextX >= MAP_HEIGHT || nextY < 0 || nextY >= MAP_WIDTH) {
                            continue;
                        }
                        // 如果该位置已经访问过，跳过
                        if (visited[nextX][nextY]) {
                            continue;
                        }
                        // 如果该位置是障碍物，跳过
                        if (map.getTerrain(nextX, nextY) == OBSTACLE) {
                            continue;
                        }
                        // 如果该位置是草地，判断是否是最近的草地
                        if (map.getTerrain(nextX, nextY) == GRASS) {
                            // 计算该位置到当前位置的距离
                            int distance = abs(nextX - x) + abs(nextY - y);
                            // 如果距离小于最小距离，更新目标草地的位置和最小距离
                            if (distance < minDistance) {
                                targetX = nextX;
                                targetY = nextY;
                                minDistance = distance;
                            }
                        }
                        // 将该位置压入栈
                        s.push(make_pair(nextX, nextY));
                        // 标记该位置已访问
                        visited[nextX][nextY] = true;
                    }
                }
            }

            // 如果找到了目标草地，向着目标草地移动一步
            if (targetX != -1 && targetY != -1) {
                // 计算目标草地的方向
                int dirX = targetX - x;
                int dirY = targetY - y;
                // 如果方向不为0，将其变为1或-1
                if (dirX != 0) {
                    dirX = dirX / abs(dirX);
                }
                if (dirY != 0) {
                    dirY = dirY / abs(dirY);
                }
                // 计算移动后的位置
                int newX = x + dirX;
                int newY = y + dirY;
                // 设置新的位置
                if (newX >= 0 && newX < MAP_HEIGHT && newY >= 0 && newY < MAP_WIDTH && map.getTerrain(newX, newY) != OBSTACLE)
                    setPosition(newX, newY);

                // 如果移动后的位置就是目标草地，吃掉该草地的一半数量的草
                if (newX == targetX && newY == targetY) {
                    // 遍历草的向量，找到对应的草对象
                    for (int i = 0; i < grasses.size(); i++) {
                        // 获取草的位置
                        int grassX, grassY;
                        grasses[i]->getPosition(grassX, grassY);
                        // 如果草的位置和目标草地的位置相同，吃掉该草的一半数量
                        if (grassX == targetX && grassY == targetY) {
                            // 获取草的数量
                            int amount = grasses[i]->getAmount();
                            // 吃掉一半数量的草
                            amount /= 2;
                            //饱食度增加
                            satiety += 20;
                            // 如果超过100，设置为100
                            if (satiety > 100) {
                                satiety = 100;
                            }
                            // 设置新的草的数量
                            grasses[i]->setAmount(amount);
                            // 如果草的数量为0，将该草从向量中删除,并设置地图
                            if (amount == 0) {
                                map.setTerrain(grassX,grassY,EMPTY);
                                grasses.erase(grasses.begin() + i);
                            }
                            // 增加羊的健康状态30点
                            health += 20;
                            // 如果健康状态超过100，设置为100
                            if (health > 100) {
                                health = 100;
                            }
                            // 跳出循环
                            break;
                        }
                    }
                }
            } else {
                // 如果没有找到目标草地，随机移动一步
                // 随机生成一个方向
                int dirX = rand() % 3 - 1; // 生成-1, 0, 1之间的随机数
                int dirY = rand() % 3 - 1; // 生成-1, 0, 1之间的随机数
                // 计算移动后的位置
                int newX = x + dirX;
                int newY = y + dirY;
                // 如果位置没有越界，也不是障碍物，设置新的位置
                if (newX >= 0 && newX < MAP_HEIGHT && newY >= 0 && newY < MAP_WIDTH && map.getTerrain(newX, newY) != OBSTACLE) {
                    setPosition(newX, newY);
                }
            }
        }



        if(this->ageState == ADULT) {
            // 遍历动物的向量，判断是否有可以繁衍的对象
            Animal *mate = nullptr; // 可以繁衍的对象
            for (int i = 0; i < animals.size(); i++) {
                // 获取动物的种类、性别、年龄状态和位置
                AnimalType type = animals[i]->getType();
                Gender gender = animals[i]->getGender();
                AgeState ageState = animals[i]->getAgeState();
                int animalX, animalY;
                animals[i]->getPosition(animalX, animalY);
                // 如果动物是羊，并且性别和自己相反，并且年龄状态是成年，并且位置和自己相同，说明可以繁衍
                if (type == SHEEP && gender != this->gender && ageState == ADULT && animalX == x && animalY == y) {
                    // 设置可以繁衍的对象为该动物
                    mate = animals[i];
                    // 跳出循环
                    break;
                }
            }
            // 如果找到了可以繁衍的对象，随机生成一个新的羊对象，性别随机，位置和自己相同，将其加入动物的向量
            if (mate != nullptr && Offspring.size() < 20 && mate->Offspring.size() < 20) { // 随机生成性别 //这里限制了动物数量
                int r = rand() % 7; //生育0-3只
                for(int i = 0; i < r; ++i) {
                    Gender gender = rand() % 2 == 0 ? MALE : FEMALE;
                    // 生成0或1的随机数，0表示雄性，1表示雌性
                    // 创建新的羊对象
                    Sheep *newSheep = new Sheep(gender, x, y); // 将新的羊对象加入动物的向量
                    animals.push_back(newSheep);
                    Offspring.push_back(newSheep);
                    mate->Offspring.push_back(newSheep);
                }
            }
        }


        //年龄大于8，未交配者，强制交配
        if(age > 16 && this->ageState == ADULT && !Offspring.size() && satiety >= 30) {
            // 寻找视野范围内的最近的羊，使用广度优先搜索算法
            int targetX = -1, targetY = -1; // 目标羊的位置
            int minDistance = INT_MAX; // 最小距离
            bool visited[MAP_HEIGHT][MAP_WIDTH]; // 记录已经访问过的位置
            memset(visited, false, sizeof(visited)); // 初始化为false
            queue<pair<int, int> > q; // 使用队列来存储搜索过程中的位置
            q.push(make_pair(x, y)); // 将当前位置入队
            visited[x][y] = true; // 标记当前位置已访问
            while (!q.empty()) { // 当队列不为空时，继续搜索
                // 取出队首的位置
                int curX = q.front().first;
                int curY = q.front().second;
                q.pop();
                // 遍历该位置的上下左右四个相邻位置
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        // 如果是对角线方向，跳过
                        if (abs(i) + abs(j) != 1) {
                            continue;
                        }
                        // 计算相邻位置的坐标
                        int nextX = curX + i;
                        int nextY = curY + j;
                        // 如果坐标越界，跳过
                        if (nextX < 0 || nextX >= MAP_HEIGHT || nextY < 0 || nextY >= MAP_WIDTH) {
                            continue;
                        }
                        // 如果该位置已经访问过，跳过
                        if (visited[nextX][nextY]) {
                            continue;
                        }
                        // 如果该位置是障碍物，跳过
                        if (map.getTerrain(nextX, nextY) == OBSTACLE) {
                            continue;
                        }
                        // 如果该位置到当前位置的距离大于视野范围，跳过
                        if (sqrt((nextX - x)*(nextX - x) + (nextY - y)*(nextY - y)) > 50) {     //找同族视野更大
                            continue;
                        }
                        // 遍历动物的向量，判断是否有羊在该位置
                        for (int k = 0; k < animals.size(); k++) {
                            // 获取动物的种类和位置
                            AnimalType type = animals[k]->getType();
                            int animalX, animalY;
                            animals[k]->getPosition(animalX, animalY);
                            // 如果动物是羊，并且位置和该位置相同，判断是否是最近的羊
                            if (type == SHEEP && animalX == nextX && animalY == nextY) {
                                // 计算该位置到当前位置的距离
                                int distance = sqrt((nextX - x)*(nextX - x) + (nextY - y)*(nextY - y));
                                // 如果距离小于最小距离，更新目标羊的位置和最小距离
                                if (distance < minDistance) {
                                    targetX = nextX;
                                    targetY = nextY;
                                    minDistance = distance;
                                }
                            }
                        }
                        // 将该位置入队
                        q.push(make_pair(nextX, nextY));
                        // 标记该位置已访问
                        visited[nextX][nextY] = true;
                    }
                }
            }

            // 如果找到了目标羊，向着目标羊移动一步
            if (targetX != -1 && targetY != -1) {
                // 计算目标羊的方向
                int dirX = targetX - x;
                int dirY = targetY - y;
                // 如果方向不为0，将其变为1或-1
                if (dirX != 0) {
                    dirX = dirX / abs(dirX);
                }
                if (dirY != 0) {
                    dirY = dirY / abs(dirY);
                }
                // 计算移动后的位置
                int newX = x + dirX;
                int newY = y + dirY;
                // 设置新的位置
                if (newX >= 0 && newX < MAP_HEIGHT && newY >= 0 && newY < MAP_WIDTH && map.getTerrain(newX, newY) != OBSTACLE)
                    setPosition(newX, newY);
                // 如果移动后的位置就是目标羊的位置，繁衍
                if (newX == targetX && newY == targetY) {
                    // 遍历动物的向量，找到对应的羊对象
                    for (int i = 0; i < animals.size(); i++) {
                        // 获取动物的种类和位置
                        AnimalType type = animals[i]->getType();
                        Gender gender = animals[i]->getGender();
                        AgeState ageState = animals[i]->ageState;
                        int animalX, animalY;
                        animals[i]->getPosition(animalX, animalY);
                        // 如果动物是羊，并且位置和目标羊的位置相同，并且性别相反，并且成年,繁衍
                        if (type == SHEEP && animalX == targetX && animalY == targetY && gender != this->gender && ageState == ADULT && Offspring.size() < 20 && animals[i]->Offspring.size() < 20) {
                            int r = rand() % 7; //生育0-7只
                            if(this->gender == FEMALE) this->health -= 50;
                            else animals[i]->health -= 50;
                            for(int j = 0; j < r; ++j){
                                Gender gender = rand() % 2 == 0 ? MALE : FEMALE;
                                // 生成0或1的随机数，0表示雄性，1表示雌性
                                // 创建新的羊对象
                                Sheep *newSheep = new Sheep(gender, x, y); // 将新的羊对象加入动物的向量
                                animals.push_back(newSheep);
                                Offspring.push_back(newSheep);
                                animals[i]->Offspring.push_back(newSheep);
                            }
                        }
                        // 跳出循环
                        break;
                    }
                }
            }
            else {
                // 如果没有找到目标羊，随机移动一步
                // 随机生成一个方向
                int dirX = rand() % 3 - 1; // 生成-1, 0, 1之间的随机数
                int dirY = rand() % 3 - 1; // 生成-1, 0, 1之间的随机数
                // 计算移动后的位置
                int newX = x + dirX;
                int newY = y + dirY;
                // 如果位置没有越界，也不是障碍物，设置新的位置
                if (newX >= 0 && newX < MAP_HEIGHT && newY >= 0 && newY < MAP_WIDTH && map.getTerrain(newX, newY) != OBSTACLE) {
                    setPosition(newX, newY);
                }
            }
        }

    }

}

Wolf::Wolf(Gender gender, int x, int y) : Animal(WOLF, gender, x, y) {
    // 调用父类的构造函数
    // 设置视野范围为10
    sight = 20;
}

// 判断狼是否死亡
bool Wolf::isDead() {
    return  health <= 0 || age > 30 || satiety <= 0;
}

// 获取饱食度
int Wolf::getSatiety() {
    return satiety;
}

// 设置饱食度
void Wolf::setSatiety(int satiety) {
    this->satiety = satiety;
}


// 判断狼是否可以捕食另一种动物
bool Wolf::canEat(Animal *other) {
    // 狼只能捕食羊
    return other->getType() == SHEEP;
}

// 狼的行为，包括移动、捕食、繁衍等
void Wolf::act(Map &map, Weather &weather, Disaster &disaster, vector<Grass*> &grasses, vector<Animal*> &animals) {
    // TODO: 实现狼的行为逻辑
    // 如果狼已经死亡，直接返回
    if (isDead()) {
        return;
    }

    //雨天健康下降
    if(weather.getCurrent() == RAINY) {
        health -= 10;
    }
    //年龄增长
    age += 1;
    // 狼的饱食度下降10点
    satiety -= 8;

    // 如果狼的年龄大于4，成年
    if (age > 10) {
        ageState = ADULT;
    }

    // 如果狼的年龄大于10，老年
    if (age > 25) {
        ageState = OLD;
        health -= 5; //额外健康衰减
    }

    //如果年龄大于15,死亡
    if(age > 30){

    }

    //不强制交配
    if(age < 16 || Offspring.size() || satiety < 30) {
        // 寻找视野范围内的最近的羊，使用广度优先搜索算法
        int targetX = -1, targetY = -1; // 目标羊的位置
        int minDistance = INT_MAX; // 最小距离
        bool visited[MAP_HEIGHT][MAP_WIDTH]; // 记录已经访问过的位置
        memset(visited, false, sizeof(visited)); // 初始化为false
        queue<pair<int, int> > q; // 使用队列来存储搜索过程中的位置
        q.push(make_pair(x, y)); // 将当前位置入队
        visited[x][y] = true; // 标记当前位置已访问
        while (!q.empty()) { // 当队列不为空时，继续搜索
            // 取出队首的位置
            int curX = q.front().first;
            int curY = q.front().second;
            q.pop();
            // 遍历该位置的上下左右四个相邻位置
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    // 如果是对角线方向，跳过
                    if (abs(i) + abs(j) != 1) {
                        continue;
                    }
                    // 计算相邻位置的坐标
                    int nextX = curX + i;
                    int nextY = curY + j;
                    // 如果坐标越界，跳过
                    if (nextX < 0 || nextX >= MAP_HEIGHT || nextY < 0 || nextY >= MAP_WIDTH) {
                        continue;
                    }
                    // 如果该位置已经访问过，跳过
                    if (visited[nextX][nextY]) {
                        continue;
                    }
                    // 如果该位置是障碍物，跳过
                    if (map.getTerrain(nextX, nextY) == OBSTACLE) {
                        continue;
                    }
                    // 如果该位置到当前位置的距离大于视野范围，跳过
                    if (sqrt((nextX - x)*(nextX - x) + (nextY - y)*(nextY - y))> sight) {
                        continue;
                    }
                    // 遍历动物的向量，判断是否有羊在该位置
                    for (int k = 0; k < animals.size(); k++) {
                        // 获取动物的种类和位置
                        AnimalType type = animals[k]->getType();
                        int animalX, animalY;
                        animals[k]->getPosition(animalX, animalY);
                        // 如果动物是羊，并且位置和该位置相同，判断是否是最近的羊
                        if (type == SHEEP && animalX == nextX && animalY == nextY) {
                            // 计算该位置到当前位置的距离
                            int distance = sqrt((nextX - x)*(nextX - x) + (nextY - y)*(nextY - y));
                            // 如果距离小于最小距离，更新目标羊的位置和最小距离
                            if (distance < minDistance) {
                                targetX = nextX;
                                targetY = nextY;
                                minDistance = distance;
                            }
                        }
                    }
                    // 将该位置入队
                    q.push(make_pair(nextX, nextY));
                    // 标记该位置已访问
                    visited[nextX][nextY] = true;
                }
            }
        }

        // 如果找到了目标羊，向着目标羊移动
        if (targetX != -1 && targetY != -1) {
            // 计算目标羊的方向
            int dirX = targetX - x;
            int dirY = targetY - y;
            //如果两步以内，直接移动
            // 如果方向不为0，将其变为1或-1
            if (dirX != 0) {
                if(dirX > dirX / abs(dirX))
                    dirX = 2 * dirX / abs(dirX); //两步
                else dirX = dirX / abs(dirX);
            }
            if (dirY != 0) {
                if(dirY > dirY / abs(dirY))
                    dirY = 2 * dirY / abs(dirY); //两步
                else dirY = dirY / abs(dirY);
            }
            // 计算移动后的位置
            int newX = x + dirX;
            int newY = y + dirY;
            // 设置新的位置
            if (newX >= 0 && newX < MAP_HEIGHT && newY >= 0 && newY < MAP_WIDTH && map.getTerrain(newX, newY) != OBSTACLE)
                setPosition(newX, newY);
            // 如果移动后的位置就是目标羊的位置，捕食该羊
            if (newX == targetX && newY == targetY) {
                // 遍历动物的向量，找到对应的羊对象
                for (int i = 0; i < animals.size(); i++) {
                    // 获取动物的种类和位置
                    AnimalType type = animals[i]->getType();
                    int animalX, animalY;
                    animals[i]->getPosition(animalX, animalY);
                    // 如果动物是羊，并且位置和目标羊的位置相同，捕食该羊
                    if (type == SHEEP && animalX == targetX && animalY == targetY) {
                        // 增加狼的饱食度
                        satiety += 20;
                        // 如果饱食度超过100，设置为100
                        if (satiety > 100) {
                            satiety = 100;
                        }
                        //增加健康
                        health += 15;
                        // 如果饱食度超过100，设置为100
                        if (health > 100) {
                            health = 100;
                        }
                        // 将该羊从向量中删除
                        animals.erase(animals.begin() + i);
                        // 跳出循环
                        break;
                    }
                }
            }
        } else {
            // 如果没有找到目标羊，随机移动一步
            // 随机生成一个方向
            int dirX = rand() % 3 - 1; // 生成-1, 0, 1之间的随机数
            int dirY = rand() % 3 - 1; // 生成-1, 0, 1之间的随机数
            // 计算移动后的位置
            int newX = x + dirX;
            int newY = y + dirY;
            // 如果位置没有越界，也不是障碍物，设置新的位置
            if (newX >= 0 && newX < MAP_HEIGHT && newY >= 0 && newY < MAP_WIDTH && map.getTerrain(newX, newY) != OBSTACLE) {
                setPosition(newX, newY);
            }
        }
    }

    if(this->ageState == ADULT) {
        // 遍历动物的向量，判断是否有可以繁衍的对象
        Animal *mate = nullptr; // 可以繁衍的对象
        for (int i = 0; i < animals.size(); i++) {
            // 获取动物的种类、性别、年龄状态和位置
            AnimalType type = animals[i]->getType();
            Gender gender = animals[i]->getGender();
            AgeState ageState = animals[i]->getAgeState();
            int animalX, animalY;
            animals[i]->getPosition(animalX, animalY);
            // 如果动物是狼，并且性别和自己相反，并且年龄状态是成年，并且位置和自己相同，说明可以繁衍
            if (type == WOLF && gender != this->gender && ageState == ADULT && animalX == x && animalY == y) {
                // 设置可以繁衍的对象为该动物
                mate = animals[i];
                // 跳出循环
                break;
            }
        }

        // 如果找到了可以繁衍的对象，随机生成一个新的狼对象，性别随机，位置和自己相同，将其加入动物的向量
        if (mate != nullptr && Offspring.size() < 10 && mate->Offspring.size() < 10) {  //这里同样限制数量
            int r = rand() % 4; //生育0-4只;
            if(this->gender == FEMALE) this->health -= 30;      //生殖掉健康
            else mate->health -= 30;
            for(int i = 0; i < r; ++i) {
                // 随机生成性别
                Gender gender = rand() % 2 == 0 ? MALE : FEMALE; // 生成0或1的随机数，0表示雄性，1表示雌性
                // 创建新的狼对象
                Wolf *newWolf = new Wolf(gender, x, y);
                // 将新的狼对象加入动物的向量
                animals.push_back(newWolf);
                Offspring.push_back(newWolf);
                mate->Offspring.push_back(newWolf);
            }
        }
    }
    //年龄大于，未交配者，强制交配
    if(age > 16 && this->ageState == ADULT && !Offspring.size() && satiety >= 30) {
        // 寻找视野范围内的最近的狼，使用广度优先搜索算法
        int targetX = -1, targetY = -1; // 目标狼的位置
        int minDistance = INT_MAX; // 最小距离
        bool visited[MAP_HEIGHT][MAP_WIDTH]; // 记录已经访问过的位置
        memset(visited, false, sizeof(visited)); // 初始化为false
        queue<pair<int, int> > q; // 使用队列来存储搜索过程中的位置
        q.push(make_pair(x, y)); // 将当前位置入队
        visited[x][y] = true; // 标记当前位置已访问
        while (!q.empty()) { // 当队列不为空时，继续搜索
            // 取出队首的位置
            int curX = q.front().first;
            int curY = q.front().second;
            q.pop();
            // 遍历该位置的上下左右四个相邻位置
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    // 如果是对角线方向，跳过
                    if (abs(i) + abs(j) != 1) {
                        continue;
                    }
                    // 计算相邻位置的坐标
                    int nextX = curX + i;
                    int nextY = curY + j;
                    // 如果坐标越界，跳过
                    if (nextX < 0 || nextX >= MAP_HEIGHT || nextY < 0 || nextY >= MAP_WIDTH) {
                        continue;
                    }
                    // 如果该位置已经访问过，跳过
                    if (visited[nextX][nextY]) {
                        continue;
                    }
                    // 如果该位置是障碍物，跳过
                    if (map.getTerrain(nextX, nextY) == OBSTACLE) {
                        continue;
                    }
                    // 如果该位置到当前位置的距离大于视野范围，跳过
                    if (abs(nextX - x) + abs(nextY - y) > 50) {        //找同族
                        continue;
                    }
                    // 遍历动物的向量，判断是否有狼在该位置
                    for (int k = 0; k < animals.size(); k++) {
                        // 获取动物的种类和位置
                        AnimalType type = animals[k]->getType();
                        int animalX, animalY;
                        animals[k]->getPosition(animalX, animalY);
                        // 如果动物是狼，并且位置和该位置相同，判断是否是最近的狼
                        if (type == WOLF && animalX == nextX && animalY == nextY) {
                            // 计算该位置到当前位置的距离
                            int distance = abs(nextX - x) + abs(nextY - y);
                            // 如果距离小于最小距离，更新目标羊的位置和最小距离
                            if (distance < minDistance) {
                                targetX = nextX;
                                targetY = nextY;
                                minDistance = distance;
                            }
                        }
                    }
                    // 将该位置入队
                    q.push(make_pair(nextX, nextY));
                    // 标记该位置已访问
                    visited[nextX][nextY] = true;
                }
            }
        }

        // 如果找到了目标狼，向着目标狼移动一步
        if (targetX != -1 && targetY != -1) {
            // 计算目标狼的方向
            int dirX = targetX - x;
            int dirY = targetY - y;
            // 如果方向不为0，将其变为1或-1
            if (dirX != 0) {
                if(dirX > dirX / abs(dirX))
                    dirX = 2 * dirX / abs(dirX); //两步
                else dirX = dirX / abs(dirX);
            }
            if (dirY != 0) {
                if(dirY > dirY / abs(dirY))
                    dirY = 2 * dirY / abs(dirY); //两步
                else dirY = dirY / abs(dirY);
            }
            // 计算移动后的位置
            int newX = x + dirX;
            int newY = y + dirY;
            // 设置新的位置
            if (newX >= 0 && newX < MAP_HEIGHT && newY >= 0 && newY < MAP_WIDTH && map.getTerrain(newX, newY) != OBSTACLE)
                setPosition(newX, newY);
            // 如果移动后的位置就是目标狼的位置，繁衍
            if (newX == targetX && newY == targetY) {
                // 遍历动物的向量，找到对应的狼对象
                for (int i = 0; i < animals.size(); i++) {
                    // 获取动物的种类和位置
                    AnimalType type = animals[i]->getType();
                    Gender gender = animals[i]->getGender();
                    AgeState ageState = animals[i]->ageState;
                    int animalX, animalY;
                    animals[i]->getPosition(animalX, animalY);
                    // 如果动物是狼，并且位置和目标狼的位置相同，并且性别不同，并且成年,繁衍
                    if (type == WOLF && animalX == targetX && animalY == targetY && gender != this->gender && ageState == ADULT && Offspring.size() < 10 && animals[i]->Offspring.size() < 10) {
                        int r = rand() % 4 ; //生育4只以内
                        if(this->gender == FEMALE) this->health -= 30;
                        else animals[i]->health -= 30;
                        for(int j = 0; j < r; ++j){
                            Gender gender = rand() % 2 == 0 ? MALE : FEMALE;
                            // 生成0或1的随机数，0表示雄性，1表示雌性
                            // 创建新的狼对象
                            Wolf *newWolf = new Wolf(gender, x, y); // 将新的狼对象加入动物的向量
                            animals.push_back(newWolf);
                            Offspring.push_back(newWolf);
                            animals[i]->Offspring.push_back(newWolf);
                        }
                    }
                    // 跳出循环
                    break;
                }
            }
        }
        else {
            // 如果没有找到目标狼，随机移动一步
            // 随机生成一个方向
            int dirX = rand() % 3 - 1; // 生成-1, 0, 1之间的随机数
            int dirY = rand() % 3 - 1; // 生成-1, 0, 1之间的随机数
            // 计算移动后的位置
            int newX = x + dirX;
            int newY = y + dirY;
            // 如果位置没有越界，也不是障碍物，设置新的位置
            if (newX >= 0 && newX < MAP_HEIGHT && newY >= 0 && newY < MAP_WIDTH && map.getTerrain(newX, newY) != OBSTACLE) {
                setPosition(newX, newY);
            }
        }
    }
}

Simulation::Simulation() {

        //初始化地图
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                int r = rand() % 100; // 生成0到99之间的随机数
                if (r < 30) { // 30%的概率生成草地
                    map.setTerrain(i, j, GRASS);
                    // 创建草对象
                    Grass *grass = new Grass(i, j);
                    // 将草对象加入草的向量
                    grasses.push_back(grass);
                } else if (r < 35) { // 5%的概率生成障碍物
                    map.setTerrain(i, j, OBSTACLE);
                    // 创建障碍物对象
                    Obstacle *obstacle = new Obstacle(i, j);
                    // 将草对象加入草的向量
                    obstacles.push_back(obstacle);
                } else { // 65%的概率生成空地
                    map.setTerrain(i, j, EMPTY);
                }
            }
        }

        // 随机生成一些草对象，将其加入草的向量，并设置对应位置的地形为草地
        /*for (int i = 0; i < 100; i++) {


            // 随机生成位置
            int x = rand() % MAP_HEIGHT;
            int y = rand() % MAP_WIDTH;
            // 如果该位置不是空地，跳过
            if (map.getTerrain(x, y) != EMPTY) {
                continue;
            }
            // 创建草对象
            Grass *grass = new Grass(x, y);
            // 将草对象加入草的向量
            grasses.push_back(grass);
            // 设置该位置的地形为草地
            map.setTerrain(x, y, GRASS);
        }*/


        // 随机生成一些羊对象，将其加入动物的向量
        for (int i = 0; i < 500; i++) {
            // 随机生成性别
            Gender gender = rand() % 2 == 0 ? MALE : FEMALE; // 生成0或1的随机数，0表示雄性，1表示雌性
            // 随机生成位置
            int x = rand() % MAP_HEIGHT;
            int y = rand() % MAP_WIDTH;
            // 如果该位置是障碍区，跳过
            if (map.getTerrain(x, y) == OBSTACLE) {
                continue;
            }
            // 创建羊对象
            Sheep *sheep = new Sheep(gender, x, y);
            //初始年龄为5
            sheep->age = 5;
            // 将羊对象加入动物的向量
            animals.push_back(sheep);
        }

        // 随机生成一些狼对象，将其加入动物的向量
        for (int i = 0; i < 100; i++) {
            // 随机生成性别
            Gender gender = rand() % 2 == 0 ? MALE : FEMALE; // 生成0或1的随机数，0表示雄性，1表示雌性
            // 随机生成位置
            int x = rand() % MAP_HEIGHT;
            int y = rand() % MAP_WIDTH;
            // 如果该位置是障碍物，跳过
            if (map.getTerrain(x, y) == OBSTACLE) {
                continue;
            }
            // 创建狼对象
            Wolf *wolf = new Wolf(gender, x, y);
            //初始年龄为5
            wolf->age = 5;
            // 将狼对象加入动物的向量
            animals.push_back(wolf);
        }
    }

    // 更新仿真的状态，包括天气、地质灾害、草、羊、狼等的变化
    void Simulation::update() {

        // 改变天气
        weather.change();

        // 不论天气，有一定概率产生地质灾害
        if (weather.getCurrent() == SUNNY || weather.getCurrent() == RAINY) {
            for(int i = 0; i < 100; ++i){
                int r = rand() % 100; // 生成0到99之间的随机数
                if (r < 5) { // 5%的概率产生新的障碍物
                    // 随机生成位置
                    int x = rand() % MAP_HEIGHT;
                    int y = rand() % MAP_WIDTH;
                    // 如果该位置不是障碍物，创建障碍物对象，将其加入障碍物的向量，并设置对应位置的地形为障碍物
                    if (map.getTerrain(x, y) != OBSTACLE) {
                        //如果是草地，删除草对象
                        if(map.getTerrain(x,y) == GRASS) {
                            int idx, GrassX, GrassY;
                            for(int i = 0; i < grasses.size(); ++i){
                                grasses[i]->getPosition(GrassX,GrassY);
                                if(GrassX == x && GrassY == y){
                                    idx = i;
                                    break;
                                }
                            }
                            grasses.erase(grasses.begin() + idx);
                        }
                        // 创建障碍物对象
                        Obstacle *obstacle = new Obstacle(x, y);
                        // 将障碍物对象加入障碍物的向量
                        obstacles.push_back(obstacle);
                        // 设置该位置的地形为障碍物
                        map.setTerrain(x, y, OBSTACLE);
                    }
                }
            }
        }

        // 如果天气是雨天，有一定概率产生新的草地
        if (weather.getCurrent() == RAINY) {
            for(int i = 0; i < 100; ++i){
                int r = rand() % 100; // 生成0到99之间的随机数
                if (r < 20) { // 20%的概率产生新的草地
                    // 随机生成位置
                    int x = rand() % MAP_HEIGHT;
                    int y = rand() % MAP_WIDTH;
                    // 如果该位置是空地，创建草对象，将其加入草的向量，并设置对应位置的地形为草地
                    if (map.getTerrain(x, y) == EMPTY) {
                        // 创建草对象
                        Grass *grass = new Grass(x, y);
                        // 将草对象加入草的向量
                        grasses.push_back(grass);
                        // 设置该位置的地形为草地
                        map.setTerrain(x, y, GRASS);
                    }
                }
            }
        }

        // 如果天气是晴天，有一定概率导致一些草地退化为空地
        if (weather.getCurrent() == SUNNY) {
                for(int i = 0; i < 100; ++i){
                int r = rand() % 100; // 生成0到99之间的随机数
                if (r < 10) { // 10%的概率导致一些草地退化为空地
                    // 随机选择一个草对象
                    int index = rand() % grasses.size();
                    // 获取草的位置
                    int x, y;
                    grasses[index]->getPosition(x, y);
                    // 将该草从向量中删除
                    grasses.erase(grasses.begin() + index);
                    // 设置该位置的地形为空地
                    map.setTerrain(x, y, EMPTY);
                }
            }
        }

        //不论天气，有一定概率导致一些障碍物变成空地
        if (weather.getCurrent() == SUNNY || weather.getCurrent() == RAINY) {
            for(int i = 0;i < 100; ++i){
                int r = rand() % 100; // 生成0到99之间的随机数
                if (r < 5) { // 10%的概率导致一些障碍物平整为空地
                    // 随机选择一个障碍物
                    int index = rand() % obstacles.size();
                    // 获取障碍物的位置
                    int x, y;
                    obstacles[index]->getPosition(x, y);
                    // 将该障碍物从向量中删除
                    obstacles.erase(obstacles.begin() + index);
                    // 设置该位置的地形为空地
                    map.setTerrain(x, y, EMPTY);
                }
            }
        }

        // 遍历动物的向量，让每个动物进行行为
        for (int i = 0; i < animals.size(); i++) {
            animals[i]->act(map, weather, disaster, grasses, animals);
        }

        // 遍历动物的向量，删除已经死亡的动物
        for (int i = 0; i < animals.size(); i++) {
            // 如果动物已经死亡，将其从向量中删除
            if (animals[i]->isDead()) {
                animals.erase(animals.begin() + i);
                // i减1，避免跳过下一个动物
                i--;
            }
        }

    }


    bool Simulation::is_SHEEP(Animal *a){
        return a->getType() == SHEEP;
    }
    bool Simulation::is_WOLF(Animal *a){
        return a->getType() == WOLF;
    }
    // 输出仿真的状态，包括天气、地质灾害、草、羊、狼等的信息
    void Simulation::output() {
        // 输出天气的信息
        cout << "当前的天气是：";
        if (weather.getCurrent() == RAINY) {
            cout << "雨天" << endl;
        } else {
            cout << "晴天" << endl;
        }

        // 输出羊的信息
        cout << "当前的羊的数量是：" << count_if(animals.begin(), animals.end(), is_SHEEP) << endl;

        // 输出狼的信息
        cout << "当前的狼的数量是：" << count_if(animals.begin(), animals.end(), is_WOLF) << endl;
    }




    void MainWindow::drawMap()
    {

        // 绘制地图
        for (int i = 0; i < MAP_HEIGHT; ++i) {
            for (int j = 0; j < MAP_WIDTH; ++j) {
                QGraphicsRectItem *item = new QGraphicsRectItem(j * 50, i * 50, 50, 50);

                // 设置不同元素的颜色
                if (object.map.getTerrain(i,j) == EMPTY) {
                        item->setBrush(QBrush(Qt::white));
                } else if (object.map.getTerrain(i,j) == GRASS) {
                        item->setBrush(QBrush(Qt::green));
                } else if (object.map.getTerrain(i,j) == OBSTACLE) {
                        item->setBrush(QBrush(Qt::black));
                }

                scene->addItem(item);
                mapItems.append(item);
            }
        }
    }


    void MainWindow::drawAnimals()
    {
        //绘制动物
        for(int i = 0; i < object.animals.size(); ++i){
            AnimalType type = object.animals[i]->getType();
            int animalX, animalY;
            object.animals[i]->getPosition(animalX, animalY);
            if(type == SHEEP){
                QPixmap originalPixmap("/Users/xurui/Desktop/temp/untitled1/sheep.png");

                // 设置贴图大小，这里将宽高都缩小到 30x30
                QPixmap scaledPixmap = originalPixmap.scaled(30, 30, Qt::KeepAspectRatio);

                QGraphicsPixmapItem *animal = new QGraphicsPixmapItem(scaledPixmap);
                animal->setPos( animalY * 50 + 10, animalX * 50 + 10);  // 设置位置
                scene->addItem(animal);
                animalItems.append(animal);
            }
        else {
            QPixmap originalPixmap("/Users/xurui/Desktop/temp/ecosystem2/wolf.png");

            // 设置贴图大小，这里将宽高都缩小到 30x30
            QPixmap scaledPixmap = originalPixmap.scaled(30, 30, Qt::KeepAspectRatio);

            QGraphicsPixmapItem *animal = new QGraphicsPixmapItem(scaledPixmap);
            animal->setPos(animalY * 50 + 10, animalX * 50 + 10);  // 设置位置
            scene->addItem(animal);
            animalItems.append(animal);
        }

    }

    }
    void MainWindow::updateSimulation(){
        if(!paused){
            object.update();
            drawMap();
            drawAnimals();
        }
    }

    void MainWindow::togglePause()
    {
        // 切换暂停状态
        paused = !paused;

        if (paused) {
            timer->stop();  // 暂停定时器

        /*// 输出天气的信息
        cout << "当前的天气是：";
        if (weather.getCurrent() == RAINY) {
            cout << "雨天" << endl;
        } else {
            cout << "晴天" << endl;
        }

        // 输出羊的信息
        cout << "当前的羊的数量是：" << count_if(animals.begin(), animals.end(), is_SHEEP) << endl;

        // 输出狼的信息
        cout << "当前的狼的数量是：" << count_if(animals.begin(), animals.end(), is_Wolf) << endl;
         */

         //输出天气
            QString qs;
            if(object.weather.getCurrent() == RAINY)
                qs = "Rainy";
            else qs = "Sunny";

            qDebug() << "Current Weather: " << qs;

            // 在 infoDisplay 中显示信息
            infoDisplay->appendPlainText(QString("Simulation paused.\nCurrent Weather: %1").arg(qs));

         // 输出羊数量
            int sheepCount = count_if(object.animals.begin(), object.animals.end(), object.is_SHEEP);
            qDebug() << "Sheep count: " << sheepCount;

            // 在 infoDisplay 中显示信息
            infoDisplay->appendPlainText(QString("Sheep count: %1").arg(sheepCount));

            // 输出狼数量
            int wolfCount =  count_if(object.animals.begin(), object.animals.end(), object.is_WOLF);
            qDebug() << "Wolf count: " << wolfCount;

            // 在 infoDisplay 中显示信息
            infoDisplay->appendPlainText(QString("Wolf count: %1\n").arg(wolfCount));

        } else {
            timer->start(2000);  // 继续定时器
            // 在 infoDisplay 中显示信息
            infoDisplay->appendPlainText("Simulation resumed...\n");
        }
    }



