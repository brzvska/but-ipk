## HTTP Server in C that provides information about the system.
BUT IPK project 1
Author: Berezovska Anastasiia (xberez04).

---
Server provides functionality for those queries:
```
cpu-name
hostname
load  
```
## Get started
#### Compilation
For compilation use command `make`.
#### Run
To run server use `./hinfosvc port &` where `port` is a positive number e.g. 12345.

## Usage
Here are a few examples of useful commands.

* **Via terminal**
    * _**load**_
`curl http://localhost:port/load`
    * _**hostname**_
`curl http://localhost:port/hostname`
    * _**cpu name**_  
`curl http://localhost:port/cpu-name`

* **Via web page**
You need to go to web page(depends on query): 
    * _**load**_
    `http://localhost:port/load`
    * _**hostname**_
    `http://localhost:port/hostname`
    * _**cpu name**_
    `http://localhost:port/cpu-name`
     
