
#include "Commands.h"


using namespace std;

const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

string _ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for(std::string s; iss >> s; ) {
        args[i] = (char*)malloc(s.length()+1);
        memset(args[i], 0, s.length()+1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h


/**
 * dima Command class implementation
 */
Command::Command(const char *line): parsed_cmd(new char*[COMMAND_MAX_ARGS])
{
    char* temp = new char[COMMAND_MAX_ARGS];
    strcpy(temp,line);
    this->cmd_line=temp;
    args_number= _parseCommandLine(cmd_line, parsed_cmd);

}

Command::~Command() noexcept
{
    delete[] parsed_cmd;
    delete cmd_line;
}

const int Command::getNumberOfArguments()
{
    return args_number;
}

const char* Command::getCommandLine()
{
    return cmd_line;
}

char** Command::getParsedCommand()
{
    return parsed_cmd;
}


void Command::removeAmpersand()
{

    char* temp = strdup(cmd_line);
    if(temp == nullptr)
    {
        return;
    }
    _removeBackgroundSign(temp);
    _parseCommandLine(temp, parsed_cmd);
    free(temp);
}

void Command::updateParsedCommand(const char *new_cmd_line)
{
    delete[] parsed_cmd;
    parsed_cmd = new char*[COMMAND_MAX_ARGS];
    int new_args_number = _parseCommandLine(new_cmd_line, parsed_cmd);
    args_number = new_args_number;
}


/**
 * dima BuiltInCommand class implemntation
 */
BuiltInCommand::BuiltInCommand(const char *cmd_line) : Command(cmd_line){};



/**
 * dima added helper funcs
 */

//---------------------HELPER FUNCTIONS IMPLEMENTATION---------------------

bool string_of_digits(char* string)
{
    const char* temp = string;
    if(string[0]=='-')
    {
        temp++;
    }
    for(int i=0; i < (int)(strlen(temp)) ; i++)
    {
        if(!isdigit(temp[i]))
        {
            return false;
        }
    }
    return true;
}

int convert_to_number(char* to_convert)
{
    std::string id_str = to_convert;
    return stoi(id_str);
}

bool isExternalCommand(const char* cmd_line)
{
    string cmd_s = _trim(string(cmd_line));
    string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

    if (firstWord.compare("pwd") == 0 || firstWord.compare("showpid") == 0 || firstWord.compare("chprompt") == 0
        || firstWord.compare("cd") == 0 || firstWord.compare("jobs") == 0 ||  firstWord.compare("quit") == 0 || firstWord.compare("chmod") == 0)
    {
        return false;
    }

    if(strchr(cmd_line, '>')) //Redirection
    {
        return false;
    }
    return true;
}



/*-----------------------BUILT IN COMMAND IMPLEMENTATION-----------------------------*/
/**
 * dima change prompt class implemntation
 */
ChangePromptCommand::ChangePromptCommand(const char *cmd_line): BuiltInCommand(cmd_line){};

void ChangePromptCommand::execute()
{
    /**
* get the command line
* parse the words into an array
* if no arguments were given shell output "smash>"
* else set shell output "A[1]>"
* after the function is done we change back to "smash>"
* @param cmd_line
*/
    {
        SmallShell& smash = SmallShell::getInstance(); //because constructor is priv
        removeAmpersand();
        if(getNumberOfArguments()==1){
            string def_prompt = "smash> ";
            smash.setCurrPrompt((char*)def_prompt.c_str());
        }
        else
        {
            char* new_prompt= strcat(getParsedCommand()[1],"> ");
            smash.setCurrPrompt(new_prompt);
        }
        return;
    }
}

/**
 * dima show pid class implementation
 */

ShowPidCommand::ShowPidCommand(const char *cmd_line) : BuiltInCommand(cmd_line){};

void ShowPidCommand::execute()
{
    removeAmpersand();
    pid_t shell_pid = getpid();
    std::cout << "smash pid is " << shell_pid << std::endl;
    return;
}

/**
 * dana - pwd
 */
GetCurrDirCommand::GetCurrDirCommand(const char* cmd_line) : BuiltInCommand(cmd_line){};

void GetCurrDirCommand::execute() {
/**
 * The command prints the full path of the current working directory
 * pwd command has no arguments
 */
    {
        removeAmpersand();
        char *path = getcwd(NULL, 0);
        if (path == nullptr) {
            perror("smash error: getcwd failed");
            return;
        } else {
            std::cout << path << std::endl;
        }
        return;

    }
}


/**
 * dana -- cd
 */
ChangeDirCommand::ChangeDirCommand(const char* cmd_line, char** plastPwd) : BuiltInCommand(cmd_line), lastPwd(plastPwd) {}

//void ChangeDirCommand::execute()
//{
///**
// * Change directory (cd) command receives a single argument <path> that describes the
// * relative or full path to change the current working directory to it.
// * If it receives - it goes back to the last working directory.
// */
//    {
//        if(getNumberOfArguments() > 2)
//        {
//            std::cerr << "smash error: cd: too many arguments" << std::endl;
//            return;
//        }
//        //find current working directory
//        long max_length = pathconf(".", _PC_PATH_MAX);
//        char* PWD = new char[max_length];
//        if (getcwd(PWD, sizeof(PWD)) == nullptr)
//        {
//            perror("smash error: getcwd failed");
//            delete[] PWD;
//            return;
//        }
//        string newPath = getParsedCommand()[1];
//        if(newPath == "-") //change the current working directory to the last working directory
//        {
//            if(!(*lastPwd)) //the last working directory is empty
//            {
//                std::cerr << "smash error: cd: OLDPWD not set" << std::endl;
//                delete[] PWD;
//                return;
//            }
//            else
//            {
//                if(chdir(*lastPwd) == -1)
//                {
//                    perror("smash error: chdir failed");
//                    delete[] PWD;
//                    return;
//                }
//                else
//                {
//                    if (*lastPwd)
//                        delete[] lastPwd;
//                    *lastPwd = PWD; ///dana note : check if there are more things to delete or errors
//                }
//            }
//        }
//        else
//        {
//            if(chdir(getParsedCommand()[1]) == -1)
//            {
//                perror("smash error: chdir failed");
//                delete[] PWD;
//                return;
//            }
//            else
//            {
//                if (*lastPwd)
//                    delete[] lastPwd;
//                *lastPwd = PWD;
//            }
//        }
//    }
//}
void ChangeDirCommand::execute()
{
/**
 * Change directory (cd) command receives a single argument <path> that describes the
 * relative or full path to change the current working directory to it.
 * If it receives - it goes back to the last working directory.
 */
    {
        removeAmpersand();
        if(getNumberOfArguments() > 2)
        {
            std::cerr << "smash error: cd: too many arguments" << std::endl;
            return;
        }
        //find current working directory
        char* PWD = getcwd(NULL, 0);
        if (PWD == nullptr)
        {
            perror("smash error: getcwd failed");
            return;
        }

        string newPath = getParsedCommand()[1];
        if(newPath == "-") //change the current working directory to the last working directory
        {
            if(!(*lastPwd)) //the last working directory is empty
            {
                std::cerr << "smash error: cd: OLDPWD not set" << std::endl;
                return;
            }
            else
            {
                if(chdir(*lastPwd) == -1)
                {
                    perror("smash error: chdir failed");
                    return;
                }
                else
                {
//                    if (*lastPwd)
//                        delete[] lastPwd;
                    *lastPwd = PWD; ///dana note : check if there are more things to delete or errors
                }
            }
        }
        else
        {
            if(chdir(getParsedCommand()[1]) == -1)
            {
                perror("smash error: chdir failed");
                return;
            }
            else
            {
//                if (*lastPwd)
//                    delete[] lastPwd;
                *lastPwd = PWD;
            }
        }
    }
}


/**
 * dima added jobsCommand class implementation
 */
JobsCommand::JobsCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line)
{
    jobsList=jobs;
}

void JobsCommand::execute()
{
    removeAmpersand();
    jobsList->printJobsList();
    return;
}


/**
 * dima added foreground class implementation
 */
ForegroundCommand::ForegroundCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line)
{
    jobsList = jobs;
}

