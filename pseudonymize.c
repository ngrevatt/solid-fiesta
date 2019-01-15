#include <stdio.h>
#include <stdlib.h> 
#include <obliv.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "util.h" // wallClock() and testTCP function
#include "dbg.h" // pretty debugging
#include "pseudonymize.h"

//#define BUFFER_SIZE 64 // this should be enough? emails are hopefully 20 characters max . . .

/*
void check_mem(person* fullData, int party) 
{
    if (fullData == NULL) {
        log_err("Memory allocation failed\n");
        clean_errno();
        exit(1);
    }
}
*/


// < POPULATE io from src file > 
void load_data(ProtocolIO *io, int party) 
{
    // open appropriate input file
    FILE * inputFile = fopen(io->src, "rb");
    if (inputFile == NULL) { // return error if null;
        log_err("File '%s' not found \n", io->src);
        clean_errno();
        exit(1);
    }

    FILE * KeyFile = fopen(io->keysrc, "rb");
    if (KeyFile == NULL) { // return error if null;
        log_info("File '%s' not found \n", io->keysrc);
        clean_errno();
        exit(1);
    }

    // size_t len = 0;
    // size_t read;
    io->numRecords = 0;
    int count = 0;
    size_t bytes = 0;

    if (party == 1) { // DPO has keys
        unsigned char buffer[KEYBYTES]; // 128 bit keys = 16 bytes each?
        unsigned char keybuffer[KEYBYTES];

        while((bytes = fread(keybuffer, sizeof(*keybuffer), sizeof(keybuffer), KeyFile)) == KEYBYTES) {
            printf("%d bytes were read\n", bytes);
            memcpy(&io->DPOKey, (void*)keybuffer, sizeof(keybuffer));
            printf("KeyFile is %s\n", keybuffer);
        }

        while((bytes = fread(buffer, sizeof(*buffer), sizeof(buffer), inputFile)) == KEYBYTES) {
            printf("%d bytes were read\n", bytes);
            // copy from buffer into start of corresponding io->key
            memcpy(&io->keys[count][0], (void*)buffer, sizeof(buffer));
            count += 1;
            printf("count is: %d and buffer is %s\n", count, buffer);
        }

    }

    if (party == 2) { // DC has data records
        unsigned char buffer[DATABYTES];
        unsigned char keybuffer[KEYBYTES];

        while((bytes = fread(keybuffer, sizeof(*keybuffer), sizeof(keybuffer), KeyFile)) == KEYBYTES) {
            printf("%d bytes were read\n", bytes);
            memcpy(&io->DCKey, (void*)keybuffer, sizeof(keybuffer));
            printf("KeyFile is %s\n", keybuffer);
        }
        
        // read 1 bytes * # databytes  
        while((bytes = fread(buffer, sizeof(*buffer), sizeof(buffer), inputFile)) == DATABYTES) {
            printf("%d bytes were read\n", bytes);
            // copy from buffer into start of corresponding io->encrypted_data_record
            memcpy(&io->encrypted_data_records[count][0], (void*)buffer, sizeof(buffer));
            count += 1;
            printf("count is: %d and buffer is %s\n", count, buffer);
        }

    }

    // could also dynamically allocate io depending on # of data records read
    io->numRecords = count; // count started at 0, so add 1
    log_info("Loaded %d data points . . . \n", io->numRecords);
    fclose(inputFile);
    fclose(KeyFile);
}


int main(int argc, char *argv[])
{

    ProtocolDesc pd;
    ProtocolIO io;

    // get connection info
    const char *remote_host = strtok(argv[1], ":");
    const char *port = strtok(NULL, ":");

    // Make connection between two shells (this is similar to ocTestUtilTcpOrDie)
    log_info("Connecting to %s on port %s . . . \n", remote_host, port);
    if(argv[2][0] == '1') {
        if(protocolAcceptTcp2P(&pd, port) != 0) {
            log_err("TCP accept from %s failed \n", remote_host);
            exit(1);
        }
    } 

    else { 
        if(protocolConnectTcp2P(&pd, remote_host, port) != 0) {
            log_err("TCP accept from %s failed \n", remote_host);
            exit(1);
        }
    }

    // get and set party
    computingParty = (argv[2][0]=='1' ? 1 : 2);
    setCurrentParty(&pd, computingParty); // only checks for a '1' (this is okay because Obliv-C only supports 2 parties atm)

    // put filename in io
    if (argc == 5) {  // this applies to both parties
        io.src = argv[3]; 
        io.keysrc = argv[4];
    }

    // load data into io (not inside the timer)
    load_data(&io, computingParty);

    // start timer
    struct timespec t1;
    clock_gettime(CLOCK_REALTIME,&t1);
    double start = t1.tv_sec+1e-9*t1.tv_nsec; 

    // alternatively "execDualexProtocol()"
    // execDualexProtocol(&pd, pseudonymize, &io); // Should this be Yao's or DualExecution? 
    execYaoProtocol(&pd, pseudonymize, &io); // Should this be Yao's or DualExecution? 
    log_info("returned from MPC\n");
    cleanupProtocol(&pd);

    // calculate time
    struct timespec t2;
    clock_gettime(CLOCK_REALTIME,&t2);
    double end = (t2.tv_sec+1e-9*t2.tv_nsec);

    log_info("Start time: %f seconds \n", start);
    log_info("End time: %f seconds \n", end);

    // double runtime = (double)(end-start);
    /*
    double runtime = 0;
    log_info("Total time: %f seconds \n", end);
    */

    // output info
    // log_info("Pseudonymization Successful\n");

    /*
    // write unencrypted records to a file
    if (computingParty == 1) { // 1 because returned to both, but I only need to write once
        FILE *outputFile = fopen("decrypted_records.txt", "w+");

        if (outputFile != NULL) {
            // probably writes decrypted records (in json?) to a file?
            for (int i = 0; i < io.numRecords; i++) {
                fprintf(outputFile, "%s\n", io.decrypted_data_records[i]);
            }
        }

        fclose(outputFile);
    }
    */

    // log_info("done, about to return 0\n");
    log_info("Total time: %f seconds\n", end - start);

    
    return 0;   
}  


