#include "trng.h"
#include "rsa.h"
#include "pssr.h"

#include <iostream>
#include <osrng.h>
#include <pssr.h>
#include <base64.h>
#include <filters.h>
#include <files.h>


void GenerateRSAKeyPair(CryptoPP::RSA::PublicKey& publicKey, CryptoPP::RSA::PrivateKey& privateKey) {
    TRNG rng;
    CryptoPP::InvertibleRSAFunction params;
    params.Initialize(rng, 2048); //creates the key-pair with modulus 2048 and default exponent 17

    publicKey = CryptoPP::RSA::PublicKey(params);
    privateKey = CryptoPP::RSA::PrivateKey(params);
}

std::string EncryptMessage(const std::string& message, const CryptoPP::RSA::PublicKey& publicKey) {
    TRNG rng;
    std::string cipher;

    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey); //RSA encryption scheme defined in PKCS #1 v2.0
    CryptoPP::StringSource ss1(message, true,
        new CryptoPP::PK_EncryptorFilter(rng, encryptor, //encrypt the message using PK_EncryptorFilter
            new CryptoPP::StringSink(cipher)
        )
    );

    return cipher;
}

std::string DecryptMessage(const std::string& cipher, const CryptoPP::RSA::PrivateKey& privateKey) {
    TRNG rng;
    std::string recovered;

    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(privateKey); //RSA encryption scheme defined in PKCS #1 v2.0
    CryptoPP::StringSource ss2(cipher, true,
        new CryptoPP::PK_DecryptorFilter(rng, decryptor,
            new CryptoPP::StringSink(recovered)
        )
    );

    return recovered;
}


std::string SignMessage(const std::string& message, const CryptoPP::RSA::PrivateKey& privateKey) {
    TRNG rng;
    std::string signature;

    //Signature Scheme with Appendix(Filters)
    CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA256>::Signer signer(privateKey);
    CryptoPP::StringSource ss1(message, true,
        new CryptoPP::SignerFilter(rng, signer,
            new CryptoPP::StringSink(signature)
        )
    );

    return signature;
}

bool VerifyMessage(const std::string& message, const std::string& signature, const CryptoPP::RSA::PublicKey& publicKey) {
    CryptoPP::RSASS<CryptoPP::PSS, CryptoPP::SHA256>::Verifier verifier(publicKey);
    CryptoPP::byte result = false;

    try {
        CryptoPP::StringSource ss(signature + message, true,
            new CryptoPP::SignatureVerificationFilter(verifier,
                new CryptoPP::ArraySink(&result, sizeof(result)),
                CryptoPP::SignatureVerificationFilter::SIGNATURE_AT_BEGIN | CryptoPP::SignatureVerificationFilter::PUT_RESULT));
    }
    catch (const CryptoPP::Exception& e) {
        std::cerr << "Verification failed with exception: " << e.what() << std::endl;
        return false;
    }

    return result;
}


int main() {
    CryptoPP::RSA::PublicKey publicKey;
    CryptoPP::RSA::PrivateKey privateKey;

    // generate key-pair
    GenerateRSAKeyPair(publicKey, privateKey);

    std::string message = "Hello, world!";

    // encrypt message
    std::string cipher = EncryptMessage(message, publicKey);
    std::cout << "Encrypted: " << cipher << std::endl;

    // decrypt message
    std::string recovered = DecryptMessage(cipher, privateKey);
    std::cout << "Decrypted: " << recovered << std::endl;

    // sign message
    std::string signature = SignMessage(message, privateKey);
    std::cout << "Signature: " << signature << std::endl;

    // testing the signature
    
    //message += "a";
    //CryptoPP::RSA::PublicKey publicKey2;
    //CryptoPP::RSA::PrivateKey privateKey2;
    //GenerateRSAKeyPair(publicKey2, privateKey2);

    // valid message
    bool isValid = VerifyMessage(message, signature, publicKey);
    std::cout << "Signature is valid: " << isValid << std::endl;

    return 0;
}