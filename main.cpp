#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <limits.h> // for CHAR_BIT

#define underline "====================================================================\n"

const int full_length = 512;

using namespace std;

/****************************************************************************\
    Author: Antti Heikkilä 2018

    References:
    https://en.wikipedia.org/wiki/SHA-1
    https://www.cryptocompare.com/coins/guides/how-does-a-hashing-algorithm-work
    http://www.metamorphosite.com/show-steps?subject=6&topic=17&input=test 

*****************************************************************************/

string to_binary(string word)
{
    printf("Convert word to binaries\n");

    string binary_word = "";
    
    for (std::size_t i = 0; i < word.size(); ++i)
    {   
        binary_word.append(bitset<8>(word.c_str()[i]).to_string());
    }

    cout << word << " converted to " << binary_word << endl;
    printf(underline);
    
    return binary_word;
}

void add_zeros_to(string &binary, int binary_length)
{
    printf("Add zeros to the end of the binary\n");

    int added_zeros = 0;
    const int max_length = 448; // 512 mod 448 = 64

    for (int i = binary_length+1; i < max_length; i++)
    {   
        binary.append("0");
        added_zeros++;
    }

    cout << "Binary resized from " << binary_length <<  " to " << binary.length() << endl;
    cout << "..and " << added_zeros << " zeros added to it" << endl;
    printf(underline);
}

void add_64bit_message_length_field_to(string &binary, int message_length)
{
    printf("Add message length to the end of the binary\n");

    binary.append(bitset<64>(message_length).to_string());
    cout << "64 chars added to the binary so it's resized to " << binary.length() << endl;
    printf(underline);
}

vector<string> split_binary_to_list_of_32bit_binaries(string binary)
{
    cout << "Split binary into 32bit fields" << endl;

    vector<string> binany_list;
    int counter = 1;
    string field = "";
    int word_number = 1;
    bool word_changed = true;

    for(int y = 0; y < full_length; y++)
    {
        if(word_changed)
        {
            cout << "word " << word_number++ << ": ";
            word_changed = false;
        }

        field += binary[y];
        cout << binary[y];

        if(counter < 32)
        {
            counter++;
        }
        else
        {          
            binany_list.push_back(field); 
            cout << "\n";
            field = "";
            counter = 1;
            word_changed = true;
        }
    }
    
    printf(underline);
    return binany_list;
}

static inline uint32_t rotate_left_uint32_t(uint32_t n, unsigned int c)
{
    // assumes width is a power of 2.
    const unsigned int mask = (CHAR_BIT*sizeof(n) - 1); 

    // assert ( (c<=mask) &&"rotate by type width or more");
    c &= mask;
    return (n<<c) | (n>>( (-c)&mask ));
}

void transform_16_words_to_80_words(vector<string> &string_list)
{
    cout << "Transform 16 words to 80 words" << endl;
    int word_number = 16;

    for(int i = 0; i < 64; i++)
    {
        auto word = bitset<32>(string_list[13+i]) ^ bitset<32>(string_list[8+i]); 
        word ^= bitset<32>(string_list[2+i]);
        word ^= bitset<32>(string_list[0+i]);

        uint32_t unsigned_word = rotate_left_uint32_t((uint32_t)word.to_ulong(), 1);
        word = bitset<32>(unsigned_word);
        cout << word_number++ << ": " << word << endl;  
        string_list.push_back(word.to_string());
    }
    printf(underline);
}

void pretend_msb_bit_if_needed(uint64_t &dec, size_t expected_length)
{
    /*
        After each iteration the new word should be one bit longer than the last. 
        Sometimes this will be a necessary carrier bit 
        and when that's not needed it must simply prepend a 1. 
    */   

    if (expected_length == 33)
    {
        auto binary = bitset<33>(dec);
        auto mask = bitset<33>("100000000000000000000000000000000");

        if(binary.to_string()[expected_length-33] == '0')
        {
            auto new_binary = binary | mask;
            dec = new_binary.to_ullong();
        }
    }

    if (expected_length == 34)
    {
        auto binary = bitset<34>(dec);
        auto mask = bitset<34>("1000000000000000000000000000000000");

        if(binary.to_string()[expected_length-34] == '0')
        {
            auto new_binary = binary | mask;
            dec = new_binary.to_ullong();
        }
    }

    if (expected_length == 35)
    {
        auto binary = bitset<35>(dec);
        auto mask = bitset<35>("10000000000000000000000000000000000");

        if(binary.to_string()[expected_length-35] == '0')
        {
            auto new_binary = binary | mask;
            dec = new_binary.to_ullong();
        }
    }
    if (expected_length == 36)
    {
        auto binary = bitset<36>(dec);
        auto mask = bitset<36>("100000000000000000000000000000000000");

        if(binary.to_string()[expected_length-36] == '0')
        {
            auto new_binary = binary | mask;
            dec = new_binary.to_ullong();
        }
    }
}

string sub_string_to_32bit(string &binary)
{
    return binary.substr(binary.length()-32, binary.length());
}

