#include <cctype>
#include <iostream>
#include <ash/lex/lexer.hpp>

namespace ash
{
    Lexer::Lexer(std::string_view& source, std::string_view filename)
        : source(source), filename(filename), pos(0), at_bol(true), has_space(false) {}

    TokenList Lexer::tokenize()
    {
        TokenList tokens;
        tokens.reserve(source.length() / 8); /* rough estimate */
        
        while (!is_at_end())
        {
            skip_whitespace();
            if (is_at_end())
                break;
                
            Token token = scan_token();
            tokens.emplace_back(token);
        }
        
        /* add EOF because the end of token stream sequence */
        Token eof = new_token(TokenType::END_OF_FILE, pos, 0);
        tokens.emplace_back(eof);
        return tokens;
    }
    
    Token Lexer::scan_token()
    {
        uint32_t start = pos;
        char c = advance();
        
        /* check identifer */
        if (std::isalpha(c) || c == '_')
        {
            pos--; /* back up to scan the whole identifier */
            uint16_t length = scan_identifier(start);
            
            /* check if it's a keyword */
            auto [keyword_type, keyword_len] = scan_keyword(start);
            if (keyword_len > 0)
                return new_token(keyword_type, start, keyword_len);
            
            return new_token(TokenType::IDENTIFER, start, length);
        }
        
        /* check for numeric literals */
        if (std::isdigit(c))
        {
            pos--; /* back up to scan the whole number */
            uint16_t length = scan_number(start);
            return new_token(TokenType::NUM_LITERAL, start, length);
        }
        
        /* check for string literals */
        if (c == '"')
        {
            pos--; /* back up to scan the whole string */
            uint16_t length = scan_string(start);
            return new_token(TokenType::STR_LITERAL, start, length);
        }
        
        /* check for operators/punctuation */
        pos--;
        uint16_t op_length = scan_operator(start);
        if (op_length > 0)
        {
            std::string_view op_str = source.substr(start, op_length);
            
            /* find the matching token type from token_map */
            for (const auto& [str, type] : token_map)
            {
                if (str == op_str)
                    return new_token(type, start, op_length);
            }
        }
        
        /* if we actually reach here, it's an unknown character */
        error_at(start, "unexpected character");
        return new_token(TokenType::UNKNOWN, start, 1);
    }
    
    Token Lexer::new_token(TokenType type, uint32_t start, uint16_t length)
    {
        Token token;
        token.start = start;
        token.len = length;
        token.type = type;
        token.props = TokenProps{};
        
        /* reset state for next token */
        at_bol = false;
        has_space = false;
        return token;
    }
    
    std::pair<TokenType, uint16_t> Lexer::scan_keyword(uint32_t start)
    {
        uint16_t length = scan_identifier(start);
        std::string_view identifier = source.substr(start, length);
        
        /* look up in keyword map */
        for (const auto& [str, type] : token_map)
        {
            if (str == identifier)
                return { type, length };
        }
        
        return { TokenType::UNKNOWN, 0 }; /* not a keyword */
    }
    
    uint16_t Lexer::scan_identifier(uint32_t start)
    {
        /* first character already checked in scan_token */
        while (std::isalnum(peek()) || peek() == '_')
            advance();
            
        return pos - start;
    }
    
    uint16_t Lexer::scan_number(uint32_t start)
    {
        /* check if this is a hex, binary, or octal literal */
        if (peek() == '0')
        {
            advance();
            if (peek() == 'x' || peek() == 'X') /* hexadecimal */
            {
                advance();
                if (!std::isxdigit(peek()))
                {
                    error_at(pos, "expected hexadecimal digits after '0x'");
                    return pos - start;
                }
                
                while (std::isxdigit(peek()) || peek() == '_')
                    advance();
                
                /* handle type suffixes */
                if (peek() == 'u' || peek() == 'U')
                    advance();
                
                return pos - start;
            }
            
            /* bin */
            if (peek() == 'b' || peek() == 'B')
            {
                advance(); /* eat the prefix */
                if (peek() != '0' && peek() != '1')
                {
                    error_at(pos, "expected binary digits after '0b'");
                    return pos - start;
                }
                
                while (peek() == '0' || peek() == '1' || peek() == '_')
                    advance();
                
                /* handle type suffix */
                if (peek() == 'u' || peek() == 'U')
                    advance();
                
                return pos - start;
            }
            
            pos--; /* go back to the '0' and handle it as a regular number */
        }
        
        /* regular decimal number */
        while (std::isdigit(peek()) || peek() == '_')
            advance();
        
        /* handle fractional part */
        if (peek() == '.' && std::isdigit(peek_next()))
        {
            advance(); /* consume the '.' */
            
            while (std::isdigit(peek()) || peek() == '_')
                advance();
                
            /* handle floating-point suffixes */
            if (peek() == 'f' || peek() == 'F')
            {
                advance();
                if (peek() == '3' && peek_next() == '2')
                {
                    advance(); /* 3 */
                    advance(); /* 2 */
                }
                else if (peek() == '6' && peek_next() == '4')
                {
                    advance(); /* 6 */
                    advance(); /* 4 */
                }
            }
            
            return pos - start;
        }
        
        /* handle integer suffixes */
        if (peek() == 'u' || peek() == 'U' || peek() == 'i' || peek() == 'I')
        {
            advance(); /* eat suffix */
            
            if (peek() == '8')
            {
                advance(); /* 8 */
            }
            else if (peek() == '1' && peek_next() == '6')
            {
                advance(); /* 1 */
                advance(); /* 6 */
            }
            else if (peek() == '3' && peek_next() == '2')
            {
                advance(); /* 3 */
                advance(); /* 2 */
            }
            else if (peek() == '6' && peek_next() == '4')
            {
                advance(); /* 6 */
                advance(); /* 4 */
            }
        }
        
        return pos - start;
    }
    
