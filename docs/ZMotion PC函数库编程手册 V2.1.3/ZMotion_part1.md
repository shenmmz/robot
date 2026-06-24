![](images/845f1c726b03a03dc42c3e0795e1046a4c9cafa106d741b2c0344f0ce8b55abd.jpg)

# Zmotion PC函数库编程手册

Version2.1.1

![](images/9a0bfc51a65711eff6aea162c0a7abdc5396ad1e58c45f740e7ecba85216aba7.jpg)

![](images/5b1e390b144b7956aa08424c271dee801c9bb935e72f6237f1081ea7417dca33.jpg)

![](images/cac3bc218297375695be43f49ebf2bc3364825696a11a58ae0aba581ccaf9d97.jpg)

![](images/4246ba6d84d9e51f8c355007101b4688495194989f726ff5770d059da11bfe96.jpg)

![](images/0e615537fb447fdc1d520c4128d1011a5b78b28c1377a721359b866c5a432360.jpg)

![](images/c85c705b89435ffd7dd30e14441d453348214717470602c0f83fbf3334e54395.jpg)

![](images/7d0cb344d6c644c6476729608a6fc8b55c8d34ef936002acc76b6b4551cb3024.jpg)

![](images/5cb02e65cff6eb0bc0507586b0b67dc50cd925c53384dc78a6cb96e30b42b992.jpg)

![](images/068042e03e70c0aa3ad64200f2408988d531da1786f61f909ef1b46d992918e2.jpg)  
python  
:: ROS

![](images/df25eb5d72ca87ff0a54a4fc0089f842c9c88c0e362d7af4ea0be388d8c16e26.jpg)

![](images/902515b5d6ea12ba457154bc0cb6257c0f5825bb2f075a76000e89551c3c1a12.jpg)  
机器视觉运动控制一体机

![](images/e7765c7c10e3043998ffc392c54fc31ee167c8a5c839d257a6820d4e28a8fc5d.jpg)  
运动控制器

![](images/47f63ec178b01ef8298ef1325d61367d53372dc3f5a311383e748ddb2a1f17d2.jpg)  
运动控制卡

![](images/9acf096f914b915e222e8077fe5d8547c13b7870c3f88ff4aba52a2202b82693.jpg)  
I0扩展模块

![](images/9ce442149b2d33260055602e3c706a6ea39970b997aa5415634d8e0688538155.jpg)  
人机界面

## 版权说明

## Zmotion

本手册介绍了正运动技术 PC 函数指令的使用及相关开发功能的简介。

为了更好的服务于广大受众，深圳市正运动技术有限公司，对所发布的信息（含文字、数据、图片等）作出以下声明

本手册版权归深圳市正运动技术有限公司所有，严禁任何媒体、网站、个人或组织以任何形式或出于任何目的在未经本公司书面授权的情况下抄袭、转载、摘编、修改本手册内容，或链接、转贴或以其他方式复制用于商业目的或发行，或稍作修改后作为他用，前述行为均将构成对本公司手册版权之侵犯，本司将依法追究其法律责任。

本手册中的信息资料仅供参考。由于改进设计和功能等原因，正运动公司保留对本资料的最终解释权！内容如有更改，恕不另行通知！

![](images/d9ca179695942381fb212bd54785a67d95c8daebe6fbf64e7b23870277f202f5.jpg)

调试机器要注意安全！请务必在机器中设计有效的安全保护装置，并在软件中加入出错处理程序，否则所造成的损失，正运动公司没有义务或责任对此负责。

为了保证产品安全、正常、有效的使用，请您务必在安装、使用产品前仔细阅读本产品手册。

本手册所有例程源码均可通过以下百度网盘获取！！

网盘链接：https://pan.baidu.com/s/19z27imhdqwTDxycFwuenrA?pwd=qcgk 提取码：qcgk

![](images/9401ddcd54c410fe1978c4093845d204a40ad91f9f3eaf2d38c702a25dcf5c01.jpg)

## 修订记录

<table><tr><td rowspan=1 colspan=1>更新日期</td><td rowspan=1 colspan=1>版本号</td><td rowspan=1 colspan=1>版本（更改）说明</td><td rowspan=1 colspan=1>更新人</td></tr><tr><td rowspan=1 colspan=1>2022/10/15</td><td rowspan=1 colspan=1>V2.1.0</td><td rowspan=1 colspan=1>1.新增 MOVEOP 例程（4.1.3）2.新增HW2相关内容及例程（4.1.3）</td><td rowspan=1 colspan=1>YJJ</td></tr><tr><td rowspan=1 colspan=1>2022/12/10</td><td rowspan=1 colspan=1>V2.1.1</td><td rowspan=1 colspan=1>1.新增 MotionRt 的章节（4.1.2-2）及相关例程(4.1.3-5)2.新增实用封装例子（2.2.2）3.新增在线命令例子（12.1.3）</td><td rowspan=1 colspan=1>YJJ</td></tr><tr><td rowspan=1 colspan=1>2024/1/30</td><td rowspan=1 colspan=1>V2.1.2</td><td rowspan=1 colspan=1>修改部分指令描述错误</td><td rowspan=1 colspan=1>ZXL</td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr></table>

## 目录

Zmotion PC 编程手册·基本功能篇..... ........................................  
第一章 产品描述说明.........  
1.1 正运动控制器特点.. .....................................................................  
1.2 产品架构......  
第二章 函数库的使用与封装...  
2.1 函数库调用.. .......................................................................  
Windows 系统 . .......................................................................... ...... 3  
LINUX 系统. .......................................................................  
Wince 系统 .....  
2.2 辅助库封装.... .........................................................................  
辅助库封装方式...  
实用封装例子... ........................................................................ .... 30  
第三章 指令一览表............  
第四章 板卡连接初始化.. .......................................................................... .... 55  
4.1 控制器连接......  
指令列表..  
重点说明..  
例程... ....................................................................... .... 75  
4.2 控制器信息获取.. .......................................................................  
指令列表...  
重点说明..  
例程... .................................................................... .... 86  
第五章 运动控制参数设置..  
5.1 运动基本参数....  
指令列表..  
重点说明...  
例程.... ....................................................................... .... 97  
5.2 轴其他参数.....  
指令列表.. ........................................................................ .... 104  
重点说明... ....................................................................... ..... 104  
例程.... ......................................................................... ..... 108  
5.3 轴参数指令表.......  
指令列表.. ........................................................................ .... 122  
重点说明...  
例程... ............................................................................ .... 124  
第六章 基本运动控制......  
6.1 运动缓冲机制.. ............................................................................ .. 128  
指令列表..  
重点说明.. ............................................................................ .. 129  
例程...........  
6.2 单轴运动... ... 135  
.... 135  
例程... ............................................................................ ... 136  
6.3 回零运动.... .............................................................................. .... 144  
....... 144  
重点说明.... ......................................................................... ... 145  
例程... .............................................................................. ..... 148  
....... 151  
指令列表.... ..................................................................... ... 151  
重点说明... ........................................................................... .... 151  
.... 152  
6.5 插补运动.... ........................................................................... ... 165  
指令列表... ............................................................................. ..... 165  
..... 167  
例程.... ............................................................................. .... 170  
6.6 轴暂停/恢复/停止相关 ...................................................................... ... 202  
指令列表....... ...................................................................... ... 202  
... 202  
例程........................................................................................... ... 202  
第七章 进阶运动控制... .......................................................................... ... 205  
7.1 坐标循环............... 指令列表...... ............................................................................ .......................................................................... .... 205 ... 205  
重点说明... .......................................................................... ... 205  
例程... .......................................................................... ... 205  
7.2 螺距补偿... ............................................................................... ... 208  
指令列表.... .................................................................... ... 208  
重点说明... ................................................................... ... 209  
例程....... ................................................................................. ..... 209  
7.3 反向间隙补偿...... .......................................................................... ... 213  
指令列表... ...................................................................... ... 213  
..... 213  
... 213  
第八章 硬件接口访问与配置.. ....................................................................... .... 217  
8.1 数字输入输出........ ...................................................................... ... 217  
指令列表..... ............................................................................. .... 217  
重点说明... ................................................................................ .... 217  
.... 218  
8.2 模拟量输入输出.................................................................................. ... 224  
指令列表.... ............................................................................ ... 224  
重点说明..... ......................................................................... ... 224  
...224  
8.3 PWM 控制... ... 226  
指令列表.. ............................................................................ .... 226  
重点说明........  
例程.... ......................................................................... ... 227  
第九章 安全机制.... ......................................................................... ... 230  
9.1 轴软件限位函数....  
指令列表... ........................................................................ .... 230  
重点说明.. ........................................................................ ... 230  
例程...........  
9.2 轴硬件限位函数.. .................................................................... ... 234  
指令列表.. .......................................................................... .... 234  
重点说明..  
例程..... ...................................................................... .... 235  
9.3 轴报警函数... ............................................................................. .... 238  
指令列表..  
重点说明...  
例程...... ............................................................................ .... 238  
第十章 板卡数据交互... ......................................................................... ..... 241  
10.1 VR 寄存器 ...... ...................................................................... ...... 241  
指令列表....  
重点说明.. ..................................................................... ... 241  
例程.... ..................................................................... ..... 241  
10.2 Table 寄存器...... ........................................................................ .... 243  
指令列表... ....................................................................... ... 243  
重点说明.. .................................................................... ... 243  
例程....... ..................................................................... ..... 244  
10.3 Modbus 寄存器 . ..................................................................... .... 246  
指令列表... ................................................................. ... 246  
重点说明..... ....................................................................... ...... 246  
例程...... ...................................................................... ... 247  
10.4 Flash/文件读写.. ..................................................................... ... 250  
指令列表... ........................................................................ ...... 250  
重点说明... .......................................................................... .... 251  
例程.. ............................................................................. ... 252  
第十一章 总线相关.... ........................................................................ ... 256  
11.1 总线初始化...... ........................................................................... .... 259  
指令列表... ......................................................................... .... 259  
重点说明.. ......................................................................... ...... 259  
例程...... ......................................................................... .... 264  
11.2 SDO/PDO 读写... .......................................................................... .... 270  
指令列表.. ..................................................................... .... 270  
重点说明....  
例程..... ... 271  
11.3 总线转矩力矩... ... 274  
..... 274  
重点说明.. ................................................................. ... 274  
例程.... ......................................................................... ... 275  
.... 277  
12.1 在线命令..... ...................................................................... ... 277  
指令列表... ......................................................................... .... 277  
.... 277  
例程... .......................................................................... .... 277  
12.2 控制器 ZBASIC 相关 ...... .............................................................. .... 293  
指令列表..... .................................................................... ... 293  
重点说明....... .................... ............................................ .... 294  
例程....... ..................................................................... .... 295  
12.3 控制器自动上报.. .................................................................... ... 298  
指令列表.... .......................................... ... 298  
重点说明..... ....................................................................... .... 298  
例程... ................................................................... ... 299  
12.4 调试相关.............. ................................................................... ... 303  
指令列表..... ........................................................................ .... 303  
重点说明... ........................................................................ .... 304  
例程............... ............................................................. .... 309  
12.5 ZDevelop 辅助调试.......................................................................... .... 315  
ZDevelop 简介..... ................................................ .... 318  
ZDevelop 如何获取.. ............................................. .... 318  
ZDevelop 如何配合调试.... ............................................... .... 319  
第十三章 指令详解....... .................................................. .... 333  
第十四章 指令返回值.. ............................................................... .... 532  
14.1 指令返回值..... .............................................................. .... 532  
14.2 指令返回值详细说明........ ......................................................... .... 532  
14.3 其他：快速入门/常见问题 . ....................................................... ..... 542  
快速入门..... ...................................................... ..... 542  
常见问题... ........................................................ ... 551  
Zmotion PC 编程手册·高级功能篇.. ......................................................... .... 555  
第一章 指令列表... ............................................................ .... 521  
第二章 轴叠加/电子凸轮 ................................................................................ ... 523  
.... 523  
.... 523  
.... 523  
..... 523  
2.2 电子凸轮同步追踪... ..................................................................... ... 526  
.... 526  
重点说明. .. 526  
例程... .. 528  
第三章 高速锁存应用. .. 547  
3.1 高速锁存... .. 547  
指令列表. .. 547  
重点说明. .. 547  
例程.. .. 550  
第四章 位置比较输出.. .. 558  
4.1 位置比较输出应用. .. 558  
指令列表.. .. 558  
重点说明. .. 558  
例程.. .. 560  
第五章 PT 运动... .. 585  
5.1 PT 运动.. .. 585  
指令列表. .. 585  
重点说明. .. 585  
例程... .. 585  
第六章 机械手控制运动应用. .. 590  
6.1 机械手模型. .. 594  
指令列表. .. 594  
重点说明.. .. 594  
例程.. .. 595  
第七章 指令详解... ..604

Zmotion PC 编程手册·基本功能篇

## 第一章 产品描述说明

## 1.1 正运动控制器特点

运动控制器支持 PC 直接在线控制，提供 DLL 函数库和 VC，VB，C#，PYTHON，LABVIEW 等例程。函数库同时提供 WINCE 和 LINUX 的支持。函数库针对于所有型号的控制器。

![](images/2a7d87d1b65728c5e8245ff5f86bbbe5bf2590cc3f166a4150d82a94fba59ef3.jpg)

运动控制器在线控制相对PCI具有下面的优势：

1、不使用插槽，稳定性更好。

2、降低对PC的要求，不需要 PCI插槽。

3、可以选用MINI 电脑或ARM工控电脑，降低整体成本。

4、控制器直接做接线板使用，节省空间。

5、控制器上可以并行运行程序，与 PC只需要简单交互，降低 PC 软件的复杂性。

6、运动控制器就是控制电动机运行方式的专用控制器，控制结构模式一般是：控制器+ 驱 动 器 + （ 步 进 或 伺 服 ） 电 机 。距补偿、固步跟踪、运动叠加、虚拟轴、脉冲闭环、硬件位置锁存、位置比较输出、连续插补、运动暂停等功能。

综上可以看出，选用以太网接口的运动控制器来代替 PCI运动控制卡，可以节省空间，降低成本，优化程序，接线更方便，这也是越来越多的应用采用以太网的原因。

## 1.2 产品架构

![](images/96e3c7ae142301850b7c384cb9a30e027d5f19c0ef5f9a9eb9663ec3b9db5fef.jpg)

## 第二章 函数库的使用与封装

## 2.1 函数库调用

## Windows 系统

2.1.1-1 VC++6.0

1) 打开 VC++6.0

2) 新建一个工程

3) 选择[MFC APPWizard(exe)]

4) 选择工程保存路径

5) 设置工程名，选择[确定]

![](images/3615e5ee5c61249ce83c59fd9901e4b8507b07a4cfa85e105e12df3d750e7627.jpg)  
6) 在应用程序类型选择基本对话框。完成新建

7) 将提供的库函数 zauxdll.h、zauxdll.lib、zauxdll.dll 及 zmotion.dll 放到工程路径下（或者直接将通过 zauxdll.cpp 的源码加入工程）

![](images/0a1ed8c3fc512ce321fa645050c029a88d59da11626be4936564007f25f8c9a1.jpg)  
8) 在 testDlg.cpp 添加头文件#include “zauxdll.h”

9) 在工程文件中，找到 CTestDlg::OnInitDialog( )函数，在括号中添加连接控制器的代码：ZAux\_OpenEth(“192.168.0.11”, &g\_handle)

![](images/cb370cb4875c9fe6dd4af588b985f90e7a1914c8a144dfe5f5b1d79398b81070.jpg)  
10) 在 CTestDlg 添加一个成员函数 CTestDlg::OnClose();用于关闭窗口断开控制器连接。在函数内添加 ZAux\_Close(g\_handle); 断开连接

![](images/4264adb38ff0f72be8831d000ec65c8710b583a252ce57877fcd1faaf1f1e624.jpg)

11) 双击启动、停止在对应的事件函数下添加对应的代码：

ZAux\_Direct\_Single\_Vmove(g\_handle,0,1);

ZAux\_Direct\_Single\_Cancel(g\_handle,0,2);

![](images/baf087765a61d44084c70dfc5f8ea749223635d79903a97d0dd280324f1cc2bd.jpg)

12) 编译程序后，运行程序，在界面下按下运行，则轴0 会按初始化速度正向转动，按下停止，轴0停止运动。

## 2.1.1-2 VB.NET

## 1) 建立项目，保存

![](images/04a7952598f178657f79bbb5b21019362890637b2c904461047913710e1d4d21.jpg)

2) 根据需要的功能和界面，选择相应的控件来完成操作界面

![](images/f3626dc01a7cd186e54d2b20988c61cc9efd9b2e0a9aa419f013dc467ebae0db.jpg)  
3) vb.net 是面向对象的编程语言，选择合理的触发事件（如 Button\_Click、Checked、Load等）来编写程序

Form1.vbForm1.vb [设计]起始页   
Button1 click   
Publie Class Form1   
Public g\_handle As Integer ‘当前使用的卡号   
Public g\_axisnun As Long 当前运动的轴号   
Private Sub Form1\_Load(ByVal sender As System.Object，ByVal e As System.EventArgs)Handles MyBase.   
BackColor = Color.Snow   
g\_handle=0 链接控制器   
ZAux\_OpenEth("127.0.0.1"，g\_handle)   
If g\_handle 0 Then   
MsgBox("控制器链接成功！“，vbOK0nly，“链接状态")   
Timer1.Enabled=True   
Timer1.Interval =1000   
Else   
MsgBox("控制器链接失败！“，vbOK0nly，“链接状态")   
End If   
End Sub   
Private Sub Button1\_Click(ByVal sender As System.Object,ByVal e As System.EventArgs) Handles Butt   
pim m\_units As Single   
Dim m\_lspeed As Single   
Dim m\_speed As Single   
Dim m\_acc As Single   
Dim m\_dec As Single   
Dim m\_sramp As Single   
Dim m\_step As Single   
m\_units = Val (TextBox4.Text）   
m\_lspeed= Val (TextBox5.Text） 1speed   
m\_speed =ValTextBox6.Text） speed   
macc=Val(TextBox7.Text） accel   
错误列表   
0个错误 A0个警告 ①0个消息   
说明

4) 添加 dll 文件和头文件，放在和项目文件同一文件夹内

![](images/711c8ee8e8fe92e4b467a087e2c39de863ddbb8c4fce5e4852fb69c7ce159c08.jpg)

5) 在项目内添加头文件（两个.vb文件）

![](images/58555ca44295776a29eba0455ce99633e2a87ce764b57ceed249948bb9a291e7.jpg)

添加成功后如下图显示：

![](images/7405ef41f190ae743a7396861a5c359a1aba89c9ba5ed1fbd1149b23a4ea0c85.jpg)

6) PC连接上控制器后，设置好 IP，然后启动调试

7) 程序运行调试之后，在文件夹：“bin \ Debug”里面有个 .exe文件，可以直接运行工作

![](images/0e90dd8067ef7cf82dfd4cf22c8b5ffe332d7817a19847b55009f2f122cecd44.jpg)

## 2.1.1-3 C#

1) 打开 VS2012，点击[文件栏]的[新建]—[项目]，选择[windows 窗体应用程序]

![](images/cedebbf7ed4c59587c2d10f8ab57bdef616bf8e150458b532f88626f5849970e.jpg)

2) 找到新建项目的文件夹，将 zmcaux.cs 文件放入文件夹中

