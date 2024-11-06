/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        include/glc/hicGlich.h
 * Project:     Glich script hic extension library.
 * Purpose:     HicGlich class header
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     22nd October 2024
 * Copyright:   Copyright (c) 2024, Nick Matthews.
 * Licence:     GNU GPLv3
 *
 *  Glich is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Glich is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Glich.  If not, see <http://www.gnu.org/licenses/>.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

*/

#ifndef INCLUDE_GLC_HICGLICH_H_GUARD
#define INCLUDE_GLC_HICGLICH_H_GUARD

#include <glc/glich.h>
#include "hicDefs.h"

namespace glich {

    enum class InitLibrary { None, Hics };

    class HicGlich : public Glich
    {
    public:
        HicGlich( InOut* inout = nullptr ) : Glich( inout ) {}
        ~HicGlich();

        void init() override;

        void load_builtin_library() override;
        void load_hics_library();
        std::string run_module( const std::string& mod ) override;

        SValue evaluate( const std::string& expression ) override;
        bool run( std::istream& in, std::ostream& out, int line = 1 ) override;

        bool add_module_def( const ModuleDef& def, const std::string& code ) override;

        SchemeList get_scheme_list( Visibility vis );
        void get_scheme_info( Scheme_info* info, const std::string& scode );
        void get_input_info( SchemeFormatInfo* info, const std::string& scode );
        void get_output_info( SchemeFormatInfo* info, const std::string& scode );
        void get_format_text_info( FormatText_info* info, const std::string& scode, const std::string& fcode );

        RList date_phrase_to_rlist( const std::string& phrase, const std::string& sig = std::string() );
        std::string date_phrase_to_text( const std::string& phrase, const std::string& sig_in = std::string(),
            const std::string& sig_out = std::string() );
        std::string rlist_to_text( RList rlist, const std::string& sig = std::string() );
        std::string range_to_text( Range range, const std::string& sig = std::string() );
        std::string field_to_text( Field field, const std::string& sig = std::string() );
        RList text_to_rlist( const std::string& text, const std::string& sig = std::string() );
        Range text_to_range( const std::string& text, const std::string& sig = std::string() );
        Field text_to_field( const std::string& text, const std::string& sig = std::string() );

        HicMarkDataVec get_hic_data() const;

        bool get_lexicon_info( Lexicon_info* info, const std::string& code );
        bool add_lexicon( Lexicon* lex, const std::string& code );
        void remove_lexicon( const std::string& code );
        Lexicon* get_lexicon( const std::string& code );
        DefinedStatus get_lexicon_status( const std::string& code ) const;
        bool add_grammar( Grammar* gmr, const std::string& code );
        void remove_grammar( const std::string& code );
        Grammar* get_grammar( const std::string& code );
        DefinedStatus get_grammar_status( const std::string& code ) const;
        bool add_format( const std::string& code );
        bool add_scheme( Scheme* sch, const std::string& scode );
        Scheme* get_scheme( const std::string& scode );
        DefinedStatus get_scheme_status( const std::string& code ) const;
        StdStrVec get_scheme_list() const;

        Mark* create_mark( const std::string& name, Mark* prev ) override;
        bool set_property( const std::string& property, const std::string& value ) override;
        void set_ischeme( Scheme* sch );
        void set_oscheme( Scheme* sch );
        Context get_context() const;
        Scheme* get_ischeme() const;
        Scheme* get_oscheme() const;

        std::string get_special_value_string( SpecialValue val ) override;

    private:
        LexiconMap m_lexicons;
        StdStrMap m_lexicon_mods;
        GrammarMap m_grammars;
        StdStrMap m_grammar_mods;
    };

} // namespace glich

#endif // INCLUDE_GLC_HICGLICH_H_GUARD
