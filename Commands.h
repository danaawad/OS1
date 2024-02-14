#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <list>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include <fcntl.h>
#include <sys/stat.h>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command {
// TODO: Add your data members
    const char* cmd_line;   //the command line
    int args_number;        //number of arguments passed in the command line
    char** parsed_cmd;     //an array of strings where each string is a word in the command line
public:
    Command(const char* line);
    virtual ~Command();
    virtual void execute() = 0;
    //virtual void prepare();
    //virtual void cleanup();
    // TODO: Add your extra methods if needed
    //added getters to the private fields
    const char* getCommandLine();
    const int getNumberOfArguments();
    char** getParsedCommand();

    //added helper functions
    void removeAmpersand();
    void updateParsedCommand(const char* new_cmd_line);
};

class BuiltInCommand : public Command {
public:
    BuiltInCommand(const char* cmd_line);
    virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
public:
    ExternalCommand(const char* cmd_line);
    virtual ~ExternalCommand() {}
    void execute() override;
};

class PipeCommand : public Command {
    // TODO: Add your data members
public:
    PipeCommand(const char* cmd_line);
    virtual ~PipeCommand() {}
    void execute() override;
};

class RedirectionCommand : public Command {
    // TODO: Add your data members
public:
    explicit RedirectionCommand(const char* cmd_line);
    virtual ~RedirectionCommand() {}
    void execute() override;
    //void prepare() override;
    //void cleanup() override;
};

/**
 * dima added change prompt class
 */
class ChangePromptCommand : public BuiltInCommand{
public:
    ChangePromptCommand(const char* cmd_line);
    virtual ~ChangePromptCommand(){}
    void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
private:
    char** lastPwd;
public:
    ChangeDirCommand(const char* cmd_line, char** plastPwd);
    virtual ~ChangeDirCommand() {}
    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
public:
    GetCurrDirCommand(const char* cmd_line);
    virtual ~GetCurrDirCommand() {}
    void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
public:
    ShowPidCommand(const char* cmd_line);
    virtual ~ShowPidCommand() {}
    void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand {
    JobsList* jobsList;
// TODO: Add your data members
public:
    QuitCommand(const char* cmd_line, JobsList* jobs);
    virtual ~QuitCommand() {}
    void execute() override;
};


/**
 * dima jobsList class - added private fields to JobEntry and methods (getters and setters)
 * also added private fields to Jobs list (a list of (jobEntry)s and max job id currently in list
 */


class JobsList {
public:
    class JobEntry {
        // TODO: Add your data members
    private:
        int jobId;
        pid_t jobPid;
        bool stopped;
        const char* cmd_line;

    public:
        JobEntry(int jobId, pid_t jobPid,bool stopped, const char* new_cmd_line);
        ~JobEntry();
        int getJobId();
        pid_t getJobPid();
        bool isStopped();
        void startJob();
        void stopJob();
        const char* getCommandLine();
    };

    std::list<JobEntry*> jobsList;
    int maxJobId=0;
    // TODO: Add your data members
public:
    JobsList()=default;
    ~JobsList()=default;
    void addJob(const char* cmd_line,pid_t job_pid, bool isStopped = false); //dima changed function paramaters
    void printJobsList();
    void killAllJobs();
    void removeFinishedJobs();
    JobEntry * getJobById(int jobId);
    void removeJobById(int jobId);
    JobEntry * getLastJob(int* lastJobId);
    JobEntry *getLastStoppedJob(int *jobId);
    // TODO: Add extra methods or modify exisitng ones as needed

    void updateMaxJobId();     //dima added helper function to update the maxJobId field upon insertion and or removal
    int size();
    void printJobsListBeforeKilling();
};

class JobsCommand : public BuiltInCommand {
    // TODO: Add your data members
    JobsList* jobsList; //dima added a jobsList field
public:
    JobsCommand(const char* cmd_line, JobsList* jobs);
    virtual ~JobsCommand() {}
    void execute() override;
};

class KillCommand : public BuiltInCommand {
    // TODO: Add your data members
    JobsList* jobsList;
public:
    KillCommand(const char* cmd_line, JobsList* jobs);
    virtual ~KillCommand() {}
    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    // TODO: Add your data members
    JobsList* jobsList; //dima added jobsList field
public:
    ForegroundCommand(const char* cmd_line, JobsList* jobs);
    virtual ~ForegroundCommand() {}
    void execute() override;
};

class ChmodCommand : public BuiltInCommand {
public:
    ChmodCommand(const char* cmd_line);
    virtual ~ChmodCommand() {}
    void execute() override;
};


class SmallShell {

private:
    pid_t curr_pid;
    char* curr_prompt;
    char* lastPwd;
    JobsList* jobsList;
    char* curr_cmd;
    bool isFG;


    SmallShell()
    {
        curr_prompt = new char[80];
        strcpy(curr_prompt, "smash> ");
        lastPwd = new char[strlen("") + 1];
        strcpy(lastPwd, "");
        jobsList = new JobsList();
        curr_cmd = new char[COMMAND_MAX_ARGS];
        isFG = true;
        curr_pid = getpid();
        if(curr_pid == -1)
        {
            perror("smash error: getpid failed");
        }
    }


public:

    Command *CreateCommand(const char* cmd_line);
    SmallShell(SmallShell const&)      = delete; // disable copy ctor
    void operator=(SmallShell const&)  = delete; // disable = operator
    static SmallShell& getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    ~SmallShell();

    void setCurrPrompt(char *currPrompt);
    void setJobsList(JobsList *jobsList);
    JobsList *getJobsList() const;
    void executeCommand(const char* cmd_line);
    pid_t getCurrPid() const;
    char *getCurrPrompt() const;
    char *getLastPwd() const;
    void setCurrPid(pid_t currPid);
    void setLastPwd(char *lastPwd);
    char *getCurrCmd() const;
    bool isFg() const;
    void setCurrCmd(char *currCmd);
    void setIsFg(bool isFg);
};


#endif //SMASH_COMMAND_H_