<table><tr><td>名称 &gt;</td><td>修改日期</td><td>类型</td><td>大小</td></tr><tr><td>bin</td><td>2022/5/14 9:36</td><td>文件夹</td><td></td></tr><tr><td>obj</td><td>2022/5/14 9:36</td><td>文件夹</td><td></td></tr><tr><td>Properties</td><td>2022/5/14 9:10</td><td>文件夹</td><td></td></tr><tr><td>国 Form2.cs</td><td>2022/6/29 9:53</td><td>Visual C# Sourc...</td><td>1 KB</td></tr><tr><td>国 Form2.Designer.cs</td><td>2022/6/29 9:53</td><td>Visual C# Sourc...</td><td>2 KB</td></tr><tr><td>国 main.cs</td><td>2022/7/20 19:36</td><td>Visual C# Sourc...</td><td>63 KB</td></tr><tr><td>国 main.Designer.cs</td><td>2022/7/19 9:20</td><td>Visual C# Sourc...</td><td>186 KB</td></tr><tr><td>D main.resx</td><td>2022/7/19 9:20</td><td>Microsoft .NET ...</td><td>7 KB</td></tr><tr><td>国 Program.cs</td><td>2022/7/18 16:04</td><td>Visual C# Sourc...</td><td>1 KB</td></tr><tr><td> WindowsFormsApplication1.csproj</td><td>2022/7/18 16:05</td><td>Visual C# Projec...</td><td>7 KB</td></tr><tr><td> WindowsFormsApplication1.csproj.user</td><td>2022/5/14 14:13</td><td>Per-User Project..</td><td>1 KB</td></tr><tr><td> Zmcaux.cs</td><td>2022/6/29 9:09</td><td>Visual C# Sourc...</td><td>216 KB</td></tr></table>

3) 将 zauxdll.dll 和 zmotion.dll 文件放入 debug 文件夹中

<table><tr><td rowspan="7">名称 国 力</td><td>A</td><td>修改日期</td><td>类型</td><td>大小</td></tr><tr><td>司 test_function1.exe</td><td>2017/1/5 9:15</td><td>应用程序</td><td>34 KB</td></tr><tr><td>国 test_function1.pdb</td><td>2017/1/5 9:15</td><td> Program Debug..</td><td>40 KB</td></tr><tr><td>test_function1.vshost.exe</td><td>2017/1/6 9:08</td><td>应用程序</td><td>12 KB</td></tr><tr><td>D test_function1.vshost.exe.manifest</td><td>2010/3/17 22:39</td><td>MANIFEST文件</td><td>1 KB</td></tr><tr><td>zauxdll.dll zmotion.dll</td><td>2016/12/27 8:40</td><td>应用程序扩展</td><td>1,445 KB</td></tr><tr><td></td><td>2016/12/27 8:40</td><td>应用程序扩展</td><td>204 KB</td></tr></table>

4) 用 VS 打开新建的项目文件，在右边的解决方案资源管理器中点击[显示所有文件]，然后鼠标右键点击[zmcaux.cs文件]，点击[包括在项目中]

![](images/fd540503c391ef0622ddf16d0d6836874e11c9d8e433a9e2343bd68dd92f6954.jpg)

5) 双击[form1]，出现代码编辑界面，在文件开头写入 using cszmcaux

using System:   
using System. Collections. Generic;   
using System. ComponentModel:   
using System Data:   
using System. Drawing:   
using System. Ling:   
using System. Text:   
using System. Windows. Form5:   
using System. Threading: //线程声明   
using System. Text.RegularExpression5://正则表达式声明   
using cszmcaux :   
namespace WindowsFormsApplication1   
{   
public partial class main : Form   
{   
public IntPtr g\_handle：//声明控制器连接句柄   
6) 开始编程

## 2.1.1-4 LABVIEW

1. 找到厂家提供的光盘资料里面的 LabVIEW的VI库文件，路径如下。

![](images/ab92804c6a8579be5bf859747534a4be12cdba2748d2d8cac89cadcd0b98ad76.jpg)

1) 进入厂商提供的光盘资料找到“04PC 函数”文件夹，并点击进入。

FileStation

<table><tr><td>C &lt;&gt; 00光盘资料</td><td colspan="4">过迪</td></tr><tr><td>下载文件夹</td><td></td><td></td><td></td><td>自</td></tr><tr><td colspan="3">名称 大小</td><td>文件类型</td><td colspan="2">修改日期</td></tr><tr><td colspan="3">01产品样本目录</td><td>文件夹</td><td colspan="2">2025-04-2109:57:27</td></tr><tr><td colspan="3">02用户手册</td><td>文件夹</td><td colspan="2">2025-02-2611:16:58</td></tr><tr><td colspan="3">03编程手册</td><td>文件夹</td><td colspan="2">2025-04-2109:57:34</td></tr><tr><td colspan="2">04PC函数</td><td></td><td>文件夹</td><td colspan="2">2025-05-0718:50:08</td></tr><tr><td colspan="2">05工具软件</td><td></td><td>文件夹</td><td colspan="2">2025-04-2110:00:07</td></tr><tr><td colspan="2">06ZDevelop参考例程</td><td></td><td>文件夹</td><td colspan="2">2024-11-27 18:36:38</td></tr><tr><td colspan="2">07参考文档资料</td><td></td><td>文件夹</td><td colspan="2">2025-03-2809:24:53</td></tr><tr><td colspan="2">08控制器接线参考</td><td></td><td>文件夹</td><td colspan="2">2024-11-27 18:43:28</td></tr><tr><td colspan="2">09控制器外观图</td><td></td><td>文件夹</td><td colspan="2">2024-01-3015:04:01</td></tr><tr><td colspan="2">10PC驱动数字签名相关</td><td></td><td>文件夹</td><td colspan="2">2025-04-2109:57:09</td></tr><tr><td colspan="2">00光盘资料.zip</td><td>14.13GB</td><td>ZIP文件</td><td colspan="2">2025-01-1709:01:13</td></tr><tr><td colspan="2">正运动光盘资料共享指引.docx W</td><td>102.6KB</td><td>DOCX文件</td><td colspan="2">2024-10-1613:45:29</td></tr></table>

C

2) 选择“01PC函数库V2.1”文件夹。

![](images/2ceb14017037a1272f0ecd0c9617093bc9c75567602176e86be8e3a6614d2fd9.jpg)

## FileStation

<table><tr><td>名称</td><td>大小</td><td>文件类型</td><td>修改日期</td></tr><tr><td>01PC函数库V2.1</td><td></td><td>文件夹</td><td>2025-04-08 09:20:10</td></tr><tr><td>02ZmotionPC函数库编程手册...</td><td></td><td>文件夹</td><td>2025-04-1410:22:25</td></tr><tr><td>03Zmotion CAD库V3.1</td><td></td><td>文件夹</td><td>2025-05-0718:49:43</td></tr><tr><td>04PC工具</td><td></td><td>文件夹</td><td>2024-11-2718:26:12</td></tr><tr><td>辅助库历史版本备份</td><td></td><td>文件夹</td><td>2025-04-0811:03:36</td></tr><tr><td>01PC函数库V2.1.rar</td><td>642.1MB</td><td>RAR文件</td><td>2024-02-0410:08:42</td></tr><tr><td>国 05辅助库封装源代码.rar</td><td>43.2MB</td><td>RAR文件</td><td>2023-11-2811:58:23</td></tr><tr><td>W PC函数库-DLL说明文件.docx</td><td>55.4KB</td><td>DOCX文件</td><td>2024-01-2410:39:16</td></tr><tr><td>ZMC接口的抓包软件.rar</td><td>5.4MB</td><td>RAR文件</td><td>2024-08-2716:43:24</td></tr><tr><td>整理日志.txt</td><td>576Bvtes</td><td>TXT文件</td><td>2025-04-0811:12:28</td></tr><tr><td></td><td></td><td></td><td>10个项目</td></tr><tr><td colspan="4">FeDtaeron C &gt; 00光盘资料&gt;04PC函数01PC函数库V2.1</td></tr><tr><td>下载文件夹</td><td colspan="3">过滤</td></tr><tr><td colspan="2">名称</td><td>大小</td><td>文件类型 修改日期</td></tr><tr><td colspan="2">linux平台</td><td></td><td>文件夹 2025-04-0810:52:50</td></tr><tr><td colspan="2">wince平台</td><td>文件夹</td><td>2024-11-27 17:23:04</td></tr><tr><td colspan="2">windows平台</td><td>文件夹</td><td>2025-04-2517:38:45</td></tr><tr><td colspan="2">PC函数库V2.1升级说明.txt 359Bytes D</td><td>TXT文件</td><td>2023-11-2816:15:21</td></tr><tr><td colspan="2">PC函数库文件夹分类.png 32.7KB</td><td>PNG文件</td><td>2023-11-28 16:15:21</td></tr><tr><td colspan="2">Thumbs.db 15KB</td><td>DB文件</td><td>2024-11-1909:21:59</td></tr><tr><td colspan="2"></td><td></td><td></td></tr><tr><td colspan="2"></td><td></td><td></td></tr><tr><td colspan="2"></td><td></td><td></td></tr><tr><td colspan="2"></td><td></td><td></td></tr><tr><td colspan="2"></td><td></td><td></td></tr><tr><td colspan="2"></td><td></td><td>6个项目 C</td></tr></table>

3) 选择“Windows 平台”文件夹。

![](images/4d7dd97c3747013687cb5734d3253d81b397c3647db107f60e7d5adb421a3dfc.jpg)

## FileStation

4) 选择“Labview”文件夹，里面有 32 位和 64 位的动态库和例程。

![](images/348c0c46d048cab40e14715182b5d0af65a4e9dd299b0b442b426807175ba4a1.jpg)

## File Station

<table><tr><td>&lt;&gt; C</td><td colspan="3">00光盘资料&gt;04PC函数&gt;01PC函数库V2.1windows平台</td><td>过</td><td></td></tr><tr><td>下载文件夹</td><td></td><td></td><td></td><td></td><td>国</td></tr><tr><td>名称</td><td></td><td>大小</td><td>文件类型</td><td>修改日期</td><td></td></tr><tr><td>00库文件</td><td></td><td></td><td>文件夹</td><td>2025-04-0810:34:05</td><td></td></tr><tr><td>01函数库源码</td><td></td><td></td><td>文件夹</td><td>2025-04-2113:37:53</td><td></td></tr><tr><td>C#</td><td></td><td></td><td>文件夹</td><td>2025-04-2818:30:10</td><td></td></tr><tr><td>C++</td><td></td><td></td><td>文件夹</td><td>2024-12-2009:08:07</td><td></td></tr><tr><td>Labview</td><td></td><td></td><td>文件夹</td><td>2024-07-0110:01:50</td><td></td></tr><tr><td>Matlab</td><td></td><td></td><td>文件夹</td><td>2024-06-28 17:15:27</td><td></td></tr><tr><td>Python</td><td></td><td></td><td>文件夹</td><td>2024-11-0816:18:21</td><td></td></tr><tr><td>Qt</td><td></td><td></td><td>文件夹</td><td>2024-06-2817:15:38</td><td></td></tr><tr><td>VB.NET</td><td></td><td></td><td>文件夹</td><td>2024-06-28 17:14:23</td><td></td></tr><tr><td>VB6.0</td><td></td><td></td><td>文件夹</td><td>2024-06-2817:14:27</td><td></td></tr><tr><td>易语言</td><td></td><td></td><td>文件夹</td><td>2024-06-2817:14:32</td><td></td></tr><tr><td>目 Labview.rar</td><td></td><td>138.8MB</td><td>RAR文件</td><td>2025-04-0910:53：55</td><td></td></tr></table>

![](images/ca2621681cb1c35d474797bdfd57fe8c41178176c274772d41a038e2b8ca1805.jpg)

2. 将厂商提供的 LabVIEW 的 VI 库文件复制到 LabVIEW 安装路径下 LabVIEW/user.lib的文件夹内。

1) 找到解压的 VI 库文件夹（zaudll 是 32 位的 VI 库，64 的 VI 库是 newZauxDll64）。

![](images/2e74fa5432d271f3bb2b1678b6a67742a080f7f1ac219f5925480eb01ce385d1.jpg)

2) 选中 LabVIEW 右击打开文件所在位置（这里以 LabVIEW2013-32 位为演示）。

![](images/6cc2524747cf5fbb3f0e05fb16123c5260e1155162e60818b79f23f488186548.jpg)

3) 将整个 VI 库文件夹直接复制放到 user.lib 该目录，然后关闭 LabVIEW 软件重新启动。

![](images/8b5eddc6e420f07c4ea15959327f61a9002c230ce6044967cee7dca372aa37a2.jpg)

3. 重新启动LabVIEW 后，选择新建VI，然后右键点击VI程序框图空白处，选择用户库来找到添加的VI库，或者直接搜索 VI，最后将需要的函数直接拖到面板上。

![](images/ea2365765aa9034582e8b2edb8ea506bbdb01d980ce22be17b72139b06816eaf.jpg)

![](images/31de5ee7df06ba8316c428f3e2ac243a0103e9d1941bed9449bbb4c023c47368.jpg)

## 2.1.1-5 Python

## 1. 导入动态库

1) python 中需要导入 platform 模块（以便获取前操作系统的类型)和 ctype 模块(导入相关的连接库--动态库主要是 C/C++)

2) 根据不同的平台选择相应的动态库(C/C++)，例如 windowsX86、windowsX64、Linux 等平台的动态库

limport platform   
import ctypes   
systype = platform.system()   
Pif systype == 'Windows' :   
if platform.architecture()[0] == ‘64bit':   
zauxdll = ctypes.WinDLL(' ./zauxdll64.dll')   
print('Windows x64')   
else:   
zauxdll = ctypes.WinDLL('./zauxdll.dll')   
print('Windows x86')   
Pelif systype == 'Darwin':   
print("macOS")   
Pelif systype == 'Linux':   
zmcdli = ctypes.CDLL(' ./zmotion.so')   
print("Linux")   
18 Pelse:   
print("OS Not Supported!!")

2. Python 函数封装

1) 进行函数封装，结合动态库使用

2) 根据需求去封装对应的函数

Eclass ZAUXWrapper:   
def\_init\_(self):   
self.handle = ctypes.c\_void\_p0   
self.is\_connected = False   
def search(self， console=[]):   
zauxdll.ZAux\_SearchEth(ctypes.byref(iplist)，1024, 200)   
s = iplist.value.decode()   
str\_iplist = s.split(   
num = len(str\_iplist)   
print(\*str\_iplist，sep='\n')   
console.append("Searching...")   
return str\_iplist， num   
日   
if self.handle.value is not None:   
self.disconnect()   
ip\_bytes = ip.encode('utf-8')   
p\_ip = ctypes.c\_char\_p(ip\_bytes)   
print("Connecting to",ip,...")   
ret = zauxdll.ZAux\_OpenEth(p\_ip， ctypes.pointer(self.handle))   
if ret = 0:

说明：对需要封装的函数进行封装处理，结合本文档进行处理即可。

3. python 函数自定义函数封装 basic 指令

举例说明：

def vmove(self，iaxis， idir):   
cmdbuffer = (ctypes.c\_char \* 2048)0)   
str1 = "VMOVE(%d) AXIS(%d)" & (idir，iaxis)   
cmd = ctypes.c\_char\_p(bytes(str1，'utf-8'))   
zauxdll.zAux DirectCommand(self.handle，cmd， cmdbuffer，2048)   
return   
白 def get\_units(self，iaxis):   
cmdbuffer = (ctypes.c\_char \* 2048)()   
str(cmdbuffer，encoding='utf-8')   
cmd = ctypes.c\_char\_p(bytes(str1，'utf-8'))   
zauxdll.zAux\_DirectCommand(self.handle， cmd，cmdbuffer，2048)   
a = list(cmdbuffer)   
b = b.join(a)   
C = b.decode() .strip(b'\xoo'.decode0)   
return C

说明：可以将想要一些功能结合 basic 指令组成字符串命令，通过字符串命令和ZAux\_DirectCommand 指令结合进行编程。

4. python 函数使用

举例说明：

untitled1 [C\Users\Administrator\PycharmProjects\untitled1]-\tstpy- Pyhar(Administator   
File Edit View Navigate Code Refactor Run Iools VCS Window Help   
untitled1 r testpy   
test.py ×   
  
b = bist(cnboffr)   
三 pyvenv.cfg c = b.decode().strip(b'\x00'.decode()   
1 test.py return c   
zauxdll.dl   
izmotion.dll zaux = ZMCWrapper()   
Ill External Libraries zaux.connect("192.168.0.218")   
Scratches and Consoles pos = zaux.get\_ax\_pos(2)   
print("Axis 0 MPOS:", pos[0])   
print("Axis 1 MP05:",pos[1])   
pos1 = zaux.set\_4x\_pos(0, 5)   
pos2 = zaux.get\_4x\_pos(e, 2)   
print("modbus\_reg(0):",pos2[0])   
print("modbus\_reg(1):",pos2[1])   
zaux.vmove(0,1)   
number = zaux.get\_units(0)   
print("Axis 0 units:",number)   
? C:\Users\Administrator\PycharmProjects\untitledl\venv\Scripts\python.exe C:/Users/Administrator/PycharmProjects/untitled/test.py   
indectingt 92.168.0.2.8   
i Axis 0 MPOS: 1024.0   
Axis 1 MPOS: 500.0   
modbus\_reg(0): 300   
modbus\_reg(1): 400   
Axis 0 units: 1000   
Process finished with exit code 0

说明：创建封装类的对象，然后进行相关函数调用。

## LINUX 系统

## 2.1.2-1 QT

## 1) 启动 Qt Creator 2.6.1，新建一个工程

2) 将产品配套光盘“..\linux\函数库\x86”文件夹中的动态连接库 zmotion.so、头文件zmotion.h 复制到工程文件夹中

3) 在 Qt Creator 界面中，打开.pro 文件，并添加

greaterThan(QT\_MAJOR\_VERSION, 4): QT += widgets (版本兼容问题)   
SOURCES + = main.cpp\   
mainwindow.cpp\   
zaux.cpp   
HEADERS + = mainwindow.h\   
zmotion.h\   
zaux.h

把 main.c 中的头文件改为#include <QApplication>

4) 添加动态库的方法

1>首先右键项目 3>选择第二个外部库，点击[确定]  
2>点击[添加库] 4>点击[浏览库文件]

## 注意事项：

动态库的名字必须是 lib 开头才能识别，然后点击下一步，自动会把动态库的信息加入到.pro中，下面便可以调用头文件中所包含的函数，可以运行程序。

5) 打开 QT 程序，点击[file]找到例程中的.pro 项目文件点击[open]

注意：QT 工程中不能出现中文路径

![](images/ad123c7385cba9efda345143f010c4ee91b6b83ec33bacbb18474fdd027c3530.jpg)

6) 将 QT 例程的.pro 中加入正确路径的动态库，双击[single\_move.pro 文件]，查看到之前设置好的动态库路径，这时候要设置自己的动态库路径

![](images/209a507d3a42f284f57ec4d25d086a809f5a9e6feda4633be61e816205b82213.jpg)

7) 将.pro 文件中的动态库路径进行删除

8) 将文件夹中32位的动态库放到指定的路径

9) 加入自己的动态库路径，首先选中项目文件

![](images/3cc4e9c95268c5e9ad83d3cae9eb534959c145b1c1dc990c2cfac37414aceefd.jpg)

## 10) 再进行右键，点击[add library 添加库]

