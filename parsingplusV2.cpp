#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <bitset>

typedef uint8_t byte;

enum DataType {
    SENSOR_DATA = 0xAA,
    PARAMETER_DATA = 0xBB
};

union IEEE754
{
    struct segments
    {
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign : 1;
    }segments;
    float fValue = 0;
};

class Sensor_Data
{
private:
    uint8_t header;
    uint8_t Identifier;
    uint8_t sensorId;
    float sensorData;
    uint8_t crc;
public:
    Sensor_Data();
    ~Sensor_Data();
    void set_header(byte *head);
    void set_identifier(byte *idfier);
    void set_sensorId(byte *sensId);
    void set_sensorData(byte *val1, byte *val2, byte *val3, byte *val4);
    void set_crc(byte *crcc);
};

Sensor_Data::Sensor_Data()
{
    header = 0x00;
    Identifier = 0x00;
    sensorId = 0x00;
    sensorData = 0.00;
    crc = 0x00;   
}

Sensor_Data::~Sensor_Data()
{
}

void Sensor_Data::set_header(byte *head){
    header = *head;
}

void Sensor_Data::set_identifier(byte *idfier){
    Identifier = *idfier;
}

void Sensor_Data::set_sensorId(byte *sensId){
    sensorId = *sensId;
}

void Sensor_Data::set_sensorData(byte *val1, byte *val2, byte *val3, byte *val4){
    uint32_t payloadValue = (static_cast<uint32_t>(*val1) << 24) | 
    (static_cast<uint32_t>(*val2) << 16) | 
    (static_cast<uint32_t>(*val3) << 8) | 
    (static_cast<uint32_t>(*val4));
    uint32_t sign  = (payloadValue >> 31) & 0x01;
    uint32_t exponent  = (payloadValue >> 23) & 0xFF;
    uint32_t Significand = payloadValue & 0x7FFFFF;
    IEEE754 ieee;
    ieee.segments.sign = sign;
    ieee.segments.exponent = exponent;
    ieee.segments.mantissa = Significand;
    std::bitset<32> binaryRepresentation(ieee.fValue);
    sensorData = ieee.fValue;
    std::cout << "Sensor Data: " << sensorData << std::endl;
}

void Sensor_Data::set_crc(byte *crcc){
    crc = *crcc;
}

class Parameter_Data
{
private:
    uint8_t header;
    uint8_t Identifier;
    uint8_t parameterId;
    std::string paraName;
    int paraData;
    uint8_t crc;
public:
    Parameter_Data();
    ~Parameter_Data();
    void set_p_header(byte *head);
    void set_p_identifier(byte *idfier);
    void set_p_paraId(byte *paraId);
    void set_paraName(byte *val1, byte *val2);
    void set_paraData(byte *val3);
    void set_p_crc(byte *crcc);
};

Parameter_Data::Parameter_Data()
{
    header;
    Identifier;
    parameterId;
    paraName;
    paraData;
    crc;
}

Parameter_Data::~Parameter_Data()
{
}

void Parameter_Data::set_p_header(byte *head){
    header = *head;
}

void Parameter_Data::set_p_identifier(byte *idfier){
    Identifier = *idfier;
}

void Parameter_Data::set_p_paraId(byte *paraId){
    parameterId = *paraId;
}

void Parameter_Data::set_paraName(byte *val1, byte *val2){
    paraName += static_cast<char>(*val1);
    paraName += static_cast<char>(*val2);
    std::cout << "Parameter: " << paraName << " ";
}

void Parameter_Data::set_paraData(byte *val3){
    paraData = static_cast<int>(*val3);
    std::cout << std::dec << paraData << std::endl;
}

void Parameter_Data::set_p_crc(byte *crcc){
    crc = *crcc;
}

byte calCRC(const std::vector<byte>& bytVals){
    const byte generator = 0x1D;
    byte crc = 0;

    for (size_t i = 0; i < bytVals.size(); i++) {
        byte currByte = bytVals[i];
        crc ^= currByte;
        for (int i = 0; i < 8; i++) {
            if ((crc & 0x80) != 0) {
                crc = static_cast<byte>((crc << 1) ^ generator);
            } else {
                crc <<= 1;
            }
        }
        //std::cout << "crc in loop " << std::hex << static_cast<int>(currByte) << " is " << std::hex << static_cast<int>(crc) << std::endl;
    }
    return crc;
}

int main(int argc, char *argv[]){
    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " [Filename]" << std::endl;
        return 1; 
    }
    
    const char *filename = argv[1];
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Unable to open file " << filename << std::endl;
        return 1;
    }
    
    std::string dataLine;
    while (std::getline(file, dataLine))
    {
        std::istringstream dataStream(dataLine);
        std::string token;
        std::vector <byte> tokens;
        while (std::getline(dataStream, token, ','))
        {
            tokens.push_back(stoi(token, nullptr, 16));
        }
        if (tokens[0] == 0x01)
        {
            byte crc = calCRC(tokens);
            std::cout << "CRC: " << std::hex << static_cast<int>(crc) << std::endl;
            if (crc == 0)
            {
                std::cout << "Data integrity is intact " << std::endl;
                if (tokens[1] == SENSOR_DATA)
                {
                    Sensor_Data sense;
                    sense.set_header(&tokens[0]); 
                    sense.set_identifier(&tokens[1]); 
                    sense.set_sensorId(&tokens[2]); 
                    sense.set_sensorData(&tokens[3], &tokens[4], &tokens[5], &tokens[6]);
                    sense.set_crc(&tokens[7]);
                }
                else if (tokens[1] == PARAMETER_DATA)
                {
                    Parameter_Data para;
                    para.set_p_header(&tokens[0]);
                    para.set_p_identifier(&tokens[1]);
                    para.set_p_paraId(&tokens[2]);
                    para.set_paraName(&tokens[3], &tokens[4]);
                    para.set_paraData(&tokens[5]);
                    para.set_p_crc(&tokens[6]);
                }
            }
        }        
    }
    std::cout << std::endl << "Press any key to exit...";
    std::cin.ignore();
    std::cin.get();
    return 0;
}