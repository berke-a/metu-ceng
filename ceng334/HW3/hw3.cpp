#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "parser.h"
#include "fat32.h"

using namespace std;

void print_current_dir();
unsigned char lfn_checksum(const unsigned char *pFCBName);
uint32_t getOffset(uint32_t dir);
void ls(char *arg1, char *arg2);
void cd(string targetDir, bool lsFlag, bool lsArg, bool catFlag);
void cat(string arg1);
string readDirectory(FatFileEntry &directory);

BPB_struct BPB;
FILE *fp;
uint32_t current;
uint32_t root;
vector<string> currentDirectory;

string months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

int main(int argc, char *argv[])
{

    ///// 	OPEN IMAGE
    fp = fopen(argv[1], "r+");
    fread(&BPB, 36, 1, fp);
    fread(&BPB.extended, 54, 1, fp);
    current = BPB.extended.RootCluster;
    root = BPB.extended.RootCluster;
    /////

    while (1)
    {
        print_current_dir();

        parsed_input *args = new parsed_input();

        char buffer[512];

        fgets(buffer, 512, stdin);

        if (buffer[0] == '\n')
            continue;

        parse(args, buffer);

        if (args->type == QUIT)
        {
            delete fp;
            clean_input(args);
            delete args;
            return 0;
        }
        if (args->type == CD)
        {
            cd(args->arg1, false, false, false);
        }
        if (args->type == LS)
        {
            ls(args->arg1, args->arg2);
        }
        if (args->type == MKDIR)
        {
        }
        if (args->type == TOUCH)
        {
        }
        if (args->type == MV)
        {
        }
        if (args->type == CAT)
        {
            cat(args->arg1);
        }
        if (args->type == ERR)
        {
            // cerr << "Invalid command entered." << endl;
        }

        clean_input(args); // Cleaning input in case of a memory leak
        delete args;
    }
    return 0;
}

void print_current_dir() // Printing current path
{
    cout << "/";
    for (int i = 0; i < currentDirectory.size(); i++)
    {
        if (i != currentDirectory.size() - 1)
        {
            cout << currentDirectory[i] << "/";
        }
        else
        {
            cout << currentDirectory[i];
        }
    }
    cout << "> ";
}

vector<string> parseDel(string s, string del) // Parsing path with the '/' delimeter
{
    size_t pos = 0;
    string tmp;
    vector<string> res;
    while ((pos = s.find(del)) != std::string::npos)
    {
        tmp = s.substr(0, pos);
        res.push_back(tmp);
        s.erase(0, pos + del.length());
    }
    res.push_back(s);
    return res;
}

