# DistributedSnapshot
Basic implementation of the Chandy Lamport Distributed Snapshot algorithm. 

## Setup

Edit `config.cfg`. Example config:

```
3000
2000
4000
5000
```
This config file contains node IDs on each new line. Ensure no trailing or leading spaces on each line. Node IDs have to be 4 digits and must be at least 2 numbers apart. 


The run the below command to compile all the source files and generate the required text files. This script can also be triggered at any point in time to reset the simulation to its orginal/initial state.
```
sh setup.sh
```

## Running the code

Each node is associated with a client and server.

In separate terminal windows/tabs run the below code with `node ID` equal to all the entries in `config.cfg`, i.e. for 5 config entries, 5 servers should be running.  

```
./server.out <node ID>
```


In separate terminal windows/tabs run the below code with `node ID` equal to all the entries in `config.cfg`, i.e. for 5 config entries, 5 clients should be running.  

```
./client.out <node ID>
```

The `client` output provides the menu options to trigger the simulations. Important control and log messages are printed out in the terminal windows. 

## Recovery Logs
The last checkpointed bank balance for every node is present in `ledger<node ID>.txt`, and the channel states or staged transactions are present in `log<node ID>.txt`


## Acknowledgements
Special thanks to https://github.com/grfz/Socket providing a  light weight wrapper library for sockets.
