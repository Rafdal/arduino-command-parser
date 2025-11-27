#ifndef CAPTURE_BUFFER_HPP
#define CAPTURE_BUFFER_HPP

#include <Arduino.h>

class CaptureBuffer {
	private:
		char* buffer_ptr = nullptr;
		uint16_t buffer_size_max = 0;
		uint16_t buffer_index = 0;
		char terminator = '\0';
		bool terminator_found = false;
		const char* ignore_chars = "\r\n"; // Characters to ignore while capturing

	public:
		CaptureBuffer(uint16_t size, char termChar = '\0') 
		: buffer_size_max(size), terminator(termChar) 
		{
			buffer_ptr = new char[buffer_size_max];
			clear();
		}
		~CaptureBuffer() 
		{
			if(buffer_ptr) 
			{
				delete[] buffer_ptr;
				buffer_ptr = nullptr;
			}
		}
		void clear() 
		{
			if(buffer_ptr) 
				memset(buffer_ptr, 0, buffer_size_max);
			buffer_index = 0;
			terminator_found = false;
		}
		bool add(char c)
		{
			if(c == terminator)
			{
				buffer_ptr[buffer_index] = '\0'; // Null-terminate
				terminator_found = true;
				return true; // Indicate termination
			}
			else if(strchr(ignore_chars, c) != nullptr)
			{
				// Ignore this character
				return false;
			}
            else if(c == '\b') // Handle backspace
            {
                if(buffer_index > 0)
                {
                    buffer_index--;
                    buffer_ptr[buffer_index] = '\0';
                }
            }
			else if(buffer_index < buffer_size_max - 1)
			{
				buffer_ptr[buffer_index++] = c;
			}
			else
			{
				clear(); // Buffer overflow, clear buffer
			}
			return false; // Indicate no termination
		}
		char* buffer() { return buffer_ptr; }
		uint16_t size() { return buffer_index; }
		bool isTerminated() { return terminator_found; }
		void dumpTo(Stream& stream)
		{
			stream.print("buffer[");
			stream.print(buffer_index);
			stream.print("]: \'");
			for(uint16_t i = 0; i < buffer_index; i++)
			{
				switch (buffer_ptr[i])
				{
				case '\n':
					stream.print("\\n");
					break;
				case '\r':
					stream.print("\\r");
					break;
				case '\0':
					stream.print("\\0");
					break;
				case '\t':
					stream.print("\\t");
					break;
				case '\b':
					stream.print("\\b");
					break;
				default:
					stream.print(buffer_ptr[i]);
					break;
				}
			}
			stream.print("\'");
		}
				
};

#endif // CAPTURE_BUFFER_HPP