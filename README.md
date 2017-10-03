## What's the pairer?

pairer can receive RADIUS messages that are exchanged between AAA server and client and pairs them for monitoring purposes.
Results are send to thrift server (Project includes sample thrift server that logs paired request/response pairs to file). 

## How to build

1. Major dependancies

- freeradius (project reuses libfreeradius-radius for RADIUS packets parsing, so include files and libraries are used)
- thrift 
- boost
- cmake 3.6

2. Build steps

- install major dependancies

- clone project to your folder

- set the LIBFREERADIUS_INCL variable to path to folder where the 'freeradius-devel/libradius.h' header file can be resolved, 
        e.g. if freeradius is built in the folder ~/Downloads/freeradius-server-3.0.15 then the variable shall be set:
        
    > export LIBFREERADIUS_INCL=~/Downloads/freeradius-server-3.0.15/src/

- create makefiles by using cmake tool from the root of cloned folder:
 
    > cmake .     
       
- build binary files from the root of cloned folder (Catch unittest project will be downloaded from github during building):

    > make all
 
As a result 3 binary files will be built:
  - Pairer              - main application
  - PairerSelfTest      - unit test runner
  - ThriftLoggingServer - sample logging thrift server
  
## How to run

1. Update the configuration file (pairer.ini)

2. Run Thrift server
> ./ThriftLoggingServer

3. Run pairer
> ./Pairer

## How to Test

1. Unittesting
> ./PairerSelfTest

2. Use radius client or seagull sending requests to pairer

3. Use proxy mode on pairer to proxy and store RADIUS requests/responses between radius client and radius server

- enable proxy mode by setting the ProxyModeEnabled to 'true'
- please configure radius server ip/port (the pairer will forward request to/responses from) by updating the proxy section of ini file