void ForegroundCommand::execute()
{
    SmallShell& smash = SmallShell::getInstance();
    removeAmpersand();
    if(getNumberOfArguments() > 2) //too many arguments passed => invalid args msg
    {
        std::cerr << "smash error: fg: invalid arguments " << std::endl;
        return;
    }
    pid_t jobPid;
    int jobId;
    JobsList::JobEntry* jobEntry;
    if(getNumberOfArguments()==1) //no jobId was provided =>check if list is empty if so empty list msg ow last job
    {
        jobEntry = jobsList->getLastJob(&jobId);
        if(jobsList == nullptr) //empty list
        {
            std::cerr << "smash error: fg: jobs list is empty " <<std::endl;
            return;
        }
        else{
            jobPid = jobEntry->getJobPid();
        }
    }
    else if(getNumberOfArguments()==2)
    {
        if(!string_of_digits(getParsedCommand()[1]))
        {
            std::cerr << "smash error: fg: invalid arguments " << std::endl;
            return;
        }
        char* arg = getParsedCommand()[1];
        jobId = convert_to_number(arg);
        jobEntry = jobsList->getJobById(jobId);
        if(jobEntry == nullptr)
        {
            std::cerr << "smash error: fg: job-id "<< jobEntry->getJobId() << "does not exist " << std::endl;
            return;
        }
        jobPid = jobEntry->getJobPid();
    }
    std::cout << jobEntry->getCommandLine() << ":" << jobPid << std::endl;
    smash.setCurrPid(jobPid); // ADD FUNCTION

    int kill_res = kill(jobPid, SIGCONT);
    if(kill_res == -1)
    {
        perror("smash error: kill failed");
        return;
    }

    int waitpid_res = waitpid(jobPid, nullptr, WUNTRACED);

    if(waitpid_res == -1)
    {

        perror("smash error: waitpid failed");
        return;
    }

    return;
}
/**
 * dima added quitCommand class
 */
