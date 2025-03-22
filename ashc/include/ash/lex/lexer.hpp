#pragma once

#include <string_view>
#include <ash/rep/tokens.hpp>

namespace ash
{
    class Lexer
    {
    public:
        Lexer(std::string_view& source, std::string_view filename);
    
        TokenList tokenize(); /* scan all token from source */

    private:
        std::string_view source;
        std::string_view filename;
        uint32_t pos = 0;
        bool at_bol = true; /* at beginning of line */
        bool has_space = false; /* preceded by whitespace */

        Token scan_token();
        Token new_token(TokenType type, uint32_t start, uint16_t length);

        std::pair<TokenType, uint16_t> scan_keyword(uint32_t start);
        uint16_t scan_identifier(uint32_t start);
        uint16_t scan_number(uint32_t start);
        uint16_t scan_string(uint32_t start);
        uint16_t scan_operator(uint32_t start);

        [[nodiscard]] bool is_at_end() const;
        [[nodiscard]] char peek() const;
        [[nodiscard]] char peek_next() const;
        char advance();
        bool match(char expected);
        void skip_whitespace();
        void skip_comment();

        void error(const std::string& message) const;
        void error_at(uint32_t pos, const std::string& message) const;
    };
}
