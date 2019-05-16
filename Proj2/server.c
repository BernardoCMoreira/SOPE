#include "sope.h"
#include "types.h"
#include "constants.h"
#include "server.h"
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

#define MAX_REQUEST 1024

char *adminPWD = NULL;

tlv_request_t requestARRAY[MAX_REQUEST];
int in = 0;
int out = 0;
int requests = 0;
int slots = MAX_REQUEST;

pthread_mutex_t array_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t slots_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t requests_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t slots_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t requests_lock = PTHREAD_MUTEX_INITIALIZER;

// typedef struct accounts_array
// {
//     bank_account_t bankAccount;
//     pthread_mutex_t mutex;
// } accounts_array_t;

accounts_array_t bankAccounts[MAX_BANK_ACCOUNTS];
int indexAccount = 0;

// typedef struct bank_office
// {
//     pthread_t tid;
//     int id;
// } bank_office_t;

bank_office_t bank_offices[MAX_BANK_OFFICES];

void insertRequest(tlv_request_t request)
{
    pthread_mutex_lock(&array_lock);
    requestARRAY[in] = request;
    in = (in + 1) % MAX_REQUEST;
    pthread_mutex_unlock(&array_lock);
}

void removeRequest(tlv_request_t *request)
{
    pthread_mutex_lock(&array_lock);
    *request = requestARRAY[out];
    out = (out + 1) % MAX_REQUEST;
    pthread_mutex_unlock(&array_lock);
}

/**
 * Use one command to get the sha256sum of a string
 * This command on macOS is "shasum -a 256", in Linux is "sha256sum";
 * 
 *_______MOSTRADO NA AULA PELO PROF________
 **/
char *get_SHA256(char *command)
{
    FILE *fp;

    fp = popen(command, "r");
    if (fp == NULL)
    {
        printf("Failed to run command\n");
        exit(1);
    }

    char output[1024];

    fscanf(fp, "%[^-\n]", output);

    /* close */
    pclose(fp);

    return strdup(&output[0]);
}

/**
 * Generate one random string in alphabet = "abcdefghijklmnopqrstuvwxyz0123456789"
 **/
char *makeSalt(void)
{
    char *aux = "abcdefghijklmnopqrstuvwxyz0123456789";

    char *result = (char *)malloc(sizeof(char) * (1 + SALT_LEN));

    //Gerar o salt, com valores aleatorios. usar rand();
    for (int i = 0; i < SALT_LEN; i++)
    {
        char temp = aux[rand() % strlen(aux)];
        result[i] = temp;
    }

    result[SALT_LEN] = '\0';

    return result;
}

/**
 * Send one reply message (tlv_reply_t) to a file descriptor
 **/
void sendReplyMessage(tlv_reply_t *packet, int fd)
{
    // const int operation = packet->type;
    // int messageSize = packet->length + 4; //Espaços

    // messageSize += (operation == OP_BALANCE) ? 1 : 0;

    // char message[messageSize];

    // sprintf(message,
    //         "%d %d %d %d",
    //         packet->type,
    //         packet->length,
    //         packet->value.header.account_id,
    //         packet->value.header.ret_code);

    // if (operation == OP_BALANCE)
    // {
    //     sprintf(message, "%s %d", message,
    //             packet->value.balance.balance);
    // }

    write(fd, packet, sizeof(tlv_reply_t));
}

int buildReplyFIFO(pid_t pid)
{
    char answerFifoName[USER_FIFO_PATH_LEN];
    sprintf(answerFifoName, "%s%d", USER_FIFO_PATH_PREFIX, pid);

    int answerFIFO = open(answerFifoName, O_WRONLY | O_NONBLOCK);

    if (answerFIFO == -1)
        fprintf(stderr, "Error opening answer fifo -buildReplyFIFO-\n");

    return answerFIFO;
}

/**
 * Construir uma mensagem do tipo reply, através de uma mensagem  do tipo request!
 **/
