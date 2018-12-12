import sys 
import os
import random
import json
from Crypto.Cipher import AES

names = ["Liam", "Emma", "Noah", "Olivia", "William", "Ava", 
    "James", "Isabella", "Logan", "Sophia", "Benjamin", "Mia", 
    "Mason", "Charlotte", "Elijah", "Amelia", "Oliver", "Evelyn", 
    "Jacob", "Abigail"]

if (len(sys.argv) == 2):
    iterations = int(sys.argv[1])
else:
    iterations = len(names)

# https://gist.github.com/forkd/7ed4a8392fe7b69307155ab379846019
BLOCK_SIZE = 16
pad = lambda s: s + (BLOCK_SIZE - len(s) % BLOCK_SIZE) * \
        chr(BLOCK_SIZE - len(s) % BLOCK_SIZE)
unpad = lambda s: s[:-ord(s[len(s) - 1:])]

keys = open("keys.txt", "w+")
data = open("data.txt", "w+")

# life expectancy ~80 -> 18+62*random
# household income ~60k -> 60k*random
for i in range(0, iterations):
    personal_data = {
            "name": names[i],
            "age": str(18 + random.randint(0,62)),
            "income": str(random.randint(0, 120000)),
    }
    json_personal_data = json.dumps(personal_data)

    # pad the file
    json_personal_data = pad(json_personal_data)

    key = os.urandom(16) #get 16 byte key
    cipher = AES.new(key, AES.MODE_ECB) #get cipher

    encrypted_personal_data = cipher.encrypt(json_personal_data)
    print("encrypted:" , str(encrypted_personal_data))

    print("decrypted:" , str(unpad(cipher.decrypt(encrypted_personal_data))) + "\n") # can also use .decode("ascii") to print

    keys.write(str(key)+"\n")
    data.write(str(encrypted_personal_data)+"\n")
    


# close files
keys.close()
data.close()
