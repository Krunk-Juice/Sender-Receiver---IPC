# Sender-Receiver---IPC


Christopher Ta      |       CPSC 351-01       |       cta002@csu.fullerton.edu


Programming Language: C++


Main Files:

    •   sender.cpp

    •   recv.cpp

    •   msg.h

    •   keyfile.txt

    •   Makefile

Optional Files:

    •   *.o

    •   sender

    •   recv

    •   keyfile__recv.txt

    •   reset_project.sh (Bash script to quickly delete *.o, sender, recv, and keyfile__recv.text)

How to run the project:

    1. In your Sender-Receiver---IPC directory open a terminal.
        NOTE: Make sure you are in the appropriate directory when opening the terminal. ../Sender-Receiver---IPC

    2. Type:

        make

        into the terminal and press ENTER. This compiles the sender.cpp and recv.cpp.

    3. Type:

        ./sender keyfile.txt

        into the terminal and press ENTER. This will run the sender process.

    4. Open a second terminal in the Sender-Receiver---IPC directory.
        NOTE: Again make sure you are in the correct directory when opening the terminal. ../Sender-Receiver---IPC

    5. Type:

        ./recv

        into the second terminal and press ENTER. This will run the receiver process.

    6. Once the receiver process terminates, you can check back on the first terminal to see of the sender process has terminated as well.


Test Run:

[![test-screenshot.png](https://i.postimg.cc/8czhsSBG/test-screenshot.png)](https://postimg.cc/1nbgYbj7)