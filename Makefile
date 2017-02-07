INCLUDE = ./src
SRCDIR = $(INCLUDE)

SRC_UX = \
	$(SRCDIR)/ux_sim.c \
	$(SRCDIR)/ipc.c 

SRC_UDPSV = \
	$(SRCDIR)/udp_server_sim.c

SRC_TSAMPLE = \
	$(SRCDIR)/main.c \
	$(SRCDIR)/ipc.c \
	$(SRCDIR)/network.c \
	$(SRCDIR)/led.c

all:
	if [ ! -d "./build" ]; then \
		mkdir build; \
	fi
	gcc -I$(INCLUDE) $(SRC_UX) -o build/ux
	gcc -I$(INCLUDE) $(SRC_UDPSV) -o build/udp_server_sim
	gcc -I$(INCLUDE) $(SRC_TSAMPLE) -o build/TestSample 