QuitCommand::QuitCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line)
{
    jobsList = jobs;
}

void QuitCommand::execute()
{
    SmallShell& smash = SmallShell::getInstance();
    removeAmpersand();
    if(getNumberOfArguments() == 1) //no specified arguments
    {
        jobsList->killAllJobs();
        exit(0);
    }
    if(getNumberOfArguments() == 2)
    {
        char* func_arg = getParsedCommand()[1];
        if(strcmp(func_arg,"kill") != 0)
        {
            std::cerr << "smash error: quit: invalid arguments" << std::endl;
            return;
        }
        jobsList->printJobsListBeforeKilling();
        jobsList->killAllJobs();
    }
}

/**
 * dima added kill command class
 */

KillCommand::KillCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line)
{
    jobsList = jobs;
}

void KillCommand::execute()
{
    SmallShell& smash = SmallShell::getInstance();
    char** parsed = getParsedCommand();
    if(getNumberOfArguments() != 3)
    {
        std::cerr << "smash error: kill: invalid arguments" <<std::endl;
        return;
    }
    else if(!string_of_digits(parsed[1]) || !string_of_digits(parsed[2]))
    {
        std::cerr << "smash error: kill: invalid arguments" <<std::endl;
        return;
    }
    int sig_num = atoi(parsed[1]);
    int jobId = atoi(parsed[2]);
    sig_num = -1 * sig_num;
    if(sig_num <= 0 || sig_num > 31 || jobId==0)
    {
        std::cerr << "smash error: kill: invalid arguments" << std::endl;
        return;
    }
    JobsList::JobEntry* currJob = jobsList->getJobById(jobId);
    if(currJob == nullptr)
    {
        std::cerr << "smash error: kill: job-id " << jobId << " does not exist"<< std::endl;
        return;
    }
    pid_t job_pid = currJob->getJobPid();
    int kill_res = kill(job_pid, sig_num);
    if(kill_res == -1)
    {
        cerr << "the job id is: " << jobId << endl;
        cerr << "the signal number is: " << sig_num << endl;
        perror("smash error: kill failed");
        return;
    }
    cout << "signal number " << sig_num << " was sent to pid " << job_pid << endl;
    if(sig_num == 9)
    {
        jobsList->getJobById(jobId);
    }

}

/**
* dima added jobsEntry class implementation (getters and setters)
*/

JobsList::JobEntry::JobEntry(int jobId, pid_t jobPid, bool stopped, const char* new_cmd_line) :
        jobId(jobId),jobPid(jobPid),stopped(stopped),cmd_line(new_cmd_line){
//    char* tmp = new char[COMMAND_MAX_ARGS];
//    strcpy(tmp,new_cmd_line);
//    cmd_line = tmp;
}

JobsList::JobEntry::~JobEntry()
{
    delete [] cmd_line;
}

int JobsList::JobEntry::getJobId()
{
    return jobId;
}

pid_t JobsList::JobEntry::getJobPid()
{
    return jobPid;
}