void cat(string arg1)
{

    vector<string> target;

    if (arg1.find("/") != string::npos)
    {
        cd(arg1, false, false, true);
        return;
    }

    FatFileEntry directory;

    fseek(fp, getOffset(current), SEEK_SET);

    while (1)
    {
        string str;

        fread(&directory.lfn, 32, 1, fp);

        if (directory.lfn.sequence_number == 0x00)
            break; // Break if it is EOC

        if (directory.lfn.sequence_number == 0xe5)
            continue; // Skip the cluster if previously deleted

        str = readDirectory(directory);

        if (directory.msdos.attributes == 0x10)
            continue; // We do not need folders.

        if (directory.msdos.attributes == 0x1 || directory.msdos.attributes == 0x20)
        {
            if (directory.msdos.extension[0] != ' ')
            {
                str += ".";
                str += tolower(directory.msdos.extension[0]);
                if (directory.msdos.extension[1] == ' ')
                    break;
                str += tolower(directory.msdos.extension[1]);
                if (directory.msdos.extension[2] == ' ')
                    break;
                str += tolower(directory.msdos.extension[2]);
            }
        }

        if (str == arg1)
        { // FILE FOUND

            uint32_t fileCluster = directory.msdos.eaIndex << 2 | directory.msdos.firstCluster;

            if (directory.msdos.fileSize == 0 || fileCluster == 0)
                break; // File does not exists or empty.

            string wholeFile = ""; // This string holds the content.

            int done = 0;
            uint32_t fileSize = directory.msdos.fileSize;

            while (true)
            {

                fseek(fp, getOffset(fileCluster), SEEK_SET);

                string printt;

                printt.resize(BPB.BytesPerSector * BPB.SectorsPerCluster);

                if (done)
                    break;

                if (fileSize > BPB.BytesPerSector * BPB.SectorsPerCluster)
                {

                    /**
                     * FileSize is bigger than cluster size therefore we read
                     * the whole cluster and get the next cluster.
                     */

                    fread(&printt[0], sizeof(char), BPB.BytesPerSector * BPB.SectorsPerCluster, fp);
                    wholeFile += printt;
                    fileSize -= BPB.BytesPerSector * BPB.SectorsPerCluster;

                    // This code segment gets nextCluster
                    fseek(fp, (BPB.BytesPerSector * BPB.ReservedSectorCount) + (4 * fileCluster), SEEK_SET);
                    fileCluster &= 0x0fffffff;
                    fread(&fileCluster, 4, 1, fp);
                    if (fileCluster >= 0x0FFFFFF8)
                        done = 1;
                }
                else
                {

                    /**
                     * Filesize is smaller than the cluster size
                     * Therefore we read the remaining file and set done.
                     */

                    fread(&printt[0], sizeof(char), fileSize, fp);
                    wholeFile += printt;
                    done = 1;
                }
            }
            cout << wholeFile << endl;
        }
    }
}

void cd(string targetDir, bool lsFlag, bool lsArg, bool catFlag) // flags are used to locate the path for other commands
{

    vector<string> backup = currentDirectory;

    vector<string> target;

    uint16_t targetCluster;

    FatFileEntry directory;

    string mkdirString, catString;

    if (targetDir.find("/") != string::npos)
    {
        target = parseDel(targetDir, "/");
    }
    else
    {
        target.push_back(targetDir);
    }

    if (catFlag)
    {
        catString = target.back();
        target.pop_back();
    }

    if (target.size() != 1 && target.back() == "")
        target.pop_back();

    uint32_t prevCurrent = current;

    bool flag = false; // set flag false if any of the directories do not exists.

    for (int i = 0; i < target.size(); i++)
    {
        if (target[i] == "" && i == 0)
        {
            current = root;
            while (!currentDirectory.empty())
            {
                currentDirectory.pop_back();
            }
            flag = true;
            continue;
        }
        if (target[i] == ".")
            continue;
        flag = false;
        fseek(fp, getOffset(current), SEEK_SET);
        while (1)
        {

            fread(&directory.lfn, 32, 1, fp);

            if (directory.lfn.sequence_number == 0x00)
                break; // Break if it is EOC

            if (directory.lfn.sequence_number == 0xe5)
                continue; // Skip the cluster if previously deleted

            // If dot case check here
            if ((char)directory.lfn.sequence_number == '.' && (char)directory.lfn.name1[0] == '.')
            {

                if (target[i] == "..")
                {
                    targetCluster = directory.lfn.firstCluster;
                    flag = true;
                    break;
                }
            }

            // Read the name if not a dot
            string str;
            if (directory.lfn.attributes == 0x0f)
            {
                str = readDirectory(directory);
                if (directory.msdos.attributes == 0x10 && str == target[i])
                {
                    targetCluster = directory.msdos.firstCluster;
                    flag = true;
                    break;
                }
            }
        }
        if (!flag)
        {
            // If the directory does not exists break
            break;
        }
        if (target[i] == "..")
        {

            // If the target is .. then we go back to the previous directory
            if (currentDirectory.empty())
            {
                flag = false;
                break;
            }
            currentDirectory.pop_back();
            current = targetCluster;
        }
        else
        {

            // If the target is not .. then we go to the next directory
            currentDirectory.push_back(target[i]);
            current = targetCluster;
        }
    }

    // Call functions according to the flags.

    if (lsFlag && flag)
    {
        if (lsArg)
            ls("-l", nullptr);
        else
            ls(nullptr, nullptr);
    }

    if (catFlag && flag)
    {
        cat(catString);
    }

    if (!flag || lsFlag || catFlag)
    {
        // If the directory does not exists or lsFlag or catFlag is true then we go back to the previous directory
        currentDirectory = backup;
        current = prevCurrent;
    }
}

