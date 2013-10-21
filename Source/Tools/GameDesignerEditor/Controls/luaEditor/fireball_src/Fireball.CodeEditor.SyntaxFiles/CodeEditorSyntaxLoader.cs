#region Fireball License
//    Copyright (C) 2005  Sebastian Faltoni sebastian{at}dotnetfireball{dot}net
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#endregion

using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using Fireball.Windows.Forms;
using System.Reflection;
using Fireball.Syntax;

namespace Fireball.CodeEditor.SyntaxFiles
{
    public sealed class CodeEditorSyntaxLoader
    {
        static LanguageList _LangList = null;

        private static string GetSyntaxFileName(SyntaxLanguage lang)
        {
            string file = Enum.GetName(typeof(SyntaxLanguage), lang);

            file += ".syn";

            return "Fireball.CodeEditor.SyntaxFiles.Syns." + file;
        }

        public static void SetSyntax(CodeEditorControl editor, SyntaxLanguage language)
        {
            Stream xml = GetSyntaxStream(GetSyntaxFileName(language));

            editor.Document.Parser.Init(Language.FromSyntaxFile(xml));
        }

        private static Stream GetSyntaxStream(string file)
        {
            Stream strm = typeof(CodeEditorSyntaxLoader).Assembly.GetManifestResourceStream(file);
            return strm;
        }

        public static void SetSyntax(CodeEditorControl editor, string filename)
        {
            editor.Document.Parser.Init(CodeEditorSyntaxLoader.LanguageList.GetLanguageFromFile(filename));
        }

        public static Language GetLanguageFrom(SyntaxLanguage syntax)
        {
            Stream xml = GetSyntaxStream(GetSyntaxFileName(syntax));

            return Language.FromSyntaxFile(xml);
        }

        public static LanguageList LanguageList
        {
            get
            {
                if (_LangList == null)
                {
                    _LangList = new LanguageList();

                    SyntaxLanguage[] languages = (SyntaxLanguage[])Enum.GetValues(typeof(SyntaxLanguage));

                    foreach (SyntaxLanguage current in languages)
                    {
                        Stream strm = GetSyntaxStream(GetSyntaxFileName(current));

                        _LangList.Add(Language.FromSyntaxFile(strm));
                    }
                }

                return _LangList;
            }
        }
    }

    public enum SyntaxLanguage
    {
        Lang6502,
        //ASP,
        CPP,
        Cobol,
        CSharp,
        CSS,
        DataFlex,
        Delphi,
        DOSBatch,
        Fortran90,
       // FoxPro,
        Java,
        JavaScript,
        JSP,
        LotusScript,
        MSIL,
        MySql_SQL,
        NPath,
        Oracle_SQL,
        Perl,
        PHP,
        Povray,
        Python,
        Rtf,
        SmallTalk,
        SqlServer2K5,
        SqlServer2K,
        SqlServer7,
        SystemPolicies,
        Template,
        Text,
        TurboPascal,
        VBNET,
        VB,
        VBScript,
        VRML97,
        XML,
        HTML,
        Nemerle,
        Gemix,
        AutoIt,
        Lua
    }
}
