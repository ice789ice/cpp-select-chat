#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>

int main(){
//阶段1：创建监听套接字
int listen_sock = socket(AF_INET,SOCK_STREAM,0);
if (listen_sock<0){
std::cout<<"sock创建失败"<<std::endl;
return 1;
	 	  }
std::cout<<"listen_sock创建成功"<<std::endl;
//允许端口快速重用（避免TIME——WAIT 导致无法立即重启服务）
int opt=1;
if(setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0){
perror("setsockopt");
return 1;
							 	      }

//阶段2：绑定地址和端口

sockaddr_in addr;
addr.sin_family =AF_INET;
addr.sin_port =htons(1234);
addr.sin_addr.s_addr = INADDR_ANY;

int bind_ret = bind(listen_sock,(sockaddr*)&addr,sizeof(addr));
if (bind_ret<0){
perror("bind");
return 1;
		}

int listen_ret =listen(listen_sock,5);
if(listen_ret<0){
perror("listen");
 return 1;
 		}

std::cout<<"服务器启动，监听1234"<<std::endl;

//新增一个容器,用来存储名称

struct Client{
int fd;
bool has_nickname;
std::string nickname;
std::chrono::steady_clock::time_point last_active;
		};
std::vector<Client> clients;
const int kIdleTimeoutSec = 60;

//主循环：事件驱动（select多路复用）
while(true){
fd_set readfds;//每次都要重新声明和清空


FD_ZERO(&readfds);//清空fd_set
//永远监视套接字（用来检测新连接）
FD_SET(listen_sock,&readfds);
int max_fd = listen_sock;//记录当前最大的fd值
//把所有已连接的客户端fd也加进监视集合
for(auto &c : clients){
if(c.fd >= FD_SETSIZE){
	std::cerr<<"fd超过FD_SETSIZE，拒绝加入select: "<<c.fd<<std::endl;
	continue;
}
FD_SET(c.fd,&readfds);
if(c.fd>max_fd)max_fd =c.fd;
			}
//调用select,等待任意一个fd可读
//使用超时便于做空闲连接清理
timeval tv{};
tv.tv_sec = 1;
int ret =select(max_fd+1,&readfds,nullptr,nullptr,&tv);
if (ret<0){
	if(errno == EINTR){
		continue; // 被信号打断，重试
	}
	perror("select");
	continue;       //出错就继续下一轮
	  }
if(ret == 0){
	//select超时，清理空闲连接
	auto now = std::chrono::steady_clock::now();
	for(auto it = clients.begin(); it != clients.end(); ){
		auto idle_sec = std::chrono::duration_cast<std::chrono::seconds>(
			now - it->last_active).count();
		if(idle_sec > kIdleTimeoutSec){
			close(it->fd);
			it = clients.erase(it);
			continue;
		}
		++it;
	}
	continue;
}
//有新客户端连接
if(FD_ISSET(listen_sock,&readfds)){
int connfd=accept(listen_sock,nullptr,nullptr);

if(connfd>=0){
	if(connfd >= FD_SETSIZE){
		std::cerr<<"新连接fd超过FD_SETSIZE，关闭: "<<connfd<<std::endl;
		close(connfd);
	}else{
Client c;
c.fd =connfd;
c.has_nickname =false;
c.last_active = std::chrono::steady_clock::now();
clients.push_back(c);

std::cout<<"新客户端fd = "<<connfd<<std::endl;
	}
}else{
	if(errno != EINTR){
		perror("accept");
	}
		}
				   }
// 已有客户端发来数据
    for (auto it = clients.begin(); it != clients.end(); ) {
        int fd = it->fd;
        if (!FD_ISSET(fd, &readfds)) {
            ++it;
	continue;
				      }
	    char buf[1024];
            int n = recv(fd, buf, sizeof(buf)-1, 0);




            if (n <= 0) {
                close(fd);
                it = clients.erase(it);
                continue;
            		}
		it->last_active = std::chrono::steady_clock::now();
		buf[n]='\0';
std::string msg(buf);
while(!msg.empty() &&
	(msg.back() =='\n' || msg.back() == '\r')){
	msg.pop_back();
						  }


//第一次recv:昵称

if(!it->has_nickname){
std::string nick(buf);
while(!nick.empty() &&
	(nick.back()=='\n' || nick.back() =='\r')){
nick.pop_back();
						   }
it->nickname =nick;
it->has_nickname =true;

std::cout <<"fd "<< it->fd
	  <<" nickname = "<< it->nickname <<std::endl;

++it;
continue;//不广播

		     }

//quit命令
if (msg =="/quit"){
	std::string left =
		"[system]: " + it ->nickname +" left\n";

	for(auto iter = clients.begin(); iter != clients.end(); ){
		if(iter->fd != it->fd && iter->has_nickname){
			ssize_t s = send(iter->fd, left.c_str(), left.size(), 0);
			if(s < 0){
				if(errno == EPIPE || errno == ECONNRESET){
					close(iter->fd);
					iter = clients.erase(iter);
					continue;
				}
				perror("send");
			}
		}
		++iter;
	}
	close(it->fd);
	it = clients.erase(it);
	continue;
}

//普通聊天消息广播
std::string out ="["+it->nickname+"]: "+buf;
for(auto iter = clients.begin(); iter != clients.end(); ){
if (iter->fd != it->fd && iter->has_nickname){
	ssize_t s = send(iter->fd, out.c_str(), out.size(), 0);
	if(s < 0){
		if(errno == EPIPE || errno == ECONNRESET){
			close(iter->fd);
			iter = clients.erase(iter);
			continue;
		}
		perror("send");
	}
}
	++iter;
}
	++it;
}


}


}
