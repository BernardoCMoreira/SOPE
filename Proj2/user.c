#include "sope.h"
#include "types.h"
#include "constants.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#define FALSE 0
#define TRUE 1

int alarmReceived = FALSE;

// Called by buildMessageFromArguments
static int buildHeader(tlv_request_t *message, char *argv[], pid_t pid);
static int addExtraInfo(tlv_request_t *message, char *argv);
static void adjustMessageSize(tlv_request_t *message);

int createUserFile(FILE *file)
{
    file = fopen("ulog.txt", "w");

    if (file == NULL)
    {
        printf("Unable to create file.\n");
        return 1;
    }
    return 0;
}
int buildMessageFromArguments(tlv_request_t *message, int argc, char *argv[], pid_t pid)
{
    if (argc != 6)
    {
        fprintf(stderr, "Wrong number of arguments. Expected: ./user <id> <password> <delay> <operation> [extra arguments]\n");
        return 1;
    }

    // Make message Header
    if (buildHeader(message, argv, pid))
        return 1;

    // Read operation
    enum op_type operation = __OP_MAX_NUMBER;

    sscanf(argv[4], "%d", (int *)&operation);

    if (operation == __OP_MAX_NUMBER)
    {
        fprintf(stderr, "Error reading operation info\n");
        return 1;
    }
    message->type = operation;

    // Add extra info depending on operation
    if (addExtraInfo(message, argv[5]))
        return 1;

    // Correct message size
    adjustMessageSize(message);

    return 0;
}

int buildServerFIFO()
{
    int serverFIFO = open(SERVER_FIFO_PATH, O_WRONLY | O_NONBLOCK);
    //int serverFIFO = open(SERVER_FIFO_PATH, O_WRONLY);
    if (serverFIFO == -1)
    {
#ifdef DEBUG
        perror("Error opening server fifo: ");
#endif
        fprintf(stderr, "Error opening server fifo\n");
    }

    return serverFIFO;
}

int buildAnswerFIFO(pid_t pid)
{
    char answerFifoName[USER_FIFO_PATH_LEN];
    sprintf(answerFifoName, "%s%d", USER_FIFO_PATH_PREFIX, pid);

    if (mkfifo(answerFifoName, 0666) == -1)
    {
        fprintf(stderr, "Error creating answer fifo");
        return -1;
    }

    int answerFIFO = open(answerFifoName, O_RDONLY | O_NONBLOCK);
    if (answerFIFO == -1)
        fprintf(stderr, "Error opening answer fifo\n");

    return answerFIFO;
}

void sendRequestMessage(tlv_request_t *packet, int destinationFD)
{

    const int operation = packet->type;
    int messageSize = packet->length + 5; // 5 spaces for value separation
    // Add more spaces if extra info is used
    messageSize += (operation == OP_CREATE_ACCOUNT) ? 3 : 0;
    messageSize += (operation == OP_TRANSFER) ? 2 : 0;
    char message[messageSize];
    sprintf(message,
            "%d %d %d %d %s %d",
            packet->type,
            packet->length,
            packet->value.header.pid,
            packet->value.header.account_id,
            packet->value.header.password,
            packet->value.header.op_delay_ms);

    if (operation == OP_CREATE_ACCOUNT)
        sprintf(message, "%s %d %d %s",
                message,
                packet->value.create.account_id,
                packet->value.create.balance,
                packet->value.create.password);
    else if (operation == OP_TRANSFER)
        sprintf(message, "%s %d %d",
                message,
                packet->value.create.account_id,
                packet->value.create.balance);

    write(destinationFD, packet, sizeof(*packet));
}

// Functions used by buildMessageFromArguments
static int buildHeader(tlv_request_t *message, char *argv[], pid_t pid)
{
    // Set pid
    message->value.header.pid = pid;

    // Set account ID

    int accountID = -1;
    accountID = atoi(argv[1]);

    if (accountID == -1)
    {
        fprintf(stderr, "Error reading id info\n");
        return 1;
    }
    message->value.header.account_id = accountID;

    // Set password

    if (strlen(argv[2]) > MAX_PASSWORD_LEN || strlen(argv[2]) < MIN_PASSWORD_LEN)
    {
        fprintf(stderr, "Password is out of limits\n");
        return 1;
    }
    if (sscanf(argv[2], "%s", message->value.header.password) == EOF)
    {
        fprintf(stderr, "Error reading password info\n");
        return 1;
    }

    // Set Delay

    int delay = -1;

    delay = atoi(argv[3]);

    if (delay == -1 || delay > MAX_OP_DELAY_MS)
    {
        fprintf(stderr, "Error reading delay info\n");
        return 1;
    }
    message->value.header.op_delay_ms = delay;

    return 0;
}

