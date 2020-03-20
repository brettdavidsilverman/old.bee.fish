#include <iostream>
#include <fstream>

using namespace std;
#define MAX_LEN (1 << 13)
#define writeInt(c) {output.put((char)(c >> 8)); output.put((char)(c & 0xFF));}

//#define writeInt(c) output << (wchar_t)c
int main(int argc, char** args) {
    //locale utf8 = locale("");
    //locale::global(utf8);
/*    unsigned char c1 = 1;
    unsigned int bits = 0;
    while (c1 > 0) {
        c1 <<= 1;
        bits++;
    }
    cout << "unsigned char bits " << bits << endl;
*/
    for (unsigned int power = 0; power <= 16; power++) {
        char file[1024];
        sprintf(file, "index%u.js", power);
        cout << file << ":";
    
        ofstream output;

        output.open(file, ios_base::out | ios_base::binary | ios_base::trunc);

       // output.put(0x0);
       // output.put(0x0);
        output.put(0xFE);
        output.put(0xFF);

        unsigned long count = 1;
        unsigned int length = 0;
        unsigned int str = 0;
        writeInt('(');
        while(count < (1 << power)) {
            writeInt('\"');
            for (length = 0;length <= MAX_LEN && count < (1 << power); count++) {
            
                switch (count) {
                case '\r':
                    writeInt('\\');
                    writeInt('r');
                    break;
                case '\n':
                    writeInt('\\');
                    writeInt('n');
                    break;
                case '\\':
                    writeInt('\\');
                    writeInt('\\');
                    break;
                case '\"':
                    writeInt('\\');
                    writeInt('\"');
                    break;
                default:
                    writeInt(count);
                    break;
                }
                length++;
            }
            
            writeInt('\"');

            if (count < (1 << power)) {
                writeInt('+');
            }
        }
        writeInt(')');
        writeInt('.');
        writeInt('b');
        writeInt('(');
        writeInt(')');
        writeInt(';');

        output.flush();
        output.close();
        cout << count << endl;
    }
    return 0;
}
