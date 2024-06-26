#include "LLVMFrontEnd.h"
#include "../util/Util.h"

LLVMFrontEnd::LLVMFrontEnd(std::unique_ptr<llvm::orc::BaseJIT> JIT)
    : JIT(std::move(JIT)) {}

llvm::Expected<std::unique_ptr<BaseFrontEnd>> LLVMFrontEnd::create(std::vector<std::string> Arguments, std::vector<std::string> Files, std::unique_ptr<llvm::orc::BaseJIT> JIT) {
    auto result = JIT->entryPoint("main");
    if (result)
        return result;

    for (const auto& file : Files) {
        print_log_data("Input", LogType::List, LogPart::FrontEnd, "input file " + file);
        auto context = std::make_unique<llvm::LLVMContext>();
        auto module = load_module(file, *context);
        auto addError = JIT->addModule(llvm::orc::ThreadSafeModule(
                std::move(module),
                std::move(context)
        ));
        if (addError)
            return addError;
    }
    return std::make_unique<LLVMFrontEnd>(std::move(JIT));
}

llvm::Expected<struct llvm::orc::CaptureModule> LLVMFrontEnd::requestModule(llvm::StringRef Name) {
    return llvm::createStringError(std::error_code(), "Everything should be loaded.");
}

llvm::Expected<int> LLVMFrontEnd::start(int argc, char **argv) {
    auto mainSymbol = this->JIT->lookup("main");
    if (!mainSymbol)
        return mainSymbol.takeError();

    auto main = mainSymbol->toPtr<int(*)(int,char**)>();
    return main(argc, argv);
}