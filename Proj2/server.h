#include "sope.h"
#include "constants.h"
#include "types.h"

typedef struct accounts_array
{
    bank_account_t bankAccount;
    pthread_mutex_t mutex;
} accounts_array_t;

typedef struct bank_office
{
    pthread_t tid;
    int id;
} bank_office_t;

void insertRequest(tlv_request_t request);

void removeRequest(tlv_request_t *request);

char *get_SHA256(char *command);

char *makeSalt(void);

void sendReplyMessage(tlv_reply_t *packet, int fd);

int buildReplyFIFO(pid_t pid);

int buildReply(tlv_reply_t *reply, const tlv_request_t *request, int retValue);

void *consumeRequestMessage(void *value);

void createBankOffices(int numOffices, int fd);

bank_account_t createBankAccount(uint32_t id, uint32_t saldo, char *pwd);

void createAdmin();

void produceRequestMessage(int fifo_id);

int buildServerFIFO();

int argVerify(int argc, char *argv[]);

int readline(int fd, char *str);

//IMPLEMENTAÇÕES RECENTES QUE NÃO FORAM TESTADAS:

int checkCreationPassword(char *pwd, tlv_request_t request);