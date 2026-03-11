# EGO-Planner
## 轨迹表示（B-spline）
EGO-Planner 的轨迹不是直接存一条曲线，而是存 控制点。
这些点通过 B-spline 生成平滑轨迹。

```python
Q1  Q2  Q3  Q4  Q5
●─-─●─-─●─-─●─-─●
```

## 初始轨迹生成
当前点 → 目标点的一条直线
## 碰撞检测
如果发现：trajectory inside obstacle，说明碰撞
## 生成 {p,v}
1. 找到碰撞轨迹段
2. 用 A* 找一条安全路径 Γ(大致方向)
3. 让轨迹绕着轨迹应该往 Γ这个方向移动，p = Γ上的点；v = normalize(p - Qi)。
4. 构造优化目标函数：
   $$J = smoothness + collision + dynamics$$
    $$J = λs Js + λc Jc + λd Jd$$
## 轨迹优化（L-BFGS）
## 时间重新分配
有时速度/加速度过大，因为轨迹太短，所以延长时间
## 总体代码结构

```python
ego_planner_node

├── map_manager
│
├── path_search
│     A*
│
├── bspline_optimizer
│
├── collision_checker
│
└── trajectory_manager
```