![](images/9de7ecfccaeabcb18fc425a2a23b9f6e138dd8e6411b2737ed1da67624fa02fd.jpg)

## 11) 选择第二个[Details]添加外部库

![](images/1ca00ce92b2261c87f8372d8aa4f5fe1f4e1ebd9e9321e5991834cfe286449c1.jpg)

## 12) 这时候找到刚才放置动态库的路径

## 注意事项：

Linux 下面的动态库的名称的话是要 lib 开头才能识别，一般的话就不要轻易去更改文件的名称。

![](images/0a83a3dae4ab74b3afce7f086acdcc122b1e5c00e8f044b0426d520f8296cbec.jpg)

## 13) 点击[next]进入下一步

![](images/c8de1c67d8d8994dd79a0d9113246c3506d0091b8486c9aef2c5107751ec7088.jpg)

14) 查看.Pro中生成动态库的路径  
![](images/afbcb3fe2bba0c3769df30180a6bfc9b86b925a053d76ac3554732adc20d7090.jpg)  
15) 点击运行之后就会出现程序

![](images/a950e32e1c892607f6d2846b9d6195800c66e05f84015fb8bd5466080330f960.jpg)  
16) ip 的话可以手动添加，打开 ui界面，双击点击按钮

![](images/ba6b4b84a0a9002ae4504ad8759bc825fa1f02425873754be0a328a0914dc7f4.jpg)

![](images/96b3b0062c423eb40e22f6fca0bd04f07a6d3e1b1e39b94c06029cbd3e999e78.jpg)

## 17) 控制器的 ip 网段 192.168.0.1

控制器的默认 ip 是 192.168.0.11

接下来就可以使用例程了，open 连接 ip，点击[run]

![](images/2b8aefb372b8062bc5c30b41825b637e2e4c1b85e6d4508e9ab7e026072237ac.jpg)

## Wince 系统

## 2.1.3-1 C#

本例程用 wince6 移动设备作为外部连接设备，用 USB 作为连接主线。

1) 首先需要PC机安装同步设备驱动和 USB驱动（不做说明）

2) 用USB 线和PC 机连接，设备会自动与PC机连接，连接完成后同步软件会显示连接成功，如图所示

![](images/b5f4589f4fe0fb89a0b3b74f04b8e305aa3a8c646321f65ee621d9c8c787bdf1.jpg)

3) 设备同步软件连接完成后，将会与VS2008连接步骤如下：

1>点击[工具>>连接到设备]

2>打开如下窗口，同时选择图中平台/设备后点击[连接按钮]

![](images/f578fd90579f976cc59c4ef652df4469ae90c3fa72ca3dad872622303b4edc68.jpg)

3>连接成功后如下图所示：

![](images/c8db11807f0177a4053a6cefb81d69e1f7f48a1e9789b1792b90892c03dac568.jpg)

4>点击[关闭]按钮即可

4) 开始调试/运行程序（本说明使用调试）

5) 点击[调试]按钮后如图显示，部署平台

6) 选择图中平台，点击[部署]即可自动部署

![](images/cde2f6defb514072f687eac82b483a1f53ea418f92c4a623d604cd9dc4fd019f.jpg)

## 7) 部署成功后会在设备上显示出设计窗口，可以开始调试运行

![](images/913f0bf7a8a138fe57d937f2a8bf58b1c708e700c0999e0a20a13eb407d50e46.jpg)  
8) 完成窗口代码设计后，通过 USB 连接到wince6 设备上后，调试运行即可在移动设备上进入此界面

9) 通过wince6 设备界面可直接操作控制器

（注：设备 IP 和控制器 IP 以及 PC 机 IP 应当处于同一网段，且设备 IP 设置时，请注

意不要设置设备网关）

10) 点击[IP 检索]按钮会自动检索 IP 地址，然后点击下拉框按钮，会展现检索到的 IP地址，需要1\~3 秒时间,然后在下拉框中选中需要连接的IP地址

11) 点击[连接]按钮，会自动连接下拉框中指定 IP 的控制器，如果连接失败会产生提示“控制器连接失败，请检查 IP 地址”。（连接成功后会显示“控制器连接成功”）

12) 分别点击“X,Y,Z,R 轴”可选定运动轴，分别对应控制器“0,1,2,3 轴”，然后选择运动方式“寸动/连续”，然后点击[运动]按钮，开始运动

13) “运动/停止/位置清零”控件，是整个例程的控制按键，可以控制程序运行、停止等。（当控制器未连接时，这些按键会提示“未连接控制器”）

14) 操作完成后可通过点击[断开]按钮，断开设备和控制器的连接

## 2.2 辅助库封装

## 辅助库封装方式

![](images/2b9d095f79a35218be6e355790fa167fc3622cc513b1f95cdcbed2aaea85c882.jpg)

ZAux 库提供了运动和基本参数设置的封装，以开放源代码的方式提供，详细内容参见源码。

ZAux 库是直接利用 ZBASIC 命令通过 ZAux\_Execute 方式或 ZAux\_DirectCommand 方式发送到控制器上，相应函数可以参考 ZBASIC 手册对应的命令介绍。如果使用到没有封装的命令或者想封装自己的函数，可以通过 ZAux\_Execute 或 ZAux\_DirectCommand 发送，或是参照已有代码修改增加相应的函数。

发送字符串命令有两种方式，缓冲方式和直接方式。

直接方式：直接执行单个变量/数组/参数相关命令，此时所有传递的参数必须是具体的数值，不能是表达式；参见 ZAux\_DirectCommand 函数。ZAux\_DirectCommand(控制器句柄,

命令字符串, 返回字符串, 返回字符长度)。

缓冲方式可以执行所有命令，并支持表达式作为参数，但是速度慢一些；参见ZAux\_Execute 函数。

ZAux\_Execute(控制器句柄, 命令字符串, 返回字符串, 返回字符长度)确认函数的输入输出，参数的意义及限制。

例:如设置轴速度函数：ZAux\_Direct\_SetSpeed(ZMC\_HANDLE handle, int iaxis, floatfValue)相当与 BASIC 命令中的“SPEED(轴号) = 速度值”。则封装函数时命令字符串就是“speed(%d) =%f”。

## 封装步骤：

1) 编写源代码：打开辅助库源码，在 zmcaux.cpp 中添加对应的函数

```c
int32 __stdcall ZAux_Direct_SetSpeed(ZMC_HANDLE handle, int iaxis, float fValue)
char cmdbuff[2048];
char cmdbuffAck[2048];
if( iaxis > MAX_AXIS_AUX)
{
return ERR_AUX_PARAERR;
}
//生成命令
sprintf(cmdbuff, "SPEED(%d)=%f", iaxis, fValue);
//调用命令执行函数
return ZAux_DirectCommand(handle, cmdbuff, cmdbuffAck, 2048);
```

2) 在 zauxdll.def 中定义新增加的函数定义

