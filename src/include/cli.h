#include <cassert>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include "db.h"
#include "Stopwatch.hpp"


// CLI State class to hold shared state
class CLIState {
private:
    DataBase& db;
    stopwatch::Stopwatch& watch;
    bool running;

public:
    CLIState(DataBase& database, stopwatch::Stopwatch& stopwatch) 
        : db(database), watch(stopwatch), running(true) {}

    bool isRunning() const { return running; }
    void stop() { running = false; }
};

// Base Command class
class Command {
public:
    virtual std::string get_name() const = 0;
    virtual std::string get_description() const = 0;
    virtual std::string get_usage() const = 0;
    virtual void execute(CLIState& state, const std::vector<std::string>& args) = 0;
};

// Help Command
class HelpCommand : public Command {
private:
    std::map<std::string, std::shared_ptr<Command>>& commands;

public:
    HelpCommand(std::map<std::string, std::shared_ptr<Command>>& cmds) : commands(cmds) {}

    std::string get_name() const override { return "help"; }
    std::string get_description() const override { return "Display available commands"; }
    std::string get_usage() const override { return "help [command]"; }

    void execute(CLIState& state, const std::vector<std::string>& args) override; 
};

// Exit Command
class ExitCommand : public Command {
public:
    std::string get_name() const override { return "exit"; }
    std::string get_description() const override { return "Exit the program"; }
    std::string get_usage() const override { return "exit"; }

    void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// Create Table Command
class CreateTableCommand : public Command {
public:
    std::string get_name() const override { return "create_table"; }
    std::string get_description() const override { return "Create a new table"; }
    std::string get_usage() const override { return "create_table <name>"; }

    void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// ListTablesCommand
class ListTablesCommand : public Command {
    public:
        std::string get_name() const override { return "list_tables"; }
        std::string get_description() const override { return "List all tables in the database"; }
        std::string get_usage() const override { return "list_tables"; }
    
        void execute(CLIState& state, const std::vector<std::string>& args) override;
    };
    

// Insert Command
class InsertCommand : public Command {
public:
    std::string get_name() const override { return "insert"; }
    std::string get_description() const override { return "Insert a single data point"; }
    std::string get_usage() const override { return "insert <table> <timestamp> <value>"; }

    void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// Insert Batch Command
class InsertBatchCommand : public Command {
public:
    std::string get_name() const override { return "insert_batch"; }
    std::string get_description() const override { return "Insert multiple data points with fixed interval"; }
    std::string get_usage() const override { return "insert_batch <table> <count> <interval_sec> [start_ts]"; }

    void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// Query Command
class QueryCommand : public Command {
public:
    std::string get_name() const override { return "query"; }
    std::string get_description() const override { return "Query data points within a time range"; }
    std::string get_usage() const override { return "query <table> <start_ts> <end_ts>"; }

    void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// CLI 
class CLI {
private:
    DataBase& db;
    stopwatch::Stopwatch watch;
    std::map<std::string, std::shared_ptr<Command>> commands;
    CLIState state;

    // tokenise a command string
    const std::vector<std::string> tokenise(const std::string& command) {
        std::vector<std::string> tokens;
        std::stringstream ss(command);
        std::string token;
        
        while (ss >> token) {
            tokens.push_back(token);
        }
        
        return tokens;
    }

public:
    CLI(DataBase& database) : db(database), state(db, watch) {
        // Register commands
        register_cmd(std::make_shared<ExitCommand>());
        
        // Register help command after others are registered
        auto helpCmd = std::make_shared<HelpCommand>(commands);
        register_cmd(helpCmd);
        
        // Register database commands
        register_cmd(std::make_shared<CreateTableCommand>());
        register_cmd(std::make_shared<InsertCommand>());
        register_cmd(std::make_shared<InsertBatchCommand>());
        register_cmd(std::make_shared<QueryCommand>());
        
        // Register alias for exit
        commands["quit"] = commands["exit"];
    }
    
    void register_cmd(std::shared_ptr<Command> command) {
        commands[command->get_name()] = command;
    }
    
    void run() {
        std::cout << "Time Series Database CLI\n";
        std::cout << "Type 'help' for available commands\n";
        
        std::string input;
        
        while (state.isRunning()) {
            std::cout << "\ntsdb> ";
            std::getline(std::cin, input);
            
            // Skip empty commands
            if (input.empty()) {
                continue;
            }
            
            auto tokens = tokenise(input);
            if (tokens.empty()) {
                continue;
            }
            
            std::string cmd_name = tokens[0];
            
            try {
                auto it = commands.find(cmd_name);
                if (it != commands.end()) {
                    it->second->execute(state, tokens);
                } else {
                    std::cout << "Unknown command: " << cmd_name << "\n";
                    std::cout << "Type 'help' for available commands\n";
                }
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        }
    }
};

