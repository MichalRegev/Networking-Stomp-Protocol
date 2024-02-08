#include <stdlib.h>
#include <string>
#include "../include/StompProtocol.h"
#include "../include/event.h"
#include "../include/json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
using json = nlohmann::json;
using namespace std;

StompProtocol::StompProtocol():id(0),receipt(0),myUserName(""),summary({{}}),subId({{}}){};


string StompProtocol::sendFrame(string userName, string team_a,string team_b, Event currEvent){
    receipt++;
    string g=to_string(receipt);
    string returnFrame="SEND\nreceipt:" +g+ "\ndestination:"+team_a+"_"+team_b+"\n\nuser:"+userName+"\nteam a:"+team_a+"\nteam b:"+team_b+"\n";
     int t= currEvent.get_time();
     string time= to_string(t);
    returnFrame=returnFrame+"event name:"+currEvent.get_name()+"\ntime:"+time+"\n";
    string general="general game updates:\n";
    map<string,string> m1=currEvent.get_game_updates();
   for(map<string,string>::const_iterator it = m1.begin();it != m1.end(); ++it)
{
    general=general+ it->first +":" +it->second+"\n";
}
string teamA="team a updates:\n";
    map<string,string> m2=currEvent.get_team_a_updates();
   for(map<string,string>::const_iterator it = m2.begin();it != m2.end(); ++it)
{
    teamA=teamA+ it->first +":" +it->second+"\n";
}
string teamB="team b updates:\n";
    map<string,string> m3=currEvent.get_team_b_updates();
   for(map<string,string>::const_iterator it = m3.begin();it != m3.end(); ++it)
{
    teamB=teamB+ it->first +":" +it->second+"\n";
}
string des="description:\n"+currEvent.get_discription()+"\n";
returnFrame=returnFrame+general+teamA+teamB+des;
    return returnFrame;
}
string StompProtocol::login(string userName, string passcode){
    return "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:"+ userName +"\npasscode:" +passcode+"\n\n";
}
string StompProtocol::handleCommand(string line){
   string ans;
   string command="";
   string transFrame="";
   size_t pos = 0;
   std::string token;
   string split[4];
   int index=0;
   pos = line.find(" ");
   command=line.substr(0,pos);
   while ((pos = line.find(" ")) != std::string::npos) {
    token = line.substr(0, pos);
    split[index]=token;
    index++;
    line.erase(0, pos + 1);
}
split[index]=line;
   if(command!="report"){
 if (command=="login"){
    transFrame=login(split[2],split[3]);
    ans=transFrame;
    myUserName=split[2];
 }
    if (command=="join"){
        string gameName= split[1];
        id++;
        receipt++;
        string ids=to_string(id);
        subId[gameName]= ids;
        string receipts=to_string(receipt);
        transFrame="SUBSCRIBE\ndestination:" + gameName +"\nid:" + ids+"\nreceipt:"+receipts+"\n\n";
         ans=transFrame;
    }
    if (command=="exit"){
        receipt++;
        string gameName= split[1];
        if (subId.count(gameName)>0){
         string receipts=to_string(receipt);
        transFrame="UNSUBSCRIBE\nid:"+subId[gameName]+"\nreceipt:"+receipts+"\n\n";
          ans=transFrame;
        }
        else{
             string receipts=to_string(receipt);
             transFrame="UNSUBSCRIBE\nid:-1\nreceipt:"+receipts+"\n\n";
             ans=transFrame;
        }
    }
    if (command=="summary"){
    string userName=split[2];
    string gameName=split[1];
    string fileName=split[3];
    game currGame=(summary[gameName])[userName];
    ofstream MyFile(fileName);
    string root1="";
    root1=root1+summary[gameName][userName].getGameVS()+"\n"+"Game stats:\nGeneral stats:\nactive:"+summary[gameName][userName].getActive()+"\nbefore halftime:"+summary[gameName][userName].getBeforeHalf()+"\n"+summary[gameName][userName].getTeamAname()+" stats:\n";
    root1=root1+"goals:"+summary[gameName][userName].getTeamAgoals()+"\npossession:"+summary[gameName][userName].getTeamApos()+"\n"+summary[gameName][userName].getTeamBname()+" stats:\n"+"goals:"+summary[gameName][userName].getTeamBgoals()+"\npossession:"+summary[gameName][userName].getTeamBpos()+"\n";
    root1=root1+"Game event reports:" + summary[gameName][userName].getGameEvent();
    while(root1.length()||root1.find("\n")==0){
        unsigned int index1=root1.find("\n");
        unsigned int index2=root1.find(",");
        if(index1<index2||index2==std::string::npos){
         MyFile<<root1.substr(0,index1);
         MyFile<<endl;
        root1.erase(0,index1+1);
        }
        else{
        MyFile<<root1.substr(0,index2+1);
         MyFile<<endl;
        root1.erase(0,index2+1);
        }
    }
 
    }    
    if (command=="logout"){
        receipt++;
        string receipts=to_string(receipt);
        transFrame="DISCONNECT\nreceipt:"+receipts+"\n\n";
        ans=transFrame;
    }
}
   else {
    if (command=="report"){
        string jsonPath=split[1];
        names_and_events curr=parseEventsFile(jsonPath);
        vector<Event> eVector=curr.events;
          for(vector<Event>::iterator it = eVector.begin();it != eVector.end(); ++it)
         {
            transFrame=sendFrame(myUserName,curr.team_a_name,curr.team_b_name,*it);
              ans=ans+transFrame+"##";
         }
        }
   }
    return ans;
}

