#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "ImageAnalysisService.h"
using namespace cv;
using namespace std;


std::string consoleOutput;

unsigned int splitstring(const std::string &txt, std::vector<std::string> &strs, char ch)
{
	unsigned int pos = txt.find(ch);
	unsigned int initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos + 1));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}

void AddConsoleOutput(string op)
{
	consoleOutput.append(op);
	consoleOutput.append("\n");
}
void DisplayStatus(string statusText)
{
	cout << endl;
	cout << statusText << endl;
	AddConsoleOutput(statusText);
}

void DisplayCommands()
{
	string command = "To load the image \n"
		"> INPUT_IMAGE_PATH *space* filename\n"
		"To Find region\n"
		"> FIND_REGION *space* seedx *space* seedy *space* tolerence\n"
		"To find perimeter\n"
		"> FIND_PERIMETER\n"
		"To make perimeter smooth\n"
		"> FIND_SMOOTH_PERIMETER\n"
		"To show input image\n"
		"> DISPLAY_IMAGE\n"
		"To show output image\n"
		"> DISPLAY_PIXELS region *OR* perimeter\n"
		"To save output\n"
		"> SAVE_PIXELS region *OR* perimeter *space* filename\n"
		"To save program output\n"
		"> SAVE_PROGRAM_OUTPUT filename\n"
		"To exit application\n"
		"> EXIT\n"
		"To get list of availabe commands\n"
		"> HELP\n";
	DisplayStatus(command);
}

void SaveProgramOutput(std::string path)
{
	std::ofstream op(path);
	op << consoleOutput;
	op.close();
	DisplayStatus("Program output saved");
}

