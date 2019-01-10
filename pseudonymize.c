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

    // size_t len = 0;
    // size_t read;
    io->numRecords = 0;
    int count = 0;

    if (party == 1) { // DPO has keys
        unsigned char buffer[16]; // 128 bit keys = 16 bytes each?

        while((fread(&buffer, 1, KEYBYTES, inputFile)) == KEYBYTES) {
            printf("%s", buffer);
            // copy from buffer into start of corresponding io->key
            memcpy(&io->keys[count][0], (void*)buffer, sizeof(buffer));
            count += 1;
            printf("count is: %d\n", count);
        }
    }

    if (party == 2) { // DC has data records
        unsigned char buffer[80];

        // read 1 bytes * # databytes  
        while((fread(&buffer, 1, DATABYTES, inputFile)) == 80) {
            printf("%s", buffer);
            // copy from buffer into start of corresponding io->encrypted_data_record
            memcpy(&io->encrypted_data_records[count][0], (void*)buffer, sizeof(buffer));
            count += 1;
            printf("count is: %d\n", count);
        }
    }

    // could also dynamically allocate io depending on # of data records read
    io->numRecords = count; // count started at 0, so add 1
    log_info("Loaded %d data points . . . \n", io->numRecords);
    fclose(inputFile);
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
    if (argc == 4) {  // this applies to both parties
        io.src = argv[3]; 
    }

    // load data into io (not inside the timer)
    load_data(&io, computingParty);

    // start timer
    // double lap = wallClock();

    execYaoProtocol(&pd, pseudonymize, &io); // Should this be Yao's or DualExecution? 
    cleanupProtocol(&pd);

    // calculate time
    // double runtime = wallClock() - lap;
    

    // output info
    log_info("Pseudonymization Successful\n");
    // log_info("Total time: %1f seconds \n", runtime);

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

    
    return 0;   
}  


