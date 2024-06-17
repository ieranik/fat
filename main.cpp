#include <iostream>
#include <string>
#include <map>
#include "FAT.cpp"
#include "buffer.h"

using namespace std;

unsigned char buffer[1024];

int main()
{
    FAT table;
    map <int, int> open_files;
    map <int, int>::iterator it;
    string command, path_name;
    int block_num, file_size, file_number, length, offset, cur_pos, start_index, end_index;
    unsigned char letter;

    while(true)
    {
        cin>>command;
        if(command.compare("cf")==0)
        {
            cin>>path_name;
            cin>>file_size;
            table.createFile(path_name, file_size);
        }
        else if(command.compare("cd")==0)
        {
            cin>>path_name;
            table.createDirectory(path_name);
        }
        else if(command.compare("df")==0)
        {
            cin>>path_name;
            table.deleteFile(path_name);
        }
        else if(command.compare("dd")==0)
        {
            cin>>path_name;
            table.deleteDirectory(path_name);
        }
        else if(command.compare("o")==0)
        {
            cin>>path_name;
            file_number=table.getFileNumber(path_name);
            open_files[file_number]=0;
            cout<<file_number<<endl<<endl;
        }
        else if(command.compare("c")==0)
        {
            cin>>file_number;
            it=open_files.find(file_number);
            open_files.erase(it);
        }
        else if(command.compare("s")==0)
        {
            cin>>file_number;
            cin>>offset;
            it=open_files.find(file_number);
            open_files.erase(it);
            open_files[file_number]=offset;
        }
        else if(command.compare("r")==0)
        {
            cin>>file_number;
            cin>>length;
            offset=open_files[file_number];
            table.readFile(file_number, length, offset);

            for(int i=0; i<length; i++)cout<<buffer[i];
            cout<<endl<<endl;

            it=open_files.find(file_number);
            open_files.erase(it);
            open_files[file_number]=offset+length;
        }
        else if(command.compare("w")==0)
        {
            cin>>file_number;
            cin>>length;
            cin>>letter;
            offset=open_files[file_number];
            table.writeFile(file_number, length, offset, letter);

            it=open_files.find(file_number);
            open_files.erase(it);
            open_files[file_number]=offset+length;
        }
        else if(command.compare("rs")==0)
        {
            cin>>path_name;
            cin>>file_size;
            table.resizeFile(path_name, file_size);
        }
        else if(command.compare("pf")==0)
        {
            cin>>path_name;
            table.printFile(path_name);
        }
        else if(command.compare("pd")==0)
        {
            cin>>path_name;
            table.printDirectory(path_name);
        }
        else if(command.compare("pt")==0)
        {
            cin>>start_index;
            cin>>end_index;
            table.printFAT(start_index, end_index);
        }
        else if(command.compare("ex")==0)
        {
            break;
        }
    }

    return 0;
}