void ls(char *arg1, char *arg2)
{
    FatFileEntry directory;

    if (!arg2 && !arg1) // First case just "ls"
    {
        int count = 0;
        fseek(fp, getOffset(current), SEEK_SET);
        while (1)
        {
            string str;

            fread(&directory.lfn, 32, 1, fp);

            if (directory.lfn.sequence_number == 0x00)
                break; // Break if it is EOC

            if (directory.lfn.sequence_number == 0xe5)
                continue; // Skip the cluster if previously deleted

            str = readDirectory(directory);

            if (directory.msdos.attributes == 0x1 || directory.msdos.attributes == 0x20) // Read extension if it is a file
            {
                if (directory.msdos.extension[0] != ' ')
                {
                    str += ".";
                    str += tolower(directory.msdos.extension[0]);
                    if (directory.msdos.extension[1] == ' ')
                        break;
                    str += tolower(directory.msdos.extension[1]);
                    if (directory.msdos.extension[2] == ' ')
                        break;
                    str += tolower(directory.msdos.extension[2]);
                }
            }

            if (!str.empty() && directory.msdos.filename[0] != 0xe5 && directory.msdos.filename[0] != 0x05 && directory.msdos.filename[0] != 0x2e)
            {
                cout << str << " ";
                count++;
            }
        }
        if (count)
            cout << endl;
    }

    if (arg1 && !arg2) // Second case with 1 argument. Can be -l or [dir]
    {
        if (!strcmp(arg1, "-l")) // -l case
        {
            fseek(fp, getOffset(current), SEEK_SET);

            while (1)
            {
                fread(&directory.lfn, 32, 1, fp);

                string str;

                if (directory.lfn.sequence_number == 0x00)
                    break; // Break if it is EOC

                if (directory.lfn.sequence_number == 0xe5)
                    continue; // Skip the cluster if previously deleted

                str = readDirectory(directory);

                if (directory.msdos.attributes != 0x10)
                {
                    if (directory.msdos.extension[0] != ' ')
                    {
                        str += ".";
                        str += tolower(directory.msdos.extension[0]);
                        if (directory.msdos.extension[1] == ' ')
                            break;
                        str += tolower(directory.msdos.extension[1]);
                        if (directory.msdos.extension[2] == ' ')
                            break;
                        str += tolower(directory.msdos.extension[2]);
                    }
                }

                if (!str.empty() && directory.msdos.filename[0] != 0xe5 && directory.msdos.filename[0] != 0x05 && directory.msdos.filename[0] != 0x2e)
                {

                    if (directory.msdos.attributes == 0x10)
                    {

                        /**
                         * DIRECTORY
                         * drwx------ 1 root root 0 <last_modified_date_and_time> <filename>
                         * drwx------ 1 root root 0 May 21 15:37 t5
                         */

                        uint16_t modDate = directory.msdos.modifiedDate;
                        int day = (modDate & 0x001f);
                        int month = (modDate & 0x01e0) >> 5;

                        uint16_t modTime = directory.msdos.modifiedTime;
                        int hour = (modTime >> 11) % 23;
                        int minute = (modTime & 0x07e0) >> 5;

                        cout << "drwx------ 1 root root 0 ";
                        cout << months[month - 1] << " ";

                        if (day < 10)
                        {
                            cout << "0";
                        }
                        cout << day << " ";

                        if (hour < 10)
                        {
                            cout << "0";
                        }
                        cout << hour << ":";
                        if (minute < 10)
                        {
                            cout << "0";
                        }
                        cout << minute << " ";

                        cout << str << endl;
                    }

                    if (directory.msdos.attributes == 0x1 || directory.msdos.attributes == 0x20)
                    {
                        /**
                         * FILE
                         * -rwx-- -- --1 root root<file_size_in_bytes><last_modified_date_and_time><filename>
                         * -rwx------ 1 root root 2464 May 25 10:12 f1
                         */

                        uint16_t modDate = directory.msdos.modifiedDate;
                        int day = (modDate & 0x001f);
                        int month = (modDate & 0x01e0) >> 5;

                        uint16_t modTime = directory.msdos.modifiedTime;
                        int hour = (modTime >> 11) % 23;
                        int minute = (modTime & 0x07e0) >> 5;

                        cout << "-rwx------ 1 root root ";
                        cout << directory.msdos.fileSize << " " << months[month - 1] << " "; //<< year - 2000 << " ";
                        if (day < 10)
                        {
                            cout << "0";
                        }
                        cout << day << " ";

                        if (hour < 10)
                        {
                            cout << "0";
                        }
                        cout << hour << ":";
                        if (minute < 10)
                        {
                            cout << "0";
                        }
                        cout << minute << " ";

                        cout << str << endl;
                    }
                }
            }
        }
        else
        {
            cd(arg1, true, false, false);
        }
    }

    if (arg1 && arg2) // Third case both -l and [dir]
    {
        cd(arg2, true, true, false);
    }
}

