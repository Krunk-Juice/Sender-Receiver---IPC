# Sender-Receiver---IPC


Christopher Ta      |       CPSC 351-01       |       cta002@csu.fullerton.edu

Gilbert Paderogo    |       CPSC 351-01       |       gspaderogo@csu.fullerton.edu

Bryan Cuevas        |       CPSC 351-01       |       bcuevas8@csu.fullerton.edu


Programming Language: C++

Operating System: Linux


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

    0. In your Sender-Receiver---IPC directory open a terminal.
        NOTE: Make sure you are in the appropriate directory when opening the terminal. ../Sender-Receiver---IPC

    1. (Optional) Before you continue look in to the Sender-Receiver---IPC directory and see if there are existing sender.o, recv.o, sender, recv, and keyfile__recv.txt.
        If those file exist in the directory, delete those files or type "./reset_project.sh" in to the terminal and press ENTER.
        If those files do not exist in the directory, continue to step 2.

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

    6. Once the receiver process terminates, you can check back to the first terminal to see of the sender process has terminated as well.


Test Run:

[![test-screenshot.png](https://i.postimg.cc/WzxTNKBC/test-screenshot.png)](https://postimg.cc/D4Qt5CyP)


Team Collaborators

Christopher Ta: Coding(recv.cpp and sender.cpp), testing, step process documentation

Gilbert Paderogo: Coding(recv.cpp), testing, data flow diagram documentation

Bryan Cuevas: Glossary definitions documentation, testing