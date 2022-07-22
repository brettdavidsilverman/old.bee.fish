// Debug:
// g++ -g -ggdb -Wall -Wextra -Wno-unused -I. -I/usr/include/cryptopp rsa_kgen_prof.cpp -o rsa_kgen_prof.exe -lcryptopp

// Release:
// g++ -O2 -Wall -Wextra -Wno-unused -I. -I/usr/include/cryptopp rsa_kgen_prof.cpp -o rsa_kgen_prof.exe -lcryptopp && strip --strip-symbols rsa_kgen_prof.exe

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <iomanip>
using std::fixed;
using std::setprecision;

#include <sstream>
using std::stringstream;

#include <stdexcept>
using std::runtime_error;

#include "cryptopp/rsa.h"
using CryptoPP::RSA;

#include "cryptopp/osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include <cryptopp/hrtimer.h>
using CryptoPP::TimerBase;
using CryptoPP::ThreadUserTimer;

#include <cryptopp/hex.h>

int main(int argc, char** argv)
{
    try
    {
        int bits = 2048;
        if(argc >= 2)
        {
            bits = atoi(argv[1]);
        }

        if(bits < 0)
        {
            throw runtime_error("Specified modulus size is not valid");
        }

        using namespace CryptoPP;

        // Grab a Generator
        AutoSeededRandomPool prng;

        // Specify modulus, accept e = 17. 128-bits is artificially small.
        RSAES_OAEP_SHA_Decryptor Decryptor( prng, bits /*, e */ );
        RSAES_OAEP_SHA_Encryptor Encryptor( Decryptor );
        // BER Encoded Keys
        std::string pbkey, pvkey;

        // Hex Encoder
        HexEncoder encoder;
        
        // Public Key
        encoder.Attach( new StringSink( pbkey ) );
        const PublicKey& publicKey = Encryptor.GetPublicKey();
        publicKey.Save( encoder );

        // Private Key
        encoder.Attach( new StringSink( pvkey ) );
        const PrivateKey& privateKey = Decryptor.GetPrivateKey();
        privateKey.Save( encoder );

        std::cerr << "Public Key" << std::endl << pbkey << std::endl;
        std::cerr << "Private Key" << std::endl << pvkey << std::endl;


        HexDecoder decoder;
        decoder.Put((const byte*)pvkey.data(), pvkey.length());
        decoder.MessageEnd();
        RSAES_OAEP_SHA_Decryptor _Decryptor;
        _Decryptor.AccessKey().Load(decoder);

        std::string _pvkey;
        encoder.Attach( new StringSink( _pvkey ) );
        const PrivateKey& _privateKey = _Decryptor.GetPrivateKey();
        _privateKey.Save( encoder );

        if (_pvkey == pvkey) {
            std::cerr << "ALL OK" << std::endl;
        }
        else
            std::cerr << "ERROR" << std::endl;

    }
    catch(CryptoPP::Exception& e)
    {
        cerr << e.what() << endl;
        return -2;
    }
    catch(std::exception& e)
    {
        cerr << e.what() << endl;
        return -1;
    }

    return 0;
}