bool JobsList::JobEntry::isStopped()
{
    return stopped;
}

void JobsList::JobEntry::stopJob()
{
    stopped=true;
    return;
}

void JobsList::JobEntry::startJob()
{
    stopped = false;
    return;
}

const char* JobsList::JobEntry::getCommandLine()
{
    return cmd_line;
}

int JobsList::size()
{
    int size=0;
    auto iterator = jobsList.begin();
    while(iterator != jobsList.end())
    {
        size++;
        ++iterator;
    }
    return size;
}

void JobsList::printJobsListBeforeKilling()
{
    removeFinishedJobs();
    std::cout << "smash: sending SIGKILL signal to " << jobsList.size() << " jobs" << std::endl;
    auto iterator = jobsList.begin();
    while(iterator != jobsList.end()){
        JobEntry* currJob = *(iterator);
        std::cout << currJob->getJobPid() << ": " << currJob->getCommandLine() << std::endl;
        ++iterator;
    }
    return;
}

/**
* dima added jobs class implementation
*/

void JobsList::addJob(const char *cmd_line,pid_t job_pid,bool isStopped)
{
    std::cout << "we are in add job func" <<std::endl;
    removeFinishedJobs();
    JobEntry* newJob= new JobEntry(maxJobId+1,job_pid,isStopped
            ,cmd_line);
    jobsList.insert(jobsList.end(),newJob);
    updateMaxJobId();
}

void JobsList::printJobsList()
{
    std::cout<< "we are in print jobs list func" << std::endl;
    removeFinishedJobs();
    updateMaxJobId();
    std::cout<<"max job id is " << maxJobId << std::endl;
    auto iterator = jobsList.begin();
    while(iterator != jobsList.end())
    {
        JobEntry* currJob = *(iterator);
        std::cout << "[" << currJob->getJobId() << "]" << currJob->getCommandLine() << std::endl;
        ++iterator;
    }
}

void JobsList::killAllJobs()
{
    auto iterator = jobsList.begin();
    while(iterator != jobsList.end())
    {
        int kill_res = kill((*iterator)->getJobPid(), SIGKILL);
        if(kill_res == -1)
        {
            perror("smash error: kill failed");
            return;
        }
        delete (*iterator);
        ++iterator;
    }
    jobsList.clear();
}

void JobsList::removeFinishedJobs()
{
    auto iterator = jobsList.begin();
    while(iterator!=jobsList.end())
    {
        int status =0;
        pid_t wait_res = waitpid( (*iterator)->getJobPid(), &status , WNOHANG);
        if(wait_res == -1)
        {
            perror("smash error: waitpid failed");
            return;
        }
        if(wait_res!=0) //if wait_res = 0 the job is not finished
        {
            delete (*iterator);
            jobsList.erase(iterator);
            iterator = jobsList.begin();
        }
        ++iterator;
    }
    updateMaxJobId();
}

JobsList::JobEntry* JobsList::getJobById(int jobId)
{
    auto iterator = jobsList.begin();
    while (iterator!=jobsList.end())
    {
        JobsList::JobEntry* currJob = *(iterator);
        int currId = currJob->getJobId();
        if(currId==jobId)
        {
            return currJob;
        }
        ++iterator;
    }
    return nullptr;
}

void JobsList::removeJobById(int jobId)
{
    auto iterator = jobsList.begin();
    while(iterator!= jobsList.end())
    {
        JobsList::JobEntry* to_remove= *(iterator);
        if(to_remove->getJobId()==jobId)
        {
            delete *(iterator);
            jobsList.erase(iterator);
            iterator= jobsList.end();
        }
        ++iterator;
    }
    updateMaxJobId();
    return;
}

JobsList::JobEntry* JobsList::getLastJob(int *lastJobId)
{
    *lastJobId= maxJobId;
    return getJobById(maxJobId);
}

JobsList::JobEntry* JobsList::getLastStoppedJob(int *jobId)
{
    int maxStoppedJobId =0;
    *jobId = 0;
    auto iterator = jobsList.begin();
    while(iterator!=jobsList.end())
    {
        JobEntry* currJob = *(iterator);
        if(currJob->isStopped())
        {
            if(maxStoppedJobId < currJob->getJobId())
            {
                maxStoppedJobId = currJob->getJobId();
            }
        }
        ++iterator;
    }
    *jobId = maxStoppedJobId;
    return getJobById(maxStoppedJobId);
}

