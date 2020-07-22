#pragma once
#include <filesystem>
#include <iostream>
#include <chrono>
#include <stack>
#include <optional>
#include <string>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <cassert>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <cassert>
#include <unordered_set>
using namespace std::chrono;

namespace Directory
{
	namespace fs = std::filesystem;
	using Path = fs::path;
	using namespace std;

	enum class Instruction
	{
		Handshake = 1,
		Placefleet = 2,
		Attack = 3
	};
	enum class Status
	{
		Rejected = false,
		Accepted = true
	};

	struct Configuration
	{
		Path inputPath;
		Path outputFile;
	};

	Configuration configuration;

	class Encoder
	{
		static inline const unordered_map<string, Instruction>
			StrInstruction = {
					{"HANDSHAKE",	Instruction::Handshake},
					{"PLACEFLEET",	Instruction::Placefleet},
					{"ATTACK",		Instruction::Attack}
		};

		static inline const unordered_map<string, Status>
			StrStatus = {
					{"ACCEPTED",	Status::Accepted},
					{"REJECTED",	Status::Rejected}
		};

		static inline const unordered_map<Instruction, string>
			InsToString = {
					{Instruction::Handshake, "HANDSHAKE"},
					{Instruction::Placefleet, "PLACEFLEET"},
					{Instruction::Attack, "ATTACK"}
		};

		static inline const unordered_map<Status, string>
			StatToString = {
					{Status::Accepted, "ACCEPTED"},
					{Status::Rejected, "REJECTED"}
		};

		Encoder() = default;

	public:
		static Instruction makeInstruction(string str)
		{
			return StrInstruction.at(str);
		}
		static Status makeStatus(string str)
		{
			return StrStatus.at(str);
		}
		static string makeString(Instruction instruction)
		{
			return InsToString.at(instruction);
		}
		static string makeString(Status status)
		{
			return StatToString.at(status);
		}
	};

	namespace Input
	{
		struct Message
		{
			using Data = string;
			using OptionalData = optional<Data>;

			Instruction		instruction;
			Status			status;
			Data			data;
			OptionalData	optionalData;

			Message(
				Instruction _instruction,
				Status _status,
				Data _data,
				Data _optionalData
			)
				: instruction(_instruction),
				status(_status),
				data(_data),
				optionalData(std::make_optional(_optionalData))
			{}

			Message(
				Instruction _instruction,
				Status _status,
				Data _data
			)
				: instruction(_instruction),
				status(_status),
				data(_data),
				optionalData(std::nullopt)
			{}

		};

		struct Handler
		{
			static queue<Message> msgQueue;

			static Message
				createMessage(Path filepath)
			{
				std::ifstream file(filepath);
				assert(file.is_open());

				unsigned i = 0;
				vector<optional<string>> dataList(4);
				std::string line, _;

				for (; std::getline(file, line, '\n'); i++)
				{
					dataList[i] = std::make_optional(line);
					std::getline(file, _, '=');
				}

				file.close();
				fs::remove(filepath);

				auto instruction = Encoder::makeInstruction(dataList[0].value());
				auto status = Encoder::makeStatus(dataList[1].value());
				auto data = dataList[2].value();

				if (!dataList[3].has_value())
					return Input::Message(instruction, status, data);

				auto optData = dataList[3].value();
				return Input::Message(instruction, status, data, optData);
			}

			static void
				fetchData()
			{
				using FilesOn = fs::directory_iterator;
				using File = fs::directory_entry;

				vector<File> processes;
				auto temp_folder = fs::temp_directory_path();

				for (const auto& entry : FilesOn(configuration.inputPath))
					processes.push_back(entry);

				if (processes.empty())
					return;

				for (auto& entry : processes)
				{
					fs::copy(entry.path(), temp_folder, fs::copy_options::overwrite_existing);
					fs::remove(entry.path());
				}

				for (auto& entry : processes)
				{
					auto nPath = temp_folder / entry.path().filename();
					auto message = createMessage(nPath);
					msgQueue.push(message);
				}
			}

			static optional<Message>
				request()
			{
				if (msgQueue.empty())
					return std::nullopt;

				auto ret = msgQueue.front();
				msgQueue.pop();
				return ret;
			}
		};
	};

	namespace Output
	{
		struct Message
		{
			using Data = string;
			using OptionalData = optional<Data>;

			Instruction instruction;
			Data instructionData;
			OptionalData token;

			Message(
				Instruction _instruction,
				Data _data
			)
				: instruction(_instruction),
				instructionData(_data),
				token(std::nullopt)
			{}

			Message(
				Data _token,
				Instruction _instruction,
				Data _data
			)
				: instruction(_instruction),
				instructionData(_data),
				token(_token)
			{}

		};

		struct Sender
		{
			static queue<Message> queue;

			static void writeFile(Message message)
			{
				std::ofstream file(configuration.outputFile);
				assert(file.is_open());

				if (message.token.has_value())
					file << "TOKEN=" << message.token.value() << std::endl;

				file << Encoder::makeString(message.instruction) << '='
					<< message.instructionData;

				file.close();
			}

			static void send(Message message)
			{
				writeFile(message);
			}
			static void enqueue(Message message)
			{
				queue.emplace(message);
			}

		};
	};
}