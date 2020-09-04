
// creating vipervm shell
// code for shell

// include required libraries
# include <stdio.h> 
# include <stdlib.h>   // used to execute subprocess and commands
# include <string.h>   
# include <unistd.h>   // used for exit, getcwd,read, write, exec
# include <sys/wait.h>  
# include <sys/types.h>
# include <dirent.h> // for ls
# include <errno.h>
# include <sys/stat.h>
# include <fcntl.h>  // used for open




//built_in_str
char *built_in_str[] = {
    "cd",
    "pwd",
    "clear",
    "rmdir",
    "mkdir",
    "ls",
    "cat",
    "cp",
    "help"
};


/*************************************/



// clear the shell
int _clear()
{
    const char* blank = "\e[1;1H\e[2J";
    write(STDOUT_FILENO,blank,12);
}




// remove folder
void _rmdir(char* name)
{
    int statrm = rmdir(name);
    if(statrm==-1)
    {
        perror("Error in rmdir ");
    }
}




// make new directory
void _mkdir(char* name)
{
    int i = mkdir(name, 0777);// -rwxrwxrwx, read, write, & execute 
    if(i==-1)
    {
        perror("Error in mkdir ");
    }
}





//cat - to concatenate  files
void _cat(char *name){
    int store_open_file;
    int byte;
    
        store_open_file = open(name, O_RDONLY);  // open file in read only mode
        if (store_open_file < 0){      // error
            perror("error in cat");
        }
        else{
            while(read(store_open_file, &byte, 1)){   // read file one byte at a time
            write(STDOUT_FILENO, &byte, 1);  // write byre in stdout
            close(store_open_file);
            }
        }
    // return 0;
}




// helps user to know what commands are there in the shell
int _help(){
    
    int num = sizeof(built_in_str) / sizeof(char *);
    printf("\n");
    printf("List of all commands for vipervm shell:");
    for (int i =0; i<num; i++){
        printf("%s\n",built_in_str[i]);           // print all commands from built_in_str
    }
    return 1;
}





// change directory 
int _cd(char **input_command){
    if (input_command[1] == NULL){   // path can be changed if cd is successful
        fprintf(stderr, "expected arguement to \"cd\"\n");
    }
    else
    {
        if(chdir(input_command[1]) != 0){  // cd not successful
            perror("error in cd");
        }
    }
    return 1;
}





// /*Implement basic exit*/
// void _exit(){
//     return 0;
// }




/* Implement pwd function in shell - 1 prints, 0 stores*/
int _pwd()
{   
    char temp[1024];
    char* path=getcwd(temp, sizeof(temp)); // array for path, getcwd represents filename represent the current working directory
    if(path != NULL)
    { 
    
            fprintf(stdout, "%s\n", temp); // print path for pwd
    
    }
    else perror("Error in getcwd() : ");

}




/* copy one file to another */
void _cp(char* file1, char* file2)
{
    FILE *f1,*f2;        // two files f1 and f2
    f1 = fopen(file1,"r");   // open in read only mode
    if(f1 == NULL)                   // no value in file 1
    {
        perror(" Error in copy/cp file1 ");
        return;
    }
    f2 = fopen(file2,"r");       // open file2 in read only mode

    f2 = fopen(file2,"ab+");   // create the file2 if it doesn't exist
    fclose(f2);

    f2 = fopen(file2,"w+");   // write in file2
    if(f2 == NULL)
    {
        perror("Error in cp/copy file2 ");
        fclose(f1);
        return;
    }
    if(open(file2,O_WRONLY)<0 || open(file1,O_RDONLY)<0)   // both don't have any content
    {
        perror("Error in cp ");
        return;
    }
    char cp;
    while((cp=getc(f1))!=EOF)
    {
        putc(cp,f2);    // put content in file2
    }

    fclose(f1);
    fclose(f2);
}





// filetype to identify the type of file used in ls
void filetype(struct dirent* type,char* space)  // direct struct for name or type of file/directory, type blank space between the file
{
    if(type->d_type == DT_REG)          // identify regular file
    {
        printf("%s%s", type->d_name, space);
    }
    else if(type->d_type == DT_DIR)    // identify directory in the current position
    {
        printf("%s%s", type->d_name, space);
    }
    else                             
    {
        printf("%s%s", type->d_name, space);    // unknown file types
    }
}




// list current directory content
void _ls()
{
    int i=0;
    struct dirent **liststr;  // a dirent structure
    int lists = scandir(".", &liststr, 0, alphasort);    // lists will keep the scanned the content of directory, alphasort used to sort 
    if (lists >= 0)
    {
        for(i = 0; i < lists; i++ )
        {
            if(strcmp(liststr[i]->d_name,".")==0 || strcmp(liststr[i]->d_name,"..")==0)   // outputs . and .. 
            {
                continue;
            }
            else{
                filetype(liststr[i],"\t");  // outputs file from filetype function
            } 
            if(i%8==0) printf("\n");   // print 8 outputs in a line
        }
        printf("\n");
    }
    else
    {
        perror ("Error in ls ");
    }

}



