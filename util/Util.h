#ifndef JIT_UTIL_H
#define JIT_UTIL_H

#include "llvm/ADT/FunctionExtras.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/EPCIndirectionUtils.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/Transforms/Scalar.h"
#include <ctime>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <chrono>


#define PRINT_ERROR(condition, message) PRINT_ERROR_FULL(condition, {}, message)

#define PRINT_ERROR_FULL(condition, message_creation, message) \
{                                                              \
    if (condition) {                                           \
        {                                                      \
            message_creation                                   \
            std::cerr << message << std::endl;                 \
        }                                                      \
        exit(-1);                                              \
    }                                                          \
}

extern std::string O1;
extern std::string O2;

/**
 * A struct wrapping the arguments to the application.
 */
struct Arguments {
    /**
     * The files to load.
     */
    std::vector<std::string> Files;
    /**
     * The arguments for the front-end.
     */
    std::string FrontEndArguments;
    /**
     * The arguments for the back-end.
     */
    std::string BackEndArguments;
    /**
     * The arguments for the application.
     */
    std::string ApplicationArguments;
};

#define LOG_TYPES     \
X(List, "LIST")       /* If you want to list all values of a specific tag. */\
X(Average, "AVERAGE") /* If you want to take the average of all the values of the tag. */

#define X(type, name) type,
enum LogType : size_t
{
    LOG_TYPES
};
#undef X

#define LOG_PART         \
X(FrontEnd, "FRONT-END") /* If you log as part of the back-end. */\
X(BackEnd, "BACK-END")   /* If you log as part of the front-end. */\
X(Whole, "WHOLE")   /* If you log something that is applicable to both parts. */

#define X(type, name) type,
enum LogPart : size_t
{
    LOG_PART
};
#undef X

/**
 * Print the current time as part of a log which can be used for getting the time the entry point of an application
 * was called.
 */
void print_main_entry_time();

/**
 * Check if a string is a number, from [here](https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c)
 * @param s The string to check.
 * @return True if the string is a number, and false otherwise.
 */
bool is_number(const std::string& s);

/**
 * Print data to the console.
 * @param tag A tag for the data to group the data by.
 * @param type The type of the data.
 * @param part The part of the jit for which the log is.
 * @param data The data to print.
 */
void print_log_data(const std::string& tag, LogType type, LogPart part, const std::string& data);

/**
 * Split the string based on a delimiter.
 * @param string The string to split.
 * @param delimiter The delimiter to split by.
 * @return The components of the original string.
 */
std::vector<std::string> split(const std::string& string, char delimiter);

/**
 * Split the string based on a delimiter only once.
 * @param string The string to split.
 * @param delimiter The delimiter to split by.
 * @return The components of the original string.
 */
std::vector<std::string> split_once(const std::string& string, char delimiter);

/**
 * Read the arguments from argv.
 * @param argc The number of arguments.
 * @param argv The original arguments.
 * @return The parsed arguments.
 */
struct Arguments getArguments(int argc, char **argv);

/**
 * Load an llvm module from a file, from [here](https://stackoverflow.com/questions/22239801/how-to-load-llvm-bitcode-file-from-an-ifstream).
 * @param file The file from which to load the llvm module.
 * @param context The context for the module.
 * @return The llvm module for the file.
 */
std::unique_ptr<llvm::Module> load_module(llvm::StringRef file, llvm::LLVMContext &context);

/**
 * If a string ends in some substring, from [here](https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c).
 * @param fullString The string to check.
 * @param ending The substring.
 * @return If the string ends in some substring.
 */
bool hasEnding(std::string const &fullString, std::string const &ending);

/**
 * Create an ir compiler.
 * @param JTMB The target machine builder.
 * @param ObjCache The cache for objects.
 * @return The ir compiler.
 */
llvm::Expected<std::unique_ptr<llvm::orc::IRCompileLayer::IRCompiler>> createCompiler(llvm::orc::JITTargetMachineBuilder JTMB, llvm::ObjectCache *ObjCache = nullptr);

/**
 * Create an linking layer.
 * @param ES The execution session.
 * @param MemMgr The memory manager.
 * @return The linking layer.
 */
llvm::Expected<std::unique_ptr<llvm::orc::ObjectLinkingLayer>> createLinkingLayer(llvm::orc::ExecutionSession &ES, std::unique_ptr<llvm::jitlink::JITLinkMemoryManager> &MemMgr);

#endif //JIT_UTIL_H
