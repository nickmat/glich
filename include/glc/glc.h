/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        include/glc/glc.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Glich language class header
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
 * Copyright:   Copyright (c) 2023, Nick Matthews.
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

#ifndef INCLUDE_GLC_GLC_H_GUARD
#define INCLUDE_GLC_GLC_H_GUARD

#include "glcDefs.h"
#include "hicDefs.h"

namespace glich {

    class SValue;
    class Command;
    using CommandMap = std::map<std::string, Command*>;
    class Object;
    using ObjectMap = std::map<std::string, Object*>;
    class File;
    using FileMap = std::map<std::string, File*>;
    class Mark;
    using MarkVec = std::vector<Mark*>;
    class Store;

    class InOut {
    public:
        InOut() {}
        virtual ~InOut() {}

        virtual std::string get_input( const std::string& prompt );
    };

    enum class InitLibrary { None, Hics };

    class Glich {
    public:
        Glich( InitLibrary lib = InitLibrary::None, InOut* inout = nullptr );
        ~Glich();

        static const char* version();

        SchemeList get_scheme_list( SchemeStyle style ) const;
        void get_scheme_info( Scheme_info* info, const std::string& scode ) const;
        void get_input_info( SchemeFormatInfo* info, const std::string& scode ) const;
        void get_output_info( SchemeFormatInfo* info, const std::string& scode ) const;
        void get_format_text_info( FormatText_info* info, const std::string& scode, const std::string& fcode ) const;
        bool get_lexicon_info( Lexicon_info* info, const std::string& code ) const;

        RList date_phrase_to_rlist( const std::string& phrase, const std::string& sig = std::string() );
        std::string date_phrase_to_text( const std::string& phrase, const std::string& sig_in = std::string(),
            const std::string& sig_out = std::string() );
        std::string rlist_to_text( RList rlist, const std::string& sig = std::string() );
        std::string range_to_text( Range range, const std::string& sig = std::string() );
        std::string field_to_text( Field field, const std::string& sig = std::string() );
        RList text_to_rlist( const std::string& text, const std::string& sig = std::string() );
        Range text_to_range( const std::string& text, const std::string& sig = std::string() );
        Field text_to_field( const std::string& text, const std::string& sig = std::string() );

        void load_builtin_library();
        void load_hics_library();

        std::string run_script( const std::string& script );
        std::string run_script_file( const std::string& filename );
        SValue evaluate( const std::string& expression, Store* store );
        SValue evaluate( const std::string& expression );

        std::string get_init_error() const { return m_init_error; }

        bool is_named( const std::string& name ) const;
        SValue get_named( const std::string& name ) const;
        bool create_local( const std::string& name );
        bool update_local( const std::string& name, SValue& value );
        SValue get_local( const std::string& name ) const;
        SValue* get_local_ptr( const std::string& name );
        bool is_local( const std::string& name ) const;
        bool is_constant( const std::string& name ) const;

        bool add_function( Function* fun );
        Function* get_function( const std::string& code ) const;
        bool add_command( Function* com );
        Function* get_command( const std::string& code ) const;
        Object* create_object( const std::string & code );
        bool add_object( Object* obj, const std::string& code );
        Object* get_object( const std::string& code ) const;
        File* create_file( const std::string& code );
        File* get_file( const std::string& code ) const;
        bool add_lexicon( Lexicon* lex, const std::string& code );
        Lexicon* get_lexicon( const std::string& code ) const;
        bool add_grammar( Grammar* gmr, const std::string& code );
        Grammar* get_grammar( const std::string& code ) const;
        bool add_format( Format* fmt, const std::string& code );
        bool add_scheme( Scheme* sch, const std::string& scode );
        Scheme* get_scheme( const std::string& scode ) const;

        void add_or_replace_mark( const std::string& name );
        bool clear_mark( const std::string& name );
        void push_store();
        bool pop_store();
        bool is_level_zero() const;
        std::string read_input( const std::string& prompt ) const;

        void set_context( Context ct );
        void set_ischeme( Scheme* sch );
        void set_oscheme( Scheme* sch );
        Context get_context() const;
        Scheme* get_ischeme() const;
        Scheme* get_oscheme() const;

    private:
        FunctionMap m_functions;
        FunctionMap m_commands;
        ObjectMap m_objects;
        FileMap m_files;
        LexiconMap m_lexicons;
        GrammarMap m_grammars;
        MarkVec m_marks;
        Store* m_store;
        SValueMap m_constants;
        std::string m_init_error;
        InOut* m_inout;
    };

    void init_glc( InitLibrary lib );
    void exit_glc();
    Glich* get_glc();


} // namespace glich

#endif // INCLUDE_GLC_GLC_H_GUARD