    uint16_t Lexer::scan_string(uint32_t start)
    {
        advance(); /* consume opening quote */
        
        while (peek() != '"' && !is_at_end())
        {
            if (peek() == '\\')
            {
                advance(); /* skip the escape character */
                advance(); /* skip the escaped character */
            }
            else if (peek() == '\n')
            {
                error_at(pos, "unterminated string: newline in string literal");
                break;
            }
            else
            {
                advance();
            }
        }
        
        if (is_at_end())
        {
            error_at(start, "unterminated string: reached end of file");
            return pos - start;
        }
        
        advance(); /* eat closing quote */
        return pos - start;
    }
    
    /* greedy method; try to match operators in descending order of length */
    uint16_t Lexer::scan_operator(uint32_t start)
    {
        /* first try 3-character operators */
        if (pos + 3 <= source.length())
        {
            std::string_view op3 = source.substr(pos, 3);
            for (const auto& [str, type] : token_map)
            {
                if (str.length() == 3 && str == op3)
                {
                    pos += 3;
                    return 3;
                }
            }
        }
        
        /* then try 2-character operators */
        if (pos + 2 <= source.length())
        {
            std::string_view op2 = source.substr(pos, 2);
            for (const auto& [str, type] : token_map)
            {
                if (str.length() == 2 && str == op2)
                {
                    pos += 2;
                    return 2;
                }
            }
        }
        
        /* finally try single-character operators */
        if (pos + 1 <= source.length())
        {
            std::string_view op1 = source.substr(pos, 1);
            for (const auto& [str, type] : token_map)
            {
                if (str.length() == 1 && str == op1)
                {
                    pos += 1;
                    return 1;
                }
            }
        }
        
        /* if no operator matched, advance by 1 and return unknown */
        pos++;
        return 1;
    }
    
    bool Lexer::is_at_end() const
    {
        return pos >= source.length();
    }
    
    char Lexer::peek() const
    {
        if (is_at_end())
            return '\0';
        return source[pos];
    }
    
    char Lexer::peek_next() const
    {
        if (pos + 1 >= source.length())
            return '\0';
        return source[pos + 1];
    }
    
    char Lexer::advance()
    {
        return source[pos++];
    }
    
    bool Lexer::match(char expected)
    {
        if (is_at_end() || source[pos] != expected)
            return false;
            
        pos++;
        return true;
    }
    
    void Lexer::skip_whitespace()
    {
        while (!is_at_end())
        {
            char c = peek();
            
            switch (c)
            {
                case ' ':
                case '\t':
                case '\r':
                    has_space = true;
                    advance();
                    break;
                    
                case '\n':
                    at_bol = true;
                    has_space = false;
                    advance();
                    break;
                    
                case '/':
                    if (peek_next() == '/' || peek_next() == '*')
                    {
                        skip_comment();
                        has_space = true;
                    }
                    else
                    {
                        return;
                    }
                    break;
                    
                default:
                    return;
            }
        }
    }
    
    void Lexer::skip_comment()
    {
        if (peek() == '/' && peek_next() == '/')
        {
            /* line comment */
            advance(); /* skip first '/' */
            advance(); /* skip second '/' */
            
            while (peek() != '\n' && !is_at_end())
                advance();
        }
        else if (peek() == '/' && peek_next() == '*')
        { 
            /* block comment */
            advance(); /* skip '/' */
            advance(); /* skip '*' */
            
            while (!is_at_end() && !(peek() == '*' && peek_next() == '/'))
            {
                if (peek() == '\n')
                    at_bol = true;
                advance();
            }
            
            if (is_at_end())
            {
                error_at(pos - 2, "unterminated block comment");
                return;
            }
            
            advance(); /* skip '*' */
            advance(); /* skip '/' */
        }
    }
    
    void Lexer::error(const std::string& message) const
    {
        std::cerr << filename << ": " << message << std::endl;
    }
    
    void Lexer::error_at(uint32_t error_pos, const std::string& message) const
    {
        /* find the beginning of the line */
        uint32_t line_start = error_pos;
        while (line_start > 0 && source[line_start - 1] != '\n')
            line_start--;
            
        /* find the line number */
        uint32_t line = 1;
        for (uint32_t i = 0; i < line_start; i++)
        {
            if (source[i] == '\n')
                line++;
        }
        
        /* find the column number */
        uint32_t column = error_pos - line_start + 1;
        
        std::cerr << filename << ":" << line << ":" << column 
                  << ": error: " << message << std::endl;
                  
        /* print the line with an error marker */
        uint32_t line_end = error_pos;
        while (line_end < source.length() && source[line_end] != '\n')
            line_end++;
            
        std::string line_str(source.substr(line_start, line_end - line_start));
        std::cerr << line_str << std::endl;
        
        /* print the marker */
        std::string marker(column - 1, ' ');
        marker += "^";
        std::cerr << marker << std::endl;
    }
}