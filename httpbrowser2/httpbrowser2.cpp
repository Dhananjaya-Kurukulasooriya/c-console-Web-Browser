// httpbrowser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
//first include the header files that need for socket programming
#include <winsock2.h>
#include <ws2tcpip.h>
//#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream> //to write the data to text file
#include <string>


#pragma comment(lib,"ws2_32.lib")//to compile the winsock2 .h fiel we have to tell the compiler to use this library by using pragma preprocessor
using namespace std;

//to define  constant
#define default_port 80	//lest defien the default web port
#define buffer_size 9000	//to store the incoming data from the server

void errorLogo();
void errorHandler(int error);
void logo();
void htmlViewer();
void lineFormatter(string lines);
void displayBody();

int main()

{
	logo();


	int iResult;//to store the function execute feedback

	

	string domain;//to hold the user entries;
	string path;//to hold the path details
	//lets get the domain name and path from the user
	cout << "							Ente the Domain Name:\t";
	getline(cin, domain);
	//cout << endl;
	cout << "  							Enter the Path details: ";
	getline(cin, path);
	cout << endl;
	cout << endl;
	cout << endl;
	

	

	//first define WSADATA file which is used to store the data from WSAStartup function ,which used to give critical data to  the program
	WSADATA wsadata;

	//lets initiate the WSAStartup function with Winsock 2.2 version
	iResult = WSAStartup(MAKEWORD(2, 2), &wsadata); //is this process success it will return 0 otherwise there are issues
	//we cannot execute or call the winsock funtion without this function getting success
	if (iResult != 0)
	{
		printf("WSAStartup function error %d\n", WSAGetLastError());
		return 1; //program need to quit in this case
	}

	//to test purposes
	//cout << "WSAStartup successful" << endl;

	//otherwise we can continue with the execution,now we have to create the sockets for getting data from the server
	//socket is a end point of the communicatiopn process.we have to prepare this program for that
	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //we crete the socket like this

	//we can use hostent structure to get the host data by its name and store those data in the program
	//string domain = "www.who.int";//later we can get custom details here
	//string path = "";


	struct hostent* host;
	host = gethostbyname(domain.c_str());
	//host = gethostbyname(url.c_str());//we can pass value by console to here from the user
	char* ipAddress = inet_ntoa(*(struct in_addr*)*host->h_addr_list);

	cout << ipAddress << " is the address of the destination!" << endl;
	cout << "http://" << domain << path << endl;
	//now we have to establish the connnection with the server
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(default_port); //use the port 80 
	SockAddr.sin_family = AF_INET; //we can use ipv4 
	SockAddr.sin_addr.s_addr = inet_addr(ipAddress);


	//now we can configure the connection to the server

	cout << "Connecting ......." << endl; //connection status



	iResult = connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr));
	if (iResult == SOCKET_ERROR)
	{
		printf("connection error %d\n", WSAGetLastError());
		closesocket(Socket);//we have to close the socket and relese all the resources in this case
		return 1; //program need to quit in this case
	}








	//now we have to set the http header file to sent to the server
	string GET = "GET " + path + " HTTP/1.1\r\nHost: " + domain + "\r\nAccept-Language: en-us,en;q=0.5\r\nUser-Agent: Mozilla/5.0 (Windows NT 8.0; Win64; x64; rv:60.0) Gecko/20100101 Firefox/60.0\r\nAccept: */*\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\nConnection: close\r\nDNT: 1 Cache-Control: private\r\nReferer: https://developer.mozilla.org/en-US/docs/Web/JavaScript\r\n\r\n";

	//for testing
	cout << endl;
	//cout << GET << endl; to print the get request header
	cout << "connected!" << endl; //to indicate the connection status



	//now we got a custom url for data request
	iResult = send(Socket, GET.c_str(), GET.length(), 0);
	//we have to check that all data sent to the server
	if (iResult != GET.length())
	{
		cout << "data send error!" << endl;
		return 1;
	}


	//now we have to capture the incoming data fromt  the web server
	//we can store them inside a buffer until bufffer get filled then we can omit that data to text filr for further processing

	char  bufferData[buffer_size];
	string buff="";
	int datalength = 1;
	while (datalength != 0)//which mean until server end the data sending process
	{
		
		datalength = recv(Socket, bufferData, buffer_size, 0);
		//buff.append(bufferData, datalength); as a string!
		//cout << bufferData; //testing

	}
	//cout << sizeof(bufferData)/sizeof(bufferData[0]);
	//cout << strlen();

	//we have to add those data to the text file for further customization

	ofstream myfile;
	myfile.open("url.txt");
	myfile.write((char*)bufferData, sizeof(bufferData));
	myfile.close();


	//now we can close the socket 
	closesocket(Socket);//this will wait until end of all data sending and recieving and then close the socket
	WSACleanup();//to release all the resources


	//now we have a text file of a web page structure,and now we have to edit that text file to get

	htmlViewer();//this  function will display the html text from the text file.

	//to view the final view of the data

	



	system("pause");
	return 0;
}