int buildReply(tlv_reply_t *reply, const tlv_request_t *request, int retValue)
{

    reply->length = request->length;
    reply->type = request->type;
    reply->value.header.account_id = request->value.header.account_id;
    reply->value.header.ret_code = retValue;

    if (retValue == RC_OK)
    {
        switch (reply->type)
        {
        case OP_SHUTDOWN:
        {
            rep_shutdown_t *shutdown_result = &(reply->value.shutdown);
            shutdown_result->active_offices = retValue;

            break;
        }
        case OP_BALANCE:
        {
            rep_balance_t *balance_result = &(reply->value.balance);
            balance_result->balance = retValue;

            break;
        }
        case OP_TRANSFER:
        {
            rep_transfer_t *transfer_result = &(reply->value.transfer);
            transfer_result->balance = retValue;

            break;
        }
        default:
            break;
        }
    }

    return 0;
}

void *consumeRequestMessage(void *value)
{
    tlv_request_t request;
    tlv_reply_t reply;

    int userFifo;

    while (1)
    {
        pthread_mutex_lock(&requests_lock);
        while (requests <= 0)
        {
            pthread_cond_wait(&requests_cond, &requests_lock);
        }
        requests--;
        pthread_mutex_unlock(&requests_lock);

        removeRequest(&request);

        //unterpretar o pedido
        //enviar resposta
        userFifo = buildReplyFIFO(request.value.header.pid);
        buildReply(&reply, &request, RC_OK);
        logReply(STDOUT_FILENO, (int)pthread_self(), &reply);

        sendReplyMessage(&reply, userFifo);

        pthread_mutex_lock(&slots_lock);
        slots++;
        pthread_cond_signal(&slots_cond);
        pthread_mutex_unlock(&slots_lock);
    }
}

void createBankOffices(int numOffices, int fd)
{
    bank_office_t office;
    for (int i = 1; i <= numOffices; i++)
    {
        if (pthread_create(&office.tid, NULL, consumeRequestMessage, NULL) < 0)
        {
            printf("ERROR: creating threads! \n");
            exit(1);
        }
        office.id = 1;
        bank_offices[i] = office;
        logBankOfficeOpen(STDOUT_FILENO, i, bank_offices[i].tid);
        logBankOfficeOpen(fd, i, bank_offices[i].tid);
    }
}

/**
 * Must be created when server is initialized.
 * Uses "createBankAccount"
 **/
void createAdmin()
{
    bank_office_t auxiliar;
    auxiliar.id = 0;
    auxiliar.tid = pthread_self();
    createBankAccount(ADMIN_ACCOUNT_ID, 0, adminPWD);
    bank_offices[0] = auxiliar;
}

/**
 * Cria uma bankAccount com um determinado ID, saldo e password
 **/
bank_account_t createBankAccount(uint32_t id, uint32_t saldo, char *pwd)
{
    char shasum[SALT_LEN + strlen(pwd)];
    bank_account_t bank_account;
    accounts_array_t arrayaccounts;

    ///Criar uma conta de banco com os valores recebidos nos parametros;
    bank_account.account_id = id;
    bank_account.balance = saldo;

    //Juntar numa variavel a password + salt
    strcpy(bank_account.salt, makeSalt());
    strcpy(shasum, pwd);
    strcat(shasum, bank_account.salt);

    //Guardar numa variavel auxiliar o comando a utilizar para obter o sha256sum
    char var[1024];
    //codio para macOS!!!! para linux é echo -n %s | sha256sum"
    sprintf(var, "echo -n %s | shasum -a 256", shasum);

    //Obter o sha256sum
    char *hash = get_SHA256(var);
    memcpy(bank_account.hash, hash, (strlen(hash) + 1) * sizeof(char));

    arrayaccounts.bankAccount = bank_account;
    pthread_mutex_init(&arrayaccounts.mutex, NULL);

    bankAccounts[indexAccount] = arrayaccounts;
    indexAccount++;

    return bank_account;
}

