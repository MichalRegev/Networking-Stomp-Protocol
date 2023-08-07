#include <stdlib.h>
using namespace std;
#include <string>
class game
{
private:
    string gameName;
    string gameVS;
    string team_a_name;
    string team_b_name;
    string active;
    string beforeHalf;
    string teamAgoals;
    string teamBgoals;
    string teamApos;
    string teamBpos;
    string gameEvent;

public:
    game(string name, string gameVS1, string teamaname, string teambname) : gameName(name), gameVS(gameVS1), team_a_name(teamaname), team_b_name(teambname), active("false"), beforeHalf("true"), teamAgoals(""), teamBgoals(""), teamApos(""), teamBpos(""), gameEvent("")
    {
    }

    game() : gameName(""), gameVS(""), team_a_name(""), team_b_name(""), active(""), beforeHalf(""), teamAgoals(""), teamBgoals(""), teamApos(""), teamBpos(""), gameEvent("") {}
    const string getTeamBname() const
    {
        return team_a_name;
    }
    const string getTeamAname() const
    {
        return team_b_name;
    }

    const string getGameVS() const
    {
        return gameVS;
    }
    const string getGameEvent() const
    {
        return gameEvent;
    }
    const string getTeamAgoals() const
    {
        return teamAgoals;
    }
    const string getTeamBgoals() const
    {
        return teamBgoals;
    }
    const string getTeamApos() const
    {
        return teamApos;
    }
    const string getTeamBpos() const
    {
        return teamBpos;
    }
    const string getGameName() const
    {
        return gameName;
    }
    const string getBeforeHalf() const
    {
        return beforeHalf;
    }
    const string getActive() const
    {
        return active;
    }

    void setGameEvent(string addTogameEvent)
    {
        gameEvent = gameEvent + "\n\n" + addTogameEvent;
    }

    void setBefore(string before)
    {
        beforeHalf = before;
    }
    void setActive(string activeNew)
    {
        active = activeNew;
    }

    void setTeamAgoals(string aGoals)
    {
        teamAgoals = aGoals;
    }
    void setTeamBgoals(string bGoals)
    {
        teamBgoals = bGoals;
    }

    void setTeamApos(string aPos)
    {
        teamApos = aPos;
    }

    void setTeamBpos(string bPos)
    {
        teamBpos = bPos;
    }
};