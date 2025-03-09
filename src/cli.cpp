#import "cli.h"
#include <exception>
#include <iostream>
#include <string>

void HelpCommand::execute(CLIState& context, const std::vector<std::string>& args)
{
	if (args.size() > 1)
	{
		// Help for specific command
		auto cmd_name = args[1];
		auto it = commands.find(cmd_name);
		if (it != commands.end())
		{
			std::cout << "Command: " << it->second->get_name() << "\n";
			std::cout << "Description: " << it->second->get_description() << "\n";
			std::cout << "Usage: " << it->second->get_usage() << "\n";
		}
		else
		{
			std::cout << "Unknown command: " << cmd_name << "\n";
		}
	}
	else
	{
		// General help
		std::cout << "Available commands:\n";
		for (const auto& cmd_pair : commands)
		{
			std::cout << "  " << std::left << std::setw(15) << cmd_pair.first << " - "
					  << cmd_pair.second->get_description() << "\n";
		}
		std::cout << "\nType 'help <command>' for more information on a specific command.\n";
	}
}

void ExitCommand::execute(CLIState& context, const std::vector<std::string>& args)
{
	std::cout << "Exiting...\n";
	context.stop();
}

void CreateTableCommand::execute(CLIState& state, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		std::cout << "Error: Table name required\n";
		std::cout << "Usage: " << get_usage() << "\n";
		return;
	}

	std::string table_name = args[1];

	// TODO: Add option to pass in config through command line
	Table::Config config(
		Config::CHUNK_INTERVAL_SECS,
		Config::CHUNK_CACHE_SIZE,
		Config::MAX_CHUNKS_TO_SAVE,
		Config::FLUSH_INTERVAL_SECS,
		Config::MIN_DATA_RESOLUTION_SECS
	);

	state.get_database().create_table(table_name, config);
	std::cout << "Table '" << table_name << "' created successfully\n";
}

void ListTablesCommand::execute(CLIState& state, const std::vector<std::string>& args)
{
	auto table_names = state.get_database().get_table_names();
	if (table_names.empty())
	{
		std::cout << "No tables found in the database\n";
	}
	else
	{
		std::cout << "Tables:\n";
		for (const auto& name : table_names)
		{
			// Get the table (you'll need a get_table method)
			auto* table = state.get_database().get_table(name);
			if (table)
			{
				std::cout << "Name: " << name << " (" << table->rows() << " rows)\n";
			}
			else
			{
				std::cout << "Name: " << name << "(Invalid Table)\n";
			}
		}
	}
}

// Utility functions
time_t parse_timestamp(const std::string& ts_str)
{
	try
	{
		return std::stoll(ts_str);
	}
	catch (const std::exception& e)
	{
		throw std::runtime_error(
			"Invalid timestamp format. Use Unix timestamp (seconds since epoch)"
		);
	}
}

void InsertCommand::execute(CLIState& state, const std::vector<std::string>& args)
{
	if (args.size() < 4)
	{
		std::cout << "Error: Required format: " << get_usage() << "\n";
		return;
	}

	std::string table_name = args[1];
	time_t timestamp = parse_timestamp(args[2]);
	double value = std::stod(args[3]);
	std::vector<DataPoint> data = { { timestamp, value } };

	auto& watch = state.get_stopwatch();
	watch.start();
	state.get_database().insert(table_name, data);
	auto insert_time = watch.elapsed<stopwatch::mus>();

	std::cout << "Inserted 1 data point in " << static_cast<double>(insert_time) / 1000 << " ms\n";
}

void InsertFromCSVCommand::execute(CLIState& state, const std::vector<std::string>& args)
{
	if (args.size() < 3)
	{
		std::cout << "Error: Required format: " << get_usage() << "\n";
		return;
	}

	std::string table_name = args[1];
	std::string filename = args[2];

	try
	{
		auto& watch = state.get_stopwatch();
		watch.start();
		auto data = state.get_database().load_data_from_csv(filename);
		state.get_database().insert(table_name, data);
		auto insert_time = watch.elapsed<stopwatch::mus>();
		std::cout << "Inserted " << data.size() << " data points in "
				  << static_cast<double>(insert_time) / 1000 << " ms\n";
	}
	catch (const std::exception& e)
	{
        std::stringstream error_message;
        error_message << "Failed to insert data from CSV file '" << filename << "': " << e.what();
		throw std::runtime_error(error_message.str());
	}
}

void QueryCommand::execute(CLIState& state, const std::vector<std::string>& args)
{
	if (args.size() < 4)
	{
		std::cout << "Error: Required format: " << get_usage() << "\n";
		return;
	}

	std::string table_name = args[1];
	time_t start_ts = parse_timestamp(args[2]);
	time_t end_ts = parse_timestamp(args[3]);

    if (start_ts > end_ts)
    {
        std::cout << "Error: Invalid time range: [" << start_ts << "," << end_ts << "]"<< "\n";
		return; 
    }

    try {
        Query q = { TimeRange{ start_ts, end_ts } };

        auto& watch = state.get_stopwatch();
        watch.start();
        auto results = state.get_database().query(table_name, q);
        auto query_time = watch.elapsed<stopwatch::mus>();
    
        std::cout << "Query executed in " << static_cast<double>(query_time) / 1000 << " ms\n";
        // std::cout << "Retrieved " << results.size() << " rows...\n\n";
    
        // // Display results (limited to first 10 for readability)
        // std::cout << "Timestamp | Value\n";
        // std::cout << "---------------------\n";
    
        // size_t display_count = std::min(results.size(), size_t(10));
        // for (size_t i = 0; i < display_count; ++i)
        // {
        //     std::cout << results[i].ts << " | " << results[i].value << "\n";
        // }
    
        // if (results.size() > 10)
        // {
        //     std::cout << "... and " << (results.size() - 10) << " more rows.\n";
        // }
    } catch (const std::exception& e) {
        std::stringstream error_message;
        error_message << "Failed to query data: " << e.what();
		throw std::runtime_error(error_message.str()); 
    }

	
}

CLI::CLI(DataBase& database)
	: db(database)
	, state(db, watch)
{
	// Register help command after others are registered
	auto helpCmd = std::make_shared<HelpCommand>(commands);
	register_cmd(helpCmd);
	// Register commands
	register_cmd(std::make_shared<ExitCommand>());

	// Register database commands
	register_cmd(std::make_shared<CreateTableCommand>());
	register_cmd(std::make_shared<ListTablesCommand>());
	register_cmd(std::make_shared<InsertCommand>());
	register_cmd(std::make_shared<InsertFromCSVCommand>());
	register_cmd(std::make_shared<QueryCommand>());
}

void CLI::run()
{
	std::cout << "Time Series Database CLI\n";
	std::cout << "Type 'help' for available commands\n";

	std::string input;

	while (state.isRunning())
	{
		std::cout << "\ntsdb> ";
		std::getline(std::cin, input);

		// Skip empty commands
		if (input.empty())
		{
			continue;
		}

		auto tokens = tokenise(input);
		if (tokens.empty())
		{
			continue;
		}

		std::string cmd_name = tokens[0];

		try
		{
			auto it = commands.find(cmd_name);
			if (it != commands.end())
			{
				it->second->execute(state, tokens);
			}
			else
			{
				std::cout << "Unknown command: " << cmd_name << "\n";
				std::cout << "Type 'help' for available commands\n";
			}
		}
		catch (const std::exception& e)
		{
			std::cout << "Error: " << e.what() << "\n";
		}
	}
}
