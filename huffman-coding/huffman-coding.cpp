#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Huffman Tree Node
struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode* left, * right;

    HuffmanNode(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue
struct CompareNodes {
    bool operator()(HuffmanNode* left, HuffmanNode* right) {
        return left->freq > right->freq;
    }
};

// Function to generate Huffman codes
void generateCodes(HuffmanNode* root, string code, unordered_map<char, string>& codes) {
    if (root == nullptr)
        return;

    if (root->left == nullptr && root->right == nullptr) {
        codes[root->data] = code;
    }

    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// Function to construct Huffman tree and generate codes
unordered_map<char, string> huffmanCoding(string text) {
    unordered_map<char, int> freq;
    for (char c : text) {
        freq[c]++;
    }

    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> pq;

    for (auto pair : freq) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        int sum = left->freq + right->freq;
        HuffmanNode* parent = new HuffmanNode('\0', sum);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    unordered_map<char, string> codes;
    generateCodes(pq.top(), "", codes);
    return codes;
}

// Function to write binary data to file
void writeBinaryToFile(const string& filename, const string& data) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Could not create output file." << endl;
        return;
    }
    file.write(data.c_str(), data.size());
    file.close();
}

int main() {
    string filename;
    cout << "Enter the name of the text file to encode using Huffman coding: ";
    cin >> filename;

    ifstream file(filename);
    if (!file) {
        cerr << "Error: Could not open file." << endl;
        return 1;
    }

    string input_text((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));

    auto start = high_resolution_clock::now(); // start timing

    unordered_map<char, string> codes = huffmanCoding(input_text);

    // Encode the input string using the generated codes
    string encoded_text;
    for (char c : input_text) {
        encoded_text += codes[c];
    }

    // Pack bits into bytes
    string encoded_bytes;
    for (size_t i = 0; i < encoded_text.size(); i += 8) {
        unsigned char byte = 0;
        for (size_t j = 0; j < 8; j++) {
            if (i + j < encoded_text.size() && encoded_text[i + j] == '1') {
                byte |= (1 << (7 - j));
            }
        }
        encoded_bytes.push_back(byte);
    }

    // Output the encoded text to a new file in binary format
    string output_filename = filename.erase(filename.length() - 4) + "_coded.bin";
    writeBinaryToFile(output_filename, encoded_bytes);

    cout << "Huffman Codes:\n";
    for (auto code : codes) {
        cout << code.first << ": " << code.second << endl;
    }

    auto stop = high_resolution_clock::now(); // stop timing
    auto duration = duration_cast<milliseconds>(stop - start); // calculate duration

    cout << "Execution time: " << duration.count() << " milliseconds" << endl;
    cout << "Encoded text written to " << output_filename << endl;

    return 0;
}
