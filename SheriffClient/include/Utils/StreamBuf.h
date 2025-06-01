#ifndef _TEEBUF_
#define _TEEBUF_

#include <iostream>
#include <fstream>
#include <streambuf>

class TeeBuf : public std::streambuf {
	std::streambuf* sb1;
	std::streambuf* sb2;

public:
	TeeBuf(std::streambuf* buf1, std::streambuf* buf2) : sb1(buf1), sb2(buf2) {}

protected:
	virtual int overflow(int c) override {
		if (c == EOF) return !EOF;
		if (sb1) sb1->sputc(c);
		if (sb2) sb2->sputc(c);
		return c;
	}

	virtual int sync() override {
		if (sb1) sb1->pubsync();
		if (sb2) sb2->pubsync();
		return 0;
	}
};

#endif // !_TEEBUF_

