#include <iostream>
#include <string>
#include <vector>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

// Struct to store parsed Org-Mode elements
struct OrgElement {
    int level;
    std::string heading;
    std::vector<std::string> listItems;
};

// Grammar definition using Boost Spirit
template <typename Iterator>
struct OrgGrammar : qi::grammar<Iterator, OrgElement(), ascii::space_type> {
    OrgGrammar() : OrgGrammar::base_type(start) {
        using namespace qi;

        heading %= repeat(1, 6)[char('*')] >> +ascii::space >> lexeme[+(char_ - eol)];
        listItem %= +ascii::space >> '-' >> +ascii::space >> lexeme[+(char_ - eol)];
        content %= (heading | listItem) >> eol;
        start %= *content;
    }

    qi::rule<Iterator, std::string()> heading;
    qi::rule<Iterator, std::string()> listItem;
    qi::rule<Iterator, OrgElement(), ascii::space_type> content;
    qi::rule<Iterator, OrgElement(), ascii::space_type> start;
};

int main() {
    std::string input =
        "* Heading 1\n"
        "  - Item 1\n"
        "  - Item 2\n"
        "** Heading 1.1\n"
        "   - Nested Item 1\n"
        "* Heading 2\n";

    std::vector<OrgElement> orgElements;
    OrgGrammar<std::string::iterator> parser;

    auto begin = input.begin();
    auto end = input.end();

    bool success = qi::phrase_parse(begin, end, parser, ascii::space, orgElements);

    if (success && begin == end) {
        for (const OrgElement& element : orgElements) {
            std::cout << "Level: " << element.level << ", Heading: " << element.heading << std::endl;
            for (const std::string& item : element.listItems) {
                std::cout << "  - " << item << std::endl;
            }
        }
    } else {
        std::cerr << "Parsing failed" << std::endl;
    }

    return 0;
}
