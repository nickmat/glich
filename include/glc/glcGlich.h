/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        include/glc/glcGlich.h
 * Project:     Glich: Extendable Script Language.
 * Purpose:     Glich language class header
 * Author:      Nick Matthews
 * Website:     https://github.com/nickmat/glich
 * Created:     8th February 2023
 * Copyright:   Copyright (c) 2023..2025, Nick Matthews.
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

#ifndef INCLUDE_GLC_GLCGLICH_H_GUARD
#define INCLUDE_GLC_GLCGLICH_H_GUARD

#include "glcDefs.h"

namespace glich {

    class SValue;
    class Command;
    using CommandMap = std::map<std::string, Command*>;
    class Object;
    using ObjectMap = std::map<std::string, Object*>;
    class File;
    using FileMap = std::map<std::string, File*>;
    class HicMark;
    class Mark;
    using MarkVec = std::vector<Mark*>;
    class Store;

    class InOut {
    public:
        InOut() {}
        virtual ~InOut() {}

        virtual std::string get_input( const std::string& prompt );
    };

    class Glich {
    public:
        Glich( InOut* inout = nullptr );
        virtual ~Glich();

        virtual void init();

        static const char* version();

        GlcMarkDataVec get_glc_data() const;

        virtual void load_builtin_library( StdStrVec args );

        std::string run_script( const std::string& script );
        std::string run_script_file( const std::string& filename );
        virtual std::string run_module( const std::string& mod );
        virtual SValue evaluate( const std::string& expression );
        virtual bool run( std::istream& in, std::ostream& out, int line = 1 );

        std::string get_init_error() const { return m_init_error; }

        bool is_named( const std::string& name ) const;
        SValue get_named( const std::string& name ) const;
        bool is_variable( const std::string& name ) const;
        bool create_variable( const std::string& name, VariableType type );
        SValue* get_variable_ptr( const std::string& name );
        bool create_local( const std::string& name );
        bool update_local( const std::string& name, SValue& value );
        SValue get_local( const std::string& name ) const;
        SValue* get_local_ptr( const std::string& name );
        bool is_local( const std::string& name ) const;
        bool create_global( const std::string& name );
        SValue* get_global_ptr( const std::string& name );
        void remove_global( const std::string& name );
        bool create_constant( const std::string& name );
        SValue* get_constant_ptr( const std::string& name );
        bool is_constant( const std::string& name ) const;
        void remove_constant( const std::string& name );

        bool add_function( SpFunction fun );
        void remove_function( const std::string& code );
        Function* get_function( const std::string& code ) const;
        bool add_command( SpFunction com );
        void remove_command( const std::string& code );
        Function* get_command( const std::string& code ) const;
        bool add_object( Object* obj, const std::string& code );
        void remove_object( const std::string& code );
        Object* get_object( const std::string& code );
        DefinedStatus get_object_status( const std::string& code ) const;
        bool add_file( File* file, const std::string& code );
        void remove_file( const std::string& code );
        File* get_file( const std::string& code ) const;
        bool add_module( const Module& mod );
        virtual bool add_module_def( const ModuleDef& def, const std::string& code );
        bool module_exists( const std::string& code ) const;
        void remove_module( const std::string& code );

        virtual Mark* create_mark( const std::string& name, Mark* prev );
        void add_or_replace_mark( const std::string& name );
        bool clear_mark( const std::string& name );
        void push_store();
        bool pop_store();
        bool is_level_zero() const;
        std::string read_input( const std::string& prompt ) const;

        virtual bool set_property( const std::string& property, const std::string& value );
        Integer get_integer() const;
        WriteText get_write_text() const;

        virtual std::string get_special_value_string( SpecialValue val );

    protected:
        SpFunctionMap m_functions;
        SpFunctionMap m_commands;
        ObjectMap m_objects;
        StdStrMap m_object_mods;
        FileMap m_files;
        StdStrSet m_modules;
        MarkVec m_marks;
        Store* m_store;
        SValueMap m_globals;
        SValueMap m_constants;
        std::string m_init_error;
        InOut* m_inout;
    };

} // namespace glich

#endif // INCLUDE_GLC_GLCGLICH_H_GUARD