void operate_off_the_five_variables(vector<string> words, uint64_t &h0, 
                            uint64_t &h1, uint64_t &h2, uint64_t &h3, uint64_t &h4)
{
    cout << "Operate off the five variables" << endl;
    auto A = bitset<32>(h0).to_ulong();
    auto B = bitset<32>(h1).to_ulong();
    auto C = bitset<32>(h2).to_ulong();
    auto D = bitset<32>(h3).to_ulong();
    auto E = bitset<32>(h4).to_ulong();
    auto F = bitset<32>("0").to_ulong();
    auto K = bitset<32>("01011010100000100111100110011001").to_ulong();

    for(int i = 0; i < 80; i++)
    {    
        if(i <= 19)
        {
            // Function 1
            F = (~B & D) | (B & C);
        }
        
        if(i >= 20 && i <= 39)
        {
            // Function 2
            F = (B ^ C) ^ D;
            K = bitset<32>("01101110110110011110101110100001").to_ulong();
        }

        if(i >= 40 && i <= 59)
        {
            // Function 3
            F = ((B & C) | (B & D)) | (C & D);
            K = bitset<32>("10001111000110111011110011011100").to_ulong();
        } 

        if(i >= 60 && i <= 79)
        {
            // Function 4
            F = (B ^ C) ^ D;
            K = bitset<32>("11001010011000101100000111010110").to_ulong();
        }         

        // A rot left + F
        uint64_t temp =  (uint64_t)rotate_left_uint32_t(A,5) + (uint64_t)F;
        pretend_msb_bit_if_needed(temp, 33);

        // A rot left + F + E
        temp += (uint64_t)E;
        pretend_msb_bit_if_needed(temp, 34);

        // A rot left + F + E + K
        temp += (int64_t)K;
        pretend_msb_bit_if_needed(temp, 35);

        // A rot left + F + E + K + word[i]
        temp += bitset<32>(words[i]).to_ulong();
        pretend_msb_bit_if_needed(temp, 36);

        string string_temp = bitset<64>(temp).to_string();
        auto truncated_temp = sub_string_to_32bit(string_temp);

        E = D;
        D = C;
        C = rotate_left_uint32_t(B, 30);
        B = A;
        A = bitset<32>(truncated_temp).to_ullong();

    }

    cout << "E:" << bitset<32>(E) << endl;
    cout << "D:" << bitset<32>(D) << endl;
    cout << "C:" << bitset<32>(C) << endl;
    cout << "B:" << bitset<32>(B) << endl;
    cout << "A:" << bitset<32>(A) << endl;
    cout << underline << endl;

    h0 += A;
    pretend_msb_bit_if_needed(h0, 33);
    h1 += B;
    pretend_msb_bit_if_needed(h1, 33);
    h2 += C;
    pretend_msb_bit_if_needed(h2, 33);
    h3 += D;
    pretend_msb_bit_if_needed(h3, 33);
    h4 += E;
    pretend_msb_bit_if_needed(h4, 33);
 
}

int main()
{ 
    // Five random variables
    uint64_t h0 = bitset<32>("01100111010001010010001100000001").to_ulong();
    uint64_t h1 = bitset<32>("11101111110011011010101110001001").to_ulong();
    uint64_t h2 = bitset<32>("10011000101110101101110011111110").to_ulong();
    uint64_t h3 = bitset<32>("00010000001100100101010001110110").to_ulong();
    uint64_t h4 = bitset<32>("11000011110100101110000111110000").to_ulong();

    // Choose message to hash
    string message = "test";

    // Convert string to binary
    // .. and join characters
    string binary = to_binary(message);

    // And store the original message length
    int message_length = binary.length();

    // Add 1 to the end of the message
    binary.append("1");

    // Add zeros to the end until the length of the message is congruent to 448 mod 512. 
    // That means that after dividing the message length by 512, the remainder will be 448. 
    // In this case, the length of the original message in binary is 48 + 1 from the last step. 
    //That means that we will need to add a total of 399 zero's to the end.
    add_zeros_to(binary, message_length);
    
    // Add the original message length into the 64 bit field left over after the 448 modular arithmetic. 
    add_64bit_message_length_field_to(binary, message_length);

    // Break the message up into sixteen sections of 32 characters/bits.
    vector<string> words = split_binary_to_list_of_32bit_binaries(binary);

    // Transform the 16 x 32 character bit words into 80 words.
    transform_16_words_to_80_words(words);

    // The next step is to run a set of functions over the words in a specific order operating off 
    // the five variables that were set in the begining.
    operate_off_the_five_variables(words, h0, h1, h2, h3, h4);

    // Turncate & Convert the H variables into hex:
    // ..and Join the variables together to give the hash digest
    printf ("digest: %x%x%x%x%x\n", bitset<32>(h0).to_ulong(),
                        bitset<32>(h1).to_ulong(),
                        bitset<32>(h2).to_ulong(),
                        bitset<32>(h3).to_ulong(),
                        bitset<32>(h4).to_ulong());

}