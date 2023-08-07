
#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include <thread>
#include <vector>
#include <ios>
#include <ostream>
#include "../include/StompProtocol.h"
using namespace std;
#include <boost/thread.hpp>
namespace std{
	using boost::mutex;
	using boost::lock_guard;
}

class socketThread
{
private:
	ConnectionHandler &ch;
	StompProtocol &p;
	bool preLogout;
	mutex &mutex_;

public:
	socketThread(ConnectionHandler &ConnectionHandler, StompProtocol &protocol,mutex &mutex) : ch(ConnectionHandler), p(protocol), preLogout(true),mutex_(mutex){}

	void set()
	{
		preLogout = false;
	}
	void taskSocketThread()
	{
		bool logout = false;
		while (!logout)
		{

			string ans;
			if (ch.getLine(ans))
			{
				string check = p.handleAnswer(ans);
				int index = check.find("\n");
				check = check.substr(0, index);
				if (!preLogout)
				{
					if (check == "RECEIPT")
					{
						logout = true;
					}
				}
				if (check == "ERROR")
				{
					lock_guard<mutex>lock(mutex_);
					ch.close();
					logout = true;
				}
			}
			else
			{
				break;
			}
		}
	}
};