zauxdll- Microsoft Visual C++ - [zauxdll.def]   
文件(E）编辑(E） 查看(V）插入() 工程(P) 组建(B)工具(I） 窗口(W)帮助(H)   
省 Replace 3   
CZauxdIlApp All class members CZauxdlIApp □ 心 AD   
；zauxdll.def :Declares the module parameters for the DLL.   
工作区auxdP:1工程 DISCRIPTION"Zauxdil Windous Dynanic Link Lirar   
Source Files   
StdAfx.cpp EXPORTS   
zauxdll.cpp ;Explicit exports can go here   
zauxdll.def ZAux\_TraceOut   
zauxdll.rc ZAux\_Execute   
zmcaux.cpp ZAuX\_DirectCommand   
zmotion.lib ZAux\_SetTraceFile   
Header Files ZAux\_OpenCom   
Resource.h ZAuX\_SearchAndOpenCom   
StdAfx.h ZAux\_SetConDefaultBaud   
zauxdll.h ZAux\_SetIp   
Resource Files ZAuX\_OpenEth   
zauxdll.rc2 ZAuX\_SearchEthlist   
ReadMe.txt Zaux\_SearchEth

## 3) 在头文件中增加对应的声明

![](images/6bfc75f4e1eabd471a2e06dc6f0485c7136bd73909befc54fa33f890694ae6ec.jpg)

## 4) 编译生成新的 DLL

## 实用封装例子

## 2.2.2-1 直接获取多种类型数据

用户若想要获取多种数据，如轴的命令位置，轴的反馈位置，板卡上的IO点等等，往往都是通过多种单独独立的函数获取不同的数据，这样堆积，会导致读写次数的上位，导致程序的卡顿。为了提升一个上位程序的，往往可以通过自己定义一个自己的函数，通过一个函数，快速的把数据传输到上位程序上面来，而非是通过多次循环来获取到不同类型的数据。如下举例：

假设有一个简易的三轴平台，需要读取轴 0，轴 1，轴 2 的命令位置，反馈位置，以及控制器板卡上的输入口 0-输入口32，输出口0-输出口 33，以及三个轴的状态。获取数据程序如下：

![](images/9864acc47ce79a39a3ee6600031ce550d7708858b4ddbd935531d76cbf12071b.jpg)

```cpp
// test1.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include "zmotion.h"
#include "zauxdll2.h"
void commandCheckHandler(const char *command, int ret)
if (ret)//非0则失败
{
printf("%s fail!return code is %d\n", command, ret);
```

```c
Description: //我的自定义直接获取数据函数
Input: //handle 卡链接
iaxisNum 轴的总数量
iaxislist 轴号列表
fDposlist 输出的命令位置值
fMposlist 输出的反馈位置值
iAxisstatuslist 输出的轴状态位置值，按位对应
startIn 要获取起始的IN编号
endIn 要获取结束的IN编号
iIn 输出的IN状态,按位对应
startOut 要获取起始的OUT编号
endOut 要获取结束的OUT编号
iOut 输出的OUT状态,按位对应
Output: //
Return: //错误码
int Demo_Direct_MyGetData(ZMC_HANDLE handle,int iaxisNum, int* iaxislist, float*
fDposlist,float* fMposlist,int32* iAxisstatuslist,int startIn , int endIn,int *iIn,int
startOut , int endOut,int *iOut)
{
char cmdbuff[2048];
char tempbuff[2048];
char cmdbuffAck[20480];
//若传进来的地址为空，则退出
if(NULL == iaxislist || NULL == fDposlist || NULL == fMposlist || NULL
iAxisstatuslist || NULL == iIn || NULL == iOut)
{
return ERR_AUX_PARAERR;
}
//若传进来的结束编号小于起始编码，则退出
if ((endIn<startIn) || (endOut<startOut))
{
return ERR_AUX_PARAERR;
int ret=0;
int i;
//生成命令
```

```c
sprintf(cmdbuff, "?");
//拼接DPOS
for (i=0;i<iaxisNum;i++)
{
sprintf(tempbuff,"DPOS(%d),",iaxislist[i]);//生成对应命令的字符串
strcat(cmdbuff, tempbuff);//字符串拼接
if (strlen(cmdbuff)>1000)
return ERR_AUX_PARAERR; //参数错误，字符串拼接过长
}
}
//拼接MPOS
for (i=0;i<iaxisNum;i++)
sprintf(tempbuff,"MPOS(%d),",iaxislist[i]);//生成对应命令的字符串
strcat(cmdbuff, tempbuff);//字符串拼接
if (strlen(cmdbuff)>1000)
return ERR_AUX_PARAERR; //参数错误，字符串拼接过长
//拼接AXISSTATUS
for (i=0;i<iaxisNum;i++)
sprintf(tempbuff,"AXISSTATUS(%d),",iaxislist[i]);//生成对应命令的字符串
strcat(cmdbuff, tempbuff);//字符串拼接
if (strlen(cmdbuff)>1000)
{
return ERR_AUX_PARAERR; //参数错误，字符串拼接过长
}
int32 ostart,istart,iend,oend; //一次最多32个
bool addflag;
addflag=false;
int32 temp; //一次最多32个
int32 temp2; //一次最多32个
```

```c
temp=endIn-startIn+1;
if (temp%32 == 0)
temp=temp/32;
}
else
{
temp=temp/32+1;
}
//拼接IN
for (i=0;i<temp;i++)
istart = startIn+32*i;
iend =istart+31;
if (iend>endIn)
{
iend=endIn;
}
//生成命令
sprintf(tempbuff, "IN(%d,%d),", istart,iend);
strcat(cmdbuff, tempbuff);//字符串拼接
if (strlen(cmdbuff)>1000)
{
return ERR_AUX_PARAERR ; //参数错误，字符串拼接过长
}
}
temp2=endOut-startOut+1;
if (temp2%32 == 0)
{
temp2=temp2/32;
}
else
{
temp2=temp2/32+1;
}
//拼接OUT
for (i=0;i<temp2;i++)
ostart = startOut+32*i;
oend =ostart+31;
```

```c
if (oend>endOut)
oend=endOut;
}
//生成命令
sprintf(tempbuff, "OUT(%d,%d)", ostart,oend);
strcat(cmdbuff, tempbuff);//字符串拼接
if (i<temp-1)
{
strcat(cmdbuff, ",");//字符串拼接
if (strlen(cmdbuff)>1000)
{
return ERR_AUX_PARAERR; //参数错误，字符串拼接过长
}
printf("拼接的字符串：\n",cmdbuff);
printf("%s\n",cmdbuff);
ret=ZAux_DirectCommand(handle,cmdbuff,cmdbuffAck,2048);
if(ERR_OK != ret)
{
return ret;
//printf("%s\n",cmdbuffAck);
//printf("%d\n",strlen(cmdbuffAck));
//
if(0 == strlen(cmdbuffAck))
{
return ERR_NOACK;
float ftempbuff[200];
int itempbuff[200];
ZAux_TransStringtoFloat(cmdbuffAck,iaxisNum*2,ftempbuff);//字符串转换为浮点数
//DPOS输出
for(i=0;i<iaxisNum;i++)
{
```

```c
//printf("%f\n",ftempbuff[i]);
fDposlist[i]=ftempbuff[i];
//MPOS输出
for(i=0;i<iaxisNum;i++)
//printf("%f\n",ftempbuff[i+iaxisNum]);
fMposlist[i]=ftempbuff[i+iaxisNum];
ZAux_TransStringtoInt(cmdbuffAck,iaxisNum*3+temp2+temp,itempbuff);//字符串转换为整
形
//AXISSTATUS输出
for(i=0;i<iaxisNum;i++)
//printf("%d\n",itempbuff[i+iaxisNum*2]);
iAxisstatuslist[i]=itempbuff[i+iaxisNum*2];
}
//IN输出
for(i=0;i<temp;i++)
{
//printf("%d\n",itempbuff[i+iaxisNum*3]);
iIn[i]=itempbuff[i+iaxisNum*3];
//OUT输出
for(i=0;i<temp2;i++)
{
//printf("%d\n",itempbuff[i+iaxisNum*3+temp]);
iOut[i]=itempbuff[i+iaxisNum*3+temp];
return ERR_OK;
}
int _tmain(int argc, _TCHAR* argv[])
{
char *ip_addr = (char *)"127.0.0.1"; //控制器IP地址
ZMC_HANDLE handle = NULL; //连接句柄
int ret = ZAux_OpenEth(ip_addr, &handle); //连接控制器
if (ERR_SUCCESS != ret)
{
printf("控制器连接失败!\n");
handle = NULL;
Sleep(2000);
return -1;
```

```c
printf("控制器连接成功！\n");
int axis[4]={0,1,2,4};
float d_dpos[4];
float d_mpos[4];
int32 d_axis_status[4];
int d_in[10];
int d_out[10];
ret
Demo_Direct_MyGetData(handle,3,axis,d_dpos,d_mpos,d_axis_status,0,32,d_in,0,33,d_out);
int i;
printf("获取到的轴命令位置：\n");
for (i=0;i<3;i++)
{
printf("\t轴%d :%f",i,d_dpos[i]);
printf("\n");
printf("获取到的轴反馈位置：\n");
for (i=0;i<3;i++)
printf("\t轴%d :%f",i,d_mpos[i]);
printf("\n");
printf("获取到的轴状态(按位对应)：\n");
for (i=0;i<3;i++)
printf("\t轴%d :%d",i,d_axis_status[i]);
}
printf("\n");
printf("获取到的输入口状态：\n");
int j=0;
int tempval;
for (i=0;i<=32;i++)
if (((i%32)==0)&&(i>0) )
{
j++;
}
//转换成位
tempval=d_in[j]>>(i-32*j);
printf(" IN(%d):%d",i,tempval &(0x01));
```

```c
if (((i%8)==0)&&(i>0) )
printf("\n");
}
}
printf("\n");
printf("获取到的输出口状态：\n");
j=0;
for (i=0;i<=33;i++)
{
if (((i%32)==0)&&(i>0) )
{
j++;
}
//转换成位
tempval=d_out[j]>>(i-32*j);
printf(" OUT(%d):%d",i,tempval &(0x01));
if (((i%8)==0)&&(i>0) )
{
printf("\n");
}
}
printf("\n");
Sleep(20000);
ret = ZAux_Close(handle); //关闭连接
commandCheckHandler("ZAux_Close", ret) ;//判断指令是否执行成功
printf("connection closed!\n");
handle = NULL;
return 0;
```

## 2.2.2-2 一行命令执行多条不同类型缓冲指令

一般点胶行业，木工行业用的居多，都是用连续轨迹，连续轨迹之间有插入缓冲输出，如果把运动跟连续轨迹的运动跟缓冲输出分开来发的话，难免会有局限性，可以通过自己单独封装运动函数，来达到一行命令执行多个函数的效果，如下举例：

假设控制一个 XY 两轴平台，从坐标点（0,0）—>（100,0）（输出口 0 输出 50ms）—>

（100,100）（输出口 0 输出 50ms）—>（0,100）（输出口 0 输出 50ms）—>（0,0）（输出口0 输出50ms）的轨迹，则可以通过自己封装，用一条函数，快速发送下去：

<table><tr><td>示波器 设置 水平刻度：100 连续采集 手动触发 显示编号 □ 间 □ 回 区 回 OP</td><td>启动 停 □ 1 MP0S(0] xyz模式 2MPOS1 3 OP[0] √跟随 斜视图 出 数据源 偏移 垂直刻度 MPOS 那 100 MPOS 印 100 印 [ 示波器 设置 启动 水平刻度： T模式 1000</td></tr></table>

Description: //我的自定义运动函数  
Input: //handle 卡链接  
iMoveLen 填写的运动长度  
iaxisNum 参与运动总轴数  
iaxislist 轴号列表  
fPoslist 距离列表  
iout 缓冲输出口  
outlist 缓冲输出列表(每条运动，决定是否输出，0为不输出，  
1为在运动后输出)  
outtime 缓冲输出时间  
Output: //  
Return: //错误码  
\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/  
int Demo\_Direct\_MyMoveABS(ZMC\_HANDLE handle,int iMoveLen,int iaxisNum, int\* iaxislist,  
float\* fPoslist,int iout,int \*outlist,int outtime)  
char cmdbuff[2048];  
char tempbuff[2048];  
char cmdbuffAck[20480];  
//若传进来的地址为空，则退出  
int ret=0;  
int i;  
//先读取剩余直线缓冲  
int iBuffLen = 0;  
ret = ZAux\_Direct\_GetRemain\_LineBuffer(handle,iaxislist[0],&iBuffLen);  
if(iBuffLen <= iMoveLen\*2)  
{  
return 1002; //运动缓冲不够  
//生成命令  
sprintf(cmdbuff, "BASE(");  
//拼接运动轴列表  
for (i=0;i<iaxisNum-1;i++)  
sprintf(tempbuff,"%d,",iaxislist[i]);//生成对应命令的字符串

```c
strcat(cmdbuff, tempbuff);//字符串拼接
if (strlen(cmdbuff)>1000)
{
return ERR_AUX_PARAERR; //参数错误，字符串拼接过长
sprintf(tempbuff,"%d)\n",iaxislist[i]);//生成对应命令的字符串
strcat(cmdbuff,tempbuff);
//拼接运动
for (i=0;i<iMoveLen;i++)
//printf("%d,%d\n",i*iaxisNum,i*iaxisNum+1);
if (outlist[i]==0) //该段运动不输出
{
strcat(cmdbuff, "MoveAbs(");
sprintf(tempbuff,"%f,%f)\n",fPoslist[i*iaxisNum],fPoslist[i*iaxisNum+1]);//生成对应
命令的字符串
strcat(cmdbuff, tempbuff);//字符串拼接
else if (outlist[i]==1)
strcat(cmdbuff, "MoveAbs(");
sprintf(tempbuff,"%f,%f)\n",fPoslist[i*iaxisNum],fPoslist[i*iaxisNum+1]);//生成对应
命令的字符串
strcat(cmdbuff, tempbuff);//字符串拼接
strcat(cmdbuff, "Move_op2(");
sprintf(tempbuff,"%d,%d,%d)\n",iout,1,outtime);//生成对应命令的字符串
strcat(cmdbuff, tempbuff);//字符串拼接
}
else
{
return ERR_AUX_PARAERR;//参数错误
}
```

```c
printf("拼接的字符串：\n");
printf("%s\n",cmdbuff);
if (strlen(cmdbuff)>1000)
{
return ERR_AUX_PARAERR; //参数错误，字符串拼接过长
ret=ZAux_DirectCommand(handle,cmdbuff,cmdbuffAck,2048);
return ret;
}
int _tmain(int argc, _TCHAR* argv[])
char *ip_addr = (char *)"127.0.0.1"; //控制器IP地址
ZMC_HANDLE handle = NULL; //连接句柄
int ret = ZAux_OpenEth(ip_addr, &handle); //连接控制器
if (ERR_SUCCESS != ret)
{
printf("控制器连接失败!\n");
handle = NULL;
Sleep(2000);
return -1;
printf("控制器连接成功！\n");
ret =ZAux_Direct_SetAtype(handle,0,1);//设置轴0轴类型为1
commandCheckHandler("ZAux_Direct_SetAtype", ret) ;//判断指令是否执行成功
ret =ZAux_Direct_SetAtype(handle,1,1);//设置轴1轴类型为1
commandCheckHandler("ZAux_Direct_SetAtype", ret) ;//判断指令是否执行成功
ret =ZAux_Direct_SetUnits(handle,0,100);//设置轴0脉冲当量为100
commandCheckHandler("ZAux_Direct_SetUnits", ret) ;//判断指令是否执行成功
ret =ZAux_Direct_SetUnits(handle,1,100);//设置轴1脉冲当量为100
commandCheckHandler("ZAux_Direct_SetUnits", ret) ;//判断指令是否执行成功
ret =ZAux_Direct_SetAccel(handle,0,500);//设置轴0加速度
commandCheckHandler("ZAux_Direct_SetAccel", ret) ;//判断指令是否执行成功
ret =ZAux_Direct_SetAccel(handle,1,500);//设置轴1加速度
commandCheckHandler("ZAux_Direct_SetAccel", ret) ;//判断指令是否执行成功
ret =ZAux_Direct_SetDecel(handle,0,500);//设置轴0减速度
commandCheckHandler("ZAux_Direct_SetDecel", ret) ;//判断指令是否执行成功
ret =ZAux_Direct_SetDecel(handle,1,500);//设置轴1减速度
```

commandCheckHandler("ZAux\_Direct\_SetDecel", ret) ;//判断指令是否执行成功   
ret =ZAux\_Direct\_SetDpos(handle,0,0);//设置轴0 DPOS清0   
commandCheckHandler("ZAux\_Direct\_SetDpos", ret) ;//判断指令是否执行成功   
ret =ZAux\_Direct\_SetDpos(handle,1,0);//设置轴1 DPOS清0   
commandCheckHandler("ZAux\_Direct\_SetDpos", ret) ;//判断指令是否执行成功   
ret =ZAux\_Direct\_SetSpeed(handle,0,100);//设置轴0速度   
commandCheckHandler("ZAux\_Direct\_SetDecel", ret) ;//判断指令是否执行成功   
ret =ZAux\_Direct\_SetSpeed(handle,1,100);//设置轴1速度   
commandCheckHandler("ZAux\_Direct\_SetDecel", ret) ;//判断指令是否执行成功   
ret =ZAux\_Direct\_SetMerge(handle,0,1);//设置开启连续插补（开启主轴的即可，如轴0，轴1   
插补，轴0为主轴，主轴号取决于连续插补运动指令轴列表的第一个轴号）   
int axis[2]={0,1};   
float POS[12]={0,0,0,100,100,100,100,0,0,0};   
int otlist[5]={0,1,1,1,1};   
ZAux\_Trigger(handle);//触发示波器   
ret = Demo\_Direct\_MyMoveABS(handle,5,2,axis,POS,0,otlist,50);//   
commandCheckHandler("Demo\_Direct\_MyMoveABS", ret) ;//判断指令是否执行成功   
Sleep(20000);   
ret = ZAux\_Close(handle); //关闭连接   
commandCheckHandler("ZAux\_Close", ret) ;//判断指令是否执行成功   
printf("connection closed!\n");   
handle = NULL;   
return 0;

## 第三章 指令一览表

注意：本手册中所有蓝色指令(如 ZAux\_OpenCom)均带有超链接，点击即可跳转到指定的指令说明。

第四章 板卡连接初始化
<table><tr><td rowspan=1 colspan=2>4.1控制器连接</td></tr><tr><td rowspan=1 colspan=1>ZAux OpenCom</td><td rowspan=1 colspan=1>串口连接控制器</td></tr><tr><td rowspan=1 colspan=1>ZAux SetComDefaultBaud</td><td rowspan=1 colspan=1>串口通讯参数设置</td></tr><tr><td rowspan=1 colspan=1>ZAux_OpenEth</td><td rowspan=1 colspan=1>以太网连接控制器</td></tr><tr><td rowspan=1 colspan=1>ZAux_OpenPci</td><td rowspan=1 colspan=1>PCI卡连接</td></tr><tr><td rowspan=1 colspan=1>ZAux_Close</td><td rowspan=1 colspan=1>关闭控制器连接</td></tr><tr><td rowspan=1 colspan=1>ZAux SetIp</td><td rowspan=1 colspan=1>设置控制IP</td></tr><tr><td rowspan=1 colspan=1> ZAux_SearchEthlist</td><td rowspan=1 colspan=1>搜索当前网段下控制器的 IP</td></tr><tr><td rowspan=1 colspan=1> ZAux_SearchAndOpenCom</td><td rowspan=1 colspan=1>快速与控制器建立连接</td></tr><tr><td rowspan=1 colspan=1>ZAux SearchEth</td><td rowspan=1 colspan=1>快速检索 IP 列表</td></tr><tr><td rowspan=1 colspan=1>ZAux_GetMaxPciCards</td><td rowspan=1 colspan=1>读取PCI的控制卡个数</td></tr><tr><td rowspan=1 colspan=1>ZAux_FastOpen</td><td rowspan=1 colspan=1>与控制器建立连接，指定连接的等待时间.MotionRT7用此方式连接</td></tr><tr><td rowspan=1 colspan=1> ZAux GetLocalRTState</td><td rowspan=1 colspan=1>获取RT 状态</td></tr><tr><td rowspan=1 colspan=1> ZAux LocalRTManage</td><td rowspan=1 colspan=1>启动停止RT</td></tr><tr><td rowspan=1 colspan=2>4.2 控制器信息获取</td></tr><tr><td rowspan=1 colspan=1>ZAux GetControllerInfo</td><td rowspan=1 colspan=1>获取控制器卡信息</td></tr><tr><td rowspan=1 colspan=1> ZAux_ GetSysSpecification</td><td rowspan=1 colspan=1>获取控制器最大规格数</td></tr><tr><td rowspan=1 colspan=1>ZAux GetRtcTime</td><td rowspan=1 colspan=1>读取控制器 RTC 时间</td></tr><tr><td rowspan=1 colspan=1>ZAux SetRtcTime</td><td rowspan=1 colspan=1>设置控制器 RTC 时间</td></tr></table>

第五章 运动控制参数设置
<table><tr><td colspan="2" rowspan="1">5.1 运动基本参数</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetAtype</td><td colspan="1" rowspan="1">设置轴类型</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_GetAtype</td><td colspan="1" rowspan="1">读取轴类型</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetUnits</td><td colspan="1" rowspan="1">设置脉冲当量（units）</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetUnits</td><td colspan="1" rowspan="1">读取脉冲当量（units）</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ SetAccel</td><td colspan="1" rowspan="1">设置加速度，单位为units /s/s</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetAccel</td><td colspan="1" rowspan="1">读取加速度，单位为units/s/s</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetDecel</td><td colspan="1" rowspan="1">设置减速度，单位为units/s/s</td></tr><tr><td colspan="1" rowspan="1">ZAux_ Direct_ GetDecel</td><td colspan="1" rowspan="1">读取减速度，单位为units/s/s</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetSpeed</td><td colspan="1" rowspan="1">设置轴速度，单位为units/s</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetSpeed</td><td colspan="1" rowspan="1">读取轴速度，单位为units/s</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetDpos</td><td colspan="1" rowspan="1">设置轴的规划位置，单位为units</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetDpos</td><td colspan="1" rowspan="1">读取轴的规划位置，单位为units</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetMpos</td><td colspan="1" rowspan="1">设置反馈位置，单位为units</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetMpos</td><td colspan="1" rowspan="1">读取反馈位置，单位为units</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetEncoder</td><td colspan="1" rowspan="1">获取控制器接收的脉冲数</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetFastDec</td><td colspan="1" rowspan="1">设置快速减速度，单位为units/s/s</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ GetFastDec</td><td colspan="1" rowspan="1">读取快速减速度，单位为units/s/s</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetLspeed</td><td colspan="1" rowspan="1">设置轴起始速度，单位为units/s</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetLspeed</td><td colspan="1" rowspan="1">读取轴起始速度，单位为units/s</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetSramp</td><td colspan="1" rowspan="1">设置S曲线设置。0-梯形加减速</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetSramp</td><td colspan="1" rowspan="1">读取S曲线设置。0-梯形加减速</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetMspeed</td><td colspan="1" rowspan="1">读取反馈速度，单位为units/s</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetVpSpeed</td><td colspan="1" rowspan="1">读取当前轴运行的命令速度,单位为units/s</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetVpMode</td><td colspan="1" rowspan="1">加减速曲线</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetIfldle</td><td colspan="1" rowspan="1">读取轴是否运动结束</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetAxisStatus</td><td colspan="1" rowspan="1">读取轴的告警状态</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ GetAxisStatus2</td><td colspan="1" rowspan="1">读取轴的使能状态</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetAxisStopReason</td><td colspan="1" rowspan="1">读取轴历史异常停止原因</td></tr><tr><td colspan="2" rowspan="1">5.2轴其他参数</td></tr><tr><td colspan="1" rowspan="1">ZAux GetModbusDpos</td><td colspan="1" rowspan="1">快速读取多个当前轴的 DPOS（modbus）</td></tr><tr><td colspan="1" rowspan="1"> ZAux_GetModbusMpos</td><td colspan="1" rowspan="1">快速读取多个当前轴的 MPOS（modbus）</td></tr><tr><td colspan="1" rowspan="1"> ZAux GetModbusCurSpeed</td><td colspan="1" rowspan="1">快速读取多个轴运动时的当前速度（modbus）</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetAxisAddress</td><td colspan="1" rowspan="1">设置轴地址</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ GetAxisAddress</td><td colspan="1" rowspan="1">读取轴地址</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetInvertStep</td><td colspan="1" rowspan="1">设置脉冲输出模式</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetInvertStep</td><td colspan="1" rowspan="1">读取脉冲输出模式</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_ SetMaxSpeed</td><td colspan="1" rowspan="1">设置脉冲输出最高频率</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ GetMaxSpeed</td><td colspan="1" rowspan="1">读取脉冲输出最高频率</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ GetMtype</td><td colspan="1" rowspan="1">读取当前正在运动指令类型</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetNtype</td><td colspan="1" rowspan="1">读取当前正在运动的下一条运动指令类型</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetOffpos</td><td colspan="1" rowspan="1">设置修改偏移相对位置</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ GetOffpos</td><td colspan="1" rowspan="1">读取修改偏移相对位置</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct Defpos</td><td colspan="1" rowspan="1">绝对位置偏移。定义DPOS，不建议使用</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_GetFe</td><td colspan="1" rowspan="1">读取随动误差</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetFeLimit</td><td colspan="1" rowspan="1">设置最大允许的随动误差值(预留)</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetFeLimit</td><td colspan="1" rowspan="1">读取最大允许的随动误差值(预留)</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetFeRange</td><td colspan="1" rowspan="1">设置报警时随动误差值(预留)</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetFeRange</td><td colspan="1" rowspan="1">获取报警时随动误差值(预留)</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct SetAxisMode</td><td colspan="1" rowspan="1">设置connect运动保持参数</td></tr><tr><td colspan="2" rowspan="1">5.3轴参数指令</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetAllAxisPara</td><td colspan="1" rowspan="1">浮点型读全部轴参数状态</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetAllAxisInfo</td><td colspan="1" rowspan="1">整型读全部轴参数状态</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ SetParam</td><td colspan="1" rowspan="1">通用的轴参数修改函数</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_GetParam</td><td colspan="1" rowspan="1">通用的轴参数读取函数</td></tr></table>

第六章 基本运动控制
<table><tr><td colspan="2" rowspan="1">6.1运动缓冲机制</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Direct GetEndMove</td><td colspan="1" rowspan="1">读取轴当前运动的最终位置</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Direct_GetEndMoveBuffer</td><td colspan="1" rowspan="1">读取当前和缓冲中运动的最终位置</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetLoaded</td><td colspan="1" rowspan="1">读取当前除了当前运动是否还有缓冲</td></tr><tr><td colspan="1" rowspan="1">ZAux_ Direct GetMovesBuffered</td><td colspan="1" rowspan="1">读取当前被缓冲起来的运动个数</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetMoveCurmark</td><td colspan="1" rowspan="1">读取当前正在运动指令的MOVE_MARK标号</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetMovemark</td><td colspan="1" rowspan="1">设置运动指令的MOVE_MARK标号</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetRemain</td><td colspan="1" rowspan="1">读取返回轴当前运动还未完成的距离</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetRemain LineBuffer</td><td colspan="1" rowspan="1">读取轴剩余的直线缓冲，按直线段来计算</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetRemain Buffer</td><td colspan="1" rowspan="1">参数轴剩余的缓冲，按最复杂的空间圆弧来计算</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_ GetVectorBuffered</td><td colspan="1" rowspan="1">读取返回轴当前运动和缓冲运动还未完成的距离</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Direct MoveOp</td><td colspan="1" rowspan="1">在运动缓冲中加入输出指令</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct MoveOpAdist</td><td colspan="1" rowspan="1">在运动缓冲中设置提前输出距离</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_MoveOpMulti</td><td colspan="1" rowspan="1">在运动缓冲中加入连续输出口输出指令</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Direct_MoveOpDelay</td><td colspan="1" rowspan="1">设置op口提前打开或关闭</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_MoveOp2</td><td colspan="1" rowspan="1">在运动缓冲中加入输出指令，指定时间后输出状态翻转</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct MoveAout</td><td colspan="1" rowspan="1">在运动缓冲中加入AOUT输出指令</td></tr><tr><td colspan="1" rowspan="1">ZAux_ Direct_MoveDelay</td><td colspan="1" rowspan="1">在运动缓冲中加入延时指令</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct MoveWait</td><td colspan="1" rowspan="1">BASE 轴运动缓冲加入一个可变的延时</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveTask</td><td colspan="1" rowspan="1">BASE 轴运动缓冲加入一个 TASK 任务</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MovePara</td><td colspan="1" rowspan="1">运动中修改参数</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MovePwm</td><td colspan="1" rowspan="1">运动中修改PWM</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveSynmove</td><td colspan="1" rowspan="1">运动中同步其他轴的运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_ MoveASynmove</td><td colspan="1" rowspan="1">运动中触发其他轴的运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveTable</td><td colspan="1" rowspan="1">运动中修改TABLE</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveCancel</td><td colspan="1" rowspan="1">运动中取消其他轴运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Direct_ MoveModify</td><td colspan="1" rowspan="1">运动中修改结束位置</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveLimit</td><td colspan="1" rowspan="1">在当前的运动末尾位置增加速度限制，用于强制拐角减速</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveModify2</td><td colspan="1" rowspan="1">运动中修改结束位置2</td></tr><tr><td colspan="1" rowspan="1"> ZAux Execute GetMoveTime</td><td colspan="1" rowspan="1">读取对应运动剩余时间</td></tr><tr><td colspan="1" rowspan="1"> ZAux Execute_ GetPreCalTime</td><td colspan="1" rowspan="1">预估点位运动时间，单位为 ms</td></tr><tr><td colspan="2" rowspan="1">6.2点位运动</td></tr><tr><td colspan="1" rowspan="1">ZAux_ Direct Single Move</td><td colspan="1" rowspan="1">单轴相对运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct Single_ MoveAbs</td><td colspan="1" rowspan="1">单轴绝对运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct Single Vmove</td><td colspan="1" rowspan="1">单轴连续运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetJogSpeed</td><td colspan="1" rowspan="1">设置JOG时速度</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetJogSpeed</td><td colspan="1" rowspan="1">读取JOG时速度</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetFastJog</td><td colspan="1" rowspan="1">读取 JOG 速度输入IN,有输入时,速度由 SPEED参数给出。如果没有输入，速度由 JOGSPEED 参数给出</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetFastJog</td><td colspan="1" rowspan="1">设置 JOG 速度输入IN,有输入时,速度由 SPEED参数给出。如果没有输入，速度由 JOGSPEED 参数给出</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetFwdJog</td><td colspan="1" rowspan="1">设置正向 JOG输入对应的输入编号-1无效。</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetFwdJog</td><td colspan="1" rowspan="1">读取正向 JOG 输入对应的输入编号-1无效。</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetRevJog</td><td colspan="1" rowspan="1">设置负向 JOG输入对应的输入编号-1无效。</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetRevJog</td><td colspan="1" rowspan="1">读取负向 JOG 输入对应的输入编号-1无效。</td></tr><tr><td colspan="1" rowspan="1"></td><td colspan="1" rowspan="1"></td></tr><tr><td colspan="2" rowspan="1">6.3回零运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct Single Datum</td><td colspan="1" rowspan="1">控制器方式回零</td></tr><tr><td colspan="1" rowspan="1"> ZAux_BusCmd_ Datum</td><td colspan="1" rowspan="1">总线驱动器回零</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ UserDatum</td><td colspan="1" rowspan="1">自定义二次回零</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetHomeWait</td><td colspan="1" rowspan="1">设置回零反找等待时间</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ GetHomeWait</td><td colspan="1" rowspan="1">读取回零反找等待时间</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd_SetDatumOffpos</td><td colspan="1" rowspan="1">设置回零偏移距离</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd _GetDatumOffpos</td><td colspan="1" rowspan="1">读取回零偏移距离</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetCreep</td><td colspan="1" rowspan="1">设置回零爬行速度</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetCreep</td><td colspan="1" rowspan="1">读取回零爬行速度</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetHomeStatus</td><td colspan="1" rowspan="1">回零完成状态</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetHomeStatus2</td><td colspan="1" rowspan="1">回零完成状态2</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd_GetHomeStatus</td><td colspan="1" rowspan="1">驱动器回零完成状态</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd GetHomeStatus2</td><td colspan="1" rowspan="1">驱动器回零完成状态2</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetDatumIn</td><td colspan="1" rowspan="1">设置回零原点信号</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ GetDatumIn</td><td colspan="1" rowspan="1">读取回零原点信号</td></tr><tr><td colspan="1" rowspan="1">6.4</td><td colspan="1" rowspan="1">电子齿轮</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetClutchRate</td><td colspan="1" rowspan="1">设置连接速率</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetClutchRate</td><td colspan="1" rowspan="1">获取连接速率</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct EncoderRatio</td><td colspan="1" rowspan="1">编码器输入齿轮比，缺省(1,1)</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct StepRatio</td><td colspan="1" rowspan="1">设置脉冲输出齿轮比，缺省(1,1)</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ Connect</td><td colspan="1" rowspan="1">连接 同步运动指令 电子齿轮</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct Connpath</td><td colspan="1" rowspan="1">连接 同步运动2指令 电子齿轮</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ GetLinkax</td><td colspan="1" rowspan="1">读取当前连接运动的参考轴号</td></tr><tr><td colspan="1" rowspan="1">6.5</td><td colspan="1" rowspan="1">连续插补</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct Move</td><td colspan="1" rowspan="1">多轴相对直线插补</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveSp</td><td colspan="1" rowspan="1">相对多轴直线插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveAbs</td><td colspan="1" rowspan="1">绝对多轴直线插补</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveAbsSp</td><td colspan="1" rowspan="1">绝对多轴直线插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct MoveCirc</td><td colspan="1" rowspan="1">相对圆心定圆弧插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveCircSp</td><td colspan="1" rowspan="1">相对圆心定圆弧插补运动 插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveCircAbs</td><td colspan="1" rowspan="1">绝对圆心圆弧插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ MoveCircAbsSp</td><td colspan="1" rowspan="1">绝对圆心圆弧插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveCirc2</td><td colspan="1" rowspan="1">相对3点定圆弧插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_MoveCirc2Abs</td><td colspan="1" rowspan="1">绝对3点定圆弧插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveCirc2Sp</td><td colspan="1" rowspan="1">相对3点定圆弧插补 SP运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ MoveCirc2AbsSp</td><td colspan="1" rowspan="1">绝对3点定圆弧插补 sp 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MHelical</td><td colspan="1" rowspan="1">相对3轴 圆心画螺旋插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MHelicalAbs</td><td colspan="1" rowspan="1">绝对3轴 圆心画螺旋插补运动</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct MHelicalSp</td><td colspan="1" rowspan="1">相对3轴 圆心画螺旋插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Direct_MHelicalAbsSp</td><td colspan="1" rowspan="1">绝对3轴 圆心画螺旋插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MHelical2</td><td colspan="1" rowspan="1">相对3轴3点画螺旋插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_ MHelical2Abs</td><td colspan="1" rowspan="1">绝对3轴3点画螺旋插补运动</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ MHelical2Sp</td><td colspan="1" rowspan="1">相对3轴3点画螺旋插补 SP运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MHelical2AbsSp</td><td colspan="1" rowspan="1">绝对3轴3点画螺旋插补 SP运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MEclipse</td><td colspan="1" rowspan="1">相对椭圆插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ MEclipseAbs</td><td colspan="1" rowspan="1">绝对椭圆插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MEclipseSp</td><td colspan="1" rowspan="1">相对椭圆插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ MEclipseAbsSp</td><td colspan="1" rowspan="1">绝对椭圆插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Direct_MEclipseHelical</td><td colspan="1" rowspan="1">相对椭圆＋螺旋插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct MEclipseHelicalAbs</td><td colspan="1" rowspan="1">绝对椭圆＋螺旋插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_ MEclipseHelicalSp</td><td colspan="1" rowspan="1">相对椭圆＋螺旋插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MEclipseHelicalAbsSp</td><td colspan="1" rowspan="1">绝对椭圆+螺旋插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MSpherical</td><td colspan="1" rowspan="1">空间圆弧 +螺旋插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MSphericalSp</td><td colspan="1" rowspan="1">空间圆弧 ＋螺旋插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct MSphericalABSSp</td><td colspan="1" rowspan="1">空间圆弧 ＋螺旋插补ABSSP 运动</td></tr><tr><td colspan="1" rowspan="1">ZAux_ Direct_MoveSpiral</td><td colspan="1" rowspan="1">渐开线圆弧插补运动，相对移动方式</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ MoveSpiralSp</td><td colspan="1" rowspan="1">渐开线圆弧插补 SP运动，相对移动方式</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct MoveSmooth</td><td colspan="1" rowspan="1">空间直线运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveSmoothSp</td><td colspan="1" rowspan="1">空间直线插补 SP 运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct MoveTurnabs</td><td colspan="1" rowspan="1">旋转台直线插补运动</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct McircTurnabs</td><td colspan="1" rowspan="1">旋转台圆弧+螺旋插补运动。</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_MultiMove</td><td colspan="1" rowspan="1">多条相对多轴直线插补</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_ MultiMoveAbs</td><td colspan="1" rowspan="1">多条绝对多轴直线插补</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_MultiLineN</td><td colspan="1" rowspan="1">多轴多段线直线连续插补</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetFhspeed</td><td colspan="1" rowspan="1">设置轴保持速度</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ GetFhspeed</td><td colspan="1" rowspan="1">读取轴保持速度</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetEndMoveSpeed</td><td colspan="1" rowspan="1">设置 SP 运动的结束速度</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_ GetEndMoveSpeed</td><td colspan="1" rowspan="1">读取 SP 运动结束速度</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetForceSpeed</td><td colspan="1" rowspan="1">设置 SP 运动的运行速度</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_ GetForceSpeed</td><td colspan="1" rowspan="1">读取 SP运动的运行速度</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetStartMoveSpeed</td><td colspan="1" rowspan="1">设置自定义速度的 SP运动的起始速度</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetStartMoveSpeed</td><td colspan="1" rowspan="1">读取自定义速度的 SP运动的起始速度</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetFholdIn</td><td colspan="1" rowspan="1">设置保持输入对应的输入点编号，-1无效。</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ GetFholdIn</td><td colspan="1" rowspan="1">读取保持输入对应的输入点编号，-1无效。</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetCornerMode</td><td colspan="1" rowspan="1">设置拐角减速模式</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetCornerMode</td><td colspan="1" rowspan="1">读取拐角减速模式</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetDecelAngle</td><td colspan="1" rowspan="1">设置拐角开始减速角度,单位为弧度</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetDecelAngle</td><td colspan="1" rowspan="1">读取拐角开始减速角度,单位为弧度</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct SetFullSpRadius</td><td colspan="1" rowspan="1">设置小圆限速最小半径</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetFullSpRadius</td><td colspan="1" rowspan="1">读取小圆限速最小半径</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetInterpFactor</td><td colspan="1" rowspan="1">设置插补时轴是否参与速度计算</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetInterpFactor</td><td colspan="1" rowspan="1">读取插补时轴是否参与速度计算</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetMerge</td><td colspan="1" rowspan="1">设置连续插补状态</td></tr><tr><td colspan="1" rowspan="1">ZAux_ Direct GetMerge</td><td colspan="1" rowspan="1">读取连续插补状态</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetStopAngle</td><td colspan="1" rowspan="1">设置减速到最低的最小拐角 弧度制</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetStopAngle</td><td colspan="1" rowspan="1">读取减速到最低的最小拐角 弧度制</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetZsmooth</td><td colspan="1" rowspan="1">设置 减速倒角半径</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetZsmooth</td><td colspan="1" rowspan="1">读取 减速倒角半径</td></tr></table>

6.6 轴暂停/恢复/停止相关
<table><tr><td rowspan=1 colspan=1> ZAux _Direct MovePause</td><td rowspan=1 colspan=1>运动暂停，插补运动暂停主轴</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct_ MoveResume</td><td rowspan=1 colspan=1>取消运动暂停</td></tr><tr><td rowspan=1 colspan=1>ZAux_ Direct Rapidstop</td><td rowspan=1 colspan=1>所有轴立即停止</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct_ CancelAxisList</td><td rowspan=1 colspan=1>多个轴运动停止</td></tr><tr><td rowspan=1 colspan=1>ZAux _Direct Single Cancel</td><td rowspan=1 colspan=1>单轴运动停止</td></tr></table>

<table><tr><td rowspan=1 colspan=2>第七章进阶运动控制</td></tr><tr><td rowspan=1 colspan=2>7.1 坐标循环</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct SetRepDist</td><td rowspan=1 colspan=1>设置循环范围</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct GetRepDist</td><td rowspan=1 colspan=1>读取设置循环范围</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct SetRepOption</td><td rowspan=1 colspan=1>设置坐标循环模式</td></tr><tr><td rowspan=1 colspan=1>ZAux _Direct_GetRepOption</td><td rowspan=1 colspan=1>读取坐标循环模式</td></tr><tr><td rowspan=1 colspan=2>7.2 螺距补偿</td></tr><tr><td rowspan=1 colspan=1>ZAux _Direct Pitchset</td><td rowspan=1 colspan=1>设置轴的螺距补偿，扩展轴无效</td></tr><tr><td rowspan=1 colspan=1> ZAux Direct Pitchset2</td><td rowspan=1 colspan=1>设置轴的螺距双向补偿，扩展轴无效。</td></tr><tr><td rowspan=1 colspan=1> ZAux_ Direct_ GetPitchStatus</td><td rowspan=1 colspan=1>设置轴的螺距补偿状态读取，扩展轴无效。</td></tr><tr><td rowspan=1 colspan=2>7.3反向补偿</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct Backlash</td><td rowspan=1 colspan=1>设置轴的反向间隙补偿，扩展轴无效</td></tr></table>

第八章 硬件接口访问与配置
<table><tr><td rowspan=1 colspan=2>8.1数字输入输出</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct GetIn</td><td rowspan=1 colspan=1>读取单个输入信号</td></tr><tr><td rowspan=1 colspan=1> ZAux Direct SetOp</td><td rowspan=1 colspan=1>设置单个输出口信号</td></tr><tr><td rowspan=1 colspan=1> ZAux_ Direct_GetOp</td><td rowspan=1 colspan=1>读取单个输出口状态</td></tr><tr><td rowspan=1 colspan=1>ZAux_GetModbusIn</td><td rowspan=1 colspan=1>快速读取多个输入(Modbus)</td></tr><tr><td rowspan=1 colspan=1> ZAux_GetModbusOut</td><td rowspan=1 colspan=1>快速读取多个当前的输出状态(Modbus)</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct GetInMulti</td><td rowspan=1 colspan=1>读取多个输入信号</td></tr><tr><td rowspan=1 colspan=1>ZAux _Direct SetOutMulti</td><td rowspan=1 colspan=1>IO 接口设置多路输出</td></tr><tr><td rowspan=1 colspan=1>ZAux_Direct_ GetOutMulti</td><td rowspan=1 colspan=1>IO 接口 获取多路输出状态</td></tr><tr><td rowspan=1 colspan=1> ZAux Direct SetInvertIn</td><td rowspan=1 colspan=1>设置输入口有效状态反转</td></tr><tr><td rowspan=1 colspan=1>ZAux _Direct_ GetInvertIn</td><td rowspan=1 colspan=1>读取输入口反转状态</td></tr><tr><td rowspan=1 colspan=2>8.2 模拟量输入输出</td></tr><tr><td rowspan=1 colspan=1> ZAux Direct GetAD</td><td rowspan=1 colspan=1>读取单个模拟量输入信号</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct SetDA</td><td rowspan=1 colspan=1>打开单个模拟量输出信号</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct GetDA</td><td rowspan=1 colspan=1>读取模拟输出口状态</td></tr><tr><td rowspan=1 colspan=2>8.3PWM控制</td></tr><tr><td rowspan=1 colspan=1> ZAux Direct SetPwmFreq</td><td rowspan=1 colspan=1>设置pwm频率</td></tr><tr><td rowspan=1 colspan=1>ZAux_Direct_GetPwmFreq</td><td rowspan=1 colspan=1>读取pwm频率</td></tr><tr><td rowspan=1 colspan=1> ZAux_Direct_SetPwmDuty</td><td rowspan=1 colspan=1>设置pwm 占空比</td></tr><tr><td rowspan=1 colspan=1> ZAux Direct GetPwmDuty</td><td rowspan=1 colspan=1>读取pwm 占空比</td></tr></table>

第九章 安全机制
<table><tr><td colspan="2" rowspan="1"></td></tr><tr><td colspan="2" rowspan="1">9.1 轴软件限位函数</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetFsLimit</td><td colspan="1" rowspan="1">设置正向软限位</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetFsLimit</td><td colspan="1" rowspan="1">读取正向软限位</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetRsLimit</td><td colspan="1" rowspan="1">设置负向软限位位置</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct GetRsLimit</td><td colspan="1" rowspan="1">读取负向软限位位置</td></tr><tr><td colspan="2" rowspan="1">9.2轴硬件限位函数</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetFwdIn</td><td colspan="1" rowspan="1">设置正向硬限位开关对应的输入点编号</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetFwdIn</td><td colspan="1" rowspan="1">读取正向硬限位开关对应的输入点编号</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetRevIn</td><td colspan="1" rowspan="1">设置负向硬件限位开关对应的输入点编号</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetRevIn</td><td colspan="1" rowspan="1">读取负向硬件限位开关对应的输入点编号</td></tr><tr><td colspan="2" rowspan="1">9.3轴报警函数</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetAlmIn</td><td colspan="1" rowspan="1">设置轴告警开关对应的输入点编号</td></tr><tr><td colspan="2" rowspan="1">第十章 板卡数据交互</td></tr><tr><td colspan="2" rowspan="1">10.1 VR 寄存器</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetVrf</td><td colspan="1" rowspan="1">设置VR寄存器</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ GetVrf</td><td colspan="1" rowspan="1">读取VR寄存器</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Direct_GetVrInt</td><td colspan="1" rowspan="1">整型方式读取VR寄存器</td></tr><tr><td colspan="2" rowspan="1">10.2 Table寄存器</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetTable</td><td colspan="1" rowspan="1">设置系统table寄存器</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetTable</td><td colspan="1" rowspan="1">读取系统 table寄存器</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetTableDouble</td><td colspan="1" rowspan="1">写入 Table</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetTableDouble</td><td colspan="1" rowspan="1">读取 Table，可以一次读取多个</td></tr><tr><td colspan="1" rowspan="1">10.3 Mo</td><td colspan="1" rowspan="1">dbus寄存器</td></tr><tr><td colspan="1" rowspan="1">ZAux Modbus_Set0x</td><td colspan="1" rowspan="1">设置 modbus 位寄存器</td></tr><tr><td colspan="1" rowspan="1">ZAux Modbus_Get0x</td><td colspan="1" rowspan="1">读取 modbus 位寄存器</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Modbus_Set4x</td><td colspan="1" rowspan="1">设置 modbus 寄存器（REG）</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Modbus_Get4x</td><td colspan="1" rowspan="1">读取 modbus 寄存器（REG）</td></tr><tr><td colspan="1" rowspan="1">ZAux Modbus_Set4x Long</td><td colspan="1" rowspan="1">Modbus4x 寄存器写操作(LONG)</td></tr><tr><td colspan="1" rowspan="1">ZAux_Modbus_ Get4x Long</td><td colspan="1" rowspan="1">Modbus4x 寄存器读操作(LONG)</td></tr><tr><td colspan="1" rowspan="1">ZAux Modbus_Set4x String</td><td colspan="1" rowspan="1">Modbus 字寄存器操作（ASCII）</td></tr><tr><td colspan="1" rowspan="1">ZAux Modbus_Get4x String</td><td colspan="1" rowspan="1">Modbus 字寄存器操作（ASCII）</td></tr><tr><td colspan="1" rowspan="1">ZAux Modbus Set4x Float</td><td colspan="1" rowspan="1">设置 modbus 寄存器(float类型)</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Modbus_Get4x Float</td><td colspan="1" rowspan="1">读取 modbus 寄存器(float 类型)</td></tr><tr><td colspan="2" rowspan="1">10.4 Flash/文件读写</td></tr><tr><td colspan="1" rowspan="1">ZAux WriteUFile</td><td colspan="1" rowspan="1">向U盘中写文件</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ReadUFile</td><td colspan="1" rowspan="1">从U盘中读取文件</td></tr><tr><td colspan="1" rowspan="1"> ZAux FlashWritef</td><td colspan="1" rowspan="1">写控制器 FLASH空间</td></tr><tr><td colspan="1" rowspan="1"> ZAux_FlashReadf</td><td colspan="1" rowspan="1">读控制器 FLASH空间</td></tr><tr><td colspan="1" rowspan="1"> ZAux_DownMem3FileRam</td><td colspan="1" rowspan="1">打包字符串运动下发</td></tr><tr><td colspan="1" rowspan="1">ZAux_Run3FileRam</td><td colspan="1" rowspan="1">运行下发的3次文件</td></tr><tr><td colspan="1" rowspan="1">ZAux_Down3FileRam</td><td colspan="1" rowspan="1">下载3次文件到RAM</td></tr><tr><td colspan="1" rowspan="1"> ZAux 3FileRamGetRemainSpace</td><td colspan="1" rowspan="1">动态下载3次文件，读取可写的空间</td></tr><tr><td colspan="1" rowspan="1"> ZAux 3FileRamDownBegin</td><td colspan="1" rowspan="1">动态下载3次文件，启动下载</td></tr><tr><td colspan="1" rowspan="1"> ZAux 3FileRamDownPart</td><td colspan="1" rowspan="1">动态下载3次文件</td></tr><tr><td colspan="1" rowspan="1"> ZAux 3FileRamDownEnd</td><td colspan="1" rowspan="1">动态下载3次文件，结束下载</td></tr></table>

第十一章 总线初始化
<table><tr><td colspan="2" rowspan="1">11.1 总线初始化</td></tr><tr><td colspan="1" rowspan="1">ZAux BusCmd_GetNodeNum</td><td colspan="1" rowspan="1">读取卡槽上节点数量</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd GetNodeInfo</td><td colspan="1" rowspan="1">读取节点上的信息</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd GetNodeStatus</td><td colspan="1" rowspan="1">读取节点总线状态</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd DriveClear</td><td colspan="1" rowspan="1">设置清除总线伺服报警</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ SetErrormask</td><td colspan="1" rowspan="1">设置错误标记</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetErrormask</td><td colspan="1" rowspan="1">读取错误标记</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetAxisEnable</td><td colspan="1" rowspan="1">设置轴使能，只针对总线控制器轴使用有效</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct_ GetAxisEnable</td><td colspan="1" rowspan="1">读取轴使能，只针对总线控制器轴使用有效</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetBusAxisEnable</td><td colspan="1" rowspan="1">读取总线轴使能状态</td></tr><tr><td colspan="2" rowspan="1">11.2 SDO/PDO 读写</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd SDOWrite</td><td colspan="1" rowspan="1">通过设备号和槽位号进行SDO 写入</td></tr><tr><td colspan="1" rowspan="1">ZAux_BusCmd_SDORead</td><td colspan="1" rowspan="1">通过设备号和槽位号进行 SDO 读取</td></tr><tr><td colspan="1" rowspan="1"> ZAux _BusCmd SDOWrite2</td><td colspan="1" rowspan="1">写入节点 SDO 参数信息</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd SDORead2</td><td colspan="1" rowspan="1">读取节点 SDO 参数信息</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd RtexRead</td><td colspan="1" rowspan="1">Rtex 读取参数信息</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd RtexWrite</td><td colspan="1" rowspan="1">Rtex 写参数信息</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd SDOReadAxis</td><td colspan="1" rowspan="1">读轴 SDO参数信息</td></tr><tr><td colspan="1" rowspan="1">ZAux BusCmd SDOWriteAxis</td><td colspan="1" rowspan="1">写轴 SDO 参数信息</td></tr><tr><td colspan="1" rowspan="1">ZAux BusCmd NodePdoWrite</td><td colspan="1" rowspan="1">Pdo 写操作</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd NodePdoRead</td><td colspan="1" rowspan="1">Pdo读操作</td></tr><tr><td colspan="1" rowspan="1"> ZAux_BusCmd_NodePdoWriteFloat</td><td colspan="1" rowspan="1">Pdo写操作</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ BusCmd NodePdoReadFloat</td><td colspan="1" rowspan="1">Pdo读操作</td></tr><tr><td colspan="2" rowspan="1">11.3总线转矩力矩</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ BusCmd GetDriveTorque</td><td colspan="1" rowspan="1">读取当前总线驱动当前力矩 需要设置对应的DRIVE_PROFILE 类型</td></tr><tr><td colspan="1" rowspan="1"> ZAux_BusCmd_SetMaxDriveTorque</td><td colspan="1" rowspan="1">设置当前总线驱动最大转矩 需要设置对应的DRIVE_PROFILE 类型</td></tr><tr><td colspan="1" rowspan="1"> ZAux BusCmd_GetMaxDriveTorque</td><td colspan="1" rowspan="1">读取当前总线驱动最大转矩 需要设置对应的DRIVE_PROFILE 类型</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct SetDAC</td><td colspan="1" rowspan="1">设置模拟量输出 力矩、速度模式下可以 总线驱动需要设置对应 DRIVE_PROFILE 类型与ATYPE</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetDAC</td><td colspan="1" rowspan="1">读取模拟量输出 力矩、速度模式下可以 总线驱</td></tr><tr><td></td><td>动需要设置对应 DRIVE_PROFILE 类型与</td></tr><tr><td></td><td>ATYPE</td></tr></table>

<table><tr><td colspan="2" rowspan="1">第十  二章其他指令</td></tr><tr><td colspan="2" rowspan="1">12.1 在线命令</td></tr><tr><td colspan="1" rowspan="1">ZAux_Execute</td><td colspan="1" rowspan="1">发送字符串（Zbasic）命令(缓冲方式)</td></tr><tr><td colspan="1" rowspan="1"> ZAux DirectCommand</td><td colspan="1" rowspan="1">发送字符串(Zbasic）命令(直接方式)</td></tr><tr><td colspan="2" rowspan="1">12.2控制器 Basic 相关</td></tr><tr><td colspan="1" rowspan="1">ZAux Pause</td><td colspan="1" rowspan="1">暂停控制器内部运行的BASIC 程序。</td></tr><tr><td colspan="1" rowspan="1">ZAux Resume</td><td colspan="1" rowspan="1">恢复控制器内部运行的暂停的 BASIC 程序。</td></tr><tr><td colspan="1" rowspan="1"> ZAux_BasDown</td><td colspan="1" rowspan="1">单 BAS 文件生成 ZAR 并且下载到控制器运行</td></tr><tr><td colspan="1" rowspan="1">ZAux_ZarDown</td><td colspan="1" rowspan="1">下载 ZAR 文件到控制器</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_ GetVariablef</td><td colspan="1" rowspan="1">全局变量读取，也可以是轴参数等等，返回值为浮点型</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Direct_ GetVariableInt</td><td colspan="1" rowspan="1">全局变量读取，也可以是轴参数等等，返回值为整型</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetUserArray</td><td colspan="1" rowspan="1">设置 BASIC 自定义全局数组</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct_ GetUserArray</td><td colspan="1" rowspan="1">读取设置 BASIC 自定义全局数组 可以一次读取多个</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetUserVar</td><td colspan="1" rowspan="1">设置自定义全局变量</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ Direct GetUserVar</td><td colspan="1" rowspan="1">读取自定义全局变量</td></tr><tr><td colspan="1" rowspan="1"> ZAux SetTimeOut</td><td colspan="1" rowspan="1">命令的延时等待时间</td></tr><tr><td colspan="1" rowspan="1"> ZAux TransStringtoFloat</td><td colspan="1" rowspan="1">字符串转为float</td></tr><tr><td colspan="1" rowspan="1"> ZAux TransStringtoInt</td><td colspan="1" rowspan="1">字符串转为int</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ TransStringtoDouble</td><td colspan="1" rowspan="1">字符串转为 double</td></tr><tr><td colspan="2" rowspan="1">12.3控制器自动上报</td></tr><tr><td colspan="1" rowspan="1">(*PZMCAutoUpCallBack)</td><td colspan="1" rowspan="1">主动上报的回调函数格式</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ SetAutoUpCallBack</td><td colspan="1" rowspan="1">控制器自动上报</td></tr><tr><td colspan="1" rowspan="1"> ZAux _CycleUpEnable</td><td colspan="1" rowspan="1">使能周期上报</td></tr><tr><td colspan="1" rowspan="1"> ZAux_CycleUpDisable</td><td colspan="1" rowspan="1">去使能周期上报</td></tr><tr><td colspan="1" rowspan="1"> ZAux CycleUpGetRecvTimes</td><td colspan="1" rowspan="1">周期上报收到的包次数，超过溢出，调试使用.</td></tr><tr><td colspan="1" rowspan="1"> ZAux_CycleUpForceOnce</td><td colspan="1" rowspan="1">强制上报一次,在运动指令后idle可能不准确的情况下调用</td></tr><tr><td colspan="1" rowspan="1"> ZAux_CycleUpReadBuff</td><td colspan="1" rowspan="1">从周期上报里面读取内容(浮点型)</td></tr><tr><td colspan="1" rowspan="1"> ZAux_CycleUpReadBuffInt</td><td colspan="1" rowspan="1">从周期上报里面读取内容(整型)</td></tr><tr><td colspan="2" rowspan="1">12.4调试相关</td></tr><tr><td colspan="1" rowspan="1">ZAux SetTraceFile</td><td colspan="1" rowspan="1">命令跟踪设置</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Trigger</td><td colspan="1" rowspan="1">示波器触发函数</td></tr><tr><td colspan="1" rowspan="1"> ZAux_Scope</td><td colspan="1" rowspan="1">示波器采样</td></tr><tr><td colspan="1" rowspan="1"> ZAux_ GetScopeLength</td><td colspan="1" rowspan="1">获取示波器采样长度</td></tr></table>

## 第四章 板卡连接初始化

## 4.1 控制器连接

## 指令列表

<table><tr><td rowspan=1 colspan=2>指令                                                  说明</td></tr><tr><td rowspan=1 colspan=1>ZAux_OpenCom</td><td rowspan=1 colspan=1>串口连接控制器</td></tr><tr><td rowspan=1 colspan=1>ZAux_SetComDefaultBaud</td><td rowspan=1 colspan=1>串口通讯参数设置</td></tr><tr><td rowspan=1 colspan=1>ZAux_OpenEth</td><td rowspan=1 colspan=1>以太网连接控制器</td></tr><tr><td rowspan=1 colspan=1>ZAux_OpenPci</td><td rowspan=1 colspan=1>PCI卡连接</td></tr><tr><td rowspan=1 colspan=1>ZAux_Close</td><td rowspan=1 colspan=1>关闭控制器连接</td></tr><tr><td rowspan=1 colspan=1>ZAux SetIp</td><td rowspan=1 colspan=1>设置控制IP</td></tr><tr><td rowspan=1 colspan=1>ZAux SearchEthlist</td><td rowspan=1 colspan=1>搜索当前网段下控制器的 IP</td></tr><tr><td rowspan=1 colspan=1>ZAux SearchAndOpenCom</td><td rowspan=1 colspan=1>快速与控制器建立连接</td></tr><tr><td rowspan=1 colspan=1>ZAux SearchEth</td><td rowspan=1 colspan=1>快速检索IP 列表</td></tr><tr><td rowspan=1 colspan=1>ZAux GetMaxPciCards</td><td rowspan=1 colspan=1>读取 PCI的控制卡个数</td></tr><tr><td rowspan=1 colspan=1>ZAux_FastOpen</td><td rowspan=1 colspan=1>与控制器建立连接，指定连接的等待时间.MotionRT7用此方式连接</td></tr><tr><td rowspan=1 colspan=1>ZAux GetLocalRTState</td><td rowspan=1 colspan=1>获取RT状态</td></tr><tr><td rowspan=1 colspan=1> ZAux_LocalRTManage</td><td rowspan=1 colspan=1>启动停止RT</td></tr></table>

## 重点说明

## 4.1.2-1 常规说明

串口

缺省情况下，控制器串口默认的参数如下：

<table><tr><td rowspan=1 colspan=1>波特率</td><td rowspan=1 colspan=1>数据位</td><td rowspan=1 colspan=1>停止位</td><td rowspan=1 colspan=1>校验位</td></tr><tr><td rowspan=1 colspan=1>38400</td><td rowspan=1 colspan=1>8</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1>无</td></tr></table>

注意：电脑上面的串口参数需要和控制器上面的串口参数保持一致才能连接上。

![](images/3e9a8f3c0dab54a2b0b5e7afbd3aadb48ef3da64d53b2bcd14cb208e667350d6.jpg)

站号：串口连接使用 485的情况，可能会有多个从站，通过站号来选择,一般情况下填 0就行。

## Ethernet 网口

网口采用RJ45标准网线接口，通讯速率为 100Mbit/s。控制器自带一个网口，但本身至少是支持2个网口通道的，可采用交换机扩展网口，?\*port打印通讯通道。

控制器出厂的IP地址为192.168.0.11，端口号为 502，通讯时保证设备处于同一网段（前三段相同），IP 地址支持修改，网口常用于连接电脑或触摸屏，电脑连接控制器一般使用网口，如果忘记了控制器的 IP，可以使用串口连接上去查看或是修改。

网口连接的能力与性能：  
![](images/575134f217307beb00481d19159223427b51242935523fb7f734bc7caa567659.jpg)

## 网线推荐

<table><tr><td colspan="1" rowspan="1">网线类型</td><td colspan="1" rowspan="1">传输频率</td><td colspan="1" rowspan="1">传输速率</td><td colspan="1" rowspan="1">传输距离</td><td colspan="1" rowspan="1">线缆类型</td></tr><tr><td colspan="1" rowspan="1">五类线</td><td colspan="1" rowspan="1">100Mhz</td><td colspan="1" rowspan="1">100Mbps</td><td colspan="1" rowspan="1">100米</td><td colspan="1" rowspan="1">屏蔽/非屏蔽</td></tr><tr><td colspan="1" rowspan="1">超五类线</td><td colspan="1" rowspan="1">100Mhz</td><td colspan="1" rowspan="1">1000Mbps</td><td colspan="1" rowspan="1">100米</td><td colspan="1" rowspan="1">屏蔽/非屏蔽</td></tr><tr><td colspan="1" rowspan="1">六类线</td><td colspan="1" rowspan="1">250Mhz</td><td colspan="1" rowspan="1">1Gbps</td><td colspan="1" rowspan="1">100米</td><td colspan="1" rowspan="1">屏蔽/非屏蔽</td></tr><tr><td colspan="1" rowspan="1">超六类线</td><td colspan="1" rowspan="1">500Mhz</td><td colspan="1" rowspan="1">10Gbps</td><td colspan="1" rowspan="1">100米</td><td colspan="1" rowspan="1">屏蔽/非屏蔽</td></tr><tr><td colspan="1" rowspan="1">七类线</td><td colspan="1" rowspan="1">600Mhz</td><td colspan="1" rowspan="1">10Gbps</td><td colspan="1" rowspan="1">100米</td><td colspan="1" rowspan="1">双屏蔽</td></tr><tr><td colspan="1" rowspan="1">八类线</td><td colspan="1" rowspan="1">2000Mhz</td><td colspan="1" rowspan="1">40Gbps</td><td colspan="1" rowspan="1">30米</td><td colspan="1" rowspan="1">双屏蔽</td></tr></table>

为保证通讯质量，在干扰较大的场合，最好采用七类以上的金属头双屏蔽层的网线，正常场合，使用超五类带屏蔽金属头的网线即可。如右图：网线带金属线的，可以起到接地保护作用。如果不采用金属头带屏蔽层的网线，在实际有干扰的现场，会出现丢包等不确定的事情。

![](images/57b136272b61c416f0ac4a40e165d393d3d3acb75cb98514337f7b733d646023.jpg)

检测函数返回值判断函数是否执行成功，返回0 时则成功，非 0 失败。以太网口连接返回不成功时，检查传递的 IP 是否正确，并且保证电脑 IP 地址和控制器IP 地址在同一个网段。

![](images/c86c21544851a1f6b900063ba8266968464bbf1b93a8e1a1287915bd120942d8.jpg)  
连接等待时间：网口连接有效。响应时间较短的话，可适当增大该值，延长响应时间。

起始 IP，结束 IP：根据自己设置的控制 IP 的网段填写对应的网段。

站号：串口连接使用 485的情况，可能会有多个从站，通过站号来选择。

PCI 卡卡号：PCI 类型的控制卡插槽卡号(需安装驱动方可扫出)。

## 4.1.2-2 MotionRT7

MotionRT7 建议配合 XPCI/XPCIE 控制卡使用，能更大发挥性能，授权信息存储在控制卡上，每个控制卡具有唯一编号。

安装前准备好最新版本的 MotionRT7 安装包并解压。（MotionRT7 软件安装包获取路径可前往正运动技术官网自行下载：http://www.zmotion.com.cn/），下图为 MotionRT7 安装包解压后的文件内容。

![](images/e0a4ae760bbfada52f4061cdc580dfb959d682b381a396f8ee01b50d230780a4.jpg)

1. Driver\_signed：驱动文件夹，里面包含了 MotionRT7 驱动安装信息文件、ECAT 协议安装信息、安装向导软件，可能还会有安全目录文件、sys系统文件和签名文件。

<table><tr><td rowspan=1 colspan=4></td></tr><tr><td rowspan=2 colspan=1>&gt;名称</td><td></td><td></td><td></td></tr><tr><td rowspan=1 colspan=1>修改日期</td><td rowspan=1 colspan=1>类型</td><td rowspan=1 colspan=1>大小</td></tr><tr><td rowspan=1 colspan=1> dpinst_amd64.exe</td><td rowspan=1 colspan=1>2022/9/6 11:21</td><td rowspan=1 colspan=1>应用程序</td><td rowspan=1 colspan=1>1,026 KB</td></tr><tr><td rowspan=1 colspan=1>国 ZMotionRt64.cat</td><td rowspan=1 colspan=1>2022/10/30 23:47</td><td rowspan=1 colspan=1>安全目录</td><td rowspan=1 colspan=1>13 KB</td></tr><tr><td rowspan=1 colspan=1>ZMotionRt64.inf</td><td rowspan=1 colspan=1>2022/10/18 19:06</td><td rowspan=1 colspan=1>安装信息</td><td rowspan=1 colspan=1>4 KB</td></tr><tr><td rowspan=1 colspan=1>® ZMotionRt64.sys</td><td rowspan=1 colspan=1>2022/10/30 23:47</td><td rowspan=1 colspan=1>系统文件</td><td rowspan=1 colspan=1>4,966 KB</td></tr><tr><td rowspan=1 colspan=1> ZMotionRtPacket.inf</td><td rowspan=1 colspan=1>2022/10/18 19:06</td><td rowspan=1 colspan=1>安装信息</td><td rowspan=1 colspan=1>2 KB</td></tr></table>

Dpinst\_amd64.exe：安装向导软件；

ZMotionRT64.cat：驱动程序的数字签名文件；

ZMotionRT64.inf：MotionRT7 软件驱动安装信息，安装驱动时浏览文件夹选择该文件；  
ZMotionRT64.sys：系统文件；

ZMotionRTPacket.inf：总线协议安装信息，安装EACT总线协议浏览文件夹选择该文件；2. flash：控制器系统文件夹。

3. Udisk：U 盘文件夹。

4. Xplcterm：xplc 屏幕文件夹，内包含 xplcterm 软件，在使用 HMI 时作为屏幕显示。

## MotionRT7 驱动安装

获取到 MotionRT7 的安装包之后，自行把安装包解压到自己选择的位置，如下图：

![](images/30c58a0a3b466cc210f366c67aca32ec3d479cf0f30b331fc7e25bbedc58414e.jpg)

## [无卡操作]

1. 无 XPCI/XPCIE 卡设备时，在设备管理器中，菜单：“操作”-“添加过时硬件”，如下图。若没有“操作”，就鼠标右击，找到“添加过时硬件”：

![](images/d4dc579ada94587419df72afda3669c9ed79fd323516b41690c6e1ec63f42b6e.jpg)  
2. 点击“添加过时硬件”后，弹出添加硬件向导界面，单击“下一步”

![](images/a070fe49dd17fe899bfd847e54e89050d5e649223bd894f827bce2446e4f5b2e.jpg)  
3. 继第2步操作，单击“下一步”之后，弹出以下窗口，选择“安装我手动从列表选择的硬件”后单击下一步。

添加硬件

这个向导可以帮助你安装其他硬件

这个向导可以搜索其他硬件并为你自动安装。或者，如果你知道要安装哪个型号的硬件，你可以从列表选择。

你想向导做什么？

○搜索并自动安装硬件(推荐)(S)

安装我手动从列表选择的硬件（高级)(M)

< 上一步(B)

下一步(N) >

取消

4. 继第3步操作，单击“下一步”之后，弹出以下窗口，常见硬件类型选择“显示所有设备”后单击下一步。

添加硬件

从以下列表，选择要安装的硬件类型

如果看不到想要的硬件类型，请单击"显示所有设备”。

常见硬件类型(H):

显示所有设备

0 Custom USB Devices

Hikvision USB3 Vision Cameras

IDEATA/ATAPI控制器

IEEE1284.4兼容打印机

IEEE1284.4设备

IEEE1394主控制器

IEEE1667接收器和控制设备

Media Center Extenders

<上一步(B)

下一步(N) >

取消

5. 继第4步操作，单击“下一步”之后，弹出以下窗口，直接点击“从磁盘安装”。

添加硬件

选择要为此硬件安装的设备驱动程序

![](images/ecf5a9075f4fbf7610c22ef9d4b209be8f15f69187738a5349e6115b5f81ffa4.jpg)

请选定硬件设备的厂商和型号，然后单击"下一步”。如果手头有包含要安装的驱动程序的磁盘，请单击”从磁盘安装。

![](images/6781750991483746cf0800b0f4367d0a7dc88791d562630106401c0e71e241b2.jpg)  
6. 继第5步操作，点击“从磁盘安装”之后，弹出以下窗口：

（1）点击“浏览”；

（2）打开 MotionRT7 压缩包下面的：MotionRT7\driver\_signed\ZMotionRt64.inf 文件路径；

（3）打开 ZMotionRt64.inf 文件；

（4）点击确定。

![](images/a938a7ddd66c1aed2be842bb02f7571934b75ea17d14437c7de8668d2ef49c7f.jpg)  
7. 继第 6 步操作，点击“确定”之后，弹出以下窗口，直接点击“下一步”；

![](images/addac9f68706be0dcc7ec78068b0fba5817fb96478822247be0a99ecc3eb5bff.jpg)

8. 继第7步操作，点击“下一步”之后，弹出以下窗口，直接点击“下一步”；

![](images/266fd8146ac8cbf5e60d561e17b480f9779b934a35b9056261ff34eb7457e2a0.jpg)

9. 最后一步，步骤8 点击“下一步”之后，弹出以下窗口，直接点击 “完成”，即可。设备管理器也会多出 ZMotion\_Device 的设备。

![](images/48abdfc168a6a8e6c1bf56c49e85b2f618e5dc2cb3bf037618431b9b1eda87b4.jpg)

## [有卡操作]

有PCI设备时，有两种方法。

方法一：向导软件安装

打开 MotionRT7 压缩包下面的：MotionRT7 / driver\_signed 目录中自带的安装向导软件dpinst\_amd64.exe 自动安装，具体操作按软件向导操作。

![](images/4f0172d71b82f47fa3d3dd1c82ae5d2634ff799e585f7cdb4b2877505e8e2443.jpg)

## 方法二：手动安装

1. 打开设备管理器菜单选择其他设备中的 PCI设备

![](images/a062ba19600388fbee0a69b760afbaa188f614ae234e56fb7a08da4333914bc5.jpg)

2. 若存在多个 PCI 设备时，鼠标右击“属性”查看详细信息，属性选择“硬件 ID”，确认为 PCI\VEN\_EF34&DEV\_1000&开头的 PCI 设备。

![](images/e62e9b150e05691530bc0f75e2a673ebd4794eda3536af951a18ac7547d8cccc.jpg)

3. 找到PCI设备，右击选择“更新驱动程序”。

计算机管理

![](images/32307338d77af6d5655cfc52a0a9da2b1401ca6e708a6f856a3b5ef33e99f6b5.jpg)

## 4. 选择“浏览我的电脑以查找驱动程序”。

文件（F）操作（A）查看（V）帮助（H）

![](images/a0d734b53fff57979eb66d65e32e0f91352d52eddd1e6c343e93a53ec4f72e1b.jpg)  
5. 点击浏览，选择驱动 driver 文件夹。

← 更新驱动程序-PCI设备

浏览计算机上的驱动程序

在以下位置搜索驱动程序：

D:\rt1118driver

浏览（R）.

包括子文件夹（1）

→让我从计算机上的可用驱动程序列表中选取（L）此列表将显示与该设备兼容的可用驱动程序，以及与该设备属于同一类别的所有驱动程

下一步（N)

取消

## 6. 点击下一步。

![](images/324b7c29048cfc7be44458ad4d7f276241d5026887dfec6ea9c713404de07767.jpg)

7. 等待安装完成，点击关闭。

![](images/508de4e079dd44beffac136ca200846a7d8a1fb729f4a4f9dd0fb7b3b2d49a0a.jpg)

8. 在设备管理器中有 ZMotionRT Controller 就是安装成功。

计算机管理

![](images/92c436cf97e11aabb41ae954f75667d351df1d07dccae25d3284d3c975dd5fde.jpg)

普通网卡安装 Ethercat 总线协议

MotionRT710 支持 XPCI/XPCIE 的 ECAT 网口，也支持电脑普通网口作为 ECAT 使用。1. 打开控制面板，在 Windows 网络连接界面，选择需要用作总线的网口，1右击属性

->2 安装->3 协议->4 添加。

控制面板\网络和Internet\网络连接

![](images/fcd507dd4b5455e702ab352eecd6c22b65e21bb0cc8f8db3333bab8e4869107b.jpg)

## 2. 直接单击“从磁盘安装”

![](images/78df3fa8ff9281b1b10f5ec7a03257cd0dbe4055b5690c85efd0e66266360eea.jpg)

3. 浏览驱动位置，选择 ZMotionRtPacket.inf 文件，并打开。

![](images/c75fcf413e7f6022b83f85d0e5c99f56c23bbae5402e4d04e231f74580283bd9.jpg)

4. 继上一步选完文件后，直接点击确定。

![](images/899add0780e9d294eb1a1907ab7d9fe2fac9bef4cac23c5a6cc2c11c1ab42ce5.jpg)  
5. 网络协议选中“ZMotionRT64 Packet Protocol Driver”后点击确定

选择网络协议

？ ×

单击你想安装的网络协议，然后单击"确定”。

网络协议：

ZMotionRT64 Packet Protocol Driver

这个驱动程序已经过数字签名。告诉我为什么驱动程序签名很重要

确定

取消

6. 完成后，以太网属性协议，会多出一条“ZMotionRT64 Packet Protocol Driver”，即代表安装成功。

以太网属性

网络

共享

连接时使用：

Realtek PCle GbE Family Controller

配置(C)..

此连接使用下列项目(O):

Internet 协议版本 4 (TCP/IPv4)

□ Microsoft网络适配器多路传送器协议

Microsoft LLDP 协议驱动程序

Internet 协议版本 6 (TCP/IPv6)

☑链路层拓扑发现响应程序

√ ZMotionRT64 Packet Protocol Driver

链路层拓扑发现映射器I/O驱动程序

√ : TwinCAT Ethernet Protocol for All Network Adapters

安装(N)..

卸戴(U)

描述

允许你的计算机访问Microsoft网络上的资源。

关闭

## MotionRT 控制台使用

## 【主界面】

![](images/1ef625b354b3feef26868b7aa20d27906512e89a0e06ccb09e5cb8b733dad69a.jpg)

## （一）MotionRT 列表：

即当前电脑上存在的 PCI卡列表

[1]: PCI 卡型号，NoXPci 指无卡

[2]: 卡的类型。

[3]: 卡的 ID。Ecat-Eth 指无卡时用的网口

[4]: 卡的拨码地址。无卡时指：网卡的唯一编号

## （二）RT消息对话框

控制台跟各个卡之间交互的消息对话框。

## （三）RT功能区

[5]: UpdateCard: 刷新卡的配置信息，并保存，下次开启控制台的时候自动加载配置信息；首次使用时，需要手动AddEcat添加卡号信息，如下图：

![](images/f412667a7cf663d5a27bc31c3beb811c5e1c2ee6795d4ce454398acc026ac517.jpg)

[6]: License 功能界面。可对所需的功能进行勾选配置，参见下表。

TrialLicense

Robot

□ Robot6

Cam

HW

ZVision

Motor:

Motion:

License:

MO8 ZV CAM R1R6 AX32

确定

取消

<table><tr><td colspan="2" rowspan="1">功能                                               描述</td></tr><tr><td colspan="1" rowspan="1">Robort</td><td colspan="1" rowspan="1">普通机械手</td></tr><tr><td colspan="1" rowspan="1">Robort6</td><td colspan="1" rowspan="1">六轴以及六轴以下的机械手</td></tr><tr><td colspan="1" rowspan="1">Cam</td><td colspan="1" rowspan="1">相机功能</td></tr><tr><td colspan="1" rowspan="1">HW</td><td colspan="1" rowspan="1">硬件位置比较输出功能</td></tr><tr><td colspan="1" rowspan="1">zVision</td><td colspan="1" rowspan="1">时间指令功能</td></tr><tr><td>Motor</td><td>实际电机数量</td></tr><tr><td>Motion</td><td>支持的运动功能 MO0：点位运动 MO1：点位运动、电子凸轮 MO2：点位运动、电子凸轮、直线插补 MO6: 点位运动、电子凸轮、直线插补、圆弧插补</td></tr><tr><td>License</td><td>MO8：点位运动、电子凸轮、直线插补、圆弧插补、连续插补 用来显示保存License 参数配置的窗口</td></tr></table>

## [7]: Config 配置界面

Config

<table><tr><td> Total Memery</td><td>256</td><td> mbytes</td></tr><tr><td> Cpu Usage</td><td>90</td><td>%</td></tr><tr><td> Hmi num </td><td>2</td><td>1-4</td></tr><tr><td> Hmi size</td><td>5242880</td><td>128k-100m</td></tr><tr><td> Zvison Latin num</td><td>4</td><td>1-8</td></tr><tr><td> Zvision Latin size</td><td>5242880</td><td> 128k-100m</td></tr><tr><td> Zar size</td><td>33554432</td><td>128k-512m</td></tr><tr><td> Task num</td><td>15</td><td> 2-30</td></tr><tr><td> Array space</td><td>2560000</td><td> 128k-10m</td></tr><tr><td> Zvobj num</td><td>2048</td><td>2048-16000</td></tr><tr><td> Sub num </td><td>4096</td><td> 256-10000</td></tr><tr><td> Com num</td><td>0</td><td>0-8</td></tr><tr><td> Eth num</td><td>2</td><td> 0-12</td></tr><tr><td> Ethcustom num</td><td>0</td><td> 0-12</td></tr><tr><td> 3Dsolid num</td><td>128</td><td>16-512 确定</td></tr><tr><td>□ AutoRunApp</td><td>□ AutoRunRT</td><td>RtLogSave 取消</td></tr></table>

<table><tr><td colspan="2" rowspan="1">功能                                               描述</td></tr><tr><td colspan="1" rowspan="1">Total Memery</td><td colspan="1" rowspan="1">总内存，包括控制器内所有能够保存数据的空间所占内存，如数组空间、Zar 文件尺寸、通道大小、hmi 分辨率等等，最好设置值为 200 以上</td></tr><tr><td colspan="1" rowspan="1">Cpu Usage</td><td colspan="1" rowspan="1">CPU 使用限制</td></tr><tr><td colspan="1" rowspan="1">Hmi num</td><td colspan="1" rowspan="1">可用的 Hmi 个数</td></tr><tr><td colspan="1" rowspan="1">Hmi size</td><td colspan="1" rowspan="1">hmi 的分辨率大小</td></tr><tr><td colspan="1" rowspan="1"> Zvision Latin num</td><td colspan="1" rowspan="1">视觉通道数量</td></tr><tr><td colspan="1" rowspan="1"> Zvision Latin size</td><td colspan="1" rowspan="1">单个视觉通道尺寸</td></tr><tr><td colspan="1" rowspan="1">Zar size</td><td colspan="1" rowspan="1">Zar 文件尺寸大小</td></tr><tr><td colspan="1" rowspan="1">Task num</td><td colspan="1" rowspan="1">可执行的任务数量</td></tr><tr><td colspan="1" rowspan="1"> Array Space</td><td colspan="1" rowspan="1">分配的数组空间大小</td></tr><tr><td colspan="1" rowspan="1">Zvobj num</td><td colspan="1" rowspan="1">Zvobject的个数</td></tr><tr><td colspan="1" rowspan="1">Sub num</td><td colspan="1" rowspan="1">可 sub 最多的函数个数</td></tr><tr><td colspan="1" rowspan="1">Com num</td><td colspan="1" rowspan="1">串口个数</td></tr><tr><td colspan="1" rowspan="1">Eth num</td><td colspan="1" rowspan="1">网口PORT个数，设置值要小于最大值</td></tr><tr><td colspan="1" rowspan="1">Ethcustom num</td><td colspan="1" rowspan="1">自定义网络 PORT数量</td></tr><tr><td colspan="1" rowspan="1"> 3Dsolid num</td><td colspan="1" rowspan="1">3D 固体个数</td></tr><tr><td colspan="1" rowspan="1"> AutoRunApp</td><td colspan="1" rowspan="1">开机自动打开软件</td></tr><tr><td colspan="1" rowspan="1">AutoRunRT</td><td colspan="1" rowspan="1">开机自动运行RT软件</td></tr><tr><td colspan="1" rowspan="1">RtLogSave</td><td colspan="1" rowspan="1">保存日志到日志文件</td></tr></table>

[8]: AddEcat添加可连接的卡

[9]: Start

启动MotionRT 按钮，启动后，可连接控制卡

[10]: Stop

停止MotionRT 按钮，停止后，不可连接控制卡

## 例程

## 4.1.3-1 网口连接控制器

本例程演示用网口去连接控制器，失败打印错误码。

![](images/90b760fb55b6c3ab369105895d0745874ff5a55896e99f4826f5d9248a218585.jpg)  
// test1.cpp : 定义控制台应用程序的入口点。

```c
//
#include "stdafx.h"
#include <windows.h>
#include "zmotion.h"
#include "zauxdll2.h"
void commandCheckHandler(const char *command, int ret)
{
if (ret)//非 0 则失败
{
printf("%s return code is %d\n", command, ret);
}
int _tmain(int argc, _TCHAR* argv[])
char *ip_addr = (char *)"127.0.0.1"; //控制器 IP 地址 192.168.0.11,仿真器 IP
地址 127.0.0.1，仿真器连接需把仿真器打开
ZMC_HANDLE handle = NULL; //连接句柄
int ret = ZAux_OpenEth(ip_addr, &handle); //连接控制器
if (ERR_SUCCESS != ret)
printf("connect controller failed!\n");
handle = NULL;
getchar();
return -1;
}
printf("控制器连接成功！\n");
//运动类型获取}
Sleep(2000);
ret = ZAux_Close(handle); //关闭连接
commandCheckHandler("ZAux_Close", ret) ;//判断指令是否执行成功
printf("connection closed!\n");
handle = NULL;
return 0;
}
```

## 4.1.3-2 串口连接控制器

```cpp
// test1.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include "zmotion.h"
#include "zauxdll2.h"
void commandCheckHandler(const char *command, int ret)
if (ret)//非 0 则失败
{
printf("%s return code is %d\n", command, ret);
int _tmain(int argc, _TCHAR* argv[])
ZMC_HANDLE handle = NULL; //连接返回的句柄
uint32 comid = 1; //连接的 COM 口
int ret = ZAux_OpenCom(comid,&handle); //COM1 口连接控制器
commandCheckHandler("ZAux_OpenCom", ret);
if (ERR_SUCCESS != ret)
{
printf("串口连接失败!\n");
handle = NULL;
}
else
{
printf("串口连接成功!\n");
}
```

```c
uint32 dwBaudRate = 38400; //波特率 38400
uint32 dwByteSize = 8; //8 位数据位
uint32 dwParity = 0; //无校验+
uint32 dwStopBits = 0; ///停止位
ret = ZAux_SetComDefaultBaud(dwBaudRate, dwByteSize, dwParity, dwStopBits); //设置串
口通讯参数
commandCheckHandler("ZAux_SetComDefaultBaud", ret);
//do something...
Sleep(2000);
ret = ZAux_Close(handle); //关闭连接
commandCheckHandler("ZAux_Close", ret) ;//判断指令是否执行成功
printf("connection closed!\n");
handle = NULL;
return 0;
}
```

## 4.1.3-3 PCI 方式连接控制器

连接PCI卡，如果失败，会打印错误码，直接返回。

```cpp
// test1.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include "zmotion.h"
#include "zauxdll2.h"
void commandCheckHandler(const char *command, int ret)
{
if (ret)//非 0 则失败
{
printf("%s return code is %d\n", command, ret);
}
int _tmain(int argc, _TCHAR* argv[])
```

```c
ZMC_HANDLE handle = NULL; //连接返回的句柄
uint32 cardnumcardnum = 0; //PCI0
int ret = ZAux_OpenPci(cardnumcardnum, &handle); ////以太网连接控制器以太网连接控制
器
commandCheckHandler("ZAux_OpenPci", ret);
if (ERR_SUCCESS != ret)
{
printf("PCI 连接失败!\n");
Sleep(3000);
handle = NULL;
return 0;
}
else
{
printf("PCI 连接成功!\n");
}
//do something.....
Sleep(3000);
ret = ZAux_Close(handle);
commandCheckHandler("ZAux_OpenPci", ret);
return 0;
```

## 4.1.3-4 修改 IP 地址

```cpp
// test1.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include "zmotion.h"
#include "zauxdll2.h"
```

```c
void commandCheckHandler(const char *command, int ret)
if (ret)//非 0 则失败
{
printf("%s return code is %d\n", command, ret);
}
int _tmain(int argc, _TCHAR* argv[])
char *ip_addr = (char *)"127.0.0.1"; //控制器 IP 地址,仿真器 IP地址，连
接需把仿真器打开
ZMC_HANDLE handle = NULL; //连接句柄
int ret = ZAux_OpenEth(ip_addr, &handle); //连接控制器
if (ERR_SUCCESS != ret)
{
printf("控制器连接失败！\n");
handle = NULL;
getchar();
return -1;
}
printf("控制器连接成功！\n");
ret = ZAux_SetIp( handle,(char *)"192.168.0.11");//仿真器 IP 无法修改，只能修改实物的，
本例程为了示范，所以连接的仿真器
commandCheckHandler("ZAux_Close", ret) ;//判断指令是否执行成功
//运动类型获取}
Sleep(2000);
handle = NULL;
return 0;
```

## 4.1.3-5 MotionRT 连接

连接 MotionRT,首先得先把 MotionRT 控制台打开：

![](images/d5da23746fb7757b05b8263c1f9386047157bc76ac6d7230d59b07608653c314.jpg)

打开控制台，配置好功能及参数后，点击 Start 启动后方可连接。

## MotionRT710

![](images/d8d2cb21e3898bef5fea4a7e4356f83d4c475f9b66168535a19b9e0032cb5ea9.jpg)  
注意：zmotion.dll 版本要在 3.8.8.50 以上。方可使用 MotionRT。

```cpp
// test1.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include "zmotion.h"
#include "zauxdll2.h"
void commandCheckHandler(const char *command, int ret)
if (ret)//非0则失败
{
printf("%s return code is %d\n", command, ret);
}
int _tmain(int argc, _TCHAR* argv[])
{
ZMC_HANDLE handle = NULL; //连接句柄
char MotionID[32]= ""; //改模式当前对字符串无要求，可填空字符串
int ret=ZAux_FastOpen(5, MotionID,1000 ,&handle); //连接MotionRT
if (ERR_SUCCESS != ret)
{
printf("控制器连接失败！\n");
handle = NULL;
getchar();
return -1;
printf("控制器连接成功！\n");
//运动类型获取}
Sleep(2000);
handle = NULL;
return 0;
```

## 4.1.3-6 启停软核

连接 MotionRT,首先得先把 MotionRT 控制台打开：

MotionRT710  
RTlist:  
UpdateCard  
License  
Config  
AddEcat  
Start  
Stop  
03/09/2024 20:45:07:421 RT start.  
04/09/2024 15:24:46:445 RT stop.  
为方便用户移对运动控制器的租于住見.提供

```cpp
// test1.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include "zmotion.h"
#include "zauxdll2.h"
void commandCheckHandler(const char *command, int ret)
{
if (ret)//非0则失败
{
("%s return code is %d\n", command, ret);
}
int (int argc, * argv[])
{
```

```c
ZMC_HANDLE handle = ; //连接句柄
int isel=0;
int ret=0;
if (!ZAux_GetLocalRTState(isel))
("软核控制台未启动！！\n");
ret=ZAux_LocalRTManage(isel,1);
if (ERR_SUCCESS != ret)
("控制台启动失败！\n");
ZAux_Close(handle);
handle = ;
();
return -1;
}else
{
("软核控制台已启动！\n");
char MotionID[32]= ""; //改模式当前对字符串无要求，可填空字符串
ret=ZAux_FastOpen(5, MotionID,1000 ,&handle); //连接MotionRT
if (ERR_SUCCESS != ret)
("控制器连接失败！\n");
handle = ;
();
return -1;
}
("控制器连接成功！\n");
//运动类型获取}
(2000);
ZAux_Close(handle);
handle = ;
ret=ZAux_LocalRTManage(isel,2);
```

```c
if (ERR_SUCCESS != ret)
{
("控制台停止失败！\n");
();
}
return 0;
```

## 4.2 控制器信息获取

## 指令列表

<table><tr><td rowspan=1 colspan=2>指令                                                 说明</td></tr><tr><td rowspan=1 colspan=1>ZAux GetControllerInfo</td><td rowspan=1 colspan=1>获取控制器卡信息</td></tr><tr><td rowspan=1 colspan=1>ZAux_ GetSysSpecification</td><td rowspan=1 colspan=1>获取控制器最大规格数</td></tr><tr><td rowspan=1 colspan=1> ZAux GetRtcTime</td><td rowspan=1 colspan=1>读取控制器 RTC 时间</td></tr><tr><td rowspan=1 colspan=1> ZAux_ SetRtcTime</td><td rowspan=1 colspan=1>设置控制器 RTC 时间</td></tr></table>

## 重点说明

为了方便用户核对运动控制器的相关信息，提供指令 ZAux\_GetControllerInfo 和ZAux\_GetSysSpecification 两个函数去获取控制器的相关信息。

可获取控制器型号类型，控制器软件版本（固件版本），控制器唯一 ID，控制器最大虚拟轴数，控制器最大电机数量，控制器最大IN、OUT、AD、DA数量，控制器系统日期，控制器系统时间等参数。若还想获取更多的控制器参数，可以通过ZAux\_Execute 指令在线发送，使用字符串“?\*max”或者“?\*set”获取更多的控制器参数。

## 例程

## 4.2.3-1 控制器信息

本例程主要演示了如何通过指令获取控制器的相关信息。

```c
// test1.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include "zmotion.h"
#include "zauxdll2.h"
void commandCheckHandler(const char *command, int ret)
if (ret)//非 0 则失败
{
printf("%s return code is %d\n", command, ret);
}
int _tmain(int argc, _TCHAR* argv[])
char *ip_addr = (char *)"127.0.0.1"; //控制器出厂默认 IP 地址
192.168.0.11,仿真器 IP 地址 127.0.0.1，仿真器连接需把仿真器打开
ZMC_HANDLE handle = NULL; //连接句柄
int ret = ZAux_OpenEth(ip_addr, &handle); //连接控制器
if (ERR_SUCCESS != ret)
{
printf("控制器连接失败！\n");
handle = NULL;
getchar();
return -1;
}
printf("控制器连接成功！\n");
char SoftTypes[20];
```

char SoftVersions[20];   
char ControllerIDs[20];   
ret = ZAux\_GetControllerInfo(handle,SoftTypes,SoftVersions,ControllerIDs);//控制卡信   
息获取   
commandCheckHandler("ZAux\_GetControllerInfo", ret) ;//判断指令是否执行成功   
printf("SoftType = %s\n", SoftTypes); //打印控制器的型号   
printf("SoftVersion = %s\n", SoftVersions); //打印软件版本号   
printf("ControllerId = %s\n", ControllerIDs); //打印控制器的 ID   
uint16 Max\_VirtuAxises1;   
uint8 Max\_motor1;   
uint8 Max\_io1[4];   
ret = ZAux\_GetSysSpecification(handle,&Max\_VirtuAxises1,&Max\_motor1,Max\_io1);//获取   
控制卡最大规格数   
commandCheckHandler("ZAux\_GetSysSpecification", ret) ;//判断指令是否执行成功   
printf("Max\_VirtuAxises1 = %u\n", Max\_VirtuAxises1); //最大虚拟轴数   
printf("Max\_motor1 = %u\n", Max\_motor1); //最大电机轴数   
printf("Max\_io1[0] = %u\n", Max\_io1[0]); //最大 IN   
printf("Max\_io1[1] = %u\n", Max\_io1[1]); //最大 out   
printf("Max\_io1[2] = %u\n", Max\_io1[2]); //最大 AD   
printf("Max\_io1[3] = %u\n", Max\_io1[3]); //最大 DA   
Sleep(2000);   
ret = ZAux\_Close( handle);//关闭连接   
commandCheckHandler("ZAux\_Close", ret) ;//判断指令是否执行成功   
handle = NULL;   
return 0;

运行结果如下：

成号 = VPLC5xx-Simu  
20180511  
1234  
= 64  
机 64  
= 27  
= 15  
= 0  
= 2

## 第五章 运动控制参数设置

在运动控制系统中，将运动控制的对象称为“轴”，运动控制系统中的一个电机控制的运动平台称为一个运动轴，每个运动轴只有一个自由度，可以做直线运动或旋转运动，正运动控制卡，轴的分类如下：

<table><tr><td rowspan=1 colspan=1>轴种类</td><td rowspan=1 colspan=1>描述</td></tr><tr><td rowspan=1 colspan=1>电机轴</td><td rowspan=1 colspan=1>使用控制器的脉冲轴接口、EtherCAT 总线或RTEX 总线接口和驱动器连接，给设备分配轴号，将1台电机作为1个轴使用。</td></tr><tr><td rowspan=1 colspan=1>虚拟轴</td><td rowspan=1 colspan=1>运动控制器内的虚拟轴，不使用实际驱动器，或作为同步控制的虚拟主轴和作为机械手算法中的笛卡尔坐标轴使用。</td></tr><tr><td rowspan=1 colspan=1>编码器轴</td><td rowspan=1 colspan=1>使用控制器本地编码器轴接口，分配为实际编码器输入加以使用。</td></tr></table>

电机轴：主动运行，电机根据控制器发出的脉冲来运动，发送的脉冲数根据(运动参数变化量)\*(ZAux\_Direct\_SetUnits 设置的 Units)确定，目标需求位置由 ZAux\_Direct\_GetDpos的规划位置参数体现。

编码器轴：被动运行，编码器跟随电机转动，产生脉冲，反馈控制器，控制器接收的脉冲数调用 ZAux\_Direct\_GetEncoder 确定获取，编码器反馈位置由 ZAux\_Direct\_GetMpos获取反馈位置参数体现。

当用户连接好一整套系统后（包括运动控制器，驱动器，电机），如何查看整个系统的状态？如何配置这一套系统？如何查看报警信息？如何进行速度，加减速的配置？本章节将介绍用户可以检测到哪些状态以及如何检测，如何配置运动控制参数。

## 5.1 运动基本参数

## 指令列表

基本轴参数：

<table><tr><td colspan="2" rowspan="1">指令                                                  说明</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetAtype</td><td colspan="1" rowspan="1">设置轴类型</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct GetAtype</td><td colspan="1" rowspan="1">读取轴类型</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetUnits</td><td colspan="1" rowspan="1">设置脉冲当量（units）</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_GetUnits</td><td colspan="1" rowspan="1">读取脉冲当量（units）</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetAccel</td><td colspan="1" rowspan="1">设置加速度，单位为units /s^2</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_GetAccel</td><td colspan="1" rowspan="1">读取加速度，单位为units/s^2</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetDecel</td><td colspan="1" rowspan="1">设置减速度，单位为units/s^2</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetDecel</td><td colspan="1" rowspan="1">读取减速度，单位为units/s^2</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetSpeed</td><td colspan="1" rowspan="1">设置轴速度，单位为units/s</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetSpeed</td><td colspan="1" rowspan="1">读取轴速度，单位为units/s</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_ SetDpos</td><td colspan="1" rowspan="1">设置轴的规划位置，单位为units</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_ GetDpos</td><td colspan="1" rowspan="1">读取轴的规划位置，单位为units</td></tr><tr><td colspan="1" rowspan="1"> ZAux Direct SetMpos</td><td colspan="1" rowspan="1">设置反馈位置，单位为units</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ GetMpos</td><td colspan="1" rowspan="1">读取反馈位置，单位为units</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetEncoder</td><td colspan="1" rowspan="1">获取控制器接收的脉冲数</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetFastDec</td><td colspan="1" rowspan="1">设置快速减速度，单位为units/s^2</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_GetFastDec</td><td colspan="1" rowspan="1">读取快速减速度，单位为units/s^2</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct_ SetLspeed</td><td colspan="1" rowspan="1">设置轴起始速度，单位为units/s</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetLspeed</td><td colspan="1" rowspan="1">读取轴起始速度，单位为units/s</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetSramp</td><td colspan="1" rowspan="1">设置S 曲线设置。0-梯形加减速</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct_ GetSramp</td><td colspan="1" rowspan="1">读取 S曲线设置。0-梯形加减速</td></tr><tr><td colspan="1" rowspan="1">ZAux_Direct GetMspeed</td><td colspan="1" rowspan="1">读取反馈速度，单位为units/s</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct GetVpSpeed</td><td colspan="1" rowspan="1">读取当前轴运行的命令速度.单位为units/s</td></tr><tr><td colspan="1" rowspan="1">ZAux Direct SetVpMode</td><td colspan="1" rowspan="1">加减速度曲线</td></tr></table>

轴基本状态：

<table><tr><td rowspan=1 colspan=2>指令                                                  说明</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct GetIfIdle</td><td rowspan=1 colspan=1>读取轴是否运动</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct_ GetAxisStatus</td><td rowspan=1 colspan=1>读取轴的状态</td></tr><tr><td rowspan=1 colspan=1>ZAux Direct GetAxisStatus2</td><td rowspan=1 colspan=1>读取轴的使能状态</td></tr><tr><td rowspan=1 colspan=1>ZAux_ Direct GetAxisStopReason</td><td rowspan=1 colspan=1>读取轴历史异常停止原因</td></tr></table>

## 重点说明

## 5.1.2-1 轴状态

通过 ZAux\_Direct\_GetAxisStatus 指令获取轴的各种状态。按十进制显示数值，按二进制对应位判断状态，可同时发生多个错误。

轴状态字的定义如下：

<table><tr><td>位 说明 打印值</td></tr></table>

<table><tr><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1>随动误差超限告警</td><td rowspan=1 colspan=1>2</td><td rowspan=1 colspan=1>2h</td></tr><tr><td rowspan=1 colspan=1>2</td><td rowspan=1 colspan=1>与远程轴通讯出错</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>4h</td></tr><tr><td rowspan=1 colspan=1>3</td><td rowspan=1 colspan=1>远程驱动器报错</td><td rowspan=1 colspan=1>8</td><td rowspan=1 colspan=1>8h</td></tr><tr><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>正向硬限位</td><td rowspan=1 colspan=1>16</td><td rowspan=1 colspan=1>10h</td></tr><tr><td rowspan=1 colspan=1>5</td><td rowspan=1 colspan=1>反向硬限位</td><td rowspan=1 colspan=1>32</td><td rowspan=1 colspan=1>20h</td></tr><tr><td rowspan=1 colspan=1>6</td><td rowspan=1 colspan=1>找原点中</td><td rowspan=1 colspan=1>64</td><td rowspan=1 colspan=1>40h</td></tr><tr><td rowspan=1 colspan=1>7</td><td rowspan=1 colspan=1>HOLD 速度保持信号输入</td><td rowspan=1 colspan=1>128</td><td rowspan=1 colspan=1>80h</td></tr><tr><td rowspan=1 colspan=1>8</td><td rowspan=1 colspan=1>随动误差超限出错</td><td rowspan=1 colspan=1>256</td><td rowspan=1 colspan=1>100h</td></tr><tr><td rowspan=1 colspan=1>9</td><td rowspan=1 colspan=1>超过正向软限位</td><td rowspan=1 colspan=1>512</td><td rowspan=1 colspan=1>200h</td></tr><tr><td rowspan=1 colspan=1>10</td><td rowspan=1 colspan=1>超过负向软限位</td><td rowspan=1 colspan=1>1024</td><td rowspan=1 colspan=1>400h</td></tr><tr><td rowspan=1 colspan=1>11</td><td rowspan=1 colspan=1>CANCEL 执行中</td><td rowspan=1 colspan=1>2048</td><td rowspan=1 colspan=1>800h</td></tr><tr><td rowspan=1 colspan=1>12</td><td rowspan=1 colspan=1>脉冲频率超过 MAX_SPEED 限制需要修改降速或修改MAX_SPEED</td><td rowspan=1 colspan=1>4096</td><td rowspan=1 colspan=1>1000h</td></tr><tr><td rowspan=1 colspan=1>14</td><td rowspan=1 colspan=1>机械手指令坐标错误</td><td rowspan=1 colspan=1>16384</td><td rowspan=1 colspan=1>4000h</td></tr><tr><td rowspan=1 colspan=1>18</td><td rowspan=1 colspan=1>电源异常</td><td rowspan=1 colspan=1>262144</td><td rowspan=1 colspan=1>40000h</td></tr><tr><td rowspan=1 colspan=1>20</td><td rowspan=1 colspan=1>轴速度保护</td><td rowspan=1 colspan=1>1048576</td><td rowspan=1 colspan=1>100000h</td></tr><tr><td rowspan=1 colspan=1>21</td><td rowspan=1 colspan=1>运动中触发特殊运动指令失败</td><td rowspan=1 colspan=1>2097152</td><td rowspan=1 colspan=1>200000h</td></tr><tr><td rowspan=1 colspan=1>22</td><td rowspan=1 colspan=1>告警信号输入</td><td rowspan=1 colspan=1>4194304</td><td rowspan=1 colspan=1>400000h</td></tr><tr><td rowspan=1 colspan=1>23</td><td rowspan=1 colspan=1>轴进入了暂停状态</td><td rowspan=1 colspan=1>8388608</td><td rowspan=1 colspan=1>800000h</td></tr></table>

调用 ZAux\_Direct\_GetAxisStopReason 查看轴历史停止原因锁存，写 0 清除，按位锁存，锁存的是轴状态的信息。  
调用 ZAux\_Direct\_GetIfIdle 判断加在轴上的运动指令是否完成，运动中返回 0，运动结束返回-1。

## 5.1.2-2 轴类型

轴类型调用 ZAux\_Direct\_SetAtype/ZAux\_Direct\_GetAtype（ATYPE）指令根据当前轴的特性去配置，在用户程序初始化时应第一时间完成轴类型的配置，类型不匹配会报错。

所有未分配的轴默认为虚拟轴，ATYPE 的值为 0。

控制器支持的轴类型如下：

<table><tr><td colspan="2" rowspan="1">ATYPE 类型                                              描述</td></tr><tr><td colspan="1" rowspan="1">0</td><td colspan="1" rowspan="1">虚拟轴</td></tr><tr><td colspan="1" rowspan="1">1</td><td colspan="1" rowspan="1">脉冲方向方式的步进或伺服</td></tr><tr><td colspan="1" rowspan="1">2</td><td colspan="1" rowspan="1">模拟信号控制方式的伺服(预留)</td></tr><tr><td colspan="1" rowspan="1">3</td><td colspan="1" rowspan="1">正交编码器</td></tr><tr><td colspan="1" rowspan="1">4</td><td colspan="1" rowspan="1">脉冲方向输出+正交编码器输入</td></tr><tr><td colspan="1" rowspan="1">5</td><td colspan="1" rowspan="1">脉冲方向输出+脉冲方向编码器输入</td></tr><tr><td colspan="1" rowspan="1">6</td><td colspan="1" rowspan="1">脉冲方向方式的编码器</td></tr><tr><td colspan="1" rowspan="1">7</td><td colspan="1" rowspan="1">脉冲方向方式步进或伺服+EZ信号输入</td></tr><tr><td colspan="1" rowspan="1">8</td><td colspan="1" rowspan="1">ZCAN 扩展脉冲方向方式步进或伺服</td></tr><tr><td colspan="1" rowspan="1">9</td><td colspan="1" rowspan="1">ZCAN 扩展正交编码器</td></tr><tr><td colspan="1" rowspan="1">10</td><td colspan="1" rowspan="1">ZCAN 扩展脉冲方向方式的编码器</td></tr><tr><td colspan="1" rowspan="1">20</td><td colspan="1" rowspan="1">振镜类型，带振镜状态反馈振镜连接不上AXISSTATUS 的bit2会置位，ENCODER返回原始的发送位置，脉冲单位ZMC408SCAN 支持</td></tr><tr><td colspan="1" rowspan="1">21</td><td colspan="1" rowspan="1">振镜轴类型，需要控制器支持缺省系统周期250us，振镜刷新周期 50us，与固件有关可以使用普通轴的所有运动控制指令，支持振镜轴与其它轴类型混合插补</td></tr><tr><td colspan="1" rowspan="1">22</td><td colspan="1" rowspan="1">振镜轴类型，带振镜位置反馈振镜连接不上AXISSTATUS 的 bit2 会置位，振镜报警AXISSTATUS 的bit3 会置位MPOS 返回反馈位置，做了反矫正处理，ENCODER 返回原始的反馈位置脉冲单位ZMC408SCAN支持</td></tr><tr><td colspan="1" rowspan="1">24</td><td colspan="1" rowspan="1">远程编码器轴类型ZHD500X上手轮使用，需要控制器5系列20180404以上固件版本支持</td></tr><tr><td colspan="1" rowspan="1">50</td><td colspan="1" rowspan="1">RTEX 周期位置模式，需RTEX 控制器</td></tr><tr><td colspan="1" rowspan="1">51</td><td colspan="1" rowspan="1">RTEX 周期速度模式，需RTEX控制器</td></tr><tr><td colspan="1" rowspan="1">52</td><td colspan="1" rowspan="1">RTEX 周期力矩模式，需RTEX控制器请先关闭驱动器2自由度控制模式，并设置速度限制</td></tr><tr><td colspan="1" rowspan="1">65</td><td colspan="1" rowspan="1">EtherCAT 周期位置模式，需支持 EtherCAT</td></tr><tr><td colspan="1" rowspan="1">66</td><td colspan="1" rowspan="1">EtherCAT 周期速度模式，需支持 EtherCATDRIVE_PROFILE 要设置为20或以上</td></tr><tr><td colspan="1" rowspan="1">67</td><td colspan="1" rowspan="1">EtherCAT 周期力矩模式，需支持 EtherCATDRIVE_PROFILE要设置为30或以上</td></tr><tr><td colspan="1" rowspan="1">70</td><td colspan="1" rowspan="1">EtherCAT自定义操作，只读取编码器，需支持 EtherCAT</td></tr></table>

## 1. 轴类型(ZAux\_Direct\_SetAtype)=0:虚拟轴

多轴同步运动时可作为机器的主轴，从轴都跟随此虚拟主轴。

作为其他轴的叠加轴，给实际运动的轴叠加一个虚拟轴，这些虚拟轴可以用 ADDAX指令（轴叠加）进行设置，然后将各个虚拟轴的运动叠加到实轴。

## 2. 轴类型(ZAux\_Direct\_SetAtype)=1 或 7:脉冲轴