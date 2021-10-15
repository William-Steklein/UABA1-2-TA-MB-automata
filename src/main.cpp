//#include <iostream>
//#include "DFA.h"
//#include "NFA.h"
//#include "ENFA.h"
//#include "RE.h"

/*
 * todo cleaning
 * 		- remove hardcoded stuff from inginious output??
 *
 * todo changes/other
 * 		- deleteFiles() : make directory if it doesn't exists
 * 		- fix deletefiles function so that it doesn't crash when the directory doesn't exists
 *
 */

#include <iostream>
#include "CFG.h"

void deleteFiles(const std::string& folderpath);

struct AllocationMetrics
{
	uint32_t TotalAllocated = 0;
	uint32_t TotalFreed = 0;

	uint32_t CurrentUsage() { return TotalAllocated - TotalFreed; }
};

static AllocationMetrics s_AllocationMetrics;

void* operator new(size_t size)
{
	s_AllocationMetrics.TotalAllocated += size;

	return malloc(size);
}

void operator delete(void* memory, size_t size)
{
	s_AllocationMetrics.TotalFreed += size;

	free(memory);
}

static void PrintMemoryUsage()
{
	std::cout << "Memory Usage: " << s_AllocationMetrics.CurrentUsage() << " bytes\n";
}

int main() {
	deleteFiles("../DOT_files");
	deleteFiles("../DOT_images");

    {
        CFG cfg;
        cfg.print();
    }

    return 0;
}