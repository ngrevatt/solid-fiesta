import sys 
import os
import random
import json
import secrets # for choice and randbelow
from Crypto.Cipher import AES

names = ["Liam", "Emma", "Noah", "Olivia", "William", "Ava", 
    "James", "Isabella", "Logan", "Sophia", "Benjamin", "Mia", 
    "Mason", "Charlotte", "Elijah", "Amelia", "Oliver", "Evelyn", 
    "Jacob", "Abigail"] # top 20 names from 2017 (US Census data)

random.shuffle(names)

if (len(sys.argv) == 2):
    iterations = int(sys.argv[1])
else:
    iterations = len(names)

# https://gist.github.com/forkd/7ed4a8392fe7b69307155ab379846019
BLOCK_SIZE = 16
pad = lambda s: s + (80 - len(s) ) * \
        chr(BLOCK_SIZE - len(s) % BLOCK_SIZE)
unpad = lambda s: s[:-ord(s[len(s) - 1:])]

keys = open("keys.txt", "wb+")
data = open("data.txt", "wb+")

# life expectancy ~80 -> 18 + 62*random()
# household income ~60k -> randint between 0 and 120000
for i in range(0, iterations):
    personal_data = {
            "name": names[i],
            "age": str(18 + secrets.randbelow(62)),
            "income": str(secrets.randbelow(120000)),
            "citizen": str(secrets.choice([0,1]))
    }
    json_personal_data = json.dumps(personal_data)

    # pad the file
    json_personal_data = pad(json_personal_data)

    key = os.urandom(16) # get 16 byte key
    cipher = AES.new(key, AES.MODE_ECB) # get cipher, can I assume ECB?

    encrypted_personal_data = cipher.encrypt(json_personal_data)
    print("encrypted:" , str(encrypted_personal_data))
    print("decrypted:" , str(unpad(cipher.decrypt(encrypted_personal_data))) + "\n") # can also use .decode("ascii") to print

    keys.write((key)) # add newline char for parsing?
    data.write((encrypted_personal_data))
    
# when done, close the files
keys.close()
data.close()
