#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <sys/wait.h>
#include "parser.h"

using namespace std;

#define buf 256;

int main()
{
	vector<vector<vector<string>>> bundles;
	int sameNameFlag = 0;

	while (1)
	{
		char buffer[256];
		parsed_input *parsed = new parsed_input();
		fgets(buffer, 256, stdin);
		int err = parse(buffer, 0, parsed);

		if (parsed->command.type == PROCESS_BUNDLE_CREATE)
		{
			string bundleName(parsed->command.bundle_name);

			for (int i = 0; i < bundles.size(); i++)
			{
				if (bundles[i][0][0] == bundleName)
				{
					// clog << "Can not create two bundles with the same name." << endl;
					sameNameFlag = 1;
					break;
				}
			}
			if (sameNameFlag)
			{
				delete parsed;
				continue;
			}

			vector<vector<string>> newBundle;
			vector<string> tmp;
			tmp.push_back(bundleName);
			newBundle.push_back(tmp);
			vector<string> commands;

			while (1)
			{
				char buffer[256];
				delete parsed;
				parsed_input *parsed = new parsed_input();
				fgets(buffer, 256, stdin);
				int err = parse(buffer, 1, parsed);

				if (parsed->command.type == PROCESS_BUNDLE_STOP)
				{
					break;
				}
				while (*(parsed->argv))
				{
					string tmpCommand(*(parsed->argv));
					commands.push_back(tmpCommand);
					(parsed->argv)++;
				}

				newBundle.push_back(commands);
				commands.clear();
			}
			bundles.push_back(newBundle);
		}

		else if (parsed->command.type == PROCESS_BUNDLE_EXECUTION)
		{
			// clog << "pbe" << endl;

			if (parsed->command.bundle_count == 1)
			{ // if there is only one bundle to execute

				string name(parsed->command.bundles->name);
				int i, found = 0, inp = 0, out = 1;

				for (i = 0; i < bundles.size(); i++)
				{
					if (bundles[i][0][0] == name)
						break;
				}

				vector<vector<string>> execute = bundles[i];
				bundles.erase(bundles.begin() + i);

				/////////////////////////////////////////////////

				// execute[], inp, out

				// clog << "////////////" << parsed->command.bundles->input << "//////////////" << endl;

				
				for (int i = 1; i < execute.size(); i++)
				{
					if (fork() == 0)
					{ // child;
						if (parsed->command.bundles->input != NULL)
						{
							inp = open(parsed->command.bundles->input, O_RDONLY);

							if (inp < 0)
							{
								perror("file open failed");
								exit(EXIT_FAILURE);
							}
						}	
						if (parsed->command.bundles->output != NULL)
						{
							out = open(parsed->command.bundles->output, O_APPEND | O_WRONLY | O_CREAT, 0666);

							if (out < 0)
							{
								perror("file open failed");
								exit(EXIT_FAILURE);
							}
						}


						dup2(inp, 0); // direct the input output to the files if exists
						dup2(out, 1);

						vector<char *> execStrings{};
						for (auto &string : execute[i])
							execStrings.push_back(&string.front());
						execvp(execStrings[0], execStrings.data());
						perror(execStrings[0]);
						exit(1);
					}
				}
				// while (wait(0) != -1)
				for (int i = 1; i < execute.size(); i++)
				{
					wait(NULL);
				}
			}
			else // THERE IS MULTIPLE BUNDLES TO EXECUTE AND PIPE :(
			{
				// clog << "burada" << endl;
				int inBackup = dup(0);
				int outBackup = dup(1);
				int fdIn, fdOut;

				bundle_execution *bundleList = parsed->command.bundles;

				if (parsed->command.bundles->input != NULL)
				{
					fdIn = open(parsed->command.bundles->input, O_RDONLY);

					if (fdIn < 0)
					{
						perror("file open failed");
						exit(EXIT_FAILURE);
					}
				}
				else
				{
					fdIn = dup(inBackup);
				}

				// clog << "Ilk for loopdan oncesi" << endl;

				for (int i = 0; i < parsed->command.bundle_count; i++)
				{
					bundle_execution bundleToExecute = *bundleList;

					dup2(fdIn, 0);
					close(fdIn);

					if (i == parsed->command.bundle_count - 1)
					{
						if (bundleToExecute.output != NULL)
						{
							fdOut = open(bundleToExecute.output, O_APPEND | O_WRONLY | O_CREAT, 0666);
							// clog << "out opened" << endl;
							if (fdOut < 0)
							{
								perror("file open failed");
								exit(EXIT_FAILURE);
							}
						}
						else
						{
							fdOut = dup(outBackup);
						}
					}
					else
					{
						int fd[2];
						if (pipe(fd) < 0)
							perror("pipe error");
						fdOut = fd[1];
						fdIn = fd[0];
					}

					// clog << "pipe redirected" << endl;

					dup2(fdOut, 1);
					close(fdOut);

					// execute bundles here ? and repeater ???

					string name(bundleToExecute.name);

					int j, found = 0;

					for (j = 0; j < bundles.size(); j++)
					{
						if (bundles[j][0][0] == name)
						{
							found = 1;
							break;
						}
					}
					if (found == 0)
					{
						continue;
					}

					vector<vector<string>> execute = bundles[j];
					bundles.erase(bundles.begin() + j);

					for (int k = 1; k < execute.size(); k++)
					{
						// clog << "child forkluyorum" << endl;

						if (fork() == 0)
						{
							vector<char *> execStrings{};
							for (auto &string : execute[k])
								execStrings.push_back(&string.front());
							execvp(execStrings[0], execStrings.data());
							perror(execStrings[0]);
							exit(1);
						}
					}
					bundleList++;
					for (int i = 1; i < execute.size(); i++)
					{
						wait(NULL);
					}
				}
				dup2(inBackup, 0);
				close(inBackup);
				dup2(outBackup, 1);
				close(outBackup);
				for (int i = 0; i < parsed->command.bundle_count; i++)
				{
					wait(NULL);
				}
			}
		}
		else if (parsed->command.type == QUIT)
		{
			while (wait(NULL) > 0)
				;
			delete parsed;
			break;
		}
		else
		{
			// clog << "Invalid command." << endl;
		}
		delete parsed;
	}
	return 0;
}