/************************************/



// read command line for the input line in shell
char *read_line(void)
{
    int buffersize = 1024;
    char *buffer = malloc(sizeof(char) * buffersize);  // give a buffer a buffersize
    int c;  // require to call getchar
    int position = 0;
    
    while ( c != '\n' ) {
        c = getchar();   // read a character
        buffer[position] = c;  // store all char in  the buffer arra
        position++;
    }
    return buffer;
}




// parse the input command
char **parse(char *my_line)
{
    int buffer_size = 64;   // buffer size for array args
    int position = 0;   // for while loop
    char *arg;
    char **buffer = malloc(buffer_size * sizeof(char*));  
    
    arg = strtok(my_line, " \t\r\n\a");   // tokenizing a line, returns pointer of first token
    while (arg != NULL) {
        buffer[position] = arg;     // store pointer in buffer array 
        position++;
        arg = strtok(NULL, " \t\r\n\a");  // token delimiter = \t\r\n\a
    }
    buffer[position] = NULL;
    return buffer;
}


/******************************/

//check for built in commands
int check_commands(char **input_command)
{
    int num = sizeof(built_in_str) / sizeof(char *);
    if ( input_command[0] == NULL ) {
        return 0;
    }
    
    for (int i=0; i < num; i++) {
        if (strcmp(input_command[0], built_in_str[i]) == 0) {
            return 1;
        }
    }
    
    return 0;
}




// execute built in commands
int execute_builtin_commands(char **input_command,char **history,int count)
{
    // strcmp is to compare two strings
    if (strcmp(input_command[0], built_in_str[0]) == 0) {   // cd
        _cd(input_command);
    }
    
    if (strcmp(input_command[0], built_in_str[1]) == 0) {   // pwd
        _pwd();
    }

    if (strcmp(input_command[0], built_in_str[2]) == 0) {   // clear
        _clear();
    }

    if (strcmp(input_command[0], built_in_str[3]) == 0) {   // rmdir
        // char* foldername = input_command[1];
        _rmdir(input_command[1]);
    }

    if (strcmp(input_command[0], built_in_str[4]) == 0) {   // mkdir
        // char* foldername = input_command[1];
        _mkdir(input_command[1]);
    }

    if (strcmp(input_command[0], built_in_str[5]) == 0) {   // ls
        _ls();
    }

    if (strcmp(input_command[0], built_in_str[6]) == 0) {   // cat
        _cat(input_command[1]);
    }
    
    if (strcmp(input_command[0], built_in_str[7]) == 0) {   // cp
        _cp(input_command[1], input_command[2]);
    }

    if (strcmp(input_command[0], built_in_str[8]) == 0) {   // help
        _help();
    }
    // if (strcmp(input_command[0], built_in_str[6]) == 0) {  // exit
    //     _exit();
    // }
}






// basic loop of a shell
int basic_loop(){    
    char *my_line; // temporary variable
    char **input_command;   // my command, array of strings
    int status;   // for getting the status such an error occued or not
    int buffersize = 1024;   // used for assigning size of history
    char **history = malloc(sizeof(char) * buffersize);   // asigning memory
    int count = 0;
    int child_pid;


    // read the commands entered by the user
    // parse the commands to understand
    // execute the commands

    while(1){
        // first print the starting script
        printf("%s", "vipervm@root >>");
        my_line = read_line();  // return the command which user recently entered
        // now parse the commands to understand
        input_command = parse(my_line);
        // now we need to ecute the command
        // execute will return the status for completion or not 
        // suppose script is cd viperassignment, then cd is the command and viperassignment is the argument

        // if the first place of script is not empty
        if (input_command[0] != 0){
            // now we need to checkk the first script is a command or not in check_commands
            if (check_commands(input_command)){
                if (input_command[0][0] != '!'){
                    history[count] = input_command[0];
                    count++;
                }
                execute_builtin_commands(input_command, history, count);  
                // to execute the builtin commands, we need the input command, the history and count parameter
            }
            else
            {
                history[count] = input_command[0];
                count++;
                // forking the original shell
                // we need a copy to run external commands
                
                // we cannot execute the command in the main parent process therefore we use fork
                child_pid = fork();
                if (child_pid == 0){
                    execvp(input_command[0], input_command);  // we will need execvp to run the external commands from child after forking
                    // expects program name and array, v = vector, p = program name
                }
                else {    // use waitpid to wait the process's state to change
                while(!WIFEXITED(status) && !WIFSIGNALED(status)){
                    waitpid(child_pid, &status, WUNTRACED);
                }
                }
            }
        }
        // we need to free the memory
        free(my_line);
        free(input_command);
    }
    return 0;
}



int main(int argc, char** argv)  // argv is a pointer
{
    // run an read evaluate/execute print loop
    // we will directly run a loop command which runs the basic loop 
    basic_loop();
    
    return 0;
}


