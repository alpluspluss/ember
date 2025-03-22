#pragma once

#include <cstdint>
#include <vector>

namespace ash
{
    enum class TokenType : std::uint8_t
    {
        IDENTIFER,
        NUM_LITERAL,
        STR_LITERAL,
        UNKNOWN,
        END_OF_FILE,

        VOID, /* void */
        U8, /* u8 */
        I8, /* i8 */
        U16, /* u16 */
        I16, /* i16 */
        U32, /* u32 */
        I32, /* i32 */
        U64, /* u64 */
        I64, /* i64 */
        F32, /* f32 */
        F64, /* f64 */
        BOOL, /* bool */
        STRING, /* string */
        OWN,   /* own[T] */
        SHARE, /* share[T] */
        REF,   /* ref[T] */
        PIN,   /* pin[T] */

        CAST,
        STATIC_CAST,
        TRUNCATE_CAST,
        REINTERPRET_CAST,


        TRUE,
        FALSE,
        NIL,
        IMPORT,
        VAR,
        CONST,
        ENUM,
        IF,
        ELSE,
        FOR,
        IN,
        RANGE,
        BREAK,
        CONTINUE,
        WHILE,
        FUNCTION,
        RETURN,
        STRUCT,
        CLASS,
        WHERE,

        /* single chars */
        PLUS, /* + */
        MINUS, /* - */
        STAR, /* * */
        SLASH, /* / */
        PERCENT, /* % */
        EQUAL, /* = */
        BANG, /* ! */
        LESS, /* < */
        GREATER, /* > */

        BAND, /* & */
        BOR, /* | */
        BXOR, /* ^ */
        BNOT, /* ~ */

        DOT, /* . */

        /* multi-chars */
        ARROW, /* -> */
        SCOPE, /* :: */

        LAND, /* && */
        LOR, /* || */
        GE, /* >= */
        LE, /* <= */
        EQ, /* == */
        NE, /* != */
        PLUS_EQ, /* += */
        MINUS_EQ, /* -= */
        STAR_EQ, /* *= */
        SLASH_EQ, /* /= */
        PERCENT_EQ, /* %= */
        BAND_EQ, /* &= */
        BOR_EQ,  /* |= */
        BXOR_EQ,  /* ^= */
        BSHL, /* << */
        BSHR, /* >> */
        BSHL_EQ, /* <<= */
        BSHR_EQ, /* >>= */

        /* delimiters */
        LEFT_PAREN, /* ( */
        RIGHT_PAREN, /* ) */
        LEFT_BRACE, /* { */
        RIGHT_BRACE, /* } */
        LEFT_BRACKET, /* [ */
        RIGHT_BRACKET, /* ] */
        COMMA, /* , */
        COLON, /* : */
        SEMICOLON, /* ; */
        QUESTION, /* ? */
    };

    enum class TokenProps : std::uint8_t {};

    struct Token
    {
        uint32_t start; /* this is relative to the source; just index source[tk.start] */
        uint16_t len; /* to get the whole token, do source[tk.start + len] */
        TokenType type;
        TokenProps props;
    };

    struct alignas(8) TokenList
    {
        std::vector<uint32_t> starts;
        std::vector<uint16_t> lens;
        std::vector<TokenType> types;
        std::vector<TokenProps> props;

        void emplace_back(const Token &tk)
        {
            starts.emplace_back(tk.start);
            lens.emplace_back(tk.len);
            types.emplace_back(tk.type);
            props.emplace_back(tk.props);
        }

        void push_back(const Token &tk)
        {
            starts.push_back(tk.start);
            lens.push_back(tk.len);
            types.push_back(tk.type);
            props.push_back(tk.props);
        }

        void reserve(const uint32_t &n)
        {
            starts.reserve(n);
            lens.reserve(n);
            types.reserve(n);
            props.reserve(n);
        }

        [[nodiscard]] size_t size() const
        {
            return starts.size();
        }

        [[nodiscard]] Token operator[](const size_t index) const
        {
            return Token {
                .start = starts[index],
                .len = lens[index],
                .type = types[index],
                .props = props[index]
            };
        }
    };
    
