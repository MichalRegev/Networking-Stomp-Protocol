#include "../include/event.h"
#include "../include/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string>
using namespace std;
using json = nlohmann::json;

Event::Event(std::string team_a_name, std::string team_b_name, std::string name, int time,
             std::map<std::string, std::string> game_updates, std::map<std::string, std::string> team_a_updates,
             std::map<std::string, std::string> team_b_updates, std::string discription)
    : team_a_name(team_a_name), team_b_name(team_b_name), name(name),
      time(time), game_updates(game_updates), team_a_updates(team_a_updates),
      team_b_updates(team_b_updates), description(discription), userName("")
{
}

Event::~Event()
{
}

const std::string &Event::get_team_a_name() const
{
    return this->team_a_name;
}
const std::string &Event::get_userName() const
{
    return this->userName;
}

const std::string &Event::get_team_b_name() const
{
    return this->team_b_name;
}

const std::string &Event::get_name() const
{
    return this->name;
}

int Event::get_time() const
{
    return this->time;
}

const std::map<std::string, std::string> &Event::get_game_updates() const
{
    return this->game_updates;
}

const std::map<std::string, std::string> &Event::get_team_a_updates() const
{
    return this->team_a_updates;
}

const std::map<std::string, std::string> &Event::get_team_b_updates() const
{
    return this->team_b_updates;
}

const std::string &Event::get_discription() const
{
    return this->description;
}

Event::Event(const std::string &frame_body) : team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description(""), userName("")
{
    size_t pos = 0;
    size_t pos1 = 0;
    size_t pos3 = 0;
    size_t pos4 = 0;
    size_t pos5 = 0;
    std::map<std::string, std::string> gamedetails;
    std::map<std::string, std::string> gameupdates;
    std::map<std::string, std::string> teamaupdates;
    std::map<std::string, std::string> teambupdates;
    std::string currLine;
    std::string currLine2;
    bool stop1 = false;
    bool stop2 = false;
    bool stop3 = false;
    bool stop4 = false;
    std::string editFrameBody = frame_body;
    while ((pos = editFrameBody.find("\n")) != std::string::npos)
    {
        currLine = editFrameBody.substr(0, pos);
        editFrameBody.erase(0, pos + 1);
        if (currLine == "")
        {
            while (((pos1 = editFrameBody.find("\n")) != std::string::npos) && !stop1)
            {
                currLine2 = editFrameBody.substr(0, pos1);
                if (currLine2 != "general game updates:")
                {
                    size_t pos2 = currLine2.find(":");
                    gamedetails[currLine2.substr(0, pos2)] = currLine2.substr(pos2 + 1, currLine2.length());
                    editFrameBody.erase(0, pos1 + 1);
                }
                else
                {
                    stop1 = true;
                }
            }
        }
        if (currLine == "general game updates:")
        {
            while (((pos3 = editFrameBody.find("\n")) != std::string::npos) && !stop2)
            {
                currLine2 = editFrameBody.substr(0, pos3);
                if (currLine2 != "team a updates:")
                {
                    size_t pos2 = currLine2.find(":");
                    gameupdates[currLine2.substr(0, pos2)] = currLine2.substr(pos2 + 1, currLine2.length());
                    editFrameBody.erase(0, pos3 + 1);
                }
                else
                {
                    stop2 = true;
                }
            }
        }
        if (currLine == "team a updates:")
        {
            while (((pos4 = editFrameBody.find("\n")) != std::string::npos) && !stop3)
            {
                currLine2 = editFrameBody.substr(0, pos4);
                if (currLine2 != "team b updates:")
                {
                    size_t pos2 = currLine2.find(":");
                    teamaupdates[currLine2.substr(0, pos2)] = currLine2.substr(pos2 + 1, currLine2.length());
                    editFrameBody.erase(0, pos4 + 1);
                }
                else
                {
                    stop3 = true;
                }
            }
        }
        if (currLine == "team b updates:")
        {
            while (((pos5 = editFrameBody.find("\n")) != std::string::npos) && !stop4)
            {
                currLine2 = editFrameBody.substr(0, pos5);
                if (currLine2 != "description:")
                {
                    size_t pos2 = currLine2.find(":");
                    teambupdates[currLine2.substr(0, pos2)] = currLine2.substr(pos2 + 1, currLine2.length());
                    editFrameBody.erase(0, pos5 + 1);
                }
                else
                {
                    size_t pos2 = currLine2.find(":");
                    editFrameBody.erase(0, pos2 + 2);
                    description = editFrameBody;
                    stop4 = true;
                }
            }
        }
    }

    team_a_updates = teamaupdates;
    team_b_updates = teambupdates;
    game_updates = gameupdates;
    userName = gamedetails["user"];
    name = gamedetails["event name"];
    team_a_name = gamedetails["team a"];
    team_b_name = gamedetails["team b"];
    time = stoi(gamedetails["time"]);
}

names_and_events parseEventsFile(std::string json_path)
{
    std::ifstream f(json_path);
    json data = json::parse(f);

    std::string team_a_name = data["team a"];
    std::string team_b_name = data["team b"];

    // run over all the events and convert them to Event objects
    std::vector<Event> events;
    for (auto &event : data["events"])
    {
        std::string name = event["event name"];
        int time = event["time"];
        std::string description = event["description"];
        std::map<std::string, std::string> game_updates;
        std::map<std::string, std::string> team_a_updates;
        std::map<std::string, std::string> team_b_updates;
        for (auto &update : event["general game updates"].items())
        {
            if (update.value().is_string())
                game_updates[update.key()] = update.value();
            else
                game_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team a updates"].items())
        {
            if (update.value().is_string())
                team_a_updates[update.key()] = update.value();
            else
                team_a_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team b updates"].items())
        {
            if (update.value().is_string())
                team_b_updates[update.key()] = update.value();
            else
                team_b_updates[update.key()] = update.value().dump();
        }

        events.push_back(Event(team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description));
    }
    names_and_events events_and_names{team_a_name, team_b_name, events};

    return events_and_names;
}