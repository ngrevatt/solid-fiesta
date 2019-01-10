CILPATH=~/obliv-c
OBLIVCC=$(CILPATH)/bin/oblivcc
RUNTIME=$(CILPATH)/_build/libobliv.a
CFLAGS=-g -Wall -D_Float128=double
LOADLIBES=-lm

ACKPATH=~/absentminded-crypto-kit

PORT=1234
REMOTE_HOST=localhost
CONNECTION=$(REMOTE_HOST):$(PORT)

# something like this... (fix "test")
# ./pseudonymize.out: test.oc test.c util.c $(ACKPATH)/build/lib/liback.a $(RUNTIME)
# 	$(OBLIVCC) $(CFLAGS) -I . $(ACKPATH)/src/ test.oc test.c util.c $(LOADLIBES) pseudonymize.oc $(ACKPATH)/src/oaes.oc
# what should I have in my util.c / util.h? maybe the wallClock?

./pseudonymize.out: pseudonymize.oc pseudonymize.c util.c $(ACKPATH)/build/lib/liback.a $(RUNTIME)
	$(OBLIVCC) $(CFLAGS) -I . -I $(ACKPATH)/src/ pseudonymize.oc pseudonymize.c $(LOADLIBES) $(ACKPATH)/src/oaes.oc

./process.out:


clean:
	rm -f a.out 

new_full_data:
	python3 generateinputs.py


# test pseudonymization (run generateinputs.py beforehand, for ex. "make new_full_data")

DPO_pseudonymize:
	./a.out $(CONNECTION) 1 keys.txt
	# $(./pseudonymize.out)

DC_pseudonymize:
	./a.out $(CONNECTION) 2 data.txt
	# $(./pseudonymize.out)


# test data processing (run after createing pseudonymized data)
#DP_process:
#	$(./process.out)
#	./process.out $(CONNECTION) 1 $(nonidentifiable.csv) $(pseudonymized.csv) 
	
#DPO_process:
#	$(./process.out)
#	./process.out $(CONNECTION) 2 $(ENCRYPTION_KEY)  