int main()
{
	//below code provides the command line functionality
	ImageAnalysisService service;
	DisplayCommands();
	std::vector<std::string> args;
	Status returnval = Status::SUCCESS;
	while (cin)
	{
		cout << ">>";
		string line;
		std::getline(std::cin, line);
		AddConsoleOutput(line);
		args.clear();
		splitstring(line, args, ' ');

		int count = args.size();

		if (count <= 0)
		{
			DisplayStatus("Please enter valid command");
			continue;
		}

		args[0].erase(remove_if(args[0].begin(), args[0].end(), isspace), args[0].end());


		if (args[0] == "INPUT_IMAGE_PATH")
		{
			if (count < 2)
			{
				DisplayStatus("Please enter valid command");
				continue;
			}

			returnval = service.INITIALIZE(args[1]);
			if (returnval == Status::FAILURE)
			{
				DisplayStatus("Something went wrong. Please see the error message above.");
				continue;
			}
			else if (returnval == Status::INVALID_IMAGE)
			{
				DisplayStatus("Invalid Image path/file.");
				continue;
			}
			else
			{
				DisplayStatus("Image loaded sucessfully.");
			}
		}
		else if (args[0] == "FIND_REGION")
		{
			if (count < 4)
			{
				DisplayStatus("Please enter valid command");
				continue;
			}
			if (!service.IsIntitialized())
			{
				DisplayStatus("Please load input image first");
				continue;
			}
			int seedx = std::stoi(args[1]);
			int seedy = std::stoi(args[2]);
			int tolerence = std::stoi(args[3]);

			returnval = service.FIND_REGION(seedx, seedy, tolerence);
			if (returnval == Status::FAILURE)
			{
				DisplayStatus("Something went wrong. Please see the error message above.");
				continue;
			}
			else if (returnval == Status::SEED_POINT_OUT_OF_RANGE)
			{
				DisplayStatus("Please enter seed point within image bounds");
				continue;
			}
			else
			{
				DisplayStatus("Region found completed.");
			}
		}
		else if (args[0] == "FIND_PERIMETER")
		{
			if (!service.IsIntitialized())
			{
				DisplayStatus("Please load input image first");
				continue;
			}
			if (!service.IsRegionCalculated())
			{
				DisplayStatus("Please calculate region first");
				continue;
			}

			returnval = service.FIND_PERIMETER();
			if (returnval == Status::FAILURE)
			{
				DisplayStatus("Something went wrong. Please see the error message above.");
				continue;
			}
			else
			{
				DisplayStatus("Perimeter find completed");
			}
		}
		else if (args[0] == "FIND_SMOOTH_PERIMETER")
		{
			if (!service.IsIntitialized())
			{
				DisplayStatus("Please load input image first");
				continue;
			}
			if (!service.IsRegionCalculated())
			{
				DisplayStatus("Please calculate region first");
				continue;
			}
			if (!service.IsPerimeterCalculated())
			{
				DisplayStatus("Please calculate perimeter first");
				continue;
			}
			returnval = service.FIND_SMOOTH_PERIMETER();
			if (returnval == Status::FAILURE)
			{
				DisplayStatus("Something went wrong. Please see the error message above.");
				continue;
			}
			else
			{
				DisplayStatus("Perimeter smoothening completed");
			}
		}
		else if (args[0] == "DISPLAY_IMAGE")
		{
			if (!service.IsIntitialized())
			{
				DisplayStatus("Please load input image first");
				continue;
			}
			returnval = service.DISPLAY_IMAGE();
			if (returnval == Status::FAILURE)
			{
				DisplayStatus("Something went wrong. Please see the error message above.");
				continue;
			}
		}
		else if (args[0] == "DISPLAY_PIXELS")
		{
			if (count < 2)
			{
				DisplayStatus("Please enter valid command");
				continue;
			}
			if (!service.IsIntitialized())
			{
				DisplayStatus("Please load input image first");
				continue;
			}

			args[1].erase(remove_if(args[1].begin(), args[1].end(), isspace), args[1].end());

			OutputImageType type;
			if (args[1] == "perimeter")
			{
				type = OutputImageType::PERIMETER;
			}
			else if (args[1] == "region")
			{
				type = OutputImageType::REGION;
			}
			else
			{
				DisplayStatus("Enter valid type");
				continue;
			}
			if (type == OutputImageType::PERIMETER)
			{
				if (!service.IsRegionCalculated())
				{
					DisplayStatus("Please calculate region first");
					continue;
				}
				if (!service.IsPerimeterCalculated())
				{
					DisplayStatus("Please calculate perimeter first");
					continue;
				}
			}
			else if (type == OutputImageType::REGION)
			{
				if (!service.IsRegionCalculated())
				{
					DisplayStatus("Please calculate region first");
					continue;
				}
			}

			returnval = service.DISPLAY_PIXELS(type);
			if (returnval == Status::FAILURE)
			{
				DisplayStatus("Something went wrong. Please see the error message above.");
				continue;
			}
		}
		else if (args[0] == "SAVE_PIXELS")
		{
			if (count < 3)
			{
				DisplayStatus("Please enter valid command");
				continue;
			}
			if (!service.IsIntitialized())
			{
				DisplayStatus("Please load input image first");
				continue;
			}

			args[1].erase(remove_if(args[1].begin(), args[1].end(), isspace), args[1].end());

			OutputImageType type;
			if (args[1] == "perimeter")
			{
				type = OutputImageType::PERIMETER;
			}
			else if (args[1] == "region")
			{
				type = OutputImageType::REGION;
			}
			else
			{
				DisplayStatus("Enter valid type");
				continue;
			}
			if (type == OutputImageType::PERIMETER)
			{
				if (!service.IsRegionCalculated())
				{
					DisplayStatus("Please calculate region first");
					continue;
				}
				if (!service.IsPerimeterCalculated())
				{
					DisplayStatus("Please calculate perimeter first");
					continue;
				}
			}
			else if (type == OutputImageType::REGION)
			{
				if (!service.IsRegionCalculated())
				{
					DisplayStatus("Please calculate region first");
					continue;
				}
			}

			returnval = service.SAVE_PIXELS(type, args[2]);
			if (returnval == Status::FAILURE)
			{
				DisplayStatus("Something went wrong. Please see the error message above.");
				continue;
			}
			else
			{
				DisplayStatus("Output save completed");
			}
		}
		else if (args[0] == "HELP")
		{
			DisplayCommands();
			continue;
		}
		else if (args[0] == "SAVE_PROGRAM_OUTPUT")
		{
			if (count < 2)
			{
				DisplayStatus("Please enter valid command");
				continue;
			}

			SaveProgramOutput(args[1]);
			continue;
		}
		else if (args[0] == "EXIT")
		{
			break;
		}
		else
		{
			DisplayStatus("Please Enter Valid Command");
			DisplayCommands();
			continue;
		}

	}

	return 0;
}
