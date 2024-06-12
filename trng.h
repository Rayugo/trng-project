#include "cryptlib.h"

class TRNG : public CryptoPP::RandomNumberGenerator {
public:
	TRNG() {}
	int getRandomByte();
	void GenerateBlock(CryptoPP::byte* output, size_t size) override;
};