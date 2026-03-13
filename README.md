# 注意！
我们最近开发的规划器 [EGO-Swarm](https://github.com/ZJU-FAST-Lab/ego-planner-swarm) 是 EGO-Planner 的演进版本。
它更鲁棒、更安全，因此更推荐使用。
如果你只有一架无人机，只需在 EGO-Swarm 的 launch 文件中将 `drone_id` 设为 `0`。
当然，部分话题名与 EGO-Planner 不同，可通过 `rqt_graph` 与 `rosnode info <package name>` 查看。

# ROS2 支持
ROS2 版本请参考 ego-planner-swarm 仓库的分支 [ros2_version](https://github.com/ZJU-FAST-Lab/ego-planner-swarm/tree/ros2_version)。

# 3 分钟快速上手
在安装了 ROS 的 ubuntu **16.04、18.04 和 20.04** 上编译测试通过。
可依次执行以下命令：
```
sudo apt-get install libarmadillo-dev
git clone https://github.com/ZJU-FAST-Lab/ego-planner.git
cd ego-planner
catkin_make
source devel/setup.bash
roslaunch ego_planner simple_run.launch
```
如果访问 github 较慢，推荐使用 gitee 镜像仓库 [https://gitee.com/iszhouxin/ego-planner](https://gitee.com/iszhouxin/ego-planner)。该仓库会自动同步。

如果你觉得这个项目有帮助或有趣，请给我们点个 star，谢谢！

# 致谢
- 本仓库框架基于 [Fast-Planner](https://github.com/HKUST-Aerial-Robotics/Fast-Planner)，作者 Zhou Boyu，在四旋翼局部规划方面表现出色。

- 我们使用的 L-BFGS 求解器来自 [LBFGS-Lite](https://github.com/ZJU-FAST-Lab/LBFGS-Lite)。
它是 C++ 单文件头文件实现，轻量且易用。

- 仿真中的地图生成来自 [mockamap](https://github.com/HKUST-Aerial-Robotics/mockamap)，作者 William Wu。

- 硬件架构基于 [Teach-Repeat-Replan](https://github.com/HKUST-Aerial-Robotics/Teach-Repeat-Replan) 的开源实现。

# EGO-Planner
EGO-Planner：面向四旋翼、无需 ESDF 的基于梯度的局部规划器

**EGO-Planner** 是一种轻量级的基于梯度的局部规划器，无需构建 ESDF，与部分最新方法相比显著降低计算时间。总规划时间仅 **约 1ms**，且不需要计算 ESDF。

<p align = "center">
<img src="pictures/title.gif" width = "413" height = "232" border="5" />
<img src="pictures/comp.jpg" width = "413" height = "232" border="5" />
<img src="pictures/indoor.gif" width = "413" height = "232" border="5" />
<img src="pictures/outdoor.gif" width = "413" height = "232" border="5" />
</p>

**视频链接：** [YouTube](https://youtu.be/UKoaGW7t7Dk), [bilibili](https://www.bilibili.com/video/BV1VC4y1t7F4/)（中国大陆）

## 1. 相关论文
EGO-Planner：一种面向四旋翼、无需 ESDF 的基于梯度局部规划器。作者：Xin Zhou、Zhepei Wang、Chao Xu、Fei Gao（已被 RA-L 接收）。相关链接：[arXiv 预印本](https://arxiv.org/abs/2008.08835)、[IEEE Xplore](https://ieeexplore.ieee.org/abstract/document/9309347)、[IEEE Spectrum 报道](https://spectrum.ieee.org/automaton/robotics/robotics-hardware/video-friday-mit-media-lab-tf8-bionic-ankle)。

## 2. 标准编译

**环境要求**：ubuntu 16.04、18.04 或 20.04，并安装 ros-desktop-full。

**步骤 1**. 安装 **uav_simulator** 依赖的 [Armadillo](http://arma.sourceforge.net/)：
```
sudo apt-get install libarmadillo-dev
```

**步骤 2**. 从 github 或 gitee 克隆代码。这两个仓库会自动同步。

github：
```
git clone https://github.com/ZJU-FAST-Lab/ego-planner.git
```

或 gitee：
```
git clone https://gitee.com/iszhouxin/ego-planner.git
```

**步骤 3**. 编译：
```
cd ego-planner
catkin_make -DCMAKE_BUILD_TYPE=Release
```

**步骤 4**. 运行。

在 _ego-planner/_ 目录打开 rviz 进行可视化与交互：
```
source devel/setup.bash
roslaunch ego_planner rviz.launch
```

在另一个终端的 _ego-planner/_ 目录中运行仿真：
```
source devel/setup.bash
roslaunch ego_planner run_in_sim.launch
```

然后可参考下图控制无人机。

<p align = "center">
<img src="pictures/sim_demo.gif" width = "640" height = "438" border="5" />
</p>

## 2.1 接入 Gazebo（使用外部仿真）

如果你已经能跑通仓库内置仿真，但希望改为 Gazebo，只需要把 EGO-Planner 的输入/输出话题和 Gazebo 对齐即可。

新增了一个不依赖内置动力学与地图生成器的 launch 文件：
```
source devel/setup.bash
roslaunch ego_planner run_in_gazebo.launch
```

默认话题如下，可通过 launch 参数覆盖：

- `odom_topic:=/odom`（无人机里程计，`nav_msgs/Odometry`）
- `cloud_topic:=/camera/depth/points`（深度点云，`sensor_msgs/PointCloud2`）
- `camera_pose_topic:=/camera/pose`（若你用深度图融合，可提供相机位姿）
- `depth_topic:=/camera/depth/image_raw`（若你用深度图融合，可提供深度图）

典型用法（仅用点云建图）：
```
roslaunch ego_planner run_in_gazebo.launch \
  odom_topic:=/your_robot/odom \
  cloud_topic:=/your_depth_camera/points
```

规划器输出控制命令话题保持为：

- `planning/pos_cmd`（`quadrotor_msgs/PositionCommand`）

你可以在 Gazebo 控制端（或中间桥接节点）中订阅该话题，再转换到你当前控制器需要的话题类型（例如速度、姿态或轨迹指令）。

## 3. 使用 IDE
推荐使用 [vscode](https://code.visualstudio.com/)。项目已包含 _.vscode_ 文件夹（默认隐藏）。
按以下步骤配置自动补全与跳转，约需 3 分钟。

**步骤 1**. 在 vscode 中安装 C++ 与 CMake 插件。

**步骤 2**. 重新编译：
```
catkin_make -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=Yes
```
该命令会导出 compile commands 文件，便于 vscode 识别代码结构。

**步骤 3**. 启动 vscode 并打开 _ego-planner_ 目录：
```
code ~/<......>/ego-planner/
```

在 vscode 中按 **Ctrl+Shift+B** 编译代码。该命令定义在 _.vscode/tasks.json_。
你可以在 **"args"** 后追加自定义参数，默认值为 **"-DCMAKE_BUILD_TYPE=Release"**。

**步骤 4**. 关闭并重新打开 vscode，可看到自动补全与跳转已生效。

## 4. 是否使用 GPU
本仓库的 **local_sensing** 包含 GPU 和 CPU 两个版本。默认使用 CPU 版本以提高兼容性。可在 **local_sensing** 包的 _CMakeList.txt_ 中将

```
set(ENABLE_CUDA false)
```

改为

```
set(ENABLE_CUDA true)
```

即可开启 CUDA，生成与真实深度相机类似的深度图。

请同时修改 _CMakeList.txt_ 中
```
    set(CUDA_NVCC_FLAGS
      -gencode arch=compute_61,code=sm_61;
    )
```
的 arch 与 code。如果因显卡不同导致编译错误或深度图异常，可参考 [参考链接 1](https://arnon.dk/matching-sm-architectures-arch-and-gencode-for-various-nvidia-cards/) 或 [参考链接 2](https://github.com/tpruvot/ccminer/wiki/Compatibility)。

别忘了重新编译！

**local_sensing** 是仿真传感器。若 `ENABLE_CUDA` 为 **true**，则用 GPU 模拟双目相机深度图；若为 **false**，则发布不带光线投射的点云。我们的局部建图模块会自动选择深度图或点云作为输入。

CUDA 安装请参考 [CUDA ToolKit](https://developer.nvidia.com/cuda-toolkit)。

## 5. 充分利用 CPU 性能
规划计算时间太短，操作系统可能来不及提升 CPU 频率，导致计算时间变长且不稳定。

因此建议手动将 CPU 频率设置为最大值。首先安装工具：
```
sudo apt install cpufrequtils
```
然后设置为最大频率：
```
sudo cpufreq-set -g performance
```
更多信息见 [http://www.thinkwiki.org/wiki/How_to_use_cpufrequtils](http://www.thinkwiki.org/wiki/How_to_use_cpufrequtils)。

注意：高负载下温度过高仍可能导致降频。

# 改进的 ROS-RealSense 驱动

我们修改了 ros-realsense 驱动，使激光发射器每隔一帧脉冲工作，从而在有发射器辅助的情况下输出高质量深度图，同时双目图像不受激光干扰。

<p align = "center">
<img src="pictures/realsense.PNG" width = "640" height = "158" border="5" />
</p>

该驱动修改自 [https://github.com/IntelRealSense/realsense-ros](https://github.com/IntelRealSense/realsense-ros)，兼容 librealsense2 2.30.0。
在 Intel RealSense D435 和 D435i 上完成测试。

参数 `emitter_on_off` 用于开启/关闭该功能。
注意：开启该功能时，设备输出帧率将减半，因为一半用于深度估计，另一半输出双目灰度图。
此外，`depth_fps` 与 `infra_fps` 必须一致，并且 `enable_emitter` 必须为 true。

## 安装

需要显式安装 librealsense2 2.30.0 驱动。
在 x86 CPU 上一般 5 分钟内可完成。
首先卸载当前驱动：
```
sudo apt remove librealsense2-utils
```
或手动删除从源码安装的 librealsense 文件。
然后安装 2.30.0 版本：
```
sudo apt-key adv --keyserver keys.gnupg.net --recv-key F6E65AC044F831AC80A06380C8B3A55A6F3EFCDE || sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-key F6E65AC044F831AC80A06380C8B3A55A6F3EFCDE
```
ubuntu 16.04：
```
sudo add-apt-repository "deb http://realsense-hw-public.s3.amazonaws.com/Debian/apt-repo xenial main" -u
```
ubuntu 18.04：
```
sudo add-apt-repository "deb http://realsense-hw-public.s3.amazonaws.com/Debian/apt-repo bionic main" -u
```
继续执行：
```
sudo apt-get install librealsense2-dkms
sudo apt install librealsense2=2.30.0-0~realsense0.1693
sudo apt install librealsense2-gl=2.30.0-0~realsense0.1693
sudo apt install librealsense2-utils=2.30.0-0~realsense0.1693
sudo apt install librealsense2-dev=2.30.0-0~realsense0.1693
sudo apt remove librealsense2-udev-rules
sudo apt install librealsense2-udev-rules=2.30.0-0~realsense0.1693
```
可通过
```
realsense-viewer
```
验证安装。

## 运行

确认无误后，可通过 `catkin_make` 编译 _modified_realsense2_camera.zip_ 中的 ros-realsense 包，然后运行：
```
roslaunch realsense_camera rs_camera.launch
```
默认 30Hz 输出深度流和双目流。

<!--
# 轻量级四旋翼仿真器

我们使用的四旋翼仿真器继承并修改自 [Fast-Planner](https://github.com/HKUST-Aerial-Robotics/Fast-Planner)。
它轻量且易用，仅需一个话题即可控制无人机。
可以执行
```
roslaunch so3_quadrotor_simulator simulator_example.launch
```
运行 ego-planner/src/uav_simulator/so3/control/src/control_example.cpp 中的简单示例。
如果这个仿真器对你有帮助，也请给 [Fast-Planner](https://github.com/HKUST-Aerial-Robotics/Fast-Planner) 点个 star。-->

# 许可
源码遵循 [GPLv3](http://www.gnu.org/licenses/) 协议。

# 维护
我们仍在扩展系统并提升代码可靠性。

技术问题请联系 Xin Zhou（iszhouxin@zju.edu.cn）或 Fei GAO（fgaoaa@zju.edu.cn）。

商业合作请联系 Fei GAO（fgaoaa@zju.edu.cn）。