uint32_t getOffset(uint32_t dir)
{
    if (dir == 0)
        dir = 2;

    return ((dir - 2) * BPB.SectorsPerCluster * BPB.BytesPerSector) + (BPB.BytesPerSector * BPB.ReservedSectorCount) + (BPB.NumFATs * BPB.extended.FATSize * BPB.BytesPerSector); //+ (BPB.RootEntryCount * 32 * BPB.BytesPerSector);
}

unsigned char lfn_checksum(const unsigned char *pFCBName)
{
    int i;
    unsigned char sum = 0;

    for (i = 11; i; i--)
        sum = ((sum & 1) << 7) + (sum >> 1) + *pFCBName++;

    return sum;
}

string readDirectory(FatFileEntry &directory)
{

    /**
     * Read the directory entry and return the filename
     */

    string str = "";

    int first = 1;

    vector<FatFileEntry> LFNs;

    while (directory.lfn.attributes == 0x0f)
    {

        LFNs.push_back(directory);
        fread(&directory.lfn, 32, 1, fp);
    }

    LFNs.push_back(directory);
    FatFileEntry msdosEntry;
    msdosEntry = LFNs.back();
    LFNs.pop_back();

    for (int i = 0; i < LFNs.size(); i++)
    {

        directory = LFNs.back();
        LFNs.pop_back();

        for (int j = 0; j < 5; j++)
        {
            if (directory.lfn.name1[j] != 0xffff && directory.lfn.name1[j] != 0x0000 && directory.lfn.name1[j] != 0x00ff && directory.lfn.name1[j] != 0xff00)
            {
                str += (char)directory.lfn.name1[j];
                continue;
            }
        }
        for (int j = 0; j < 6; j++)
        {
            if (directory.lfn.name2[j] != 0xffff && directory.lfn.name2[j] != 0x0000 && directory.lfn.name2[j] != 0x00ff && directory.lfn.name2[j] != 0xff00)
            {
                str += (char)directory.lfn.name2[j];
                continue;
            }
        }
        for (int j = 0; j < 2; j++)
        {
            if (directory.lfn.name3[j] != 0xffff && directory.lfn.name3[j] != 0x0000 && directory.lfn.name3[j] != 0xff00 && directory.lfn.name3[j] != 0x00ff)
            {
                str += (char)directory.lfn.name3[j];
                continue;
            }
        }
    }

    directory = msdosEntry;
    LFNs.clear();

    return str;
}