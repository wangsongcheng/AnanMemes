#ifndef ANAN_STRING_HPP
#define ANAN_STRING_HPP
#include <stack>
#include <string>
#include <vector>
#include <stdint.h>
#include <cwctype>
#include <algorithm>
namespace ananStr{
    static inline void removeSpaces(const std::wstring& text, std::wstring& out) {
        out = text;
        out.erase(std::remove(out.begin(), out.end(), L' '), out.end());
    }
    static inline std::wstring fixBrackets(const std::wstring& input) {
        std::wstring result = input;
        std::stack<size_t> bracketStack;
        for (size_t i = 0; i < result.length(); ++i) {
            if (result[i] == L'[') {
                bracketStack.push(i);
            } else if (result[i] == L']') {
                if (!bracketStack.empty()) {
                    bracketStack.pop();
                }
            }
        }
        std::vector<size_t> unpairedPositions;
        while (!bracketStack.empty()) {
            unpairedPositions.push_back(bracketStack.top());
            bracketStack.pop();
        }
        std::reverse(unpairedPositions.begin(), unpairedPositions.end());
        
        if (unpairedPositions.empty()) {
            return result;
        }

        for (auto it = unpairedPositions.rbegin(); it != unpairedPositions.rend(); ++it) {
            size_t startPos = *it;
            size_t insertPos = std::wstring::npos;
            
            for (size_t i = startPos + 1; i < result.length(); ++i) {
                wchar_t c = result[i];
                if (std::iswpunct(c) && c != L']') {
                    insertPos = i;
                    break;
                }
            }
            
            if (insertPos != std::wstring::npos) {
                result.insert(insertPos, L"]");
            } else {
                result.append(L"]");
            }
        }
        
        return result;
    }
    //该函数将空格视为标点符号
    static inline size_t GetFirstPunctuationPos(const std::wstring& text) {
        for (size_t i = 0; i < text.length(); ++i) {
            if (std::iswpunct(text[i]) || text[i] == L' ') {
                return i;
            }
        }
        return std::wstring::npos;
    }

    static inline std::vector<std::wstring> splitByDelimiters(const std::wstring& text) {
        std::vector<std::wstring> result;
        std::wstring current_segment;
        
        for (wchar_t c : text) {
            if (std::iswpunct(c) || c == L' ') {
                if (!current_segment.empty()) {
                    result.push_back(current_segment);
                    current_segment.clear();
                }
            } else {
                current_segment += c;
            }
        }
        
        if (!current_segment.empty()) {
            result.push_back(current_segment);
        }
        
        return result;
    }
    static inline std::vector<std::wstring> splitEvenly(const std::wstring& text, uint32_t count) {
        std::vector<std::wstring> result;
        
        if (text.empty() || count == 0) {
            return result;
        }
        
        if (count == 1) {
            result.push_back(text);
            return result;
        }
        
        size_t text_length = text.length();
        size_t approx_chars_per_line = text_length / count;
        size_t remainder = text_length % count;
        
        size_t start_pos = 0;
        
        for (uint32_t i = 0; i < count; ++i) {
            if (start_pos >= text_length) {
                result.push_back(L"");
                continue;
            }
            
            size_t current_chars = approx_chars_per_line;
            if (i < remainder) {
                current_chars++;
            }
            
            size_t end_pos = start_pos + current_chars;
            if (end_pos > text_length) {
                end_pos = text_length;
            }
            
            result.push_back(text.substr(start_pos, end_pos - start_pos));
            start_pos = end_pos;
        }
        
        return result;
    }
    static inline size_t split(const std::wstring&text, wchar_t chr, std::vector<std::wstring>&out, size_t pos){
        const uint32_t len = text.length();
        size_t openBracket = text.find(chr, pos);
        if (openBracket == std::wstring::npos) {
            if (pos < len) {
                out.push_back(text.substr(pos));
            }
            return len;
        }
        if (openBracket > pos) {
            out.push_back(text.substr(pos, openBracket - pos));
        }
        return openBracket;
    }
    static inline void split(const std::wstring& text, std::vector<std::wstring>& out) {
        size_t pos = 0;
        while (pos < text.length()) {
            size_t closeBracket = split(text, ']', out, split(text, '[', out, pos));
            out[out.size() - 1] += L"]";
            pos = closeBracket + 1;
        }
    }
    static inline std::vector<std::wstring> split(const std::wstring& text, uint32_t count) {
        auto pos = GetFirstPunctuationPos(text);
        if (pos == std::wstring::npos) {
            return splitEvenly(text, count);
        }

        std::vector<std::wstring> result, temp;
        if(text.find('[') != std::wstring::npos){
            split(text, temp);
        }
        else{
            return splitByDelimiters(text);
        }
        for (auto&it:temp){
            auto out = splitByDelimiters(it);
            if(!out.empty()){
                if(it[0] == '['){
                    const uint32_t last = out.size() - 1;
                    out[0] = L"[" + out[0];
                    out[last] += L"]";
                }
                result.insert(result.end(), out.begin(), out.end());
            }
        }
        if(result.empty())result.push_back(text);
        return result;
    }
};
#endif