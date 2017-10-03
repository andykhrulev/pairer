## What's the pairer?

pairer can receive RADIUS messages that are exchanged between AAA server and client and pairs them for monitoring purposes.
Results are send to thrift server (Project includes sample thrift server that logs paired request/response pairs to file). 

## How to build

1. Major dependancies

- freeradius (project reuses libfreeradius-radius for RADIUS packets parsing, so include files and libraries are used)
- thrift 
- boost
- cmake 3.6

2. How to build 

a. install major dependancies

b. clone project to your folder

c. set the LIBFREERADIUS_INCL variable to path to folder where the 'freeradius-devel/libradius.h' header file can be resolved, 
        e.g. if freeradius is built in the folder ~/Downloads/freeradius-server-3.0.15 then the variable shall be set:
        
    > export LIBFREERADIUS_INCL=~/Downloads/freeradius-server-3.0.15/src/

d. create makefiles by using cmake tool from the root of cloned folder:
 
    > cmake .     
       
e. build binary files from the root of cloned folder:

    > make all
 
As a result 3 binary files will be built:
  - Pairer              - main application
  - PairerSelfTest      - unit test runner
  - ThriftLoggingServer - sample logging thrift server
  
 