void JobsList::updateMaxJobId()
{
    int lastId = 0;
    auto iterator = jobsList.begin();
    while(iterator!=jobsList.end())
    {
        JobsList::JobEntry* currJob = *(iterator);
        if(lastId < currJob->getJobId())
        {
            lastId = currJob->getJobId();
        }
        ++iterator;
    }
    maxJobId = lastId;
}

/*-----------------------SMALLSHELL IMPLEMENTATION-----------------------------*/
SmallShell::~SmallShell()
{
    delete[] curr_prompt;
    delete[] lastPwd;
    delete[] jobsList;
}


pid_t SmallShell::getCurrPid() const {
    return curr_pid;
}

char *SmallShell::getCurrPrompt() const {
    return curr_prompt;
}

char *SmallShell::getLastPwd() const {
    return lastPwd;
}

void SmallShell::setCurrPid(pid_t currPid) {
    curr_pid = currPid;
}

void SmallShell::setLastPwd(char *lastPwd) {
    SmallShell::lastPwd = lastPwd;
}

void SmallShell::setCurrPrompt(char *currPrompt) {
    curr_prompt = currPrompt;
}

void SmallShell::setJobsList(JobsList *jobsList) {
    SmallShell::jobsList = jobsList;
}

JobsList* SmallShell::getJobsList() const {
    return jobsList;
}

char *SmallShell::getCurrCmd() const {
    return curr_cmd;
}

bool SmallShell::isFg() const {
    return isFG;
}


void SmallShell::setCurrCmd(char *currCmd) {
    curr_cmd = currCmd;
}

void SmallShell::setIsFg(bool isFg) {
    isFG = isFg;
}

