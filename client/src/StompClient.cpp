using namespace std;
#include "../include/ConnectionHandler.h"
#include <thread>
#include <vector>
#include "../include/StompProtocol.h"
#include "../src/socketThread.cpp"
#include <boost/lexical_cast/lexical_cast_old.hpp>

int processFrames(ConnectionHandler& connectionHandler, StompProtocol& protocol, mutex& mutex_, vector<string>& frames){
	while (!frames.empty()){
		string frame = frames.front();
		frames.erase(frames.begin());
		if (connectionHandler.isOpen()){
			lock_guard<mutex> lock(mutex_);
			if (connectionHandler.isOpen()){
				line = "";
				if (!connectionHandler.sendLine(frame)){
					std::cout << "Disconnected. Exiting...\n"
								<< std::endl;
					break;
				}
			}
			else{
				return true;
			}
		}
		else{
			return true;
		}
	}
	return false;
}



int main(int argc, char *argv[])
{
	string line = "";
	while (1)
	{
		string command = "";
		bool login = false;
		string host;
		short port;
		string userName;
		string passcode;
		const short bufsize = 1024;
		char buf[bufsize];
		mutex mutex_;

		while (!login)
		{
			if (line.size() == 0)
			{
				cin.getline(buf, bufsize);
				line=buf;
			}
			command = line;
			if (line.substr(0, 5) == "login")
			{
				login = true;
				line.erase(0, 6);
				int index = line.find(":");
				host = line.substr(0, index);
				line.erase(0, index + 1);
				index = line.find(" ");
				port = boost::lexical_cast<short>(line.substr(0, index));
			}
			line = "";
		}
		ConnectionHandler connectionHandler(host, port);
		if (!connectionHandler.connect())
		{
			cerr << "Cannot connect to " << host << ":" << port << endl;
			return 1;
		}
		StompProtocol protocol = StompProtocol();
		string frame = protocol.handleCommand(command);
		if (!connectionHandler.sendLine(frame))
		{
			std::cout << "Disconnected. Exiting...\n"
					  << std::endl;
			break;
		}
		socketThread task2 = socketThread(connectionHandler, protocol, mutex_);
		thread socketThread(&socketThread::taskSocketThread, &task2);
		bool logout = false;
		while (!logout)
		{
			cin.getline(buf, bufsize);
			string line1(buf);
			line = line1;
			string frame = "";
			if (line.length() > 0)
			{
				string copy = line;
				frame = protocol.handleCommand(copy);
				if (frame.length() > 0)
				{
					size_t pos = 0;
					vector<string> frames;
					string curr_frame = "";
					//## Separator between frames
					while ((pos = frame.find("##")) != std::string::npos) 
					{
						curr_frame = frame.substr(0, pos);
						int ind = curr_frame.find("\n");
						string currCommand = curr_frame.substr(0, ind);
						frames.push_back(curr_frame);
						frame.erase(0, pos + 9);
					}
					if (frame.length() > 0)
					{
						frames.push_back(frame);
						if (frame.substr(0, 10) == "DISCONNECT")
						{
							logout = true;
							task2.gotDisconnect();
						}
					}
					logout= processFrames(connectionHandler, protocol, mutex_, frames) || logout;	
				}
			}
		}
		socketThread.join();
		login = false;
		command = "";
	}
	return 0;
};
