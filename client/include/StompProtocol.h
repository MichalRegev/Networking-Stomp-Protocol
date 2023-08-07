#pragma once
#include <stdlib.h>
#include <string>
#include "../include/ConnectionHandler.h"
#include "../src/game.cpp"
#include "../include/event.h"
#include <vector>
using namespace std;
class game;



// TODO: implement the STOMP protocol
class StompProtocol
{
private:
int id=0;
int receipt=0;
string myUserName;
map<string,map<string,game>> summary;
map <string,string> subId;
public:
StompProtocol();
string login(string userName, string passcode);
string sendFrame(string userName, string team_a,string team_b, Event currEvent);
string handleCommand(string line);
string handleAnswer(string ans);
void printAns(string ans);

};
