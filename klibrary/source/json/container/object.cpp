#include "klibrary.h"


kl::json::Object::Object()
{}

kl::json::Object::Object(const std::initializer_list<std::pair<std::string, Ref<Container>>>& items)
{
    insert(items.begin(), items.end());
}

kl::json::Object::Object(const std::string& data)
{
    const auto tokens = Lexer::parse(data);
    compile(tokens.begin(), tokens.end());
}

bool kl::json::Object::compile(std::vector<Token>::const_iterator first, std::vector<Token>::const_iterator last)
{
    if (first == last) {
        return false;
    }

    int depth = 0;
    std::optional<std::string> key;
    for (auto it = first; it != last; ++it) {
        if (depth == 1) {
            if (key) {
                Ref<Container> container;
                if (it->type == TokenType::_ARRAY_START) {
                    container = new Array();
                }
                else if (it->type == TokenType::_OBJECT_START) {
                    container = new Object();
                }
                else {
                    container = new Literal();
                }
                if (container->compile(it, last)) {
                    (*this)[key.value()] = container;
                }
                key.reset();
            }
            else if (it->type == TokenType::_STRING) {
                key = it->value;
            }
        }
        if (it->type == TokenType::_OBJECT_START || it->type == TokenType::_ARRAY_START) {
            depth += 1;
        }
        else if (it->type == TokenType::_OBJECT_END || it->type == TokenType::_ARRAY_END) {
            depth -= 1;
            if (depth <= 0) {
                break;
            }
        }
    }
    return true;
}

std::string kl::json::Object::decompile(const int depth) const
{
    if (empty()) {
        return format(Standard::object_start_literal, Standard::object_end_literal);
    }

    std::stringstream stream;
    if (depth >= 0) {
        const std::string map_depth(depth * 2, ' ');
        const std::string content_depth((depth + 1) * 2, ' ');
        const auto last_it = --end();
        stream << Standard::object_start_literal << '\n';
        for (auto i = begin(); i != end(); i++) {
            std::string name = i->first;
            Lexer::from_escaping(name);
            stream << content_depth << Standard::string_literal << name << Standard::string_literal;
            stream << Standard::assign_literal << ' ';
            stream << i->second->decompile(depth + 1);
            if (i != last_it) {
                stream << Standard::splitter_literal;
            }
            stream << '\n';
        }
        stream << map_depth << Standard::object_end_literal;
    }
    else {
        const auto last_it = --end();
        stream << Standard::object_start_literal << ' ';
        for (auto i = begin(); i != end(); i++) {
            std::string name = i->first;
            Lexer::from_escaping(name);
            stream << Standard::string_literal << name << Standard::string_literal;
            stream << Standard::assign_literal << ' ';
            stream << i->second->decompile(-1);
            if (i != last_it) {
                stream << Standard::splitter_literal;
            }
            stream << ' ';
        }
        stream << Standard::object_end_literal;
    }
    return stream.str();
}
