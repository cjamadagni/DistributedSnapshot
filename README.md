# DistributedSnapshot
Basic simulation of the Chandy Lamport Distributed Snapshot algorithm. Realized through a basic banking/ledger application.

## Compilation/Build

Cmake is being used for building. Run the below commands.

1. `cmake .`
2. `make`


## Setup

Edit `config.cfg`. This config files contains the IP address and port of all nodes in the network. The IP and port needs to be separated by a single space. NOTE: if running on the same machine, make sure each port number is separated by a difference of atleast 2. Example config:

```
127.0.0.1 3000
192.168.10.3 2000
192.168.10.2 4000
``` 

Run the below command to generate new log files and ledger files. These files are generated in the `Data` directory. This script can also be triggered at any point in time to reset the simulation to its orginal/initial state.
```
sh setup.sh
```

## Running the code

Each node is associated with a client and server. The server is responsible for logging and state changes to the ledger, while the client application is the UI to trigger simulation components.

In separate terminal windows/tabs run the below code with `node ID` equal to all the port numbers entered in `config.cfg`, i.e. for 5 config entries, 5 servers should be running. 

NOTE: port numbers double up as the node IDs

```
./server.out <node ID>
```


In separate terminal windows/tabs run the below code with `node ID` equal to all the port numbers entered in `config.cfg`, i.e. for 5 config entries, 5 clients should be running.  

```
./client.out <node ID>
```
NOTE: Important control and log messages are printed out in the terminal windows. 

## Recovery Logs
The last checkpointed bank balance for every node is present in `ledger<node ID>.txt`, and the channel states or staged transactions are present in `log<node ID>.txt`


## Acknowledgements
Special thanks to https://github.com/grfz/Socket for giving me a foundation to write my own lightweight socket library.
