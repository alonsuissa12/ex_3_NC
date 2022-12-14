#include "Sender.h"

int main(){

    char allFIle [FILE_SIZE];
    int halfSize = FILE_SIZE/2;
    char firstHalf[halfSize];
    char secondHalf[halfSize];
    int index = 0;

    //A pointer to a file.
    FILE * fPointer;

    //The pointer get the address of the file and get permition to read from it
    fPointer = fopen( "1mb_file.txt","r");

    //reading the file into 'allFile' array.
    fgets(allFIle, FILE_SIZE ,fPointer);

    //copping the first part of the array into 'firstHalf' array
    for(int i = 0; i < halfSize; i++){
        firstHalf[index] = allFIle[index];
        index++;
    }

    //copping the second part of the array into 'secondHalf' array
    for(int i = 0; i < halfSize; i++){
        secondHalf[index] = allFIle[index];
        index++;
    }

    //initializing a TCP socket.
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        printf("Could not create socket.\n");
        return -1;
    }

    struct sockaddr_in receiver_adderess;
    //setting to zero the struct senderAddress
    memset(&receiver_adderess,0,sizeof(receiver_adderess));
    receiver_adderess.sin_family = AF_INET;
    receiver_adderess.sin_port = htons(port);
    int checkP = inet_pton(AF_INET,(const char*)IP,&receiver_adderess.sin_addr);
    
    if(checkP < 0){
        printf("inet_pton() FAILED.\n");
        return -1;
    }
    
    //connecting to the Receiver on the socket
    int connectCheck = connect(sock,(struct sockaddr*) &receiver_adderess,sizeof(receiver_adderess));
    
    if(connectCheck == -1){
        printf("connect() FAILED.\n");
        return -1;
    }
    
    char again = 'y';
    char exit = 'n';
    while (again == 'y' || again == 'Y') {
        //chang CC algorithm to 'reno'
        char CC[7] ="reno";
        int checkSSO = setsockopt(sock,IPPROTO_TCP,TCP_CONGESTION,CC, strlen(CC) );
        
        if(checkSSO == -1){
            printf("setsockopt() FAILED.\n");
        }

        //Sending the first part of the file.
        printf("Sending file...\n");
        
        if(send(sock, firstHalf, sizeof(firstHalf), 0) == -1){
            printf("Error in send().\n");
            return -1;
        }

        //Authentication check.
        int gotX;
        recv(sock, &gotX, sizeof(gotX),0);
        
        if(gotX != (7351^4015)){
            printf("Authentication FAILED.\n");
            break;
        }

        //Changing the CC algorithm to 'cubic'.
        strcpy(CC, "cubic");
        int checkSSO2 = setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, CC, strlen(CC));
        
        if(checkSSO2 == -1) {
            printf("Second setsockopt() FAILED.\n");
        }

        //Sending the second part.
        if(send(sock, secondHalf, sizeof(secondHalf), 0) == -1){
            printf("send() FAILED.\n");
            return -1;
        }

        //User decision: sending the file again or exit (have to chose one of them).
        while(1){
            printf("Do u want to send again? (y/n)\n");
            
            if(scanf(" %c", &again) != 1){
                printf("Scanning FAILED!.\n");
                return -1;
            }

            //checking for proper input
            while((again != 'y') && (again != 'Y') && (again != 'N') && (again != 'n')) {
                printf("Please enter (y/n).\n");
                if(scanf(" %c", &again) != 1) {
                    printf("Scanning FAILED!.\n");
                    return -1;
                }          
            }

            if( again == 'n' || again == 'N') {
                printf("Do you want to exit? (y/n)\n");

                if (scanf(" %c", &exit) != 1) {
                    printf("Scanning FAILED!.\n");
                    return -1;
                }
                
                //checking for proper input
                while ((exit != 'y') && (exit != 'Y') && (exit != 'N') && (exit != 'n')) {
                    printf("Please enter (y/n).\n");

                    if(scanf(" %c", &exit) != 1) {
                        printf("Scanning FAILED!.\n");
                        return -1;
                    }
                }

                if (exit == 'y' || exit == 'Y') {
                    printf("Sending exit massage.\n");

                    if(send(sock, "I want to exit NOW!!!! thanks :)", 34, 0) == -1){
                        printf("send() failed;");
                        return -1;
                    }

                    //Closing TCP connection.
                    printf("Closing connection...\n");
                    close(sock);
                    printf("Connection closed.\n");

                    //Closing the file.
                    fclose(fPointer);
                    return 0;
                }
            }

            else {
                break;
            }
        }
    }

    //if Authentication is wrong

    //Sending exit message:
    printf("Sending exit massage.\n");
    send(sock, "I want to exit NOW!!!! thanks :)",34,0);

    //Closing TCP connection:
    printf("Closing connection...\n");
    close(sock);
    printf("Connection closed.\n");

    //Closing the file.
    fclose(fPointer);
    return -1;
}