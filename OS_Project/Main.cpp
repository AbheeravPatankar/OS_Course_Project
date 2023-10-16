#include<iostream>
#include<string>
#include<cstring>
#include<fstream>
#include"exeUnit.h"
#include"memory.h"
#include"memController.h"

int main()
{
	Memory memory;
	PageDirectory pageDirectory;
	MemoryController memController;

	int casevar{};
	int pageTableIndex{ -1 };
	int jobId;
	char a;
	std::string filepath;
	std::string inputBuffer;
	std::string outputBuffer;
	std::ifstream file;
	int timeLimit;
	int lineLimit;
	int* ptrToPageTable{ NULL };
	char c{};
	int fileCount{};
	do
	{
		std::cout << "Enter the code for the operation you have to perform :";
		std::cin >> casevar;
		switch (casevar)
		{
		case 1:
			// Insert a Job1

			pageTableIndex = memController.getEmptyPageTable();
			if (pageTableIndex == -1)
			{
				std::cout << "The Memory is full " << std::endl;
			}
			std::cout << "Enter the file name :";
			while (filepath == "")
			{
				filepath.clear();
				std::getline(std::cin, filepath);
			}
			file.open(filepath);
			if (file.fail() == true)
			{
				std::cout << "Error opening the file " << std::endl;
				break;
			}
			while (file.get(c))
			{

				inputBuffer += c;
				fileCount++;
			}
			inputBuffer += '\0';
			file.close();
			ptrToPageTable = pageDirectory.getPtrToPageDirectory(pageTableIndex);  // returns pointerToPageTable
			jobId = memController.getJobId(inputBuffer);
			memController.setJobId(jobId, pageTableIndex);
			timeLimit = memController.getTimeLimit(inputBuffer);
			lineLimit = memController.getLineLimit(inputBuffer);
			memController.generateFrames(pageDirectory, pageTableIndex);
			memController.pushInstructions(jobId, pageDirectory, memory, inputBuffer);
			std::cout << std::endl;
			memController.printFrame(ptrToPageTable[0], memory);
			std::cout << std::endl;
			memController.printFrame(ptrToPageTable[1], memory);
			if (memController.outOfData(inputBuffer))
			{
				std::cout << "Out of data error";
				break;
			}
			else
			{
				memController.pushData(jobId, pageDirectory, memory, inputBuffer);
				std::cout << std::endl;
			}
			memController.printFrame(ptrToPageTable[2], memory);
			std::cout << std::endl;
			memController.printFrame(ptrToPageTable[3], memory);
			std::cout << std::endl;

			filepath.clear();
			inputBuffer.clear();
			break;
		case 2:
			std::cout << "Enter the job id :";
			std::cin >> jobId;
			std::cout << "--------------------OUTPUT---------------------------" << std::endl;
			pageTableIndex = memController.getPageTableIndex(jobId);
			ptrToPageTable = pageDirectory.getPtrToPageDirectory(pageTableIndex);
			ExecutionUnit exec(ptrToPageTable[0], ptrToPageTable[1], ptrToPageTable[2], ptrToPageTable[3], jobId, memory);
			char* IR = exec.getIR();
			int PC = exec.getPC();
			int time = 0;
			int line = 0;

			while (1)
			{
				exec.loadIR(exec);
				PC++;

				if (IR[0] == 'P' && IR[1] == 'D')
				{

					time++;
					line++;
					if (exec.putData(outputBuffer)) break;
					std::cout << outputBuffer;
					outputBuffer.clear();
					std::cout << std::endl;
				}
				else if (IR[0] == 'L' && IR[1] == 'R')
				{
					time++;
					if (exec.loadInReg()) break;
				}
				else if (IR[0] == 'S' && IR[1] == 'R')
				{
					time += 2;
					if (exec.storeFromReg()) break;
				}
				else if (IR[0] == 'C' && IR[1] == 'R')
				{
					time++;
					if (exec.compareWithReg() == 2) break;
				}
				else if (IR[0] == 'B' && IR[1] == 'T')
				{
					time++;
					exec.branch(exec);
				}
				else if (IR[0] == 'H')
				{
					time++;
					break;
				}
				// errors
				else {
					std::cout << "Opcode error";
					break;
				}

				if (time > timeLimit) {
					std::cout << "Time limit exceeded";
					break;
				}
				if (line > lineLimit)
				{
					std::cout << "Line limit exceeded";
					break;
				}

			}
			break;
		}
		std::cout << "--------------------------------------------------------" << std::endl;
	} while (casevar != -1);
	
}