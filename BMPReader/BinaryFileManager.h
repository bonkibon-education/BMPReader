#pragma once


#ifndef _BINARY_FILE_MANAGER // _BINARY_FILE_MANAGER
#define _BINARY_FILE_MANAGER


#include <iostream>
#include <fstream>


/**
 * Provides a basic interface for managing binary files.
 */
class BinaryFileManager {
private:
	const std::string _filename;
	const std::string _extension;
	std::ifstream _file;

public:
	BinaryFileManager(const std::string& filename, const std::string& allowed_extension) : _filename(filename), _extension(allowed_extension) {}

	/**
	 * @return const std::string& A reference to the file filename.
	*/
	const std::string& fileName() const& { return _filename; }

	/**
	 * @return const std::string& A reference to the file extension.
	*/
	const std::string& fileExtension() const& { return _extension; }

	/**
	 * Opens the binary file.
	 *
	 * @return true if the file was successfully opened, false otherwise.
	*/
	bool openFile() {

		if (_file.is_open() || _extension != getExtension(_filename)) {
			return false;
		}

		_file.open(_filename, std::ofstream::in | std::ofstream::binary);
		return _file.is_open();
	}

	/**
	 * Reads the contents of the binary file.
	 *
	 * @param fileSize The size of the binary file in bytes.
	 * @param data The unique_ptr to hold the dynamically allocated buffer.
	 * @return true if the file was successfully read, false otherwise.
	*/
	bool readFile(std::ifstream::pos_type& fileSize, std::unique_ptr<char[]>& data) {

		if (!_file.is_open() || !fileSize) {
			return false;
		}

		data.reset(new char[static_cast<unsigned int>(fileSize)]);
		_file.read(data.get(), fileSize);

		return _file.good();
	}

	/**
	 * Closes the binary file.
	 *
	 * @return true if the file was successfully closed, false otherwise.
	*/
	bool closeFile() {

		if (_file.is_open()) {
			_file.close();
			return true;
		}

		return false;
	}

	/**
	 * Retrieves the size of the binary file in bytes.
	 *
	 * @param fileSize The reference to store the file size.
	 * @return true if the file size was successfully retrieved, false otherwise.
	*/
	const bool getFileSize(std::ifstream::pos_type& fileSize) {

		if (!_file.is_open()) {
			return false;
		}

		_file.seekg(0, std::ofstream::end);
		fileSize = _file.tellg();
		_file.seekg(0, std::ios::beg);

		return true;
	}

	/**
	 * Find file extension.
	 *
	 * @param filename The filename to extract the extension from.
	 * @return const std::string The extracted file extension.
	*/
	static const std::string getExtension(const std::string& filename) {
		return filename.substr(filename.find_last_of('.') + 1);
	}

	~BinaryFileManager() {
		closeFile();
	}
};


#endif // _BINARY_FILE_MANAGER