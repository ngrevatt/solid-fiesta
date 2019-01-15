#ifndef PSEUDONYMIZE_H
#define PSEUDONYMIZE_H

#define ALLOC 20 // there are 20 reccords and this is used in pseudonymize.oc as well
#define KEYBYTES 16 // 128 bit keys (16 * 8 = 128)
#define DATABYTES 80 // padded to 80 bytes (or is it chars?)

double lap; // used for time keeping
int computingParty; // for readability

typedef struct {
    char *src; // filename for text file
    char *keysrc; // filename for DPO/DC key
    uint8_t keys [ALLOC][KEYBYTES]; // change these numbers depending on amount of results
    uint8_t encrypted_data_records[ALLOC][DATABYTES];
    char decrypted_data_records[ALLOC][DATABYTES]; // uint8_t or char here?
    // struct encrypted_record encrypted_data[ALLOC];
    // struct decrypted_record decrypted_data[ALLOC];
    int numRecords; // could be updated by alloc, but I also do not intend to change alloc? (I should build this functionality in, though)
    uint8_t DPOKey [KEYBYTES]; 
    uint8_t DCKey [KEYBYTES]; 
} ProtocolIO;


double wallClock();
void pseudonymize(void* args);
void load_data(ProtocolIO *io, int party);
// needed for pseudonymize.oc, NOW uses uint8_t, like Jack's




// no longer needed ?
typedef struct {
    obliv uint8_t key_obliv[KEYBYTES];  // key
    obliv uint8_t data_record_obliv[DATABYTES]; // encrypted data
} person_obliv; // create an array of 20 of these

// void check_mem(person * fullData, int party)    

#endif



// OLD, struct based program

/*
typedef struct {
    uint8_t key_data[16];
} key;

typedef struct {
    uint8_t data[MAXBYTES];
} encrypted_record;

typedef struct { // this is populated by the obliv function
    uint8_t data[MAXBYTES]; // just a string, but later, should be a person struct
} decrypted_record;
*/


