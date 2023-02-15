#include "Chunk.hpp"
#include "Logger.hpp"
#include "defines.hpp"
#include <iostream>
#include <sstream>

ChunkBuilder::ChunkBuilder() : _state(ChunkBuilder::waiting_for_size) {}

std::vector<Chunk> ChunkBuilder::parse(std::string data) {
	std::string &save = this->_save;
	ChunkBuilder::ParseState &state = this->_state;
	size_t &size = this->_size;

	std::vector<Chunk> chunks;

	data = save + data;
	while (true) {
		switch (state) {
			case waiting_for_size: {
				size_t found = data.find(CRLF);
				if (found == std::string::npos) {
					save = data;
					return (chunks);
				}
				std::string size_hex = data.substr(0, found);
				std::stringstream ss;
				ss << std::hex << size_hex;
				logger << Logger::debug << "size_hex: <" << size_hex << ">" << std::endl;
				logger << Logger::debug << "ss: <" << ss.str() << ">" << std::endl;
				ss >> size;
				state = waiting_for_data;
				data = data.substr(found + 2);
			} break;

			case waiting_for_data: {
				if (data.size() < size) {
					save = data;
					return (chunks);
				}
				std::string chunk_content = data.substr(0, size);
				chunks.push_back(Chunk(chunk_content, size));
				state = waiting_for_crlf;
				data = data.substr(size);
			} break;

			case waiting_for_crlf: {
				if (data.substr(0, 2) != CRLF) {
					save = data;
					return (chunks);
				}
				state = waiting_for_size;
				data = data.substr(2);
			} break;

			default:
				throw std::runtime_error("Tried to enter unknown state");
				break;
		}
	}
}

Chunks::Chunks(std::string data) {
	size_t found;
	std::stringstream ss;
	std::string size_hex;
	size_t size;
	size_t next_chunk_start;
	next_chunk_start = 0;

	while (true) {
		ss.str("");
		ss.clear();
		found = data.find(CRLF, next_chunk_start);
		if (found == std::string::npos)
			break;
		size_hex = data.substr(next_chunk_start, found - next_chunk_start);
		ss << std::hex << size_hex;
		logger << Logger::info << "size_hex: <" << size_hex << ">" << std::endl;
		logger << Logger::info << "ss: <" << ss.str() << ">" << std::endl;
		ss >> size;

		std::string chunk_content = data.substr(found + 2, size);
		Chunk chunk(chunk_content, size);
		this->_chunks.push_back(chunk);

		next_chunk_start = data.find(CRLF, found + 2 + size);
		if (next_chunk_start == std::string::npos)
			break;
		next_chunk_start += 2;
	}
	logger << Logger::info << "Chunks found: " << this->_chunks.size() << std::endl;
}

const std::vector<Chunk> & Chunks::get_chunks(void) const {
	return (this->_chunks);
}

const std::string & Chunk::get_content(void) const {
	return (this->_content);
}

size_t Chunk::get_size(void) const {
	return (this->_size);
}

Chunks::iterator Chunks::begin() {
	return (this->_chunks.begin());
}

Chunks::iterator Chunks::end() {
	return (this->_chunks.end());
}

Chunks::const_iterator Chunks::begin() const {
	return (this->_chunks.begin());
}

Chunks::const_iterator Chunks::end() const {
	return (this->_chunks.end());
}

Chunks::reverse_iterator Chunks::rbegin() {
	return (this->_chunks.rbegin());
}

Chunks::reverse_iterator Chunks::rend() {
	return (this->_chunks.rend());
}

Chunks::const_reverse_iterator Chunks::rbegin() const {
	return (this->_chunks.rbegin());
}

Chunks::const_reverse_iterator Chunks::rend() const {
	return (this->_chunks.rend());
}