void produceRequestMessage(int fifo_id)
{
    // int n;

    // do
    // {
    //     //Ler a mensagem do fifo para uma variável tlv_request_t
    //     n = read(fifo_id, &requestARRAY[0], sizeof(tlv_request_t));

    // } while (n <= 0);
    tlv_request_t request;
    while (1)
    {
        if (read(fifo_id, &request, sizeof(tlv_request_t)) > 0)
        {
            pthread_mutex_lock(&slots_lock);
            while (slots <= 0)
            {
                pthread_cond_wait(&slots_cond, &slots_lock);
            }
            slots--;
            pthread_mutex_unlock(&slots_lock);

            insertRequest(request);

            pthread_mutex_lock(&requests_lock);
            requests++;
            pthread_cond_signal(&requests_cond);
            pthread_mutex_unlock(&requests_lock);
        }
    }
}

int buildServerFIFO()
{
    unlink(SERVER_FIFO_PATH);

    if (mkfifo(SERVER_FIFO_PATH, 0666) == -1)
    { //Make fifo with read/write permissions for all users
        fprintf(stderr, "Error creating answer fifo\n");
        return -1;
    }

    int fifo_id = open(SERVER_FIFO_PATH, O_RDONLY | O_NONBLOCK);
    if (fifo_id == -1)
    {
        fprintf(stderr, "Error opening  answer fifo\n");
    }

    return fifo_id;
}

//Debug. Argument Error verification
int argVerify(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Expected 3 arguments! \n");
        return 1;
    }
    if (atoi(argv[1]) > MAX_BANK_OFFICES || atoi(argv[1]) < 0)
    {

        fprintf(stderr, "Creating not ilegal number of banks. Should be between 0 and 99!\n");
        return 1;
    }

    if (strlen(argv[argc - 1]) < MIN_PASSWORD_LEN || strlen(argv[argc - 1]) > MAX_PASSWORD_LEN)
    {
        fprintf(stderr, "Password length should be between 8 and 20! \n");
        return 1;
    }

    return 0;
}

//COPY FROM SLIDES!!
int readline(int fd, char *str)
{
    int n;
    do
    {
        n = read(fd, str, 1);
    } while (n > 0 && *str++ != '\0');
    return (n > 0);
}

//IMPLEMENTAÇÕES RECENTES QUE NÃO FORAM TESTADAS------------------------------!

int checkCreationPassword(char *pwd, tlv_request_t request)
{
    if (strcmp(pwd, request.value.header.password) != 0)
    {
        return 1;
    }

    return 0;
}
int main(int argc, char *argv[])
{
    //Verificação dos argumentos
    argVerify(argc, argv);
    adminPWD = argv[2];

    //sLog.txt file
    FILE *myServerFile = fopen("slog.txt", "w");
    if (myServerFile == NULL)
    {
        printf("Unable to create file.\n");
        return 1;
    }

    //Criar a conta do admin
    createAdmin();
    logAccountCreation(STDOUT_FILENO, 0, &bankAccounts[0].bankAccount);
    logAccountCreation(fileno(myServerFile), 0, &bankAccounts[0].bankAccount);

    createBankOffices(atoi(argv[1]), fileno(myServerFile));

    //Fifo para ler o request
    int serverFifo = 0;
    serverFifo = buildServerFIFO();

    //Ciclo de leitura de pedidos
    produceRequestMessage(serverFifo);

    //Fifo para enviar a resposta
    //Só está a responder a um pedido hardcoded.
    //TODO: generalizar

    // int userFifo;
    // userFifo = buildReplyFIFO(requestARRAY[0].value.header.pid);

    //Criar uma mensagem de resposta
    /**
     * Predefinido para teste!
     * TODO: Caso geral
     **/

    // tlv_reply_t replyMessage;
    // buildReply(&replyMessage, &requestARRAY[0], RC_OK);
    // sendReplyMessage(&replyMessage, userFifo);
    // logReply(fileno(myServerFile), getpid(), &replyMessage);
    // logReply(STDOUT_FILENO, getpid(), &replyMessage);

    // while (1)
    // {
    // }

    fclose(myServerFile);
    //close(userFifo);
    close(serverFifo);
    unlink(SERVER_FIFO_PATH);
    pthread_exit(NULL);
}
