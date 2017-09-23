#include "ValueRefParserImpl.h"

#include "EnumParser.h"

#include "../universe/Enums.h"

namespace parse {
    struct visibility_complex_parser_rules {
        visibility_complex_parser_rules(const parse::lexer& tok) :
            simple_int_rules(tok)
        {
            namespace phoenix = boost::phoenix;
            namespace qi = boost::spirit::qi;

            using phoenix::construct;
            using phoenix::new_;

            qi::_1_type _1;
            qi::_a_type _a;
            qi::_b_type _b;
            qi::_c_type _c;
            qi::_d_type _d;
            qi::_e_type _e;
            qi::_f_type _f;
            qi::_val_type _val;

            const parse::value_ref_rule<int>& simple_int = simple_int_rules.simple;

            empire_object_visibility
                =   tok.EmpireObjectVisibility_ [ _a = construct<std::string>(_1) ]
                >   detail::label(Empire_token) >   simple_int [ _b = _1 ]
                >   detail::label(Object_token) >   simple_int [ _c = _1 ]
                    [ _val = new_<ValueRef::ComplexVariable<Visibility>>(_a, _b, _c, _f, _d, _e) ]
                ;

            start
                =   empire_object_visibility
                ;

            empire_object_visibility.name("EmpireObjectVisibility");

#if DEBUG_DOUBLE_COMPLEX_PARSERS
            debug(empire_object_visibility);

#endif
        }
        parse::detail::simple_int_parser_rules  simple_int_rules;
        parse::detail::complex_variable_rule<Visibility> empire_object_visibility;
        parse::detail::complex_variable_rule<Visibility> start;
    };

    const parse::detail::complex_variable_rule<Visibility>& visibility_var_complex(const parse::lexer& tok)
    {
        //TODO make into a grammar
        static visibility_complex_parser_rules visibility_complex_parser(tok);
        return visibility_complex_parser.start;
    }
}

namespace parse { namespace detail {
    visibility_parser_rules::visibility_parser_rules(
        const parse::lexer& tok,
        Labeller& labeller,
        const condition_parser_grammar& condition_parser
    ) :
        enum_value_ref_rules("Visibility", tok, condition_parser)
    {
        namespace phoenix = boost::phoenix;
        namespace qi = boost::spirit::qi;

        using phoenix::new_;

        qi::_val_type _val;

        // variable_name left empty, as no direct object properties are
        // available that return a visibility

        enum_expr
            =   tok.Invisible_  [ _val = VIS_NO_VISIBILITY ]
            |   tok.Basic_      [ _val = VIS_BASIC_VISIBILITY ]
            |   tok.Partial_    [ _val = VIS_PARTIAL_VISIBILITY ]
            |   tok.Full_       [ _val = VIS_FULL_VISIBILITY ]
            ;

        free_variable_expr
            =   tok.Value_      [ _val = new_<ValueRef::Variable<Visibility>>(ValueRef::EFFECT_TARGET_VALUE_REFERENCE) ]
            ;

        complex_expr
            =   visibility_var_complex(tok)
            ;
    }

    }
}