    static constexpr std::pair<std::string_view, TokenType> token_map[] = {
        { "true", TokenType::TRUE },
        { "false", TokenType::FALSE },
        { "null", TokenType::NIL },
        { "import", TokenType::IMPORT },
        { "var", TokenType::VAR },
        { "const", TokenType::CONST },
        { "function", TokenType::FUNCTION },
        { "return", TokenType::RETURN },
        { "enum", TokenType::ENUM },
        { "if", TokenType::IF },
        { "else", TokenType::ELSE },
        { "for", TokenType::FOR },
        { "while", TokenType::WHILE },
        { "break", TokenType::BREAK },
        { "continue", TokenType::CONTINUE },
        { "struct", TokenType::STRUCT },
        { "class", TokenType::CLASS },

        { "u8", TokenType::U8 },
        { "i8", TokenType::I8 },
        { "u16", TokenType::U16 },
        { "i16", TokenType::I16 },
        { "u32", TokenType::U32 },
        { "i32", TokenType::I32 },
        { "u64", TokenType::U64 },
        { "i64", TokenType::I64 },
        { "f32", TokenType::F32 },
        { "f64", TokenType::F64 },
        { "string", TokenType::STRING },
        { "bool", TokenType::BOOL },
        { "void", TokenType::VOID },
        { "Own", TokenType::OWN },
        { "Share", TokenType::SHARE },
        { "Ref", TokenType::REF },
        { "Pin", TokenType::PIN },

        { "cast", TokenType::CAST },
        { "static_cast", TokenType::STATIC_CAST },
        { "truncate_cast", TokenType::TRUNCATE_CAST },
        { "reinterpret_cast", TokenType::REINTERPRET_CAST },

        { "+", TokenType::PLUS },
        { "-", TokenType::MINUS },
        { "*", TokenType::STAR },
        { "/", TokenType::SLASH },
        { "%", TokenType::PERCENT },
        { "=", TokenType::EQUAL },
        { "!", TokenType::BANG },
        { "<", TokenType::LESS },
        { ">", TokenType::GREATER },
        { "&", TokenType::BAND },
        { "|", TokenType::BOR },
        { "^", TokenType::BXOR },
        { "~", TokenType::BNOT },
        { ".", TokenType::DOT },

        { "->", TokenType::ARROW },
        { "::", TokenType::SCOPE },
        { "..", TokenType::RANGE },
        { "&&", TokenType::LAND },
        { "||", TokenType::LOR },
        { ">=", TokenType::GE },
        { "<=", TokenType::LE },
        { "==", TokenType::EQ },
        { "!=", TokenType::NE },
        { "+=", TokenType::PLUS_EQ },
        { "-=", TokenType::MINUS_EQ },
        { "*=", TokenType::STAR_EQ },
        { "/=", TokenType::SLASH_EQ },
        { "%=", TokenType::PERCENT_EQ },
        { "&=", TokenType::BAND_EQ },
        { "|=", TokenType::BOR_EQ },
        { "^=", TokenType::BXOR_EQ },
        { "<<", TokenType::BSHL },
        { ">>", TokenType::BSHR },
        { "<<=", TokenType::BSHL_EQ },
        { ">>=", TokenType::BSHR_EQ },

        { "(", TokenType::LEFT_PAREN },
        { ")", TokenType::RIGHT_PAREN },
        { "{", TokenType::LEFT_BRACE },
        { "}", TokenType::RIGHT_BRACE },
        { "[", TokenType::LEFT_BRACKET },
        { "]", TokenType::RIGHT_BRACKET },
        { ",", TokenType::COMMA },
        { ":", TokenType::COLON },
        { ";", TokenType::SEMICOLON },
        { "?", TokenType::QUESTION },
    };

    static constexpr std::array<std::string_view, static_cast<size_t>(ash::TokenType::QUESTION) + 1> token_names = []()
    {
        std::array<std::string_view, static_cast<size_t>(ash::TokenType::QUESTION) + 1> map {};
        
        map[static_cast<size_t>(ash::TokenType::IDENTIFER)] = "<identifier>";
        map[static_cast<size_t>(ash::TokenType::NUM_LITERAL)] = "<number>";
        map[static_cast<size_t>(ash::TokenType::STR_LITERAL)] = "<string>";
        map[static_cast<size_t>(ash::TokenType::UNKNOWN)] = "<unknown>";
        map[static_cast<size_t>(ash::TokenType::END_OF_FILE)] = "<EOF>";
        
        for (const auto &[str, token]: ash::token_map) {
            map[static_cast<size_t>(token)] = str;
        }
        
        return map;
    }();
}