Command * SmallShell::CreateCommand(const char* cmd_line)
{
    /**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
    {
        string cmd_s = _trim(string(cmd_line));
        string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

        if (firstWord.compare("pwd") == 0) {
            return new GetCurrDirCommand(cmd_line);
        }
        else if (firstWord.compare("showpid") == 0) {
            return new ShowPidCommand(cmd_line);
        }
        else if (firstWord.compare("chprompt") == 0)
        {
            return new ChangePromptCommand(cmd_line);
        }
        else if (firstWord.compare("cd") == 0)
        {
            return new ChangeDirCommand(cmd_line, &(this->lastPwd));
        }
        else if (firstWord.compare("jobs") == 0)
        {
            return new JobsCommand(cmd_line, this->jobsList);
        }
        else if (firstWord.compare("fg") == 0)
        {
            return new ForegroundCommand(cmd_line, this->jobsList);
        }
        else if (firstWord.compare("quit") == 0)
        {
            return new QuitCommand(cmd_line, this->jobsList);
        }
        else if(firstWord.compare("kill") == 0)
        {
            return new KillCommand(cmd_line, this->jobsList);
        }
        else if(firstWord.compare("chmod") == 0)
        {
            return new ChmodCommand(cmd_line);
        }
        else if(strchr(cmd_line, '>') != nullptr) //redirection command
        {
            return new RedirectionCommand(cmd_line);
        }
        else
        {
            return new ExternalCommand(cmd_line);
        }
        return nullptr;
    }
}
void SmallShell::executeCommand(const char *cmd_line)
{
    Command* command = CreateCommand(cmd_line);
    getJobsList()->removeFinishedJobs();
    if(!isExternalCommand(cmd_line))
    {
        setIsFg(false);
    }
    else if(isExternalCommand(cmd_line) && _isBackgroundComamnd(cmd_line))
    {

        setIsFg(false);
    }
    else if(isExternalCommand(cmd_line) && !_isBackgroundComamnd(cmd_line))
    {
        setIsFg(false);
    }

    curr_pid = getpid();
    if(curr_pid == -1)
    {
        perror("smash error: getpid failed");
        return;
    }
    strcpy(curr_cmd, cmd_line);
    command->execute();
}

/**
 * dana Background implementation
 */


/**
 * dana external command implementation
 */
/*-----------------------EXTERNAL COMMAND IMPLEMENTATION-----------------------------*/

ExternalCommand::ExternalCommand(const char *cmd_line) : Command(cmd_line) {} ;

void ExternalCommand::execute()
{
    SmallShell& smash = SmallShell::getInstance();

    pid_t smash_pid = getpid();
    if(smash_pid == -1)
    {
        perror("smash error: getpid failed");
        return;
    }

    pid_t p = fork();
    if(p == -1)
    {
        perror("smash error: fork failed");
        return;
    }
    if(p > 0)//parent
    {
        smash.setCurrPid(p); // set smash pid to child pid
        if(_isBackgroundComamnd(getCommandLine())) //parent doesn't wait for background commands
        {
            smash.getJobsList()->addJob(getCommandLine(),getpid(),false);
        }
        else
        {
            int status;
            if(waitpid(p, &status,WUNTRACED) == -1) //wait for child to finish
            {
                perror("smash error: waitpid failed");
                return;
            }
            smash.setCurrPid(smash_pid); //set back pid to parent pid
        }
    }
    if(p == 0)//son
    {

        if (setpgrp() == -1)
        {
            perror("smash error: setpgrp failed");
            return;
        }

        pid_t new_pid = getpid(); //get son pid
        if(new_pid == -1)
        {
            perror("smash error: getpid failed");
            return;
        }

        smash.setCurrPid(new_pid); // set shell to son pid

        if(strchr(getCommandLine(), '*') || strchr(getCommandLine(), '?')) //Complex external command
        {
            removeAmpersand();

            char* argv[] = {(char*) "/bin/bash", (char*) "-c",
                            (char*) getParsedCommand(), (char*) nullptr};

            int res = execv(argv[0], argv);
            if(res == -1)
            {
                perror("smash error: execv failed");
                exit(0);
            }
            return;
        }
        else // Simple external command
        {

            removeAmpersand();
            if(execvp(getParsedCommand()[0], getParsedCommand()) == -1)
            {
                perror("smash error: execvp failed");
                exit(0);
            }
        }
    }
}

/*--------------------------------SPECIAL COMMANDS------------------------*/
ChmodCommand::ChmodCommand(const char *cmd_line) : BuiltInCommand(cmd_line){};

void ChmodCommand::execute()
{
    if(getNumberOfArguments()!= 3)
    {
        cerr << "smash error: chmod: invalid arguments" <<endl;
        return;
    }
    int perm = atoi(getParsedCommand()[1]);
    if(perm < 0 || perm > 777)
    {
        cerr << "smash error: chmod: invalid arguments" <<endl;
        return;
    }
    mode_t permissions = ((perm/100) << 6) | (((perm/10)%10) << 3) | (perm % 10);
    if(chmod(getParsedCommand()[2],permissions)==-1){
        perror("smash error: chmod failed");
    }
}

/*----------------------------redirection func-------------------------*/
std::string filename;
std::string command;

RedirectionCommand::RedirectionCommand(const char* cmd_line) : Command(cmd_line)
{
    command = ((std::string) cmd_line).substr(0, ((std::string) cmd_line).find('>'));
    if (std::string(command).end().operator*() == '>')
    {
        std::string(command).pop_back();
    }

    filename = ((std::string) cmd_line).substr(((std::string) cmd_line).find_last_of('>'));
    if (std::string(filename).begin().operator*() == '>')
    {
        filename = std::string(filename).substr(1, std::string(filename).size() - 1);

    }
    filename = _trim(filename);
}


void RedirectionCommand::execute() {
    SmallShell& smash = SmallShell::getInstance();

    string str(getCommandLine());
    pid_t p = fork();
    if(p == -1)
    {
        perror("smash error: fork failed");
        return;
    }
    else if (p == 0)
    {
        setpgrp();

        int fd;
        if(str.find(">>")!=string::npos) // << symbol
        {
            fd = open(filename.c_str(), O_RDWR | O_APPEND | O_CREAT, 0655);
            if(fd == -1)
            {
                perror("smash error: open failed");
                return;
            }
        }
        else // < symbol
        {
            fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0655);
            if(fd == -1)
            {
                perror("smash error: open failed");
                return;
            }
        }
        if (close(1) == -1) {
            perror("smash error: close failed");
            exit(0);
        }
        if (dup2(fd, 1) == -1) {
            perror("smash error: dup2 failed");
            return;
        }
        smash.executeCommand(command.c_str());
        if(close(fd)==-1)
        {
            perror("smash error: close failed");
            return;
        }
        exit(0);
    }
    else
    {
        if (waitpid(p, NULL, WUNTRACED) == -1) {
            perror("smash error: waitpid failed");
            return;
        }
    }
}

