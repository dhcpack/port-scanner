/*
编程实现TCP全连接扫描（也叫TCP Connect扫描），对指定目标主机实现端口扫描。
要求是：
操作系统：windows
TCP全连接扫描：Socket客户端，协议格式为TCP
判断端口是否开放：判断Socket连接是否成功
对指定目标主机实现端口扫描：可以设置Socket客户端要连接的服务器IP和端口，在一定范围内进行循环扫描
*/

/*
    编译指令：gcc '.\scanner.c' -l wsock32 -o '.\scanner.exe'
    使用方法：Usage: .\scanner.exe <host> <start_port> <end_port>
    example:
        '.\scanner.exe' 127.0.0.1 78 85
        '.\scanner.exe' 127.0.0.1 1 10000 > res.txt
*/

#include <stdio.h>
#include <winsock2.h>

#define MAX_PORT (1 << 16)
#define TIMEOUT 1500
char host[16];
int open_cnt, open_ports[MAX_PORT];

DWORD WINAPI scan(LPVOID lpParameter) {
    int port = (int) lpParameter;

    WSADATA wsadata;
    int ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (ret != 0) {
        fprintf(stderr, "WSAStartup failed with errno: %d\n", ret);
        return -1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "socket failed with errno: %d\n", WSAGetLastError());
        return -1;
    }
    int timeout = TIMEOUT;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

    printf("Scanning %s:%d\n", host, port);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host);
    addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) == 0) open_ports[port] = 1;
    closesocket(sock);

    WSACleanup();
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <host> <start_port> <end_port>\n", argv[0]);
        exit(1);
    }

    int start_port = atoi(argv[2]), end_port = atoi(argv[3]);
    strcpy(host, argv[1]);
    if (start_port < 1) {
        fprintf(stderr, "The range of ports is [1, 65535], start from 0 instead\n");
        start_port = 1;
    }
    if (end_port >= MAX_PORT) {
        fprintf(stderr, "The range of ports is [1, 65535], end with 65535 instead\n");
        end_port = MAX_PORT - 1;
    }

    printf("Scan Begin!\n");

    int thread_count = end_port - start_port + 1;
    printf("Start %d thread(s)\n", thread_count);
    HANDLE *thread_handles = malloc(thread_count * sizeof(HANDLE));
    for (int i = start_port; i <= end_port; i++) {
        int port = i;
        thread_handles[i - start_port] = CreateThread(NULL, 0, scan, (PVOID) port, 0, NULL);
        if (thread_handles[i - start_port] == NULL) {
            i--;
        }
    }

    WaitForMultipleObjects(thread_count, thread_handles, TRUE, INFINITE);
    for (int i = 0; i < thread_count; i++) CloseHandle(thread_handles[i]);
    free(thread_handles);

    printf("Scan Complete!\n");
    for (int i = 0; i < MAX_PORT; i++)
        if (open_ports[i]) {
            open_cnt++;
            printf("%d ", i);
        }
    printf("\n%d open port(s) in total\n", open_cnt);
    return 0;
}
