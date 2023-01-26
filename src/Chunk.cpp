#include "Chunk.hpp"
#include "Logger.hpp"
#include <iostream>
#include <sstream>

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
		found = data.find("\r\n", next_chunk_start);
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

		next_chunk_start = data.find("\r\n", found + 2 + size);
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
