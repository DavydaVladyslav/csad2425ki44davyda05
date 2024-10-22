#include <windows.h>
#include <iostream>
#include <string>




using namespace std;


//JSON creation function
string createJSON(const string& message) {
    return "{\"message\":\"" + message + "\"}";
}

//JSON parsing function
string parseJSON(const string& json) {
    size_t start = json.find("\"message\":\"") + 11;
    size_t end = json.find("\"", start); //Find the end of the value

    if (start != string::npos && end != string::npos) {
        return json.substr(start, end - start);
    }

    return ""; //If JSON is invalid
}




int main() {

    /*Request COM port number from user*/
    int portNumber;
    cout << "Enter COM port number (e.g., 6 for COM6): ";
    cin >> portNumber;

    //Generating a port name
    string portName_s = "COM" + to_string(portNumber);
    const char* portName = portName_s.c_str();


    /*const char* portName = "COM6";*/

    //Opening COM port
    HANDLE hSerial = CreateFileA(portName, // portName.c_str()
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        cerr << "Error opening port" << endl;
        system("pause");
        return 1;
    }

    //Configure port parameters
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error getting port state" << endl;
        system("pause");
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error setting port parameters" << endl;
        system("pause");
        return 1;
    }

    //Setting timeouts
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        cerr << "Error setting timeouts!" << endl;
        system("pause");
        return 1;
    }

    Sleep(2000);

    //Loop for endlessly sending messages
    while (true) {
        string message = "hello from pc";  //Message to send
        string messageXML = createJSON(message);
        DWORD bytesWritten;



        //Sending a message to Arduino
        if (!WriteFile(hSerial, messageXML.c_str(), messageXML.size(), &bytesWritten, NULL)) {
            cerr << "Error writing to port" << endl;
            break;
        }
        cout << "Sent: " << message << " (" << messageXML << ")" << endl;

        //Reading the response from Arduino
        char buffer[128];
        DWORD bytesRead;

        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';
            if (bytesRead > 0) {
                cout << "Received from Arduino: " << parseJSON(buffer) << " (" << buffer << ")" << endl << endl;
            }
        }
        else {
            cerr << "Error reading from port" << endl;
            break;
        }

        //Delay before next request
        Sleep(1000);
    }

    //Closing COM port
    CloseHandle(hSerial);
    return 0;
}