void StompProtocol::printAns(string frame){
    while(frame.length()!=0){
        int index=frame.find("\n");
        cout<<frame.substr(0,index)<<endl;
        frame.erase(0,index+1);
    }

}
string StompProtocol::handleAnswer(string ans){
   string copy1= ans;
   string copy= ans;
   int index= ans.find("\n");
   string command=ans.substr(0,index);
   copy=copy.erase(0, index + 1);
   ans.erase(0, index + 1);
   index= ans.find("\n");
   ans.erase(0, index + 1);
   index= ans.find("\n");
   ans.erase(0, index + 1);
   if(command=="MESSAGE"){
    index= ans.find(":");
    ans.erase(0,index+1);
    index=ans.find("\n");
    string gameName= ans.substr(0,index);
    Event event= Event(copy);
    string userName= event.get_userName();
    string gameVS= event.get_team_a_name()+" vs "+event.get_team_b_name();
    if (summary[gameName].count(userName)==0){
       game game1 = game(gameName,gameVS,event.get_team_a_name(),event.get_team_b_name());
       map<string,string> m1=event.get_game_updates();
       if (m1.count("active")>0)
            game1.setActive(m1["active"]);
        if (m1.count("before halftime")>0)
            game1.setBefore(m1["before halftime"]);
        map<string,string> m2=event.get_team_a_updates();
        if(m2.count("goals")>0)
            game1.setTeamAgoals(m2["goals"]);
        if(m2.count("possession")>0)
            game1.setTeamApos(m2["possession"]);
        map<string,string> m3=event.get_team_b_updates();
        if(m3.count("goals")>0)
            game1.setTeamBgoals(m3["goals"]);
        if(m3.count("possession")>0)
            game1.setTeamBpos(m3["possession"]);
        int t= event.get_time();
        string time= to_string(t);
       string eventName= event.get_name() + "-"+ time+"\n\n"+event.get_discription();
       game1.setGameEvent(eventName);
       (summary[gameName])[userName]=game1;
    }
    else{
        game &game1= summary[gameName][userName];
        map<string,string> m1=event.get_game_updates();
       if (m1.count("active")>0)
            game1.setActive(m1["active"]);
        if (m1.count("before halftime")>0)
            game1.setBefore(m1["before halftime"]);
        map<string,string> m2=event.get_team_a_updates();
        if(m2.count("goals")>0)
            game1.setTeamAgoals(m2["goals"]);
        if(m2.count("possession")>0)
            game1.setTeamApos(m2["possession"]);
        map<string,string> m3=event.get_team_b_updates();
        if(m3.count("goals")>0)
            game1.setTeamBgoals(m3["goals"]);
        if(m3.count("possession")>0)
            game1.setTeamBpos(m3["possession"]);
        int t= event.get_time();
        string time= to_string(t);
        string eventName= event.get_name() + "-"+ time+"\n\n"+event.get_discription();
         game1.setGameEvent(eventName);
    }
    
   }
   printAns(copy1);
   return copy1;
   }



