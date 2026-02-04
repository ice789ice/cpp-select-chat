#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
int main(){
	//创建socket(TCP)
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	//准备服务器地址结构
sockaddr_in serv{};
serv.sin_family =AF_INET;
serv.sin_port =htons(1234);
inet_pton(AF_INET,"127.0.0.1",&serv.sin_addr);
//连接服务器
connect(sockfd,(sockaddr*)&serv,sizeof(serv));
//提示输入昵称
std::cout << "Welcome to SmallChat!\n";
std::cout << "Please enter your nickname and press Enter:\n";

fd_set readfds;
std::string buf(10024,'\0');

bool nickname_sent =false;
//主循环：同时监听键盘输入+socket数据
while(true){
FD_ZERO(&readfds);
//监听键盘输入
FD_SET(STDIN_FILENO,&readfds);
//监听socket(服务器消息)
FD_SET(sockfd,&readfds);
//select 阻塞等任意一个事件发生
select(sockfd+1,&readfds,nullptr,nullptr,nullptr);
//键盘有输入，发送给服务器
if(FD_ISSET(STDIN_FILENO,&readfds)){
int n=read(STDIN_FILENO,buf.data(),buf.size());
if(n>0){
send(sockfd,buf.data(),n,0);
	if(!nickname_sent){
	   nickname_sent =true;
	  std::cout <<"You can start chatting now.\n";
	  std::cout<<"Type /quit to exit.\n";
				}
	}

}
//socket可读，服务器发来消息
if(FD_ISSET(sockfd,&readfds)){
int n = recv(sockfd,buf.data(),buf.size(),0);
if(n<=0){
//服务器关闭连接
break;
	}
std::cout.write(buf.data(),n);
			}
}
close(sockfd);
}





