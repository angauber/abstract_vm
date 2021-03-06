#include "lexer.hpp"

int Lexer::readFromFile(char* filename) {
	std::ifstream fileStream;

	fileStream.open(filename, std::ifstream::in);

	this->shouldBuffer = false;

	return this->read(fileStream);
}

int	Lexer::readFromStdin() {
	this->shouldBuffer = true;

	return this->read(std::cin);
}

int	Lexer::read(std::istream &stream) {
	try {
		this->readStream(stream);

		return 0;
	} catch (Exception &e) {
		e.setLine(&this->line);

		std::cerr << e.what() << std::endl;

		return -1;
	}
}

void Lexer::readStream(std::istream &stream) {
	lineInfos	line;
	std::vector<std::string>	tokens;
	std::vector<lineInfos>	buffer;

	line.nb = 0;

	this->line = line;

	if (this->shouldBuffer)
		this->parser.buffer();

	while (std::getline(stream, line.content)) {
		if (line.nb == std::numeric_limits<int>::max())
			throw OverflowException();
		
		line.nb++;

		if (this->shouldBuffer && line.content == ";;") {
			for (lineInfos buffered : buffer) {
				this->line = buffered;

				parser.setLine(this->line);
				if (parser.parseTokens()) {
					return ;
				}
			}

			this->line = buffer.empty() ? line : buffer.back(),
			throw ParsingException("Error: No exit instruction provided");
		}

		tokens = this->getTokensFromLine(line.content);
		line.tokens = tokens;
		if (tokens.size() > 0) {
			if (this->shouldBuffer) {
				buffer.push_back(line);
			} else {
				this->line = line;

				parser.setLine(this->line);
				if (parser.parseTokens()) {
					return ;
				}
			}
		}
	}

	if (this->shouldBuffer) {
		this->line = buffer.empty() ? line : buffer.back();
		throw ParsingException("Error: No end instruction provided while reeding from standard input");
	} else {
		throw ParsingException("Error: No exit instruction provided");
	}
}

std::vector<std::string> Lexer::getTokensFromLine(std::string line) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream iss(line);

	while(iss >> token) {
		if (token.at(0) == ';') {
			break;
		}
		tokens.push_back(token);
	}

	return tokens;
}