void htmlViewer()
{

	//first of all we have to remove all the data from the finalText file to write final data to tahat file
	cout << "==========================================================================web page start here!==========================================================================" << endl;
	cout << endl;
	cout << endl;
	ofstream clearFile;
	clearFile.open("finalFile.txt", ios::out | ios::trunc);
	clearFile.close();



	//lets open the text file
	ifstream logFile("url.txt");

	string lines;
	bool flag = false;
	bool working = true;
	int errorCode = 0;
	if (logFile.is_open())
	{
		//cout << "file working!"<<endl;//for testing
		while (getline(logFile, lines))
		{
			
			if (lines.find("TTP/1.1 302 Found") != string::npos)
			{
				working = false;
				errorHandler(302);

			}
			else if(lines.find("TTP/1.1 400 Bad Request") != string::npos)
			{
				working = false;
				errorHandler(400);

			}
			else if(lines.find("TTP/1.1 301 Moved Permanently") != string::npos)
			{
				working = false;
				errorHandler(301);

			}
			if (lines.find("TTP/1.1 200 OK") != string::npos)
			{
				working = true;
			}

			if(working)
			{
						
				if (lines.empty() || lines.length() == 0) //to avoid empty lines
				{
					continue;
				}
				if (lines.find("<body") != string::npos || lines.find("</body") != string::npos) //we need to read only the data from its body
				{
					flag = !flag;


				}


				if (lines.find("</body") != string::npos)
				{
					flag = false;
					logFile.close();//we need to close that file
					break;
				}


				if (flag) //when the flag is set it will pass the data to other function for remove the tags and get the data
				{
					lineFormatter(lines);
					//cout << lines;
				}

			}
			else
			{
				break;
			}
			
			

		}



	}

	displayBody();
	cout << endl;
	cout << endl;
	cout<<"========================================================================================================================================================================"<<endl;
	cout << "																									DESIGN by:  "<< endl;
																										
	cout << "==========================================================================web page end here!============================================================================" << endl;




}



void errorHandler(int error)
{
	
	switch (error)
	{
	case 301:
	case 302:
		cout << "301 Moved Permanently! " << endl;
		break;
	case 400:
		cout << "400 Bad Request!" << endl;
		break;
	
	}

	errorLogo();
	
	
}



void lineFormatter(string lines)
{
	bool done = false;
	static bool inTag = false;

	while (!done)
	{
		if (inTag)
		{
			size_t rightpos = lines.find(">");
			if (rightpos != string::npos)
			{
				inTag = false;
				lines.erase(0, rightpos + 1);
			}
			else
			{
				done = true;
				lines.erase();
			}

		}
		else
		{
			size_t leftpos = lines.find("<");
			if (leftpos != string::npos)
			{
				size_t rightpos = lines.find(">");
				if (rightpos == string::npos)
				{
					inTag = done = true;
					lines.erase(leftpos);
				}
				else
				{
					lines.erase(leftpos, rightpos - leftpos + 1);
				}

			}
			else
			{
				done = true;
			}
		}

	}




	if (lines.length() != 0) //after removing tags if it is a empty line we have to ignore that line
	{
		//here we can append those data to a new file and then finally we can read those data from that file
		lines = lines + '\n';
		ofstream writeFile("finalFile.txt", ios::app);
		writeFile << lines;
		//cout << lines;



	}



}


void displayBody()
{
	fstream readFile("finalFile.txt");

	string lines1;
	int count = 0;//to track the read line we need first 10 lines;

	if (readFile.is_open())
	{

		while (getline(readFile, lines1))
		{
			if (count == 40)
			{
				break;//we need first 40 lines
			}
			count++;

			cout << lines1 << endl;;
		}

	}
	else
	{
		cout << "file not opened";

	}
}


void logo()
{

	cout << "                                                             _     _  _____ ____ " << endl;
	cout << "                                                            | |   | |/ ____|  _ \\ " << endl;
	cout << "                                                            | |   | | (___ | |_) |" << endl;
	cout << "                                                            | |   | |\\___ \\|  _ < " << endl;
	cout << "                                                            | |___| |____) | |_) |" << endl;
	cout << "                                                            \\______/|_____/|____/ " << endl;
	cout << endl;
	cout << "                                  if a picture is worth a thousand words then text-only pages are cheaper to read" << endl;
	cout << endl;
	cout << endl;

}

void errorLogo()
{

	cout << "	                                                                                 _ " << endl;
	cout << "	                                                                                | |" << endl;
	cout << "	                                              ___   _ __   _ __    ___    _ __  | |" << endl;
	cout << "	                                             / _ \\ | '__| | '__|  / _ \\  | '__| | |" << endl;
	cout << "	                                            |  __/ | |    | |    | (_) | | |    |_|" << endl;
	cout << "	                                             \\___| |_|    |_|     \\___/  |_|    (_)" << endl;


		  cout << endl;




}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
