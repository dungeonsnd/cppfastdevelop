
cppfastdevelop
==============

A simple but powerful c++ project called "cppfastdevelop" (a.k.a. "cfd") which makes your develop fast. The "cfd" project include "cppfoundation" (a.k.a. "cf") and "cpplibrary" (a.k.a. "cl") projects.

WE PROVIDE:

1.A simple c++ classes-tree named "cf" which includes many classes frequently used.
  For example, synchronization(between threads and between processes) , IPC , socket (tcp,unix-domain,udp)... These classes is the system-call wrapping,they DONOT include any framework or library. They're very helpful to  develop your own library or framework.

2.A library repertory named "cl" which include many libraries c++ developer frequently used.
  For example, we'll provide net library,circularqueue,logserver,simple log library,config,simple message queue, database access library,simple memory pool, crypto library and etc. We'll add other useful libraries later.


ATTENTION:

1.We'll never try to replace boost,poco,tbsys(tbnet) or any other c++ library.
  The "cfd" project only helps you to develop more easily when using cplusplus and other libraries are complementary to the "cfd" project. It gives you some choices to choose libraries . You may use "cf" as the main foundation classes.
  
2."cfd" will be compatible with linux and bsd ( maybe also mac os x ,ibm aix and windows , if we have enough time to do that ).

3.We hope you to join us to make the project more perfect.


cfd project website(up-to-date):

https://bitbucket.org/dungeonsnd/cppfastdevelop

https://github.com/dungeonsnd/cppfastdevelop

other website(maybe not up-to-date):

https://code.google.com/p/cppfastdevelop/

https://sourceforge.net/projects/cppfastdevelop/

project sponsor: Jeffery
email: dungeonsnd@gmail.com


作者根据多年c++开发经历总结提炼了一套类似于jdk的c++类库, 方便程序开发者使用.
cppfoundation是对常用类的封装,并形成了良好的继承体系,cpplibrary是作者封装的一些实用库.

本项目目前仅支持linux环境,作者暂无支持其它环境的开发计划.

项目目前基本稳定, 作者已经将其用于某生产环境且运行良好. 暂无大规模修改计划. 但是请注意, 网络库cpplibrary/netserver/有bug, 作者目前没有时间来完善. 其它的类和库已经基本稳定,如cppfoundation/及cpplibrary, 建议用于非苛刻条件下的商业生产环境.



/*
 * Copyright 2014, Jeffery Qiu. All rights reserved.
 *
 * Licensed under the GNU LESSER GENERAL PUBLIC LICENSE(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.gnu.org/licenses/lgpl.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//// Author: Jeffery Qiu (dungeonsnd at gmail dot com)
////

