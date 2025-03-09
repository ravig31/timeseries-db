#include <cassert>
#include <cstddef>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "Stopwatch.hpp"
#include "db.h"

// CLI State class to hold shared state
class CLIState
{
  private:
	DataBase& db;
	stopwatch::Stopwatch& watch;
	bool running;

  public:
	CLIState(DataBase& database, stopwatch::Stopwatch& stopwatch)
		: db(database)
		, watch(stopwatch)
		, running(true)
	{
	}

	DataBase& get_database() { return db; }
	stopwatch::Stopwatch& get_stopwatch() const { return watch; }
	bool isRunning() const { return running; }
	void stop() { running = false; }
};

// Base Command class
class Command
{
  public:
	virtual ~Command() = default;
	virtual std::string get_name() const = 0;
	virtual std::string get_description() const = 0;
	virtual std::string get_usage() const = 0;
	virtual void execute(CLIState& state, const std::vector<std::string>& args) = 0;
};

// Help Command
class HelpCommand : public Command
{
  private:
	std::map<std::string, std::shared_ptr<Command>>& commands;

  public:
	HelpCommand(std::map<std::string, std::shared_ptr<Command>>& cmds)
		: commands(cmds)
	{
	}

	std::string get_name() const override { return "help"; }
	std::string get_description() const override { return "Display available commands"; }
	std::string get_usage() const override { return "help [command]"; }

	void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// Exit Command
class ExitCommand : public Command
{
  public:
	std::string get_name() const override { return "exit"; }
	std::string get_description() const override { return "Exit the program"; }
	std::string get_usage() const override { return "exit"; }

	void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// Create Table Command
class CreateTableCommand : public Command
{
  public:
	std::string get_name() const override { return "create_table"; }
	std::string get_description() const override { return "Create a new table"; }
	std::string get_usage() const override { return "create_table <name>"; }

	void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// ListTablesCommand
class ListTablesCommand : public Command
{
  public:
	std::string get_name() const override { return "list_tables"; }
	std::string get_description() const override { return "List all tables in the database"; }
	std::string get_usage() const override { return "list_tables"; }

	void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// Insert Command
class InsertCommand : public Command
{
  public:
	std::string get_name() const override { return "insert"; }
	std::string get_description() const override { return "Insert a single data point"; }
	std::string get_usage() const override { return "insert <table> <timestamp> <value>"; }

	void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// Insert Batch Command
class InsertFromCSVCommand : public Command
{
  public:
	std::string get_name() const override { return "insert_csv"; }
	std::string get_description() const override { return "Ingest data from csv"; }
	std::string get_usage() const override { return "insert_csv <table> <filename>.csv"; }

	void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// Query Command
class QueryCommand : public Command
{
  public:
	std::string get_name() const override { return "query"; }
	std::string get_description() const override { return "Query data points within a time range"; }
	std::string get_usage() const override { return "query <table> <start_ts> <end_ts>"; }

	void execute(CLIState& state, const std::vector<std::string>& args) override;
};

// CLI
class CLI
{
  private:
	DataBase& db;
	stopwatch::Stopwatch watch;
	std::map<std::string, std::shared_ptr<Command>> commands {};
	CLIState state;

	// tokenise a command string
	const std::vector<std::string> tokenise(const std::string& command)
	{
		std::vector<std::string> tokens;
		std::stringstream ss(command);
		std::string token;

		while (ss >> token)
		{
			tokens.push_back(token);
		}

		return tokens;
	}

  public:
	CLI(DataBase& database);

	void register_cmd(std::shared_ptr<Command> command)
	{
		commands.emplace(command->get_name(), command);
	}
	void run();
};
