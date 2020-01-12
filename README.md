# Simple Web Server

Implementation of a simple HTTP Server using programming language C.<br>
The server can serve simple static file and show file list if the request is a directory.<br>
It can also print the HTTP request and response message in the terminal.<br>
Their is an article examplaining how it implemented.<br>
https://medium.com/@feveraly/%E7%94%A8c%E5%AF%A6%E4%BD%9C%E4%B8%80%E5%80%8B%E7%B0%A1%E5%96%AE%E7%9A%84http-server-35dc27bc1a80

![](https://i.imgur.com/FwU36m9.png)
![](https://i.imgur.com/xQhP9Jp.jpg)

## Build
Successfully build on **Ubuntu 16.04** and **Arch Linux**.<br>
Did Not test on other linux platform but normally it would work.<br>
- `make`

## Run
- simply run with default port(8080) and default file path(./)<br>
`./webserver`<br>
- identify port number and directory path<br>
`./webserver 8787 ./path/to/file/folder`<br>