static int addExtraInfo(tlv_request_t *message, char *argv)
{
    int extraID = -1;
    int balance = -1;
    char password[MAX_PASSWORD_LEN + 1];

    switch (message->type)
    {
    case (OP_CREATE_ACCOUNT):
    {
        if (sscanf(argv, "%d %d %s", &extraID, &balance, password) == -1 || extraID == -1 || balance == -1)
        {
            fprintf(stderr, "Error reading extra info\n");
            return 1;
        }

        if (strlen(password) > MAX_PASSWORD_LEN || strlen(password) < MIN_PASSWORD_LEN)
        {
            fprintf(stderr, "New password is out of limits\n");
            return 1;
        }

        message->value.create.account_id = extraID;
        message->value.create.balance = balance;
        sscanf(password, "%s", message->value.create.password);
        printf("Password = %s\n", password);
        printf("MSGpassword = %s\n", message->value.create.password);
    }
    break;
    case (OP_TRANSFER):
    {
        sscanf(argv, "%d%d", &extraID, &balance);
        if (extraID == -1 || balance == -1)
        {
            fprintf(stderr, "Error reading extra info\n");
            return 1;
        }
        message->value.transfer.account_id = extraID;
        message->value.transfer.amount = balance;
    }
    break;
    default:
        break;
    }
    return 0;
}

static void adjustMessageSize(tlv_request_t *message)
{
    message->length = sizeof(*message);

    // remove non-necessary characters of password

    message->length -= (MAX_PASSWORD_LEN - strlen(message->value.header.password));
    switch (message->type)
    {
    case (OP_CREATE_ACCOUNT):
        // remove non-necessary characters of new password
        message->length -= (MAX_PASSWORD_LEN - strlen(message->value.create.password));
        break;
    case (OP_TRANSFER):
        message->length -= sizeof(message->value.create.password);
        break;
    default:
        message->length -= sizeof(message->value) - sizeof(message->value.header);
        break;
    }
}

//Modifies alarmReceived value if catch a signal
static void catchSignal(int sign)
{
    printf("Caught the %d signal\n", sign);
    printf("[ALARM RECEIVED]\n");
    alarmReceived = TRUE;
}
int main(int argc, char *argv[])
{
    pid_t pid = getpid();
    int serverFIFO;
    int answerFIFO;
    tlv_request_t message;
    FILE *myUserFile = fopen("ulog.txt", "w");
    createUserFile(myUserFile);

    // 1. Verify arguments and build message
    if (buildMessageFromArguments(&message, argc, argv, pid))
        return 1;

    // 2. Open server FIFO
    serverFIFO = buildServerFIFO();
    if (serverFIFO == -1)
        return 1;

    // 3. Make and open answer FIFO
    answerFIFO = buildAnswerFIFO(pid);

    if (answerFIFO == -1)
    {
        close(serverFIFO);
        return 1;
    }

    // 4. Send Message, close server fifo and create alarm
    sendRequestMessage(&message, serverFIFO);

    //int alarmReceived = FALSE;
    alarm(FIFO_TIMEOUT_SECS);

    signal(SIGALRM, catchSignal);

    close(serverFIFO);

    // //Send the request. write to file
    logRequest(fileno(myUserFile), pid, &message);
    //write(serverFIFO, &message, sizeof(message));
    // //Send the request. write to terminal NOT NEED!
    logRequest(1, pid, &message);

    // 5. Wait and Read response
    int msgReceived = FALSE;

    /**
     * Enquanto não receber o alarme -> 30 segundos de espera
     * ou 
     * Enquanto não receber uma mensagem 
     * Continua a ler
     **/
    while (!alarmReceived && !msgReceived)
    {

        tlv_reply_t answer[sizeof(tlv_reply_t)]; //TODO : TROCAR POR UMA TLV_REPLY_T
        int n;
        int i = 0;

        printf("Waiting for answer message, or alarm! \n");

        do
        {
            n = read(answerFIFO, &answer, sizeof(tlv_reply_t));
            i++;

        } while (n <= 0 && !alarmReceived);

        if (i >= 1)
        {
            printf("[MESSAGE RECEIVED]\n");
            msgReceived = TRUE;
            alarm(0);
        }
    }
    //fclose(answerFfile);

    // 6. Close and delete FIFO

    char answerFifoName[USER_FIFO_PATH_LEN];
    sprintf(answerFifoName, "%s%d", USER_FIFO_PATH_PREFIX, pid);
    close(answerFIFO);
    if (unlink(answerFifoName) == -1)
    {
        fprintf(stderr, "Error deleting answer fifo\n");
        return 1;
    }

    fclose(myUserFile);

    return 0;
}