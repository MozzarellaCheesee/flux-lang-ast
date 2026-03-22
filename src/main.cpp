#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>


#include "flux/lexer/lexer.h"
#include "flux/common/diagnostic.h"
#include "flux/preprocessor/preprocessor.h"
#include "flux/parser/parser.h"
#include "flux/sema/sema.h"
#include "flux/ast/ast_printer.h"

std::string read_file(const std::filesystem::path& filepath);

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source.flx>\n";
        return 1;
    }

    std::filesystem::path filepath = argv[1];
    std::string source = read_file(filepath);
    if (source.empty()) return 1;

    flux::DiagEngine diag;

    // 1. Лексер
    flux::Lexer lexer(source, filepath.string(), diag);
    auto tokens = lexer.tokenize();

    if (diag.has_errors()) {
        diag.print_all();
        return 1;
    }

    // 2. Препроцессор
    flux::Preprocessor preprocessor(diag, filepath.parent_path());
    auto processed_tokens = preprocessor.process(tokens, filepath);

    if (diag.has_errors()) {
        diag.print_all();
        return 2;
    }

    // 3. Парсер
    flux::Parser parser(std::move(processed_tokens), diag);
    auto program = parser.parse_program();

    if (diag.has_errors()) {
        diag.print_all();
        return 3;
    }

    if (!program) {
        std::cerr << "Parse failed.\n";
        return 4;
    }

    flux::SemanticAnalyzer sema(diag);
    sema.analyze(*program);

    if (diag.has_errors()) {
        diag.print_all();
        return 1;
    }

    flux::ASTPrinter printer(std::cout);
    program->accept(printer);
    return 0;
}


std::string read_file(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filepath << '\n';
        return "";
    }
    return std::string{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
}
