master分支的主要功能一个功能丰富的多机器人路径规划与模拟软件，可以实现但AMR与多AMR路径规划，单AMR路径规划包含很多常见的路径规划算法，如A*、Dijistra、以及各种变体A*。多AMR路径规划算法主要是PBS与CBS算法

1、该平台首先基于JSON格式，实现了地图的管理，能够有效兼容仙工上位机AMR导出的地图，并一比一复原（包含障碍物信息与路线信息）。

2、能够兼容多中类型AMR的通信功能（TCP ModbusTCP）

3、实时显示AMR姿态与位置

4、便捷有效的地图构建与修改功能

5、简单的AMR路径规划（基于已构建的地图），复杂多AMR路径规划（基于冲突的搜索算法）代码不在此仓库，需要代码请联系我本人，wx：18010708329

<img width="1602" height="934" alt="image" src="https://github.com/user-attachments/assets/c380b705-d14e-45b6-ab03-11c673388d8f" />

其他控件设计 
1、QTableWidget - 表格
为了实现一个灵活的表格功能，我基于QTableWidget设计了一款表格控件，能够支持自定义表格内的控件，灵活满足各种类型的功能开发需要。 - 代码未上传的，需要的联系我本人

<img width="349" height="219" alt="image" src="https://github.com/user-attachments/assets/2379ec98-2d0c-4acb-93d5-f5a5155138b2" />
