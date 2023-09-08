# parsing_CplusV2

**Description**
ParsingPlusV2 is a C++ program designed to read a data file provided as a command line argument and process its contents line by line. 
It performs several operations to ensure data integrity and relevance before extracting meaningful information. 
This README file provides an overview of how the program works, its functionalities, and instructions on how to use it.

**Features**
Reads a data file specified as a command line argument.
Parses the file contents line by line into tokens.
Verifies the header to determine if the data is intended for processing.
Performs a CRC check to verify the integrity of the data.
Processes data based on the second byte to identify sensor data or parameter data.
Provides an option to calculate CRC values for new data lines to be added to the data file.
Usage
To run the ParsingPlusV2 program, follow these steps:

Compile the code: Ensure you have a C++ compiler installed on your system.

g++ -o parsingplusV2 parsingplusV2.cpp
Run the program with the data file as a command line argument:

./parsingplusV2 [DataFileName]
Replace [DataFileName] with the path to your data file.

The program will process the data file as described below:

It checks the header to verify that the data is intended for processing.
It performs a CRC check to ensure data integrity.
Based on the second byte, it determines whether the data is sensor data or parameter data and processes it accordingly.
If you need to calculate CRC values for new data lines to be added to the data file, follow the instructions below:
To calculate CRC values for new data lines:

Open the parsingplusV2.cpp file in a text editor.

Modify the following line:

for (size_t i = 0; i < bytVals.size(); i++)

to

for (size_t i = 0; i < bytVals.size() - 1; i++)

Uncomment the following line:

//std::cout << "crc in loop " << std::hex << static_cast<int>(currByte) << " is " << std::hex << static_cast<int>(crc) << std::endl;
Recompile the code as shown in step 1.

Run the program as described in step 2 with the data file containing the new line.

The program will calculate and display CRC values for all data lines in the file.

After calculating CRC values, don't forget to undo the changes made in step 4 by reverting the code to its original state.

**Example**
Here's an example command to run the program:

./parsingplusV2 dataPackets.txt
This will process the dataPackets.txt file, perform header verification, CRC checks, and process the data based on the second byte.
