#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlCHandler(int sig_num) {
    SmallShell& smash = SmallShell::getInstance();
    std::cout << "smash: got ctrl-C" <<std::endl;
    pid_t currCmdPid = smash.getCurrPid();
    pid_t smashPid = getpid();

    if(smashPid == -1)
    {
        perror("smash error: getpid failed");
        return;
    }
    if(!smash.isFg())
    {
        return;
    }
    if(currCmdPid==smashPid)
    {
        return;
    }
    int killCmd_res = kill(SIGINT,currCmdPid);
    if(killCmd_res == -1)
    {
        perror("smash error: kill failed");
        return;
    }
    cout << "smash: proccess " << currCmdPid << " was killed" << endl;
    return;
}

