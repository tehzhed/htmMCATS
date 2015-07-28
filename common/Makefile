ifndef TOTAL_ATTEMPTS
    TOTAL_ATTEMPTS=-DTOTAL_ATTEMPTS=5
endif

ifndef APRIORI_LOCK_ATTEMPTS
    APRIORI_LOCK_ATTEMPTS=-DAPRIORI_LOCK_ATTEMPTS=2
endif

ifndef NUMBER_THREADS
	NUMBER_THREADS=-DNUMBER_THREADS=8
endif

ifndef NUMBER_ATOMIC_BLOCKS
	NUMBER_ATOMIC_BLOCKS=-DNUMBER_ATOMIC_BLOCKS=10
endif

ifndef TXS_PER_TUNING_CYCLE
	TXS_PER_TUNING_CYCLE=-DTXS_PER_TUNING_CYCLE=1000
endif


CFLAGS   += -w $(TOTAL_ATTEMPTS) $(APRIORI_LOCK_ATTEMPTS) $(NUMBER_THREADS) $(NUMBER_ATOMIC_BLOCKS) $(TXS_PER_TUNING_CYCLE)

CPPFLAGS := $(CFLAGS)
LIBS     += -lrt -lpthread -mrtm -lm


.PHONY: default
default: $(PROG)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(PROG) $(OUTPUT)

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $(PROG)

include ../common/Makefile.common


# ==============================================================================
#
# Makefile.rtm
#
# ==============================================================================

