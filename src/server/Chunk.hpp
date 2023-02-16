#pragma once
#ifndef CHUNK_HPP
# define CHUNK_HPP

#include <string>
#include <vector>

class Chunk {
public:
	Chunk(std::string content, size_t size) : _content(content), _size(size) {}
	const std::string & get_content(void) const;
	size_t get_size(void) const;
private:
	std::string	_content;
	size_t		_size;
};

class Chunks {
public:
	Chunks(std::string data);
	const std::vector<Chunk> & get_chunks(void) const;

	typedef std::vector<Chunk>::iterator iterator;
	iterator begin();
	iterator end();
	typedef std::vector<Chunk>::const_iterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;
	typedef std::vector<Chunk>::reverse_iterator reverse_iterator;
	reverse_iterator rbegin();
	reverse_iterator rend();
	typedef std::vector<Chunk>::const_reverse_iterator const_reverse_iterator;
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;

private:
	std::vector<Chunk> _chunks;
};

class ChunkBuilder {
	public:
		ChunkBuilder();
		std::vector<Chunk> parse(std::string data);
	private:
		std::string _save;
		enum ParseState {
			waiting_for_size,
			waiting_for_data,
			waiting_for_crlf
		};
		ParseState _state;
		size_t _size;
};

#endif
